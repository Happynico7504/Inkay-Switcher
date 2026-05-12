#include <string>
#include <cstring>
#include <sysapp/switch.h>
#include "eg2/libs/typedefs.h"

namespace SYS {
    namespace Apps {
        static void FillURL(char*& dst, u32& size, const char* src) {
            if (!src) {
                dst = nullptr;
                size = 0;
                return;
            }

            dst = const_cast<char*>(src);
            size = strlen(src);
        }

        std::string BrowserEncodeURL(const std::string& url) {
            std::string out;
            out.reserve(url.size());

            for (char c : url) {
                switch (c) {
                    case ' ': out += "%20"; break;
                    case '?': out += "%3F"; break;
                    case '&': out += "%26"; break;
                    case '#': out += "%23"; break;
                    case '+': out += "%2B"; break;
                    case '=': out += "%3D"; break;
                    case ':': out += "%3A"; break;
                    default: out += c; break;
                }
            }

            return out;
        }

        bool IsValidURL(const char* url) { return url && *url; }

        s32 BrowserOpen(const char* url) {
            if (!IsValidURL(url)) return -1;

            SysAppBrowserArgs args{};

            FillURL(args.url, args.urlSize, url);

            return SYSSwitchToBrowser(&args);
        }

        s32 BrowserOpenViewer(const char* url) {
            if (!IsValidURL(url)) return -1;

            SysAppBrowserArgs args{};

            FillURL(args.url, args.urlSize, url);

            return SYSSwitchToBrowserForViewer(&args);
        }

        s32 BrowserOpenCallback(const char* url, const char* callbackUrl, bool disableHbm) {
            if (!IsValidURL(url) || !IsValidURL(callbackUrl)) return -1;

            SysAppBrowserArgsWithCallback args{};

            FillURL(args.browserArgs.url, args.browserArgs.urlSize, url);
            FillURL(args.cbUrl, args.cbUrlSize, callbackUrl);

            args.hbmDisable = disableHbm;

            return SYSSwitchToBrowserForCallbackURL(&args);
        }
    }
}