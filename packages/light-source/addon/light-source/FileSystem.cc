/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FileSystem.h"
#include <cstdio>
#include <sys/stat.h>

constexpr auto PATH_SEPARATOR =
#ifdef _WIN32
                            '\\';
#else
                            '/';
#endif

namespace ls {

using FileHandle = std::unique_ptr<FILE, decltype(&fclose)>;

std::vector<uint8_t> ReadBytes(const std::string filename) {
    std::vector<uint8_t> buffer;
    FileHandle file(fopen(filename.c_str(), "rb"), fclose);

    if (!file) {
        throw std::runtime_error("Failed to open file.");
    }

    fseek(file.get(), 0, SEEK_END);
    auto size{ static_cast<size_t>(ftell(file.get())) };
    fseek(file.get(), 0, SEEK_SET);

    buffer.reserve(size);

    if (fread(&buffer[0], 1, size, file.get()) != size) {
        throw std::runtime_error("Failed to read file.");
    }

    return buffer;
}


bool HasExtension(const std::string& filename) {
    auto sepIndex = filename.find_last_of("/\\");

    if (sepIndex == std::string::npos) {
        sepIndex = 0;
    }

    return filename.find('.', sepIndex + 1) != std::string::npos;
}

bool FileExists(const std::string& filename) {
  struct stat buffer;
  return (stat(filename.c_str(), &buffer) == 0);
}

std::string Join(const std::string& path, const std::string& filename) {
    return path + PATH_SEPARATOR + filename;
}

std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions) {
    if (HasExtension(filename)) {
        if (!FileExists(filename)) {
            throw std::runtime_error("file does not exist");
        }

        return filename;
    }

    std::string filenameWithExtension;

    for (auto& ext : extensions) {
        filenameWithExtension = filename + ext;

        if (FileExists(filenameWithExtension)) {
            return filenameWithExtension;
        }
    }

    throw std::runtime_error("no known file extension found");
}

std::string FindFile(const std::string& filename, const std::vector<std::string>& extensions,
        const std::vector<std::string>& resourcePaths) {
    for (auto& resourcePath : resourcePaths) {
        try {
            return FindFile(Join(resourcePath, filename), extensions);
        } catch (std::exception& e) {
            // continue
        }
    }

    throw std::runtime_error("no known file extension found");
}

} // namespace ls
