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

#include <lse/System.h>
#include <cstdlib>
#include <cstring>

namespace lse {

bool HasEnv(const char* name) noexcept {
  return *GetEnv(name) != '\0';
}

const char* GetEnv(const char* name) noexcept {
  auto value = name ? std::getenv(name) : nullptr;

  if (value == nullptr || *value == '\0') {
    return "";
  }

  return value;
}

const char* GetEnvOrDefault(const char* name, const char* defaultValue) noexcept {
  return HasEnv(name) ? GetEnv(name) : defaultValue;
}

bool EnvEquals(const char* env, const char* expectedValue) noexcept {
  return strcmp(GetEnv(env), expectedValue) == 0;
}

} // namespace lse
