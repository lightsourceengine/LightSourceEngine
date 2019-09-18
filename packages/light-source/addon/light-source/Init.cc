/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include "Style.h"
#include "StyleEnumMappings.h"
#include "ResourceManager.h"
#include "FontStoreView.h"
#include "Scene.h"
#include "Stage.h"
#include "Style.h"
#include "BoxSceneNode.h"
#include "ImageSceneNode.h"
#include "RootSceneNode.h"
#include "TextSceneNode.h"
#include "test/LightSourceSpec.h"

using Napi::Env;
using Napi::Function;
using Napi::HandleScope;
using Napi::Object;
using ls::ResourceManager;
using ls::FontStoreView;
using ls::Scene;
using ls::Stage;
using ls::Style;
using ls::SceneNode;
using ls::BoxSceneNode;
using ls::ImageSceneNode;
using ls::RootSceneNode;
using ls::TextSceneNode;

void ExportClass(Object* exports, const Function& constructor) {
    exports->Set(constructor.Get("name").ToString(), constructor);
}

Object Init(Env env, Object exports) {
    HandleScope scope(env);

    ExportClass(&exports, Style::Constructor(env));
    ExportClass(&exports, ResourceManager::Constructor(env));
    ExportClass(&exports, Scene::Constructor(env));
    ExportClass(&exports, Stage::Constructor(env));
    ExportClass(&exports, FontStoreView::Constructor(env));
    ExportClass(&exports, BoxSceneNode::Constructor(env));
    ExportClass(&exports, ImageSceneNode::Constructor(env));
    ExportClass(&exports, TextSceneNode::Constructor(env));
    ExportClass(&exports, RootSceneNode::Constructor(env));

    Style::Init(env);
    ls::StyleEnumMappings::Init(env, exports);

    exports["getSceneNodeInstanceCount"] = Function::New(env, &SceneNode::GetInstanceCount);

    #ifdef LIGHT_SOURCE_NATIVE_TESTS
    exports["test"] = ls::LightSourceSpec(env);
    #endif

    return exports;
}

NODE_API_MODULE(LightSource, Init);
