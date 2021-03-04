/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "fs-ext.h"

#include <memory>
#include <cstdio>
#include <cstring>

namespace lse {

static void FileClose(FILE* fp) noexcept;

ByteArray NewByteArray(std::size_t size) noexcept {
  ByteArray contents;

  try {
    contents.reserve(size);
  } catch (const std::bad_alloc&) {
    return {};
  }

  contents.resize(size);

  return contents;
}

ByteArray NewByteArray(uint8_t* data, std::size_t size) noexcept {
  auto result{NewByteArray(size)};

  if (!result.empty()) {
    memcpy(result.data(), data, size);
  }

  return result;
}

ByteArray ReadFileContents(const char* filename) noexcept {
  if (!filename) {
    return {};
  }

  std::unique_ptr<FILE, decltype(&FileClose)> fp(fopen(filename, "rb"), &FileClose);

  if (!fp) {
    return {};
  }

  fseek(fp.get(), 0, SEEK_END);
  auto length = ftell(fp.get());
  fseek(fp.get(), 0, SEEK_SET);

  if (length <= 0) {
    return {};
  }

  auto size{static_cast<std::size_t>(length)};
  ByteArray contents{NewByteArray(size)};

  if (contents.empty() || fread(contents.data(), 1, length, fp.get()) != size) {
    return {};
  }

  return contents;
}

static void FileClose(FILE* fp) noexcept {
  if (fp) {
    fclose(fp);
  }
}

} // namespace lse
