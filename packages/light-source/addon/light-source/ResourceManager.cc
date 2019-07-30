/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ResourceManager.h"
#include "napi-ext.h"
#include "Renderer.h"
#include <algorithm>
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

StyleFontStyle GetFontStyle(Object options, const char* name);
StyleFontWeight GetFontWeight(Object options, const char* name);

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
}

void ResourceManager::RegisterFont(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);
    auto options{ info[0].As<Object>() };
    auto family{ GetString(options, "family") };
    auto uri{ GetString(options, "uri") };
    auto fontStyle{ GetFontStyle(options, "style") };
    auto fontWeight{ GetFontWeight(options, "weight") };
    auto index{ GetNumberOrDefault(options, "index", 0) };

    auto fontId{ FontResource::MakeId(family, fontStyle, fontWeight) };

    if (this->fonts.find(fontId) != this->fonts.end()) {
        throw Error::New(env, fmt::format("Font '{}' already registered.", fontId));
    }

    auto fontResource{ std::make_shared<FontResource>(info.Env(), fontId, uri, index, family, fontStyle, fontWeight) };

    this->fonts[fontId] = fontResource;

    fontResource->Load();
}

void ResourceManager::Attach(Renderer* renderer) {
    this->renderer = renderer;
}

void ResourceManager::Detach() {
    this->renderer = nullptr;
}

ImageResource* ResourceManager::GetImage(const std::string& id) {
    auto p{ this->images.find(id) };

    if (p != this->images.end()) {
        return p->second.get();
    }

    // TODO: check renderer?

    auto imageResource{ std::make_shared<ImageResource>(this->Env(), id) };

    this->images[id] = imageResource;

    imageResource->AddRef();
    imageResource->Load(this->renderer, this->extensions, this->path);

    return imageResource.get();
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

StyleFontStyle GetFontStyle(Object options, const char* name) {
    if (options.Has(name)) {
        auto prop{ options.Get(name) };

        if (prop.IsString()) {
            auto value{ prop.As<String>().Utf8Value() };

            std::transform(value.begin(), value.end(), value.begin(), ::tolower);

            if (value == StyleFontStyleToString(StyleFontStyleNormal)) {
                return StyleFontStyleNormal;
            } else if (value == StyleFontStyleToString(StyleFontStyleNormal)) {
                return StyleFontStyleItalic;
            }
        }
    }

    return StyleFontStyleNormal;
}

StyleFontWeight GetFontWeight(Object options, const char* name) {
    if (options.Has(name)) {
        auto prop{ options.Get(name) };

        if (prop.IsString()) {
            auto value{ prop.As<String>().Utf8Value() };

            std::transform(value.begin(), value.end(), value.begin(), ::tolower);

            if (value == StyleFontWeightToString(StyleFontWeightNormal)) {
                return StyleFontWeightNormal;
            } else if (value == StyleFontWeightToString(StyleFontWeightBold)) {
                return StyleFontWeightBold;
            }
        }
    }

    return StyleFontWeightNormal;
}

} // namespace ls
