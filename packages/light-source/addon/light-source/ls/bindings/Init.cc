/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include <ls/bindings/Logger.h>
#include <ls/bindings/GlobalFunctions.h>
#include <ls/Scene.h>
#include <ls/Stage.h>
#include <ls/Style.h>
#include <ls/BoxSceneNode.h>
#include <ls/ImageSceneNode.h>
#include <ls/LinkSceneNode.h>
#include <ls/RootSceneNode.h>
#include <ls/TextSceneNode.h>

#ifdef LIGHT_SOURCE_NATIVE_TESTS
#include <test/LightSourceTestSuite.h>
#endif

using Napi::Env;
using Napi::Function;
using Napi::HandleScope;
using Napi::Object;

void ExportClass(Object* exports, const Function& constructor) {
    exports->Set(constructor.Get("name").ToString(), constructor);
}

Object Init(Env env, Object exports) {
    HandleScope scope(env);

    ls::Style::Init(env);

    ExportClass(&exports, ls::bindings::Logger::GetClass(env));

    ExportClass(&exports, ls::Style::GetClass(env));
    ExportClass(&exports, ls::Scene::GetClass(env));
    ExportClass(&exports, ls::Stage::GetClass(env));
    ExportClass(&exports, ls::BoxSceneNode::GetClass(env));
    ExportClass(&exports, ls::ImageSceneNode::GetClass(env));
    ExportClass(&exports, ls::LinkSceneNode::GetClass(env));
    ExportClass(&exports, ls::TextSceneNode::GetClass(env));
    ExportClass(&exports, ls::RootSceneNode::GetClass(env));

    ls::bindings::InitGlobalFunctions(env, exports);

    #ifdef LIGHT_SOURCE_NATIVE_TESTS
    exports["test"] = ls::LightSourceTestSuite(env);
    #endif

    return exports;
}

NODE_API_MODULE(LightSource, Init);
