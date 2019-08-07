/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RootSceneNode.h"
#include "Scene.h"
#include "StyleUtils.h"
#include <fmt/format.h>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

constexpr auto DEFAULT_ROOT_FONT_SIZE{ 16 };

RootSceneNode::RootSceneNode(const CallbackInfo& info) : ObjectWrap<RootSceneNode>(info), SceneNode(info) {
}

Function RootSceneNode::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func{ DefineClass(env, "RootSceneNode", SceneNode::Extend<RootSceneNode>({})) };

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void RootSceneNode::ApplyStyle(Style* style) {
    SceneNode::ApplyStyle(style);

    auto rootStyleFontSize{ this->GetStyleOrEmpty()->fontSize() };
    auto computedFontSize{ 0 };

    if (rootStyleFontSize) {
        switch (rootStyleFontSize->GetUnit()) {
            case StyleNumberUnitPoint:
                computedFontSize = rootStyleFontSize->GetValue();
                break;
            case StyleNumberUnitViewportWidth:
                computedFontSize = rootStyleFontSize->GetValuePercent() * this->scene->GetWidth();
                break;
            case StyleNumberUnitViewportHeight:
                computedFontSize = rootStyleFontSize->GetValuePercent() * this->scene->GetHeight();
                break;
            case StyleNumberUnitViewportMin:
                computedFontSize = rootStyleFontSize->GetValuePercent() * this->scene->GetViewportMax();
                break;
            case StyleNumberUnitViewportMax:
                computedFontSize = rootStyleFontSize->GetValuePercent() * this->scene->GetViewportMin();
                break;
            case StyleNumberUnitRootEm:
                computedFontSize = rootStyleFontSize->GetValue() * DEFAULT_ROOT_FONT_SIZE;
                break;
            default:
                break;
        }
    }

    this->scene->NotifyRootFontSizeChanged(computedFontSize);
}

} // namespace ls
