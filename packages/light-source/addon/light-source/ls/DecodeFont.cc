/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "DecodeFont.h"
#include <array>
#include <ls/Format.h>

using std17::filesystem::detail::equals_simple_insensitive;

// TODO: this list should be user configurable.
// List of font extensions to search when path contains the '.*' extension.
constexpr std::array<const char*, 4> kFontExtensions {{
    ".ttf",
    ".otf",
    ".ttc",
    ".otc",
}};

namespace ls {

BLFontFace DecodeFontFromFile(const std17::filesystem::path& path, uint32_t index) {
    // Handle the '.*' extension search recursively.
    if (equals_simple_insensitive(path.extension().c_str(), ".*")) {
        auto pathCopy{ path };

        for (auto ext : kFontExtensions) {
            try {
                return DecodeFontFromFile(pathCopy.replace_extension(ext), index);
            } catch (std::exception& e) {
                // continue search
            }
        }

        throw std::runtime_error(Format("No font file found for %s", path.c_str()));
    }

    // Read and parse font file.

    BLFontData fontData;
    BLResult result;

    result = fontData.createFromFile(path.c_str(), 0);

    if (result) {
        throw std::runtime_error(Format("Error reading font from disk (%u) %s", result, path.c_str()));
    }

    if (index >= fontData.faceCount()) {
        throw std::runtime_error(Format(
            "Error invalid index (%u), fonts (%u), font: %s", index, fontData.faceCount(), path.c_str()));
    }

    BLFontFace fontFace;

    result = fontFace.createFromData(fontData, index);

    if (result) {
        throw std::runtime_error(Format("Error parsing font (%u) %s", result, path.c_str()));
    }

    return fontFace;
}

} // namespace ls
