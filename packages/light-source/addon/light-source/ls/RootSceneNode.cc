/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/RootSceneNode.h>

#include <ls/Scene.h>
#include <ls/StyleUtils.h>
#include <ls/Style.h>
#include <ls/Renderer.h>
#include <ls/CompositeContext.h>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::SafeObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

void RootSceneNode::Constructor(const Napi::CallbackInfo& info) {
    this->SceneNodeConstructor(info, SceneNodeTypeRoot);
}

Function RootSceneNode::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "RootSceneNode", true, SceneNode::Extend<RootSceneNode>(env, {}));
    }

    return constructor.Value();
}

void RootSceneNode::OnPropertyChanged(StyleProperty property) {
    switch (property) {
        case StyleProperty::backgroundColor:
        case StyleProperty::opacity:
            this->QueueComposite();
            break;
        case StyleProperty::fontSize:
            this->scene->OnRootFontSizeChange(ComputeFontSize(
                this->style->fontSize, this->scene, DEFAULT_REM_FONT_SIZE));
            break;
        default:
            break;
    }
}

void RootSceneNode::Composite(CompositeContext* composite) {
    const auto& backgroundColor{ this->GetStyleOrEmpty()->backgroundColor };

    if (!backgroundColor.empty()) {
        composite->renderer->FillRenderTarget(backgroundColor.value);
        SceneNode::Composite(composite);
    }
}

} // namespace ls
