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
 * Reads an image file into memory as a BLImage.
 *
 * If path contains a '.*' extension, the method will search the file system for files that match well known,
 * supported image extensions, such as .png or .jpg.
 *
 * @param path The filesystem path to the image.
 * @param resize The dimensions to scale the image to. Only used by SVG files.
 * @return image bytes
 * @throws std::exception for file not found or other parsing errors
 */
BLImage DecodeImageFromFile(const std17::filesystem::path& path, const BLSizeI& resize);

} // namespace ls
