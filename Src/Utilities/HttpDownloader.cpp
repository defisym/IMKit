#include "HttpDownloader.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <3rdLib/CppHttpLib/httplib.h>

#pragma comment(lib,"libcrypto.lib")
#pragma comment(lib,"libssl.lib")

#include "GUIContext/Param/Param.h"

std::size_t std::hash<ProxyParams>::operator()(ProxyParams const& s) const noexcept {
    std::size_t hash = 0xcbf29ce484222325; // FNV-1a

    hash ^= GetStringHash(s.localHost);
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= std::hash<unsigned short>{}(s.port);
    hash *= 0x100000001b3;  // FNV-1a

    return hash;
}

std::size_t std::hash<HttpParams>::operator()(HttpParams const& s) const noexcept {
    std::size_t hash = 0xcbf29ce484222325; // FNV-1a

    hash ^= std::hash<bool>{}(s.bHttps);
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= GetStringHash(s.site);
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= GetStringHash(s.getFormat);
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= GetStringHash(s.userAgent);
    hash *= 0x100000001b3;  // FNV-1a

    return hash;
}

std::size_t std::hash<DownloaderParams>::operator()(DownloaderParams const& s) const noexcept {
    return GetParamHash(s);
}

HttpDownloader::HttpDownloader(const HttpParams& hParams) {
    downloaderParams.httpParams = hParams;

    pClient = new httplib::Client{ hParams.bHttps
        ? std::format("https://{}", hParams.site)
        : std::format("http://{}", hParams.site) };
    pHeader = new httplib::Headers{ { "User-Agent", hParams.userAgent } };
}

HttpDownloader::HttpDownloader(const ProxyParams& pParams,
    const HttpParams& hParams) {
    downloaderParams.bProxy = true;
    downloaderParams.proxyParams = pParams;
    downloaderParams.httpParams = hParams;

    pClient = new httplib::Client{ pParams.localHost,pParams.port };
    pHeader = new httplib::Headers{ 
        { "Host", hParams.site },
        { "User-Agent", hParams.userAgent }
    };
}

HttpDownloader::~HttpDownloader() {
    delete (httplib::Client*)pClient;
    delete (httplib::Headers*)pHeader;
}

HttpDownloader::HttpDownloader(HttpDownloader&& r) noexcept {
    *this = std::forward<HttpDownloader&&>(r);
}

HttpDownloader& HttpDownloader::operator=(HttpDownloader&& r) noexcept {
    downloaderParams = r.downloaderParams;
    pClient = r.pClient; r.pClient = nullptr;
    pHeader = r.pHeader; r.pHeader = nullptr;

    return *this;
}

int HttpDownloader::Get(const DataCallback& cb, const std::string path) {
    auto res = ((httplib::Client*)(pClient))->Get(path,
        *(httplib::Headers*)pHeader,
        [] (uint64_t len, uint64_t total) {
        printf("%lld / %lld bytes => %d%% complete\n",
            len, total,
            (int)(len * 100 / total));
        return true; // return 'false' if you want to cancel the request.
        }
    );

    if (res && res->status == 200) {
        cb(res->body.c_str(), res->body.size());
        return 200;
    }
    else {
        return res == nullptr ? -1 : res->status;
    }
}
