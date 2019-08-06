/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "SceneNode.h"

namespace ls {

class RootSceneNode : public Napi::ObjectWrap<RootSceneNode>, public SceneNode {
 public:
    explicit RootSceneNode(const Napi::CallbackInfo& info);
    virtual ~RootSceneNode() = default;

    static Napi::Function Constructor(Napi::Env env);

    Napi::Reference<Napi::Object>* AsReference() override { return this; }

 private:
    void ApplyStyle(Style* style) override;
};

} // namespace ls
