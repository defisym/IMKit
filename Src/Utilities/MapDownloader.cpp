#include "MapDownloader.h"

#include <fstream>
#include <filesystem>

#include "HttpDownloader.h"

// Useful Links and Resources
//
// https://operations.osmfoundation.org/policies/tiles/
// https://wiki.openstreetmap.org/wiki/Tile_servers
// https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames

// ------------------------------------------------
// General
// ------------------------------------------------

namespace fs = std::filesystem;

constexpr auto TILE_SIZE = 256;     // the expected size of tiles in pixels, e.g. 256x256px;
constexpr auto MAX_ZOOM = 19;       // the maximum zoom level provided by the server;
constexpr auto MAX_THREADS = 2;     // the maximum threads to use for downloading tiles (OSC strictly forbids more than 2);
constexpr auto PI = 3.14159265359;

int long2tilex(double lon, int z) {
    return (int)(floor((lon + 180.0) / 360.0 * pow(2, z)));
}

int lat2tiley(double lat, int z) {
    double latrad = lat * PI / 180.0;
    return (int)(floor((1.0 - asinh(tan(latrad)) / PI) / 2.0 * pow(2, z)));
}

double tilex2long(int x, int z) {
    return x / pow(2, z) * 360.0 - 180;
}

double tiley2lat(int y, int z) {
    double n = PI - 2.0 * PI * y / pow(2, z);
    return 180.0 / PI * atan(0.5 * (exp(n) - exp(-n)));
}

// ------------------------------------------------
// TileCoord
// ------------------------------------------------

std::tuple<ImPlotPoint, ImPlotPoint> TileCoord::bounds() const {
    double n = std::pow(2, z);
    double t = 1.0 / n;
    return {
        { x * t, (1 + y) * t },
        { (1 + x) * t , (y)*t }
    };
}

bool operator<(const TileCoord& l, const TileCoord& r) {
    if (l.z < r.z)  return true;
    if (l.z > r.z)  return false;
    if (l.x < r.x)  return true;
    if (l.x > r.x)  return false;
    if (l.y < r.y)  return true;
    if (l.y > r.y)  return false;
    return false;
}

// ------------------------------------------------
// TileManager
// ------------------------------------------------

bool TileManager::Tile::Load(D3DContext* pCtx, const char* pPath) { 
    auto texture = LoadTextureFromFile(pCtx->pDevice.Get(), pPath);
    if (texture.pSrv == nullptr) { return false; }
    this->texture = texture;

    return true; 
}

TileManager::TileManager(D3DContext* pCtx)
    :pContext(pCtx) {
    start_workers();
}

TileManager::~TileManager() {
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_stop = true;
    }

    m_condition.notify_all();
    for (std::thread& worker : m_workers) {
        worker.join();
    }
}

const TileManager::Region& TileManager::get_region(ImPlotRect view, ImVec2 pixels) {
    double min_x = std::clamp(view.X.Min, 0.0, 1.0);
    double min_y = std::clamp(view.Y.Min, 0.0, 1.0);
    double size_x = std::clamp(view.X.Size(), 0.0, 1.0);
    double size_y = std::clamp(view.Y.Size(), 0.0, 1.0);

    double pix_occupied_x = (pixels.x / view.X.Size()) * size_x;
    double pix_occupied_y = (pixels.y / view.Y.Size()) * size_y;
    double units_per_tile_x = view.X.Size() * (TILE_SIZE / pix_occupied_x);
    double units_per_tile_y = view.Y.Size() * (TILE_SIZE / pix_occupied_y);

    int z = 0;
    double r = 1.0 / pow(2, z);
    while (r > units_per_tile_x && r > units_per_tile_y && z < MAX_ZOOM) {
        r = 1.0 / pow(2, ++z);
    }

    m_region.clear();
    if (!append_region(z, min_x, min_y, size_x, size_y) && z > 0) {
        append_region(--z, min_x, min_y, size_x, size_y);
        std::reverse(m_region.begin(), m_region.end());
    }

    return m_region;
}

TileManager::TilePtr TileManager::request_tile(TileCoord coord) {
    std::lock_guard<std::mutex> lock(m_tiles_mutex);

    if (m_tiles.count(coord)) { return get_tile(coord); }
    else if (fs::exists(coord.path())) { return load_tile(coord); }
    else { download_tile(coord); }

    return nullptr;
}

