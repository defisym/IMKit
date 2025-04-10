#pragma once
#pragma warning(disable : 4996)

#include "implot.h"

#include <map>
#include <queue>
#include <vector>
#include <string>

#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "IMGuiEx/LoadTexture.h"

int long2tilex(double lon, int z);

int lat2tiley(double lat, int z);

double tilex2long(int x, int z);

double tiley2lat(int y, int z);

struct TileCoord {
    int z; // zoom    [0......20]
    int x; // x index [0...z^2-1]
    int y; // y index [0...z^2-1]
    inline std::string subdir() const { return std::to_string(z) + "/" + std::to_string(x) + "/"; }
    inline std::string dir() const { return "Tiles/" + subdir(); }
    inline std::string file() const { return std::to_string(y) + ".png"; }
    inline std::string path() const { return dir() + file(); }
    inline std::string url() const { return subdir() + file(); }
    inline std::string label() const { return subdir() + std::to_string(y); }
    std::tuple<ImPlotPoint, ImPlotPoint> bounds() const;
};

bool operator<(const TileCoord& l, const TileCoord& r);

constexpr auto BASEPATH_LENGTH = 512;

struct MapDownloadParams {
    int tileSize = 256;      // the expected size of tiles in pixels, e.g. 256x256px;
    int maxZoom = 19;       // the maximum zoom level provided by the server;
    int maxThreads = 4;     // the maximum threads to use for downloading tiles;
    char basePath[BASEPATH_LENGTH] = {};
    char suffix[BASEPATH_LENGTH] = ".png";
};

struct D3DContext;

struct TileManager {
    enum TileState : int {
        Unavailable = 0, // tile not available
        Loaded,          // tile has been loaded into  memory
        Downloading,     // tile is downloading from server
        OnDisk           // tile is saved to disk, but not loaded into memory
    };

    struct Tile {
        TileState state;
        IMGUITexture texture = {};
        double alpha = 0.0f;

        Tile() :state(TileState::Unavailable) {}
        Tile(TileState s) : state(s) {}

        bool Load(D3DContext* pCtx, const char* pPath);        
        double FadeIn(double step = 0.05) {
            alpha += step;
            alpha = std::min(1.0, alpha);

            return alpha;
        }
        void FadeComplete() { alpha = 1.0; }
    };

    TileManager(D3DContext* pCtx);
    ~TileManager();

    using TilePtr = std::shared_ptr<Tile>;
    using Region = std::vector<std::pair<TileCoord, TilePtr>>;
    const Region& get_region(ImPlotRect view, ImVec2 pixels);
    TilePtr request_tile(TileCoord coord);

    int tiles_loaded() const { return m_loads; }
    int tiles_downloaded() const { return m_downloads; }
    int tiles_cached() const { return m_loads - m_downloads; }
    int tiles_failed() const { return m_fails; }
    int tiles_pending();
    int threads_working() const { return m_working; }

private:
    bool append_region(int z, double min_x, double min_y, double size_x, double size_y);
    void download_tile(TileCoord coord);
    TilePtr get_tile(TileCoord coord);
    TilePtr load_tile(TileCoord coord);
    void start_workers();

    D3DContext* pContext = nullptr;
    MapDownloadParams mapDownloadParams = {};

    std::atomic<int> m_loads = 0;
    std::atomic<int> m_downloads = 0;
    std::atomic<int> m_fails = 0;
    std::atomic<int> m_working = 0;

    std::mutex m_tiles_mutex;
    std::map<TileCoord, TilePtr> m_tiles;
    Region m_region;

    bool m_stop = false;
    std::vector<std::thread> m_workers;

    std::mutex m_queue_mutex;
    std::condition_variable m_condition;
    std::queue<TileCoord> m_queue;
};
