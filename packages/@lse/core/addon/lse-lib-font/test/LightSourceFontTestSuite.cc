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

#include <napi-unit.h>

using Napi::Env;
using Napi::HandleScope;
using Napi::Object;

namespace lse {
void FontSpec(Napi::TestSuite* parent);
void FontManagerSpec(Napi::TestSuite* parent);
}

Object Init(Env env, Object exports) {
  HandleScope scope(env);

  exports["test"] = Napi::TestSuite::Build(env, "lse-lib-font native tests", {
      &lse::FontSpec,
      &lse::FontManagerSpec,
  });

  return exports;
}

NODE_API_MODULE(LightSourceEngineFontLibTestSuite, Init);
