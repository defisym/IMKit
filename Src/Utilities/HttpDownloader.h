#pragma once

#include <format>
#include <string>
#include <memory>
#include <functional>

constexpr auto SITE_LENGTH = 512;

struct ProxyParams {
    char localHost[SITE_LENGTH] = "127.0.0.1";
    int port = 10808;
};

template<>
struct std::hash<ProxyParams> {
    std::size_t operator()(ProxyParams const& s) const noexcept;
};

struct HttpParams {
    // use https instead of http
    // note: http is force used in proxy mode
    bool bHttps = true;
    // site without http or https
    // e.g., url is https://www.lovelive-anime.jp/nijigasaki
    // then site here is www.lovelive-anime.jp/nijigasaki
    char site[SITE_LENGTH] = {};
    // get request formatter
    char getFormat[SITE_LENGTH] = {};

    // User-Agent: <product> / <product-version> <comment>
    // see: 
    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers/User-Agent
    char userAgent[SITE_LENGTH] = "Shizuku/146.0 (OOSAKA)";
};

template<>
struct std::hash<HttpParams> {
    std::size_t operator()(HttpParams const& s) const noexcept;
};

struct DownloaderParams {
    bool bProxy = false;
    ProxyParams proxyParams = {};
    HttpParams httpParams = {};
};

template<>
struct std::hash<DownloaderParams> {
    std::size_t operator()(DownloaderParams const& s) const noexcept;
};

// ------------------------------------------------------
// Usage:
// ------------------------------------------------------
// 
// ------------------------------------
//  Download directly:
// ------------------------------------
// 
//    HttpDownloader d = { {.bHttps = true,
//        .site = "a.tile.opentopomap.org",
//        .getFormat = "/{}/{}/{}.png",
//        .userAgent = "HttpDownloader/1.0 (OOSAKA)"} };
// 
//    const auto err = d.Get([] (const char* pData, size_t sz) {
//        std::ofstream ofs("tile_1_1_1.png", std::ios::binary);
//        ofs.write(pData, sz);
//        },
//        1, 1, 1);
// 
// ------------------------------------
//  Download from proxy:
// ------------------------------------
// 
//    HttpDownloader dp = { {.localHost = "127.0.0.1",.port = 10808},
//        {.bHttps = true,
//        .site = "tile.openstreetmap.org",
//        .getFormat = "/{}/{}/{}.png",
//        .userAgent = "HttpDownloader/1.0 (OOSAKA)"} };
// 
//    const auto err = dp.Get([] (const char* pData, size_t sz) {
//        std::ofstream ofs("ptile_1_1_1.png", std::ios::binary);
//        ofs.write(pData, sz);
//        },
//        1,1,1);
struct HttpDownloader {
    DownloaderParams downloaderParams = {};

    void* pClient = nullptr;
    void* pHeader = nullptr;

    HttpDownloader(const HttpParams& hParams);
    HttpDownloader(const ProxyParams& pParams, const HttpParams& hParams);
    ~HttpDownloader();

    HttpDownloader(const HttpDownloader&) = delete;
    HttpDownloader& operator=(const HttpDownloader&) = delete;
    HttpDownloader(HttpDownloader&& r) noexcept;
    HttpDownloader& operator=(HttpDownloader&& r) noexcept;

    using DataCallback = std::function<void(const char*, size_t)>;

private:
    int Get(const DataCallback& cb, const std::string path);
public:
    // Error code:
    //  200 -> OK
    //  -1  -> No Connection
    template <class... Types>
    int Get(const DataCallback& cb, Types&&... args) {
        const auto& [bProxy, proxyParams, httpParams] = downloaderParams;

        auto path = std::vformat(httpParams.getFormat, std::make_format_args(args...));
        // cpp httplib doesn't support https in proxy mode
        if (bProxy) { path = std::format("http://{}{}", httpParams.site, path); }

        return Get(cb, path);
    }
};
