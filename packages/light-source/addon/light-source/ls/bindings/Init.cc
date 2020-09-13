/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <event/event.h>
#include <ls/Style.h>
#include <ls/StyleValidator.h>
#include <ls/BoxSceneNode.h>
#include <ls/ImageSceneNode.h>
#include <ls/LinkSceneNode.h>
#include <ls/RootSceneNode.h>
#include <ls/TextSceneNode.h>
#include <ls/System.h>
#include <ls/Log.h>
#include <ls/bindings/Bindings.h>
#include <ls/bindings/JSEnums.h>
#include <ls/bindings/JSScene.h>
#include <ls/bindings/JSStage.h>
#include <ls/bindings/JSStyle.h>
#include <ls/bindings/JSStyleClass.h>
#include <ls/bindings/JSStyleTransformSpec.h>
#include <ls/bindings/JSStyleValue.h>
#include <ls/bindings/Logger.h>
#include <napi.h>

#if defined(LS_ENABLE_NATIVE_TESTS)
#include <test/LightSourceTestSuite.h>
#endif

using Napi::Env;
using Napi::Function;
using Napi::HandleScope;
using Napi::Object;
using facebook::yoga::Event;

void ExportFunction(Object exports, const Function& function) {
    exports.Set(function.Get("name").ToString(), function);
}

void ExportClass(Object exports, const Function& constructor) {
    ExportFunction(exports, constructor);
}

Object Init(Env env, Object exports) {
    HandleScope scope(env);

    auto logLevel = ls::GetEnvOrDefault("LS_LOG_LEVEL", "INFO");

    if (!ls::SetLogLevel(logLevel)) {
        ls::SetLogLevel(ls::LogLevelInfo);
        LOG_ERROR("LS_LOG_LEVEL contains invalid value of %s. Defaulting to INFO.", logLevel);
    }

    ls::Style::Init();
    ls::StyleValidator::Init();
    ls::StylePropertyValueInit();

    Event::subscribe(ls::SceneNode::YogaNodeLayoutEvent);

    ExportClass(exports, ls::bindings::NewLogLevelClass(env));
    ExportClass(exports, ls::bindings::NewStyleTransformClass(env));
    ExportClass(exports, ls::bindings::NewStyleUnitClass(env));
    ExportClass(exports, ls::bindings::NewStyleAnchorClass(env));

    ExportClass(exports, ls::bindings::JSStage::GetClass(env));
    ExportClass(exports, ls::bindings::JSScene::GetClass(env));
    ExportClass(exports, ls::bindings::JSStyle::GetClass(env));
    ExportClass(exports, ls::bindings::JSStyleClass::GetClass(env));
    ExportClass(exports, ls::bindings::JSStyleValue::GetClass(env));
    ExportClass(exports, ls::bindings::JSStyleTransformSpec::GetClass(env));

    ExportClass(exports, ls::BoxSceneNode::GetClass(env));
    ExportClass(exports, ls::ImageSceneNode::GetClass(env));
    ExportClass(exports, ls::LinkSceneNode::GetClass(env));
    ExportClass(exports, ls::TextSceneNode::GetClass(env));
    ExportClass(exports, ls::RootSceneNode::GetClass(env));

    ExportFunction(exports, Function::New(env, &ls::bindings::ParseColor, "parseColor"));
    ExportFunction(exports, Function::New(env, &ls::bindings::LoadPlugin, "loadPlugin"));
    ExportFunction(exports, Function::New(env, &ls::SceneNode::GetInstanceCount, "getSceneNodeInstanceCount"));

    exports["logger"] = ls::bindings::NewLoggerClass(env);
    exports["styleProperties"] = ls::bindings::GetStyleProperties(env);

    #if defined(LS_ENABLE_NATIVE_TESTS)
    exports["test"] = ls::LightSourceTestSuite(env);
    #endif

    return exports;
}

NODE_API_MODULE(LightSource, Init);
