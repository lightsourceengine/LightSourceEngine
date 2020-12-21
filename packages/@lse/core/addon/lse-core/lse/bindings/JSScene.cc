/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/GraphicsContext.h>
#include <lse/RootSceneNode.h>
#include <lse/Scene.h>
#include <lse/bindings/JSScene.h>
#include <lse/bindings/JSStage.h>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;

static FunctionReference jsSceneConstructor;

namespace lse {
namespace bindings {

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
            InstanceMethod("$setup", &JSScene::Setup),
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

void JSScene::Frame(const Napi::CallbackInfo& info) {
    NAPI_TRY(info.Env(), this->native->Frame());
}

void JSScene::Setup(const Napi::CallbackInfo& info) {
    StageRef stage;
    auto classInstance{ JSStage::Cast(info[0]) };

    if (classInstance) {
        stage = classInstance->GetNative();
    }

    NAPI_EXPECT_NOT_NULL(info.Env(), stage, "stage arg must be a Stage instance");
    NAPI_TRY(info.Env(), this->native->SetStage(stage));

    auto root{ RootSceneNode::Cast(info[1]) };

    NAPI_EXPECT_NOT_NULL(info.Env(), root, "root arg must be a RootSceneNode instance");
    NAPI_TRY(info.Env(), this->native->SetRoot(root));

    auto graphicsContext{ GraphicsContext::Cast(info[2]) };

    NAPI_EXPECT_NOT_NULL(
        info.Env(), graphicsContext, "graphicsContext arg must be a GraphicsContext instance");
    NAPI_TRY(info.Env(), this->native->SetGraphicsContext(graphicsContext));
}

void JSScene::Destroy(const CallbackInfo& info) {
    this->native->Destroy();
}

} // namespace bindings
} // namespace lse
