#pragma once

#include <string>
#include <vector>
#include <atomic>
#include <curl/curl.h>
#include "eg2/libs/typedefs.h"

namespace Network {
    bool VerifyConnection(void);

    bool Init(void);
    void Shutdown(void);

    bool GiveCertCA(const std::string& path);

    bool SetConnection(const std::string& url);
    bool Download(const std::string& url, std::vector<u8>& outData);
    void DownloadAsync(const std::string& url, std::vector<u8>* outData);

    bool IsDownloadFinished(void);
    bool WasDownloadSuccessful(void);

    bool SetUserAgent(const std::string& ua);
    bool SetHeader(const std::string& header);

    std::string GetLastResponse(void);
    std::string GetLastError(void);
    long GetNumericCode(void);
}