bool TileManager::append_region(int z, double min_x, double min_y, double size_x, double size_y) {
    auto k = pow(2, z);
    auto xa = min_x * k;
    auto ya = min_y * k;

    auto r = 1.0 / k;
    auto xb = xa + ceil(size_x / r) + 1;
    auto yb = ya + ceil(size_y / r) + 1;
    xb = std::clamp(xb, 0.0, k);
    yb = std::clamp(yb, 0.0, k);
    
    bool covered = true;
    for (int x = (int)xa; x < (int)xb; ++x) {
        for (int y = (int)ya; y < (int)yb; ++y) {
            TileCoord coord{ z,x,y };
            std::shared_ptr<Tile> tile = request_tile(coord);
            m_region.push_back({ coord,tile });
            if (tile == nullptr || tile->state != TileState::Loaded)
                covered = false;
        }
    }

    return covered;
}

void TileManager::download_tile(TileCoord coord) {
    auto dir = coord.dir();
    fs::create_directories(dir);
    if (!fs::exists(dir)) { return; }

    m_tiles[coord] = std::make_shared<Tile>(Downloading);
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_queue.emplace(coord);
    }
    m_condition.notify_one();
}

TileManager::TilePtr TileManager::get_tile(TileCoord coord) {
    if (m_tiles[coord]->state == Loaded) { return m_tiles[coord]; }
    else if (m_tiles[coord]->state == OnDisk) { return load_tile(coord); }

    return nullptr;
}

TileManager::TilePtr TileManager::load_tile(TileCoord coord) {
    auto path = coord.path();
    if (!m_tiles.count(coord)) {
        m_tiles[coord] = std::make_shared<Tile>();
    }

    if (m_tiles[coord]->Load(pContext, path.c_str())) {
        m_tiles[coord]->state = TileState::Loaded;
        m_loads++;
        return m_tiles[coord];
    }

    m_fails++;
    printf("TileManager[00]: Failed to load \"%s\"\n", path.c_str());
    if (!fs::remove(path)) {
        printf("TileManager[00]: Failed to remove \"%s\"\n", path.c_str());
    }
    printf("TileManager[00]: Removed \"%s\"\n", path.c_str());
    m_tiles.erase(coord);

    return nullptr;
}

void TileManager::start_workers() {
    for (int thrd = 1; thrd < MAX_THREADS + 1; ++thrd) {
        m_workers.emplace_back(
            [this, thrd] {
                printf("TileManager[%02d]: Thread started\n", thrd);
                HttpDownloader downloader = { { .bHttps = true,
                    .site = "a.tile.opentopomap.org",
                    .getFormat = "/{}/{}/{}.png",
                    .userAgent = "HttpDownloader/1.0 (OOSAKA)" } };

                for (;;) {
                    TileCoord coord;

                    {
                        std::unique_lock<std::mutex> lock(m_queue_mutex);
                        m_condition.wait(lock,
                            [this] { return m_stop || !m_queue.empty(); });

                        if (m_stop && m_queue.empty()) {
                            printf("TileManager[%02d]: Thread terminated\n", thrd);
                            return;
                        }

                        coord = std::move(m_queue.front());
                        m_queue.pop();
                    }

                    m_working++;
                    bool success = true;
                    const auto err = downloader.Get([&] (const char* pData, size_t sz) {
                        std::ofstream ofs(coord.path(), std::ios::binary);
                        ofs.write(pData, sz);
                        },
                        coord.z, coord.x, coord.y);

                    if (err != 200) {
                        printf("TileManager[%02d]: Failed to download: \"%s\"\n", thrd, coord.url().c_str());
                        printf("TileManager[%02d]: Response code: %d\n", thrd, err);
                        success = false;
                    }

                    if (success) {
                        m_downloads++;
                        std::lock_guard<std::mutex> lock(m_tiles_mutex);
                        m_tiles[coord]->state = OnDisk;
                    }
                    else {
                        m_fails++;
                        std::lock_guard<std::mutex> lock(m_tiles_mutex);
                        m_tiles.erase(coord);
                    }

                    m_working--;
                }
            }
        );
    }
}

