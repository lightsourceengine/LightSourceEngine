/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <YGNode.h>
#include <event/event.h>
#include <ls/BoxSceneNode.h>
#include <ls/ImageSceneNode.h>
#include <ls/LinkSceneNode.h>
#include <ls/RootSceneNode.h>
#include <ls/Scene.h>
#include <ls/Style.h>
#include <ls/TextSceneNode.h>
#include <ls/bindings/GlobalFunctions.h>
#include <ls/bindings/JSEnums.h>
#include <ls/bindings/JSStage.h>
#include <ls/bindings/JSScene.h>
#include <ls/bindings/Logger.h>
#include <napi.h>

#ifdef LIGHT_SOURCE_NATIVE_TESTS
#include <test/LightSourceTestSuite.h>
#endif

using Napi::Env;
using Napi::Function;
using Napi::HandleScope;
using Napi::Object;
using facebook::yoga::Event;

void ExportClass(Object* exports, const Function& constructor) {
    exports->Set(constructor.Get("name").ToString(), constructor);
}

Object Init(Env env, Object exports) {
    HandleScope scope(env);

    Event::subscribe(ls::SceneNode::YogaNodeLayoutEvent);

    ExportClass(&exports, ls::bindings::NewLoggerClass(env));
    ExportClass(&exports, ls::bindings::NewLogLevelClass(env));
    ExportClass(&exports, ls::bindings::NewStyleTransformClass(env));
    ExportClass(&exports, ls::bindings::NewStyleUnitClass(env));

    ExportClass(&exports, ls::bindings::JSStage::GetClass(env));
    ExportClass(&exports, ls::bindings::JSScene::GetClass(env));

    ls::Style::Init(env);
    ExportClass(&exports, ls::Style::GetClass(env));

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
