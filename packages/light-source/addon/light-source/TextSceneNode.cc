/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextSceneNode.h"

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;

namespace ls {

TextSceneNode::TextSceneNode(const CallbackInfo& info) : ObjectWrap<TextSceneNode>(info), SceneNode(info) {
    this->isLeaf = true;
}

Function TextSceneNode::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "TextSceneNode", {
            InstanceAccessor("x", &SceneNode::GetX, nullptr),
            InstanceAccessor("y", &SceneNode::GetY, nullptr),
            InstanceAccessor("width", &SceneNode::GetWidth, nullptr),
            InstanceAccessor("height", &SceneNode::GetHeight, nullptr),

            InstanceAccessor("parent", &SceneNode::GetParent, nullptr),
            InstanceAccessor("style", &SceneNode::GetStyle, &SceneNode::SetStyle),

            InstanceMethod("appendChild", &SceneNode::AppendChild),
            InstanceMethod("removeChild", &SceneNode::RemoveChild),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

} // namespace ls
