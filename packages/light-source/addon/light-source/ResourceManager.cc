/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ResourceManager.h"
#include "napi-ext.h"
#include <ls/Renderer.h>
#include "LayerResource.h"
#include <algorithm>
#include <fmt/println.h>

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

void CopyStringArrayToVector(const Array& array, std::vector<std::string>* vector);
Array VectorToArray(Napi::Env env, const std::vector<std::string>& vector);
void ValidateStringArray(Napi::Env env, const Array& array);

ResourceManager::ResourceManager(const CallbackInfo& info) : ObjectWrap<ResourceManager>(info) {
}

Function ResourceManager::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "ResourceManager", {
            InstanceMethod("registerImage", &ResourceManager::RegisterImage),
            InstanceAccessor("images", &ResourceManager::GetImages, nullptr),
            InstanceAccessor(SymbolFor(env, "resourcePath"), &ResourceManager::GetPath, &ResourceManager::SetPath),
            InstanceAccessor("imageExtensions",
                &ResourceManager::GetImageExtensions, &ResourceManager::SetImageExtensions),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value ResourceManager::GetImages(const CallbackInfo& info) {
    auto env{ info.Env() };
    auto imagesArray{ Array::New(env, this->images.size()) };
    auto i{ 0u };

    for (auto& p : this->images) {
        imagesArray.Set(i++, p.second->ToObject(env));
    }

    return imagesArray;
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

Value ResourceManager::GetImageExtensions(const CallbackInfo& info) {
    return VectorToArray(info.Env(), this->imageExtensions);
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
}

Napi::Value ResourceManager::GetPath(const Napi::CallbackInfo& info) {
    return VectorToArray(info.Env(), this->path);
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
}

void ResourceManager::PostConstruct(Renderer* renderer) {
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
        auto imageResourceShared{ std::make_shared<ImageResource>(imageUri) };

        this->images[imageUri.GetId()] = imageResourceShared;
        imageResourceShared->Load(&this->asyncTaskQueue, this->renderer, this->imageExtensions, this->path);

        return imageResourceShared.get();
    } catch (std::exception& e) {
        fmt::println("Error: LoadImage: {}", e.what());
    }

    return nullptr;
}

void ResourceManager::Attach() {
    for (auto& entry : this->images) {
        auto imageResource{ entry.second };

        if (imageResource->resourceState == ResourceStateInit) {
            imageResource->Load(&this->asyncTaskQueue, this->renderer, this->imageExtensions, this->path);
        }
    }
}

void ResourceManager::Detach() {
    for (auto& resource : this->images) {
        resource.second->Detach(this->renderer);
    }

    // TODO: clean up layers
}

void ResourceManager::ProcessEvents() {
    this->asyncTaskQueue.ProcessTasks();
}

void ResourceManager::Destroy() {
    if (!this->renderer) {
        return;
    }

    this->Detach();

    this->renderer = nullptr;

    this->images.clear();
    // TODO: clean up layers
    this->registeredImageUris.clear();

    this->asyncTaskQueue.Shutdown();
}

LayerResource* ResourceManager::CreateLayerResource() {
    auto layerResource{ new LayerResource(this->renderer) };

    this->layers.insert(layerResource);

    return layerResource;
}

void ResourceManager::RemoveLayerResource(LayerResource* layerResource) {
    auto it{ this->layers.find(layerResource) };

    if (it != this->layers.end()) {
        delete *it; // NOLINT(readability/pointer_notation)
        this->layers.erase(it);
    }
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
