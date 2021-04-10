/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
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
