/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <cstdint>
#include <vector>

namespace lse {

using ByteArray = std::vector<uint8_t>;

ByteArray NewByteArray(size_t size) noexcept;
ByteArray NewByteArray(uint8_t* data, size_t size) noexcept;

ByteArray ReadFileContents(const char* filename) noexcept;

} // namespace lse
