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

using Napi::Array;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Object;
using Napi::ObjectReference;
using Napi::String;
using Napi::Value;
using Napi::ObjectGetString;
using Napi::ObjectGetNumberOrDefault;

namespace ls {

StyleFontStyle GetFontStyle(Object options, const char* name);
StyleFontWeight GetFontWeight(Object options, const char* name);
void CopyStringArrayToVector(const Array& array, std::vector<std::string>* vector);
Array VectorToArray(Napi::Env env, const std::vector<std::string>& vector);
void ValidateStringArray(Napi::Env env, const Array& array);

ResourceManager::ResourceManager(const CallbackInfo& info) : ObjectWrap<ResourceManager>(info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    this->pathObject.Reset(VectorToArray(env, this->path));
    this->imageExtensions = { ".jpg", ".jpeg", ".png", ".gif", ".svg" };
    this->imageExtensionsObject.Reset(VectorToArray(env, this->imageExtensions), 1);
}

Function ResourceManager::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "ResourceManager", {
            InstanceMethod("registerImage", &ResourceManager::RegisterImage),
            InstanceMethod("addFont", &ResourceManager::AddFont),
            InstanceAccessor("path", &ResourceManager::GetPath, &ResourceManager::SetPath),
            InstanceAccessor("imageExtensions",
                &ResourceManager::GetImageExtensions, &ResourceManager::SetImageExtensions),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void ResourceManager::RegisterImage(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    auto imageUri{ ImageUri::FromObject(info[0].As<Object>()) };
    auto id{ imageUri.GetId() };

    if (this->registeredImageUris.find(id) != this->registeredImageUris.end()) {
        throw Error::New(env, fmt::format("Image id '{}' has already been registered.", id));
    }

    fmt::println("Image Spec: id={} uri={} width={} height={} capInsets={},{},{},{}",
        id,
        imageUri.GetUri(),
        imageUri.GetWidth(),
        imageUri.GetHeight(),
        imageUri.GetCapInsets().top,
        imageUri.GetCapInsets().right,
        imageUri.GetCapInsets().bottom,
        imageUri.GetCapInsets().left);

    registeredImageUris.emplace(id, imageUri);
}

void ResourceManager::AddFont(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    auto options{ info[0].As<Object>() };
    auto family{ ObjectGetString(options, "family") };
    auto uri{ ObjectGetString(options, "uri") };
    auto fontStyle{ GetFontStyle(options, "style") };
    auto fontWeight{ GetFontWeight(options, "weight") };
    auto index{ ObjectGetNumberOrDefault(options, "index", 0) };
    auto fontId{ FontResource::MakeId(family, fontStyle, fontWeight) };

    if (this->fonts.find(fontId) != this->fonts.end()) {
        throw Error::New(env, fmt::format("Font '{}' already registered.", fontId));
    }

    auto fontResource{ std::make_shared<FontResource>(info.Env(), fontId, uri, index, family, fontStyle, fontWeight) };

    this->fonts[fontId] = fontResource;

    fontResource->Load(this->path);
}

Value ResourceManager::GetImageExtensions(const CallbackInfo& info) {
    return this->imageExtensionsObject.Value();
}

void ResourceManager::SetImageExtensions(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto env{ info.Env() };
    HandleScope scope(env);

    if (value.IsUndefined() || value.IsNull()) {
        this->imageExtensions.clear();
    } else if (value.IsArray()) {
        auto array{ value.As<Array>() };

        ValidateStringArray(env, array);
        CopyStringArrayToVector(array, &this->imageExtensions);
    } else {
        throw Error::New(env, "imageExtensions can only be assigned to an array of strings");
    }

    this->imageExtensionsObject.Reset(VectorToArray(env, this->imageExtensions), 1);
}

Napi::Value ResourceManager::GetPath(const Napi::CallbackInfo& info) {
    return this->pathObject.Value();
}

void ResourceManager::SetPath(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto env{ info.Env() };
    HandleScope scope(env);

    if (value.IsUndefined() || value.IsNull()) {
        this->path.clear();
    } else if (value.IsString()) {
        this->path = { value.As<String>().Utf8Value() };
    } else if (value.IsArray()) {
        auto array{ value.As<Array>() };

        ValidateStringArray(env, array);
        CopyStringArrayToVector(array, &this->path);
    } else {
        throw Error::New(env, "path can only be assigned to a string or an array of strings");
    }

    this->pathObject.Reset(VectorToArray(env, this->path), 1);
}

void ResourceManager::SetRenderer(Renderer* renderer) {
    this->renderer = renderer;
}

ImageResource* ResourceManager::GetImage(const std::string& id) {
    auto p{ this->images.find(id) };

    if (p != this->images.end()) {
        auto imageResource{ p->second.get() };

        imageResource->AddRef();

        return imageResource;
    }

    return nullptr;
}

ImageResource* ResourceManager::LoadImage(const ImageUri& uri) {
    ImageResource* imageResource{ this->GetImage(uri.GetId()) };

    if (imageResource) {
        return imageResource;
    }

    auto registeredImage{ this->registeredImageUris.find(uri.GetId()) };
    const auto& imageUri{ registeredImage != this->registeredImageUris.end() ? registeredImage->second : uri };

    try {
        auto imageResourceShared{ std::make_shared<ImageResource>(this->Env(), imageUri) };

        this->images[imageUri.GetId()] = imageResourceShared;
        imageResourceShared->Load(this->renderer, this->imageExtensions, this->path);

        return imageResourceShared.get();
    } catch (std::exception& e) {
        fmt::println("Error: LoadImage: {}", e.what());
    }

    return nullptr;
}

FontSampleResource* ResourceManager::LoadFontSample(const std::string& family, StyleFontStyle fontStyle,
        StyleFontWeight fontWeight, int32_t fontSize) {
    FontSampleResource* fontSample{ this->FindFontSample(family, fontStyle, fontWeight, fontSize) };

    if (fontSample) {
        return fontSample;
    }

    auto font{ this->FindFontInternal(family, fontStyle, fontWeight) };

    if (!font) {
        return nullptr;
    }

    try {
        auto newFontSample{ std::make_shared<FontSampleResource>(this->Env(), font, fontSize) };

        this->fontSamples[newFontSample->GetId()] = newFontSample;

        return newFontSample.get();
    } catch (std::exception& e) {
        fmt::println("Error: LoadFontSample: {}", e.what());
        font->RemoveRef();
    }

    return nullptr;
}

FontSampleResource* ResourceManager::FindFontSample(const std::string& family, StyleFontStyle fontStyle,
        StyleFontWeight fontWeight, int32_t fontSize) {
    auto iter{ this->fontSamples.find(FontSampleResource::MakeId(family, fontStyle, fontWeight, fontSize)) };

    if (iter != this->fontSamples.end()) {
        iter->second->AddRef();

        return iter->second.get();
    }

    return nullptr;
}

FontResource* ResourceManager::FindFontInternal(const std::string& family, StyleFontStyle fontStyle,
        StyleFontWeight fontWeight) {
    auto iter{ this->fonts.find(FontResource::MakeId(family, fontStyle, fontWeight)) };

    if (iter != this->fonts.end()) {
        return iter->second.get();
    }

    return nullptr;
}

FontResource* ResourceManager::FindFont(const std::string& family, StyleFontStyle fontStyle,
        StyleFontWeight fontWeight) {
    auto font{ this->FindFontInternal(family, fontStyle, fontWeight) };

    if (font) {
        font->AddRef();
    }

    return font;
}

void ResourceManager::Attach() {
}

void ResourceManager::Detach() {
}

void ResourceManager::ProcessEvents() {
}

void ResourceManager::Destroy() {
    this->renderer = nullptr;
    images.clear();
    fontSamples.clear();
    fonts.clear();
    registeredImageUris.clear();
    imageExtensions.clear();
    imageExtensionsObject.Reset();
    path.clear();
    pathObject.Reset();
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

void ValidateStringArray(Napi::Env env, const Array& array) {
    auto len{ array.Length() };

    for (auto i{0u}; i < len; i++) {
        if (!array.Get(i).IsString()) {
            throw Error::New(env, "Expected an array containing string objects.");
        }
    }
}

void CopyStringArrayToVector(const Array& array, std::vector<std::string>* vector) {
    vector->clear();

    auto len{ array.Length() };

    for (auto i{ 0u }; i < len; i++) {
        vector->push_back(array.Get(i).As<String>());
    }
}

Array VectorToArray(Napi::Env env, const std::vector<std::string>& vector) {
    auto array{ Array::New(env, vector.size()) };
    auto i{ 0u };

    for (auto& item : vector) {
        array.Set(i++, String::New(env, item));
    }

    return array;
}

} // namespace ls
