/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageStoreView.h"
#include "ImageStore.h"
#include "ImageResource.h"
#include "Stage.h"
#include "Scene.h"

using Napi::Array;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

ImageStoreView::ImageStoreView(const CallbackInfo& info) : ObjectWrap<ImageStoreView>(info) {
    auto env{ info.Env() };

    if (info[0].IsObject()) {
        this->scene = Scene::Unwrap(info[0].As<Object>());
    }

    if (scene == nullptr) {
        throw Error::New(env, "ImageStoreView expects a Scene instance.");
    }

    this->scene->Ref();
}

ImageStoreView::~ImageStoreView() {
    this->scene->Unref();
}

Function ImageStoreView::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "ImageStoreView", {
            InstanceMethod("list", &ImageStoreView::List),
            InstanceAccessor("extensions", &ImageStoreView::GetExtensions, &ImageStoreView::SetExtensions),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value ImageStoreView::List(const CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope{env};
    auto imageList{ Array::New(env) };

    this->scene->GetImageStore()->ForEach([&](std::shared_ptr<ImageResource> resource) {
        auto info{ resource->ToObject(env).As<Object>() };

        info["refs"] = Number::New(env, resource.use_count() - 2);

        imageList[imageList.Length()] = info;
    });

    return scope.Escape(imageList);
}

Value ImageStoreView::GetExtensions(const CallbackInfo& info) {
    const auto& extensions{ this->scene->GetImageStore()->GetSearchExtensions() };
    auto env{ info.Env() };
    auto i{ 0u };
    auto result{ Array::New(env, extensions.size()) };

    for (const auto& extension : extensions) {
        result[i++] = String::New(env, extension);
    }

    return result;
}

void ImageStoreView::SetExtensions(const CallbackInfo& info, const Napi::Value& value) {
    auto imageStore{ this->scene->GetImageStore() };
    std::vector<std::string> extensions;

    if (value.IsString()) {
        extensions = { value.As<String>().Utf8Value() };
    } else if (value.IsArray()) {
        auto array{ value.As<Array>() };
        auto size{ array.Length() };
        std::vector<std::string> extensions(size);

        for (decltype(size) i = 0; i < size; i++) {
            extensions.push_back(array.Get(i).As<String>().Utf8Value());
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

} // namespace ls
