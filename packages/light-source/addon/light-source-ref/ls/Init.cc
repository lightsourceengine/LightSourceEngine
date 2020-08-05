/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/PlatformPlugin.h>
#include <ls/RefPlatformPluginImpl.h>
#include <napi.h>

using Napi::Env;
using Napi::Object;

Object Init(Env env, Object exports) {
    return ls::PlatformPluginInit<ls::RefPlatformPluginImpl>(env, exports, "light-source-ref");
}

NODE_API_MODULE(LightSourceRef, Init);
