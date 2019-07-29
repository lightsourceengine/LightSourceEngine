/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ResourceManager.h"
#include "napi-ext.h"
#include <fmt/format.h>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Object;
using Napi::String;
using Napi::Value;

namespace ls {

ResourceManager::ResourceManager(const CallbackInfo& info) : ObjectWrap<ResourceManager>(info) {
}

Function ResourceManager::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "ResourceManager", {
            InstanceMethod("registerImage", &ResourceManager::RegisterImage),
            InstanceMethod("registerFont", &ResourceManager::RegisterFont),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void ResourceManager::RegisterImage(const Napi::CallbackInfo& info) {
    auto uri{ info[0].As<String>() };

    this->RegisterImage(uri);
}

void ResourceManager::RegisterFont(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);
    auto options{ info[0].As<Object>() };
    auto family{ GetString(options, "family") };
    auto uri{ GetString(options, "uri") };
    // TODO: string => number
    auto fontStyle{ GetEnumOrDefault(options, "style", StyleFontStyleNormal) };
    auto fontWeight{ GetEnumOrDefault(options, "weight", StyleFontWeightNormal) };
    auto index{ GetNumberOrDefault(options, "index", 0) };

    auto fontId{ FontResource::MakeId(family, fontStyle, fontWeight) };

    if (this->fonts.find(fontId) != this->fonts.end()) {
        throw Error::New(env, fmt::format("Font '{}' already registered.", fontId));
    }

    auto fontResource{ std::make_shared<FontResource>(fontId, uri, index, family, fontStyle, fontWeight) };

    this->fonts[fontId] = fontResource;

    fontResource->Load(info.Env());
}

void ResourceManager::Attach(Renderer* renderer) {
    this->renderer = renderer;
}

void ResourceManager::Detach() {
    this->renderer = nullptr;
}

void ResourceManager::RegisterImage(const std::string& id) {
}

ImageResource* ResourceManager::GetImage(const std::string& id) {
    return nullptr;
}

FontResource* ResourceManager::FindFont(
        const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight) {
    auto p{ this->fonts.find(FontResource::MakeId(family, fontStyle, fontWeight)) };

    if (p != this->fonts.end()) {
        return p->second.get();
    }

    return nullptr;
}

void ResourceManager::ProcessEvents() {
}

} // namespace ls
