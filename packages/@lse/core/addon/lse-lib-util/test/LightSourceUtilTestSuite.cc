/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>

using Napi::Env;
using Napi::HandleScope;
using Napi::Object;

namespace lse {
void FormatSpec(Napi::TestSuite* parent);
void StringExtSpec(Napi::TestSuite* parent);
void UriSpec(Napi::TestSuite* parent);
}

Object Init(Env env, Object exports) {
  HandleScope scope(env);

  exports["test"] = Napi::TestSuite::Build(env, "lse-lib-util native tests", {
      &lse::FormatSpec,
      &lse::StringExtSpec,
      &lse::UriSpec,
  });

  return exports;
}

NODE_API_MODULE(LightSourceEngineUtilLibTestSuite, Init);
