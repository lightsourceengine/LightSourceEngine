/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefSceneAdapter.h"
#include <napi-ext.h>

using Napi::Boolean;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::String;
using Napi::Value;

namespace ls {

RefSceneAdapter::RefSceneAdapter(const Napi::CallbackInfo& info) : SafeObjectWrap<RefSceneAdapter>(info) {
}

void RefSceneAdapter::Constructor(const Napi::CallbackInfo& info) {
    HandleScope scope(info.Env());
    auto config{ info[0].As<Object>() };

    this->width = Napi::ObjectGetNumberOrDefault(config, "width", 0);
    this->height = Napi::ObjectGetNumberOrDefault(config, "height", 0);
    this->displayIndex = Napi::ObjectGetNumberOrDefault(config, "displayIndex", 0);
    this->fullscreen = Napi::ObjectGetBooleanOrDefault(config, "fullscreen", true);
}

Function RefSceneAdapter::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "RefSceneAdapter", true, {
            InstanceAccessor("width", &RefSceneAdapter::GetWidth, nullptr),
            InstanceAccessor("height", &RefSceneAdapter::GetHeight, nullptr),
            InstanceAccessor("displayIndex", &RefSceneAdapter::GetDisplayIndex, nullptr),
            InstanceAccessor("fullscreen", &RefSceneAdapter::GetFullscreen, nullptr),
            InstanceAccessor("title", &RefSceneAdapter::GetTitle, &RefSceneAdapter::SetTitle),
            InstanceMethod("attach", &RefSceneAdapter::Attach),
            InstanceMethod("detach", &RefSceneAdapter::Detach),
            InstanceMethod("resize", &RefSceneAdapter::Resize),
        });
    }

    return constructor.Value();
}

void RefSceneAdapter::Attach(const Napi::CallbackInfo& info) {
}

void RefSceneAdapter::Detach(const Napi::CallbackInfo& info) {
}

void RefSceneAdapter::Resize(const Napi::CallbackInfo& info) {
}

Value RefSceneAdapter::GetTitle(const Napi::CallbackInfo& info) {
    return String::New(info.Env(), this->title);
}

void RefSceneAdapter::SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (value.IsString()) {
        this->title = value.As<String>();
    } else {
        throw Error::New(info.Env(), "title property must be a string");
    }
}

Value RefSceneAdapter::GetWidth(const Napi::CallbackInfo& info) {
    return Number::New(info.Env(), this->width);
}

Value RefSceneAdapter::GetHeight(const Napi::CallbackInfo& info) {
    return Number::New(info.Env(), this->height);
}

Value RefSceneAdapter::GetFullscreen(const Napi::CallbackInfo& info) {
    return Boolean::New(info.Env(), this->fullscreen);
}

Renderer* RefSceneAdapter::GetRenderer() const {
    return &this->renderer;
}

Value RefSceneAdapter::GetDisplayIndex(const Napi::CallbackInfo& info) {
    return Number::New(info.Env(), this->displayIndex);
}

} // namespace ls
