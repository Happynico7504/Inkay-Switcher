#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <curl/curl.h>
#include <nn/ac.h>
#include <nsysnet/nssl.h>
#include "eg2/libs/typedefs.h"
#include "eg2/network/socket.hh"
#include "eg2/external/core.hh"
#include "eg2/heap/mem2.hh"
#include "eg2/sys/report.hh"

namespace Network {
    static NSSLContextHandle gNsslCtx = -1;
    static std::string gResponse;
    static std::string gUserAgent;
    static std::string gLastCurlError;
    static bool gIsInitialized = false;
    static long gHttpCode = 0;
    static struct curl_slist* gHeaders = nullptr;

    static std::string gCACertData;
    static bool gHasCACert = false;

    static std::atomic<bool> gDownloadFinished(false);
    static std::atomic<bool> gDownloadSuccess(false);

    static int initSocket(void *ptr, curl_socket_t socket, curlsocktype type) {
        (void)ptr;
        (void)type;
        int o = 1;

        setsockopt(socket, SOL_SOCKET, SO_WINSCALE, &o, sizeof(o));

        o = 128 * 1024;
        setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &o, sizeof(o));

        return CURL_SOCKOPT_OK;
    }

    static usize WriteCallback(char* data, usize size, usize nmemb, void* userdata) {
        usize total = size * nmemb;
        if (!userdata) return 0;

        auto* out = static_cast<std::string*>(userdata);
        out->append(data, total);
        return total;
    }

    static usize WriteFileCallback(char* data, usize size, usize nmemb, void* userdata) {
        usize total = size * nmemb;
        if (!userdata) return 0;

        auto* out = static_cast<std::vector<u8>*>(userdata);
        out->insert(out->end(), data, data + total);
        return total;
    }

    // Tiny stub for GitHub to work, fuck you microslop
    static curl_slist* BuildDefaultHeaders(void) {
        curl_slist* list = nullptr;

        list = curl_slist_append(list, "Accept: application/vnd.github+json");
        list = curl_slist_append(list, "X-GitHub-Api-Version: 2022-11-28");

        return list;
    }

    static void ApplyCommonCurlOptions(CURL* curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

        if (!gUserAgent.empty()) curl_easy_setopt(curl, CURLOPT_USERAGENT, gUserAgent.c_str());
        else curl_easy_setopt(curl, CURLOPT_USERAGENT, "Inkay-Updater/1.0");

        curl_slist* headers = gHeaders ? gHeaders : BuildDefaultHeaders();
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        if (gHasCACert && !gCACertData.empty()) {
            static curl_blob blob{};
            blob.data  = (void*)gCACertData.data();
            blob.len   = gCACertData.size();
            blob.flags = CURL_BLOB_COPY;

            curl_easy_setopt(curl, CURLOPT_CAINFO_BLOB, &blob);
        }
    }

    bool VerifyConnection(void) {
        if (!gIsInitialized) return false;

        bool connected = false;
        nn::ac::IsApplicationConnected(&connected);
        return connected;
    }

    bool Init(void) {
        if (gIsInitialized) return true;

        if (nn::ac::Initialize().IsFailure()) return false;
        if (nn::ac::Connect().IsFailure()) return false;

        Socket::Init();

        if (NSSLInit() < 0) return false;

        gNsslCtx = NSSLCreateContext(1);
        if (gNsslCtx < 0) return false;

        if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
            return false;

        gIsInitialized = true;
        return true;
    }

    void Shutdown(void) {
        if (gHeaders) {
            curl_slist_free_all(gHeaders);
            gHeaders = nullptr;
        }

        if (gNsslCtx >= 0) {
            NSSLDestroyContext(gNsslCtx);
            gNsslCtx = -1;
        }

        curl_global_cleanup();

        NSSLFinish();
        Socket::Finish();
        nn::ac::Finalize();

        gIsInitialized = false;
    }

    bool GiveCertCA(const std::string& path) {
        void* data = nullptr;
        u32 size = 0;

        data = IO::ReadFile(path.c_str(), FILEMODE_BIN, &size);
        if (!data || size == 0) {
            SYS::Report::Log("Network::GiveCertCA(): failed to read %s\n", path.c_str());
            return false;
        }

        gCACertData.assign((char*)data, (char*)data + size);
        Heap::MEM2::Free(data);

        gHasCACert = true;

        SYS::Report::Log("Network::GiveCertCA(): CA loaded into memory\n");
        return true;
    }

    bool SetConnection(const std::string& url) {
        if (!gIsInitialized) return false;

        CURL* curl = curl_easy_init();
        if (!curl) return false;

        gResponse.clear();
        gLastCurlError.clear();
        gHttpCode = -1;

        ApplyCommonCurlOptions(curl);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &gResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_SOCKOPTFUNCTION, initSocket);

        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &gHttpCode);
        } else {
            gLastCurlError = curl_easy_strerror(res);
        }

        curl_easy_cleanup(curl);
        return res == CURLE_OK;
    }

    bool Download(const std::string& url, std::vector<u8>& outData) {
        if (!gIsInitialized) return false;

        CURL* curl = curl_easy_init();
        if (!curl) return false;

        outData.clear();

        ApplyCommonCurlOptions(curl);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outData);
        curl_easy_setopt(curl, CURLOPT_SOCKOPTFUNCTION, initSocket);

        CURLcode res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        return res == CURLE_OK;
    }

    void DownloadAsync(const std::string& url, std::vector<u8>* outData) {
        gDownloadFinished = false;
        gDownloadSuccess = false;

        std::thread([url, outData]() {
            gDownloadSuccess = Download(url, *outData);
            gDownloadFinished = true;
        }).detach();
    }

    bool IsDownloadFinished(void) { return gDownloadFinished.load(); }
    bool WasDownloadSuccessful(void) { return gDownloadSuccess.load(); }

    bool SetHeader(const std::string& header) {
        if (!gIsInitialized) return false;
        gHeaders = curl_slist_append(gHeaders, header.c_str());
        return true;
    }

    bool SetUserAgent(const std::string& ua) {
        if (!gIsInitialized) return false;
        gUserAgent = ua;
        return true;
    }

    std::string GetLastResponse(void) { return gResponse; }
    std::string GetLastError(void) { return gLastCurlError; }
    long GetNumericCode(void) { return gHttpCode; }
}