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
#include <ls/CompositeContext.h>

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
    switch (property) {
        case StyleProperty::backgroundColor:
            this->QueueComposite();
            break;
        case StyleProperty::fontSize:
            this->scene->QueueRootFontSizeChange(
                ComputeFontSize(this->style->fontSize, this->scene, DEFAULT_ROOT_FONT_SIZE));
            break;
        default:
            if (IsYogaLayoutProperty(property)) {
                this->QueueAfterLayout();
            }
            break;
    }

    SceneNode::OnPropertyChanged(property);
}

void RootSceneNode::AfterLayout() {
    if (YGNodeGetHasNewLayout(this->ygNode)) {
        YGNodeSetHasNewLayout(this->ygNode, false);
        this->QueueComposite();
    }
}

void RootSceneNode::Composite(CompositeContext* context) {
    const auto& backgroundColor{ this->GetStyleOrEmpty()->backgroundColor };

    if (!backgroundColor.empty()) {
        context->renderer->FillRenderTarget(backgroundColor.value);
    }

    SceneNode::Composite(context);
}

} // namespace ls
