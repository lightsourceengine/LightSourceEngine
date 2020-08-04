/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <blend2d.h>
#include <std17/filesystem>

namespace ls {

/**
 * Reads a ttf or otf font file into memory as a BLFontFace.
 *
 * If the font is a collection, this method can load only one font face (by index).
 *
 * If path contains a '.*' extension, the method will search the file system for files that match well known,
 * supported font extensions, such as .otf or .ttf.
 *
 * @param path The filesystem path to the font.
 * @param index The index of the font face to load. Should be 0 for non-collection fonts.
 * @return font face
 * @throws std::exception for file not found or other parsing errors
 */
BLFontFace DecodeFontFromFile(const std17::filesystem::path& path, uint32_t index);

} // namespace ls
