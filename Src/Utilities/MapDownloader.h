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

enum TileState : int {
    Unavailable = 0, // tile not available
    Loaded,          // tile has been loaded into  memory
    Downloading,     // tile is downloading from server
    OnDisk           // tile is saved to disk, but not loaded into memory
};

struct TileImage {
    void* ID = nullptr;
    bool Load(const char* pPath) { return false; }
};

struct Tile {
    Tile() : state(TileState::Unavailable) {}
    Tile(TileState s) : state(s) {}
    TileState state;
    TileImage image;
};

struct TileManager {
    TileManager() {
        start_workers();
    }

    ~TileManager();

    using Region = std::vector<std::pair<TileCoord, std::shared_ptr<Tile>>>;
    const Region& get_region(ImPlotRect view, ImVec2 pixels);
    std::shared_ptr<Tile> request_tile(TileCoord coord);

    int tiles_loaded() const { return m_loads; }
    int tiles_downloaded() const { return m_downloads; }
    int tiles_cached() const { return m_loads - m_downloads; }
    int tiles_failed() const { return m_fails; }
    int threads_working() const { return m_working; }

private:
    bool append_region(int z, double min_x, double min_y, double size_x, double size_y);
    void download_tile(TileCoord coord);
    std::shared_ptr<Tile> get_tile(TileCoord coord);
    std::shared_ptr<Tile> load_tile(TileCoord coord);
    void start_workers();

    std::atomic<int> m_loads = 0;
    std::atomic<int> m_downloads = 0;
    std::atomic<int> m_fails = 0;
    std::atomic<int> m_working = 0;
    std::map<TileCoord, std::shared_ptr<Tile>> m_tiles;
    std::mutex m_tiles_mutex;
    std::vector<std::pair<TileCoord, std::shared_ptr<Tile>>> m_region;
    std::vector<std::thread> m_workers;
    std::queue<TileCoord> m_queue;
    std::mutex m_queue_mutex;
    std::condition_variable m_condition;
    bool m_stop = false;
};