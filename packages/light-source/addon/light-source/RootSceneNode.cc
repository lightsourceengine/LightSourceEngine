/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RootSceneNode.h"
#include "Scene.h"
#include "StyleUtils.h"
#include "Style.h"
#include <ls/Renderer.h>

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

        auto func{ DefineClass(env, "RootSceneNode", SceneNode::Extend<RootSceneNode>(env, {})) };

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void RootSceneNode::OnPropertyChanged(StyleProperty property) {
    if (property == StyleProperty::fontSize) {
        const auto& fontSize{ this->style->fontSize };
        float computedFontSize;

        switch (fontSize.unit) {
            case StyleNumberUnitPoint:
                computedFontSize = fontSize.value;
                break;
            case StyleNumberUnitViewportWidth:
                computedFontSize = fontSize.AsPercent() * this->scene->GetWidth();
                break;
            case StyleNumberUnitViewportHeight:
                computedFontSize = fontSize.AsPercent() * this->scene->GetHeight();
                break;
            case StyleNumberUnitViewportMin:
                computedFontSize = fontSize.AsPercent() * this->scene->GetViewportMax();
                break;
            case StyleNumberUnitViewportMax:
                computedFontSize = fontSize.AsPercent() * this->scene->GetViewportMin();
                break;
            case StyleNumberUnitRootEm:
                computedFontSize = fontSize.value * DEFAULT_ROOT_FONT_SIZE;
                break;
            default:
                computedFontSize = 0;
                break;
        }

        this->scene->NotifyRootFontSizeChanged(computedFontSize);
    }
}

void RootSceneNode::ComputeStyle() {
    SceneNode::ComputeStyle();
}

void RootSceneNode::Paint(Renderer* renderer) {
    SceneNode::Paint(renderer);
}

void RootSceneNode::Composite(CompositeContext* context, Renderer* renderer) {
    const auto& backgroundColor{ this->GetStyleOrEmpty()->backgroundColor };

    if (!backgroundColor.undefined) {
        renderer->FillRenderTarget(backgroundColor.value);
    }

    SceneNode::Composite(context, renderer);
}

} // namespace ls
