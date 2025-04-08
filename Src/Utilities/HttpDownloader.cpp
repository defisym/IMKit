#include "HttpDownloader.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <3rdLib/CppHttpLib/httplib.h>

#pragma comment(lib,"libcrypto.lib")
#pragma comment(lib,"libssl.lib")

HttpDownloader::HttpDownloader(const HttpParams& hParams)
    :httpParams(hParams) {
    pClient = new httplib::Client{ hParams.bHttps
        ? std::format("https://{}", hParams.site)
        : std::format("http://{}", hParams.site) };
    pHeader = new httplib::Headers{ {"User-Agent", hParams.userAgent } };
}

HttpDownloader::HttpDownloader(const ProxyParams& pParams,
    const HttpParams& hParams)
    :bProxy(true), proxyParams(pParams),
    httpParams(hParams) {
    pClient = new httplib::Client{ pParams.localHost,pParams.port };
    pHeader = new httplib::Headers{ 
        { "Host", hParams.site },
        {"User-Agent", hParams.userAgent }
    };
}

HttpDownloader::~HttpDownloader() {
    delete (httplib::Client*)pClient;
    delete (httplib::Headers*)pHeader;
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
