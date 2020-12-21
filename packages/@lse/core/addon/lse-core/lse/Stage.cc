/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/Stage.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;

namespace lse {

void Stage::Update() {
    this->GetResources()->Compact();
}

void Stage::Destroy() {
    // TODO: cleanup resources
    this->threadPool.ShutdownNow();
}

} // namespace lse
