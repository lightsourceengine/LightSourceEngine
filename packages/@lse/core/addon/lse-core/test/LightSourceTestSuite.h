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

#include <napi-unit.h>

namespace lse {

void ThreadPoolSpec(Napi::TestSuite* parent);
void StyleSpec(Napi::TestSuite* parent);
void StyleContextSpec(Napi::TestSuite* parent);
void ImageSpec(Napi::TestSuite* parent);
void FTFontDriverSpec(Napi::TestSuite* parent);
void DecodeImageSpec(Napi::TestSuite* parent);
void ImageManagerSpec(Napi::TestSuite* parent);

inline
Napi::Value LightSourceTestSuite(Napi::Env env) {
  return Napi::TestSuite::Build(env, "lse-core native tests", {
      &ThreadPoolSpec,
      &StyleSpec,
      &StyleContextSpec,
      &ImageSpec,
      &FTFontDriverSpec,
      &DecodeImageSpec,
      &ImageManagerSpec,
  });
}

} // namespace lse
