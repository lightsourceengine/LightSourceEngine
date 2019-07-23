/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include "Style.h"
#include "StyleEnumMappings.h"
#include "ResourceManager.h"

using Napi::Env;
using Napi::Function;
using Napi::Object;
using lse::Style;
using lse::ResourceManager;

void ExportClass(Object* exports, const Function& constructor) {
    exports->Set(constructor.Get("name").ToString(), constructor);
}

Object Init(Env env, Object exports) {
    ExportClass(&exports, Style::Constructor(env));
    ExportClass(&exports, ResourceManager::Constructor(env));

    lse::StyleEnumMappings::Init(env, exports);

    return exports;
}

NODE_API_MODULE(LightSource, Init);
