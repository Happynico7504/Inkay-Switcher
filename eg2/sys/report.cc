#include <coreinit/debug.h>

namespace SYS {
    namespace Report {
        void Log(const char* fmt) { OSReport("%s", fmt); }
        void Warn(const char* fmt) { OSReportWarn("%s", fmt); }
        void Fatal(const char *msg) { OSFatal(msg); }
    }
}