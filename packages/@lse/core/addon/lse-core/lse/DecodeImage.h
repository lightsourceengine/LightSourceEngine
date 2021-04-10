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

#pragma once

#include <std17/filesystem>
#include <lse/ImageBytes.h>

namespace lse {

/**
 * Reads an image file into memory as a 32-bit image.
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

/**
 * Reads an image data URI into memory as a 32-bit image.
 *
 * Data URI types supported:
 * - data:image/svg+xml;utf8,
 * - data:image/svg+xml;base64,
 * - data:image/image;base64,
 *
 * @param path The filesystem path to the image.
 * @param resizeWidth If greater than 0, force the image width to be this value. Only used by SVG files.
 * @param resizeHeight If greater than 0, force the image height to be this value. Only used by SVG files.
 * @return image bytes
 * @throws std::exception for file not found or other parsing errors
 */
ImageBytes DecodeImageFromDataUri(const std::string& uri, int32_t resizeWidth, int32_t resizeHeight);

} // namespace lse
