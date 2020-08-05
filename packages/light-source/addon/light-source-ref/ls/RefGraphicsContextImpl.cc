/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefGraphicsContextImpl.h"

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

RefGraphicsContextImpl::RefGraphicsContextImpl(const Napi::CallbackInfo& info) {
    HandleScope scope(info.Env());
    auto config{ info[1].As<Object>() };

    this->width = Napi::ObjectGetNumberOrDefault(config, "width", 0);
    this->height = Napi::ObjectGetNumberOrDefault(config, "height", 0);
    this->displayIndex = Napi::ObjectGetNumberOrDefault(config, "displayIndex", 0);
    this->fullscreen = Napi::ObjectGetBooleanOrDefault(config, "fullscreen", true);
}

void RefGraphicsContextImpl::Attach(const Napi::CallbackInfo& info) {
}

void RefGraphicsContextImpl::Detach(const Napi::CallbackInfo& info) {
}

void RefGraphicsContextImpl::Resize(const Napi::CallbackInfo& info) {
}

Value RefGraphicsContextImpl::GetTitle(const Napi::CallbackInfo& info) {
    return String::New(info.Env(), this->title);
}

void RefGraphicsContextImpl::SetTitle(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (value.IsString()) {
        this->title = value.As<String>();
    } else {
        throw Error::New(info.Env(), "title property must be a string");
    }
}

Value RefGraphicsContextImpl::GetWidth(const Napi::CallbackInfo& info) {
    return Number::New(info.Env(), this->width);
}

Value RefGraphicsContextImpl::GetHeight(const Napi::CallbackInfo& info) {
    return Number::New(info.Env(), this->height);
}

Value RefGraphicsContextImpl::GetFullscreen(const Napi::CallbackInfo& info) {
    return Boolean::New(info.Env(), this->fullscreen);
}

Renderer* RefGraphicsContextImpl::GetRenderer() const {
    return &this->renderer;
}

Value RefGraphicsContextImpl::GetDisplayIndex(const Napi::CallbackInfo& info) {
    return Number::New(info.Env(), this->displayIndex);
}

void RefGraphicsContextImpl::Finalize() {
    delete this;
}

} // namespace ls
