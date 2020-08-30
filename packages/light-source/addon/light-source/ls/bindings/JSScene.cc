/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/GraphicsContext.h>
#include <ls/RootSceneNode.h>
#include <ls/Scene.h>
#include <ls/bindings/Bindings.h>
#include <ls/bindings/JSScene.h>
#include <ls/bindings/JSStage.h>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;

static FunctionReference jsSceneConstructor;

namespace ls {
namespace bindings {

JSScene::JSScene(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<JSScene>(info) {
}

void JSScene::Constructor(const CallbackInfo& info) {
    this->native = std::make_shared<Scene>();
}

Function JSScene::GetClass(Napi::Env env) {
    if (jsSceneConstructor.IsEmpty()) {
        HandleScope scope(env);

        jsSceneConstructor = DefineClass(env, "SceneBase", true, {
            InstanceMethod("$attach", &JSScene::Attach),
            InstanceMethod("$detach", &JSScene::Detach),
            InstanceMethod("$destroy", &JSScene::Destroy),
            InstanceMethod("$frame", &JSScene::Frame),
            InstanceMethod("$setRoot", &JSScene::SetRoot),
            InstanceMethod("$setStage", &JSScene::SetStage),
            InstanceMethod("$setGraphicsContext", &JSScene::SetGraphicsContext),
        });
    }

    return jsSceneConstructor.Value();
}

SceneRef JSScene::GetNative() const noexcept {
    return this->native;
}

void JSScene::Attach(const Napi::CallbackInfo& info) {
    this->native->Attach();
}

void JSScene::Detach(const Napi::CallbackInfo& info) {
    this->native->Detach();
}

void JSScene::Frame(const Napi::CallbackInfo& info) { NAPI_TRY(info.Env(), this->native->Frame());
}

Napi::Value JSScene::SetRoot(const Napi::CallbackInfo& info) {
    auto root{ RootSceneNode::Cast(info[0]) };

    NAPI_EXPECT_NOT_NULL(info.Env(), root, "root arg must be a RootSceneNode instance");
    NAPI_TRY(info.Env(), this->native->SetRoot(root));

    return info[0];
}

Napi::Value JSScene::SetStage(const Napi::CallbackInfo& info) {
    auto stage{ ToNative<Stage, JSStage>(info[0]) };

    NAPI_EXPECT_NOT_NULL(info.Env(), stage, "stage arg must be a Stage instance");
    NAPI_TRY(info.Env(), this->native->SetStage(stage));

    return info[0];
}

Napi::Value JSScene::SetGraphicsContext(const Napi::CallbackInfo& info) {
    auto graphicsContext{ GraphicsContext::Cast(info[0]) };

    NAPI_EXPECT_NOT_NULL(
        info.Env(), graphicsContext, "graphicsContext arg must be a GraphicsContext instance");
    NAPI_TRY(info.Env(), this->native->SetGraphicsContext(graphicsContext));

    return info[0];
}

void JSScene::Destroy(const CallbackInfo& info) {
    this->native->Destroy();
}

} // namespace bindings
} // namespace ls
