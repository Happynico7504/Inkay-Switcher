#include "inkay/repos/verify.hh"
#include "eg2/external/core.hh"
#include "eg2/sys/report.hh"
#include "eg2/heap/mem2.hh"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Inkay {
    namespace Verify {
        std::string ComputeFileHash(const std::string& path) {
            void* data = nullptr;
            u32 size = 0;

            data = IO::ReadFile(path.c_str(), FILEMODE_BIN, &size);
            if (!data) {
                return "";
            }

            sha256::SHA256 hasher;
            hasher.update(data, size);
            sha256::SHA256::digest_t digest;
            hasher.finalize(digest);

            Heap::MEM2::Free(data);

            return sha256::hex_string(digest, 32);
        }

        bool AtomicWrite(
            const std::string& finalPath,
            const void* data,
            usize size
        ) {
            std::string tempPath = finalPath + ".tmp";

            // Write to temp file
            if (!IO::WriteFile(tempPath.c_str(), data, size, FILEMODE_BIN)) {
                SYS::Report::Log("Verify::AtomicWrite(): Failed to write temp file %s\n", tempPath.c_str());
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            // Verify temp file was created and has correct size
            void* verifyData = nullptr;
            u32 verifySize = 0;

            verifyData = IO::ReadFile(tempPath.c_str(), FILEMODE_BIN, &verifySize);
            if (!verifyData) {
                SYS::Report::Log("Verify::AtomicWrite(): Failed to read back temp file %s\n", tempPath.c_str());
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            bool sizeMatch = (verifySize == size);
            if (verifyData) Heap::MEM2::Free(verifyData);

            if (!sizeMatch) {
                SYS::Report::Log("Verify::AtomicWrite(): Size mismatch for %s (expected %u, got %u)\n",
                                tempPath.c_str(), (unsigned)size, verifySize);
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            // Atomic rename (overwrites existing file)
            if (rename(tempPath.c_str(), finalPath.c_str()) != 0) {
                SYS::Report::Log("Verify::AtomicWrite(): Failed to rename %s to %s\n", tempPath.c_str(), finalPath.c_str());
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            SYS::Report::Log("Verify::AtomicWrite(): Successfully wrote %s (%u bytes)\n", finalPath.c_str(), (unsigned)size);
            return true;
        }

        void CleanupTempFiles(const std::string& basePath) {
            std::string tempPath = basePath + ".tmp";
            IO::RemoveFile(tempPath.c_str());
        }
    }
}
