#include "inkay/repos/verify.hh"
#include "eg2/external/core.hh"
#include "eg2/sys/report.hh"
#include "eg2/heap/mem2.hh"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Inkay {
    namespace Verify {
        static const char* TEMP_DIR = "fs:/vol/external01/wiiu/revivetendo_installer/tmp/";

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

        static std::string GetTempFilePath(const std::string& finalPath) {
            // Extract just the filename from final path
            usize lastSlash = finalPath.find_last_of('/');
            std::string filename = (lastSlash != std::string::npos) ? finalPath.substr(lastSlash + 1) : finalPath;
            return std::string(TEMP_DIR) + filename + ".tmp";
        }

        bool AtomicWrite(
            const std::string& finalPath,
            const void* data,
            usize size
        ) {
            SYS::Report::Log("Verify::AtomicWrite(): Starting write for %s (%u bytes)\n", finalPath.c_str(), (unsigned)size);

            std::string tempPath = GetTempFilePath(finalPath);
            SYS::Report::Log("Verify::AtomicWrite(): Using temp file %s\n", tempPath.c_str());

            // Clean up any stale temp file from previous crash
            if (IO::FileExists(tempPath.c_str())) {
                SYS::Report::Log("Verify::AtomicWrite(): Cleaning up stale temp file\n");
                IO::RemoveFile(tempPath.c_str());
            }

            SYS::Report::Log("Verify::AtomicWrite(): Writing to temp file\n");

            // Write to temp file - IO::WriteFile handles directory creation
            if (!IO::WriteFile(tempPath.c_str(), data, size, FILEMODE_BIN)) {
                SYS::Report::Log("Verify::AtomicWrite(): Failed to write temp file\n");
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            SYS::Report::Log("Verify::AtomicWrite(): Temp file written, verifying\n");

            // Strict verification: read back and verify exact match
            void* verifyData = nullptr;
            u32 verifySize = 0;

            verifyData = IO::ReadFile(tempPath.c_str(), FILEMODE_BIN, &verifySize);
            if (!verifyData) {
                SYS::Report::Log("Verify::AtomicWrite(): Failed to read back temp file for verification\n");
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            // Verify exact size match
            if (verifySize != size) {
                SYS::Report::Log("Verify::AtomicWrite(): Size mismatch (expected %u, got %u)\n", (unsigned)size, verifySize);
                Heap::MEM2::Free(verifyData);
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            // Verify content matches byte-for-byte
            if (memcmp(verifyData, data, size) != 0) {
                SYS::Report::Log("Verify::AtomicWrite(): Content verification failed\n");
                Heap::MEM2::Free(verifyData);
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            Heap::MEM2::Free(verifyData);

            // Write verified data to final location using the same proven path
            if (!IO::WriteFile(finalPath.c_str(), data, size, FILEMODE_BIN)) {
                SYS::Report::Log("Verify::AtomicWrite(): Failed to write final file\n");
                IO::RemoveFile(tempPath.c_str());
                return false;
            }

            // Clean up temp file
            IO::RemoveFile(tempPath.c_str());

            SYS::Report::Log("Verify::AtomicWrite(): Successfully wrote and verified %s (%u bytes)\n", finalPath.c_str(), (unsigned)size);
            return true;
        }

        void CleanupTempFiles(const std::string& basePath) {
            std::string tempPath = GetTempFilePath(basePath);
            IO::RemoveFile(tempPath.c_str());
        }
    }
}
