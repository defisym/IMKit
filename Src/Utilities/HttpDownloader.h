#pragma once

#include <format>
#include <string>
#include <memory>
#include <functional>

struct ProxyParams {
    std::string localHost = "127.0.0.1";
    unsigned short port = 10808;
};

struct HttpParams {
    // use https instead of http
    // note: http is force used in proxy mode
    bool bHttps = true;
    // site without http or https
    // e.g., url is https://www.lovelive-anime.jp/nijigasaki
    // then site here is www.lovelive-anime.jp/nijigasaki
    std::string site = {};
    // get request formatter
    std::string getFormat = {};

    // see: 
    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers/User-Agent
    std::string userAgent = "Shizuku/146.0 (OOSAKA)";
};

struct HttpDownloader {
    bool bProxy = false;
    ProxyParams proxyParams = {};
    HttpParams httpParams = {};

    void* pClient = nullptr;
    void* pHeader = nullptr;

    HttpDownloader(const HttpParams& hParams);
    HttpDownloader(const ProxyParams& pParams, const HttpParams& hParams);
    ~HttpDownloader();

    using DataCallback = std::function<void(const char*, size_t)>;

private:
    int Get(const DataCallback& cb, const std::string path);
public:
    // Error code:
    //  200 -> OK
    //  -1  -> No Connection
    template <class... Types>
    int Get(const DataCallback& cb, Types&&... args) {
        auto path = std::vformat(httpParams.getFormat, std::make_format_args(args...));
        // cpp httplib doesn't support https in proxy mode
        if (bProxy) { path = std::format("http://{}{}", httpParams.site, path); }

        return Get(cb, path);
    }
};