#pragma once

#include <coreinit/debug.h>

namespace SYS {
    namespace Report {
        void Log(const char* fmt);
        void Warn(const char* fmt);
        void Fatal(const char *msg);

        template<typename... Args>
        inline void Log(const char* fmt, Args... args) { OSReport(fmt, args...); }
        template<typename... Args>
        inline void Warn(const char* fmt, Args... args) { OSReportWarn(fmt, args...); }
    }
}