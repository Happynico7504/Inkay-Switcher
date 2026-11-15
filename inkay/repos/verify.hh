#pragma once

#include <string>
#include <vector>
#include "eg2/libs/typedefs.h"
#include "eg2/libs/tinysha256.hh"

namespace Inkay {
    namespace Verify {
        enum class VerifyError {
            OK,
            FILE_NOT_FOUND,
            HASH_MISMATCH,
            SIZE_MISMATCH,
            READ_ERROR,
            WRITE_ERROR,
            UNKNOWN
        };

        struct VerifyResult {
            VerifyError error{VerifyError::OK};
            std::string errorMsg;
            std::string computedHash;
            u32 fileSize{0};
        };

        // Compute SHA256 hash of data
        static std::string ComputeHash(const std::vector<u8>& data) {
            sha256::SHA256 hasher;
            hasher.update(data.data(), data.size());
            sha256::SHA256::digest_t digest;
            hasher.finalize(digest);
            return sha256::hex_string(digest, 32);
        }

        // Compute SHA256 hash from file
        std::string ComputeFileHash(const std::string& path);

        // Verify downloaded data against expected hash and size
        inline static VerifyResult VerifyDownload(
            const std::vector<u8>& data,
            const std::string& expectedHash,
            u32 expectedSize = 0
        ) {
            VerifyResult result;

            if (expectedSize > 0 && data.size() != expectedSize) {
                result.error = VerifyError::SIZE_MISMATCH;
                result.errorMsg = "Size mismatch: expected " + std::to_string(expectedSize) +
                                  " bytes, got " + std::to_string(data.size()) + " bytes";
                result.fileSize = data.size();
                return result;
            }

            if (!expectedHash.empty()) {
                std::string computed = ComputeHash(data);
                result.computedHash = computed;

                if (computed != expectedHash) {
                    result.error = VerifyError::HASH_MISMATCH;
                    result.errorMsg = "Hash mismatch: expected " + expectedHash +
                                      ", got " + computed;
                    result.fileSize = data.size();
                    return result;
                }
            }

            result.fileSize = data.size();
            return result;
        }

        // Atomic write: write to temp file, verify, then rename
        bool AtomicWrite(
            const std::string& finalPath,
            const void* data,
            usize size
        );

        // Clean up temp files (in case of crash/error)
        void CleanupTempFiles(const std::string& basePath);
    }
}
