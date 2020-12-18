/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <std17/filesystem>
#include <ls/ImageBytes.h>

namespace ls {

/**
 * Reads an image file into memory as a BLImage.
 *
 * If path contains a '.*' extension, the method will search the file system for files that match well known,
 * supported image extensions, such as .png or .jpg.
 *
 * @param path The filesystem path to the image.
 * @param resizeWidth If greater than 0, force the image width to be this value. Only used by SVG files.
 * @param resizeHeight If greater than 0, force the image height to be this value. Only used by SVG files.
 * @return image bytes
 * @throws std::exception for file not found or other parsing errors
 */
ImageBytes DecodeImageFromFile(const std17::filesystem::path& path, int32_t resizeWidth, int32_t resizeHeight);

} // namespace ls
