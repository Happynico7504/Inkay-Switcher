#pragma once

#include <string>

struct FileHashResult {
    bool exists;
    std::string sha1;
};

namespace IO {
    namespace SHA1 {
        FileHashResult ReadFile(const char* path);
    }
}