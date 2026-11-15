#include "inkay/repos/verify.hh"
#include "eg2/external/core.hh"
#include "eg2/sys/report.hh"
#include "eg2/heap/mem2.hh"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>

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

        static void CreateDirectories(const std::string& path) {
            std::string current;
            for (char c : path) {
                current += c;
                if (c == '/') mkdir(current.c_str(), 0777);
            }
        }

        bool AtomicWrite(
            const std::string& finalPath,
            const void* data,
            usize size
        ) {
            // Create parent directories
            usize lastSlash = finalPath.find_last_of('/');
            if (lastSlash != std::string::npos) {
                std::string folderPath = finalPath.substr(0, lastSlash + 1);
                CreateDirectories(folderPath);
            }

            std::string tempPath = finalPath + ".tmp";

            // Write to temp file
            if (!IO::WriteFile(tempPath.c_str(), data, size, FILEMODE_BIN)) {
                SYS::Report::Log("Verify::AtomicWrite(): Failed to write temp file %s\n", tempPath.c_str());
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            // Strict verification: read back and verify exact match
            void* verifyData = nullptr;
            u32 verifySize = 0;

            verifyData = IO::ReadFile(tempPath.c_str(), FILEMODE_BIN, &verifySize);
            if (!verifyData) {
                SYS::Report::Log("Verify::AtomicWrite(): Failed to read back temp file %s for verification\n", tempPath.c_str());
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            // Verify exact size match
            if (verifySize != size) {
                SYS::Report::Log("Verify::AtomicWrite(): Size mismatch verification failed for %s (expected %u bytes, got %u bytes)\n",
                                tempPath.c_str(), (unsigned)size, verifySize);
                Heap::MEM2::Free(verifyData);
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            // Verify content matches byte-for-byte
            if (memcmp(verifyData, data, size) != 0) {
                SYS::Report::Log("Verify::AtomicWrite(): Content mismatch verification failed for %s\n", tempPath.c_str());
                Heap::MEM2::Free(verifyData);
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            Heap::MEM2::Free(verifyData);

            // Atomic rename (overwrites existing file)
            if (rename(tempPath.c_str(), finalPath.c_str()) != 0) {
                SYS::Report::Log("Verify::AtomicWrite(): Failed to rename %s to %s\n", tempPath.c_str(), finalPath.c_str());
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            SYS::Report::Log("Verify::AtomicWrite(): Successfully wrote and verified %s (%u bytes)\n", finalPath.c_str(), (unsigned)size);
            return true;
        }

        void CleanupTempFiles(const std::string& basePath) {
            std::string tempPath = basePath + ".tmp";
            IO::RemoveFile(tempPath.c_str());
        }
    }
}
