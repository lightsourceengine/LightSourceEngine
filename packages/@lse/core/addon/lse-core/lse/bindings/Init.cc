/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <event/event.h>
#include <lse/Style.h>
#include <lse/StyleValidator.h>
#include <lse/BoxSceneNode.h>
#include <lse/ImageSceneNode.h>
#include <lse/LinkSceneNode.h>
#include <lse/RootSceneNode.h>
#include <lse/TextSceneNode.h>
#include <lse/System.h>
#include <lse/Log.h>
#include <lse/bindings/Bindings.h>
#include <lse/bindings/JSEnums.h>
#include <lse/bindings/JSScene.h>
#include <lse/bindings/JSStage.h>
#include <lse/bindings/JSStyle.h>
#include <lse/bindings/JSStyleClass.h>
#include <lse/bindings/JSStyleTransformSpec.h>
#include <lse/bindings/JSStyleValue.h>
#include <lse/bindings/Logger.h>
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

    auto logLevel = lse::GetEnvOrDefault("LS_LOG_LEVEL", "INFO");

    if (!lse::SetLogLevel(logLevel)) {
        lse::SetLogLevel(lse::LogLevelInfo);
        LOG_ERROR("LS_LOG_LEVEL contains invalid value of %s. Defaulting to INFO.", logLevel);
    }

    lse::Style::Init();
    lse::StyleValidator::Init();
    lse::StylePropertyValueInit();

    Event::subscribe(lse::SceneNode::YogaNodeLayoutEvent);

    ExportClass(exports, lse::bindings::NewLogLevelClass(env));
    ExportClass(exports, lse::bindings::NewStyleTransformClass(env));
    ExportClass(exports, lse::bindings::NewStyleUnitClass(env));
    ExportClass(exports, lse::bindings::NewStyleAnchorClass(env));

    ExportClass(exports, lse::bindings::JSStage::GetClass(env));
    ExportClass(exports, lse::bindings::JSScene::GetClass(env));
    ExportClass(exports, lse::bindings::JSStyle::GetClass(env));
    ExportClass(exports, lse::bindings::JSStyleClass::GetClass(env));
    ExportClass(exports, lse::bindings::JSStyleValue::GetClass(env));
    ExportClass(exports, lse::bindings::JSStyleTransformSpec::GetClass(env));

    ExportClass(exports, lse::BoxSceneNode::GetClass(env));
    ExportClass(exports, lse::ImageSceneNode::GetClass(env));
    ExportClass(exports, lse::LinkSceneNode::GetClass(env));
    ExportClass(exports, lse::TextSceneNode::GetClass(env));
    ExportClass(exports, lse::RootSceneNode::GetClass(env));

    ExportFunction(exports, Function::New(env, &lse::bindings::ParseColor, "parseColor"));
    ExportFunction(exports, Function::New(env, &lse::bindings::LoadPlugin, "loadPlugin"));
    ExportFunction(exports, Function::New(env, &lse::SceneNode::GetInstanceCount, "getSceneNodeInstanceCount"));

    exports["logger"] = lse::bindings::NewLoggerClass(env);
    exports["styleProperties"] = lse::bindings::GetStyleProperties(env);

    #if defined(LS_ENABLE_NATIVE_TESTS)
    exports["test"] = lse::LightSourceTestSuite(env);
    #endif

    return exports;
}

NODE_API_MODULE(LightSource, Init);
