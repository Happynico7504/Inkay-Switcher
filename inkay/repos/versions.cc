#include "eg2/external/sha1.hh"
#include "eg2/external/core.hh"
#include "eg2/heap/mem2.hh"
#include "inkay/repos/versions.hh"
#include <string>

namespace Inkay {
    namespace Versions {
        namespace WMS {
            std::string LocalSHA1;
            std::string JuxtSHA1; 
        }

        namespace WPS {
            std::string LocalSHA1;
            std::string JuxtSHA1; 
        }

        static std::string ActiveEnvironment = "aroma";
        LocalFileState FileState = LocalFileState::Missing;

        static std::string ReadDefaultCFG(void) {
            u32 size = 0;

            void* data = IO::ReadFile("fs:/vol/external01/wiiu/environments/default.cfg", FILEMODE_TXT, &size);

            if (!data || size == 0) return "";

            std::string env((char*)data, size);
            Heap::MEM2::Free(data);

            while (!env.empty() && (env.back() == '\n' || env.back() == '\r' || env.back() == ' ')) env.pop_back();

            return env;
        }

        static std::string ResolveEnvironment(void) {
            std::string cfg = ReadDefaultCFG();
            if (!cfg.empty()) return cfg;

            auto envs = IO::ListDirs("fs:/vol/external01/wiiu/environments");

            if (envs.empty()) return "aroma";

            if (envs.size() == 1) return envs[0];

            return "aroma";
        }

        bool VerifyJuxtInstall(void) {
            ActiveEnvironment = ResolveEnvironment();

            std::string base = "fs:/vol/external01/wiiu/environments/" + Inkay::Versions::ActiveEnvironment + "/";

            auto wms = IO::SHA1::ReadFile((base + "modules/Inkay-pretendo.wms").c_str());
            auto wps = IO::SHA1::ReadFile((base + "plugins/Inkay-pretendo.wps").c_str());

            if (!wms.exists || !wps.exists) {
                Inkay::Versions::FileState = Inkay::Versions::LocalFileState::Missing;
                return false;
            }

            bool isJuxt = (wms.sha1 == WMS::JuxtSHA1 && wps.sha1 == WPS::JuxtSHA1);

            if (isJuxt) {
                Inkay::Versions::FileState = Inkay::Versions::LocalFileState::ValidJuxt;
                return true;
            }

            Inkay::Versions::FileState = Inkay::Versions::LocalFileState::ValidUnknown;
            return false;
        }
    }
}