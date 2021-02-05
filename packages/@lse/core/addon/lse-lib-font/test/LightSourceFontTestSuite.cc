/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
