/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include "Style.h"
#include "StyleEnumMappings.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Style.h"
#include "BoxSceneNode.h"
#include "ImageSceneNode.h"
#include "RootSceneNode.h"
#include "TextSceneNode.h"
#include "napi-ext.h"
#include <fmt/format.h>

using Napi::Env;
using Napi::Function;
using Napi::HandleScope;
using Napi::Object;
using ls::ResourceManager;
using ls::Scene;
using ls::Style;
using ls::SceneNode;
using ls::BoxSceneNode;
using ls::ImageSceneNode;
using ls::RootSceneNode;
using ls::TextSceneNode;

void AddNativeTests(Env env, Object exports);

void ExportClass(Object* exports, const Function& constructor) {
    exports->Set(constructor.Get("name").ToString(), constructor);
}

Object Init(Env env, Object exports) {
    HandleScope scope(env);

    ExportClass(&exports, Style::Constructor(env));
    ExportClass(&exports, ResourceManager::Constructor(env));
    ExportClass(&exports, Scene::Constructor(env));
    ExportClass(&exports, BoxSceneNode::Constructor(env));
    ExportClass(&exports, ImageSceneNode::Constructor(env));
    ExportClass(&exports, TextSceneNode::Constructor(env));
    ExportClass(&exports, RootSceneNode::Constructor(env));

    Style::Init(env);
    ls::StyleEnumMappings::Init(env, exports);

    exports["getSceneNodeInstanceCount"] = Function::New(env, &SceneNode::GetInstanceCount);

    // Note: test are only added if LIGHT_SOURCE_ENABLE_NATIVE_TESTS is defined.
    AddNativeTests(env, exports);

    return exports;
}

#ifdef LIGHT_SOURCE_ENABLE_NATIVE_TESTS

#include "test/LightSourceSpecList.h"
#include "test/TestGroup.h"

void AddNativeTests(Env env, Object exports) {
    auto testGroupObject{ ls::TestGroup::New(env, "light-source native tests") };
    auto testGroup{ ls::TestGroup::Unwrap(testGroupObject) };

    ls::FileSystemSpec(testGroup);
    ls::SurfaceSpec(testGroup);

    exports["test"] = testGroupObject;
}

#else

void AddNativeTests(Env env, Object exports) {
}

#endif

NODE_API_MODULE(LightSource, Init);
