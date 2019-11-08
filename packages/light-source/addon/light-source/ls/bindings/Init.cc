/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi.h>
#include <ls/bindings/Logger.h>
#include <ls/bindings/GlobalFunctions.h>
#include <ls/bindings/FontStoreView.h>
#include <ls/bindings/ImageStoreView.h>
#include <Scene.h>
#include <Stage.h>
#include <Style.h>
#include <BoxSceneNode.h>
#include <ImageSceneNode.h>
#include <RootSceneNode.h>
#include <TextSceneNode.h>

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

    ExportClass(&exports, ls::bindings::Logger::Constructor(env));
    ExportClass(&exports, ls::bindings::FontStoreView::Constructor(env));
    ExportClass(&exports, ls::bindings::ImageStoreView::Constructor(env));

    ExportClass(&exports, ls::Style::Constructor());
    ExportClass(&exports, ls::Scene::Constructor(env));
    ExportClass(&exports, ls::Stage::Constructor(env));
    ExportClass(&exports, ls::BoxSceneNode::Constructor(env));
    ExportClass(&exports, ls::ImageSceneNode::Constructor(env));
    ExportClass(&exports, ls::TextSceneNode::Constructor(env));
    ExportClass(&exports, ls::RootSceneNode::Constructor(env));

    ls::bindings::InitGlobalFunctions(env, exports);

    #ifdef LIGHT_SOURCE_NATIVE_TESTS
    exports["test"] = ls::LightSourceTestSuite(env);
    #endif

    return exports;
}

NODE_API_MODULE(LightSource, Init);
