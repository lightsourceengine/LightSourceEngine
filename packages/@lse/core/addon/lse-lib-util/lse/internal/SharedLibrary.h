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
