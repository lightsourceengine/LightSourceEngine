/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageStoreView.h"
#include <ls/ImageStore.h>
#include <ls/ImageResource.h>
#include <ls/Stage.h>
#include <ls/Scene.h>
#include <ls/Log.h>

using Napi::Array;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::SafeObjectWrap;
using Napi::QueryInterface;
using Napi::String;
using Napi::Value;

namespace ls {
namespace bindings {

ImageStoreView::ImageStoreView(const CallbackInfo& info) : SafeObjectWrap<ImageStoreView>(info) {
}

ImageStoreView::~ImageStoreView() {
    if (this->scene) {
        this->scene->Unref();
    }
}

void ImageStoreView::Constructor(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    if (info[0].IsObject()) {
        this->scene = QueryInterface<Scene>(info[0]);
        if (this->scene) {
            this->scene->Ref();
        }
    }
}

Function ImageStoreView::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "ImageStoreView", true, {
            InstanceMethod("add", &ImageStoreView::Add),
            InstanceMethod("list", &ImageStoreView::List),
            InstanceAccessor("extensions", &ImageStoreView::GetExtensions, &ImageStoreView::SetExtensions),
        });
    }

    return constructor.Value();
}

void ImageStoreView::Add(const CallbackInfo& info) {
    EnsureScene();

    auto env{ info.Env() };
    HandleScope scope(env);
    ImageUri imageUri;
    auto arg{ info[0] };

    if (arg.IsString() || arg.IsObject()) {
        imageUri = ImageUri::Unbox(arg);

        if (imageUri.IsEmpty()) {
            throw Error::New(env, "Invalid image uri specified.");
        }
    } else {
        throw Error::New(env, "Expected an image uri specified string or object.");
    }

    this->scene->GetImageStore()->LoadImage(imageUri);
}

Value ImageStoreView::List(const CallbackInfo& info) {
    EnsureScene();

    auto env{ info.Env() };
    EscapableHandleScope scope{env};
    auto imageList{ Array::New(env) };

    this->scene->GetImageStore()->ForEach([&](const std::shared_ptr<ImageResource>& resource) {
        imageList[imageList.Length()] = ImageUri::Box(env, resource->GetUri());
    });

    return scope.Escape(imageList);
}

Value ImageStoreView::GetExtensions(const CallbackInfo& info) {
    EnsureScene();

    auto env{ info.Env() };
    HandleScope scope(env);
    const auto& extensions{ this->scene->GetImageStore()->GetSearchExtensions() };
    auto i{ 0u };
    auto result{ Array::New(env, extensions.size()) };

    for (const auto& extension : extensions) {
        result[i++] = String::New(env, extension);
    }

    return result;
}

void ImageStoreView::SetExtensions(const CallbackInfo& info, const Napi::Value& value) {
    EnsureScene();

    HandleScope scope(info.Env());
    auto imageStore{ this->scene->GetImageStore() };
    std::vector<std::string> extensions;

    if (value.IsString()) {
        extensions = { value.As<String>().Utf8Value() };
    } else if (value.IsArray()) {
        auto array{ value.As<Array>() };
        const auto size{ array.Length() };

        extensions.reserve(size);

        for (uint32_t i = 0; i < size; i++) {
            extensions.emplace_back(array.Get(i).As<String>().Utf8Value());
        }
    } else if (value.IsNull() || value.IsUndefined()) {
        extensions = ImageStore::defaultExtensions;
    } else {
        throw Error::New(info.Env(), "extensions value must be an Array of strings");
    }

    for (auto& extension : extensions) {
        if (extension[0] != '.') {
            extension.insert(0, ".");
        }
    }

    imageStore->SetSearchExtensions(extensions);
}

void ImageStoreView::EnsureScene() const {
    if (this->scene == nullptr) {
        throw Error::New(this->Env(), "ImageStoreView not connected to Scene.");
    }
}

} // namespace bindings
} // namespace ls
