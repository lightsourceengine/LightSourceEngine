/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once
#include <cstdint>

namespace lse {
namespace internal {

class SharedLibrary {
 public:
  void Open(const char* libraryName);
  void* GetSymbol(const char* name, bool required = true) const;
  bool IsOpen() const noexcept;
  void Close() noexcept;
  int32_t Ref() noexcept;

 private:
  int32_t refs{0};
  void* handle{};
};

} // namespace internal
} // namespace lse
