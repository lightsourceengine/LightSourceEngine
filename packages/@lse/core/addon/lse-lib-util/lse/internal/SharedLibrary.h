/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

namespace lse {
namespace internal {

class SharedLibrary {
 public:
  void Open(const char* libraryName);
  void* GetSymbol(const char* name) const;
  bool IsOpen() const noexcept;
  void Close() noexcept;

 private:
  void* handle{};
};

} // namespace internal
} // namespace lse
