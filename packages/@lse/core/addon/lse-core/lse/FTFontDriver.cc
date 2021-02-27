/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FTFontDriver.h"

#include <memory>
#include <lse/Log.h>

namespace lse {

using FileContents = std::unique_ptr<uint8_t[], void(*)(const uint8_t*)>;

static void FileContentsDeleter(const uint8_t* data) noexcept;
static FileContents ReadFileContents(const char* filename, size_t* size) noexcept;
static FT_Face LoadFace(FT_Library lib, std::mutex& lock, FileContents contents, size_t size, int32_t index) noexcept;

FTFontDriver::FTFontDriver() {
  auto error = FT_Init_FreeType(&this->library);

  if (error) {
    LOG_ERROR("%s", FT_Error_String(error));
    this->library = nullptr;
  }
}

FTFontDriver::~FTFontDriver() {
  if (this->library) {
    FT_Done_FreeType(this->library);
    this->library = nullptr;
  }
}

FontSource* FTFontDriver::LoadFontSource(void* data, size_t dataSize, int32_t index) {
  if (!this->library) {
    return {};
  }

  FileContents contents(new (std::nothrow) uint8_t[dataSize], &FileContentsDeleter);

  if (contents) {
    memcpy(contents.get(), data, dataSize);
  } else {
    return {};
  }

  return LoadFace(this->library, this->lock, std::move(contents), dataSize, index);
}

FontSource* FTFontDriver::LoadFontSource(const char* file, int32_t index) {
  if (!this->library) {
    return {};
  }

  size_t size{};
  auto contents{ReadFileContents(file, &size)};

  if (!contents) {
    return {};
  }

  return LoadFace(this->library, this->lock, std::move(contents), size, index);
}

void FTFontDriver::DestroyFontSource(FontSource* fontSource) {
  if (fontSource) {
    std::lock_guard<std::mutex> guard(this->lock);
    FT_Done_Face(static_cast<FT_Face>(fontSource));
  }
}

static void FileContentsDeleter(const uint8_t* data) noexcept {
  delete [] data;
}

static FileContents ReadFileContents(const char* filename, size_t* size) noexcept {
  FileContents contents(nullptr, &FileContentsDeleter);
  std::shared_ptr<FILE> fp(fopen(filename, "rb"), &fclose);

  if (!fp) {
    return contents;
  }

  fseek(fp.get(), 0, SEEK_END);
  auto length = ftell(fp.get());
  fseek(fp.get(), 0, SEEK_SET);

  if (length > 0) {
    contents.reset(new(std::nothrow) uint8_t[length]);
  }

  if (!contents) {
    return contents;
  }

  if (fread(contents.get(), 1, length, fp.get()) != static_cast<size_t>(length)) {
    contents.reset();
  }

  *size = static_cast<size_t>(length);
  return contents;
}

static FT_Face LoadFace(FT_Library lib, std::mutex& lock, FileContents contents, size_t size, int32_t index) noexcept {
  FT_Face face{};
  FT_Error result;

  {
    std::lock_guard<std::mutex> guard(lock);
    result = FT_New_Memory_Face(lib, contents.get(), size, index, &face);
  }

  if (result == FT_Err_Ok) {
    face->stream->close = [](FT_Stream stream) noexcept {
      FileContentsDeleter(stream->base);
    };
    contents.release();
  } else {
    face = nullptr;
  }

  return face;
}

} // namespace lse
