/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include "SceneNode.h"

namespace ls {

class Style;

class RootSceneNode : public Napi::SafeObjectWrap<RootSceneNode>, public SceneNode {
 public:
    explicit RootSceneNode(const Napi::CallbackInfo& info);

    void OnPropertyChanged(StyleProperty property) override;
    void BeforeLayout() override {}
    void AfterLayout() override;
    void Paint(PaintContext* paint) override {}
    void Composite(CompositeContext* composite) override;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;

    friend Napi::SafeObjectWrap<RootSceneNode>;
};

} // namespace ls
