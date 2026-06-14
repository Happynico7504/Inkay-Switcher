#pragma once

#include <string>
#include <vector>
#include "eg2/libs/typedefs.h"

typedef enum FileMode {
    FILEMODE_TXT,
    FILEMODE_BIN
} FileMode;

namespace IO {
    void* ReadFile(const char* path, FileMode mode, u32* outSize);
    bool WriteFile(const char* path, const void* data, usize size, FileMode mode);
    bool FileExists(const char* path);
    std::vector<std::string> ListDirs(const std::string& path);
}