/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/LinkSceneNode.h>

#include <cctype>
#include <algorithm>
#include <ls/Scene.h>
#include <ls/Stage.h>
#include <ls/Log.h>
#include <ls/Uri.h>
#include <ls/string-ext.h>

using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectReference;
using Napi::String;

namespace ls {

Napi::FunctionReference LinkSceneNode::constructor;

Function LinkSceneNode::GetClass(Napi::Env env) {
    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(
            env,
            "LinkSceneNode",
            true,
            SceneNode::Extend<LinkSceneNode>(env, {
                InstanceMethod("fetch", &LinkSceneNode::Fetch),
                InstanceAccessor("rel", &LinkSceneNode::GetRel, &LinkSceneNode::SetRel),
                InstanceAccessor("as", &LinkSceneNode::GetAs, &LinkSceneNode::SetAs),
                InstanceAccessor("href", &LinkSceneNode::GetHref, &LinkSceneNode::SetHref),
                InstanceAccessor("onLoad", &LinkSceneNode::GetOnLoadCallback, &LinkSceneNode::SetOnLoadCallback),
                InstanceAccessor("onError", &LinkSceneNode::GetOnErrorCallback, &LinkSceneNode::SetOnErrorCallback),
            }));
    }

    return constructor.Value();
}

void LinkSceneNode::Constructor(const Napi::CallbackInfo& info) {
    this->SceneNodeConstructor(info);
    this->SetFlag(FlagLeaf, true);
}

void LinkSceneNode::Fetch(const Napi::CallbackInfo& info) {
    if (this->href.empty() || this->resource) {
        return;
    }

    auto env{ info.Env() };
    auto resources{ this->GetResources() };

    switch (this->category) {
        case LinkCategoryAuto:
            if (this->HasFontFileExtension(this->href)) {
                this->resource = resources->AcquireFontFace(this->href);
            } else {
                this->resource = resources->AcquireImage(this->href);
            }
            break;
        case LinkCategoryImage:
            this->resource = resources->AcquireImage(this->href);
            break;
        case LinkCategoryFont:
            this->resource = resources->AcquireFontFace(this->href);
            break;
    }

    if (!this->resource) {
        LOG_WARN("Failed to acquire resource: %s", this->href);

        return;
    }

    auto listener{ [this](Resource::Owner owner, Resource* res) { this->ResourceListener(owner, res); } };

    switch (this->resource->GetState()) {
        case Resource::State::Init:
            this->resource->AddListener(this, listener);
            this->resource->Load(env);
            break;
        case Resource::State::Loading:
            this->resource->AddListener(this, listener);
            break;
        case Resource::State::Ready:
        case Resource::State::Error:
            listener(this, this->resource.get());
            break;
    }
}

void LinkSceneNode::ResourceListener(Resource::Owner owner, Resource* res) {
    if (this != owner || this->resource.get() != res) {
        LOG_WARN("Invalid owner or resource: %s", this->href);
        return;
    }

    this->resourceProgress.Dispatch(this, this->resource.get());

    res->RemoveListener(owner);
}

bool LinkSceneNode::HasFontFileExtension(const std::string& uri) const noexcept {
    static const std::array<std::string, 4> fontExtensions{
        { ".ttf", ".ttc", ".otf", ".otc" }
    };

    std::string temp;

    if (GetUriScheme(uri) == UriSchemeFile) {
        temp = GetPathFromFileUri(uri);
    }

    const std::string& path = temp.empty() ? uri : temp;
    const auto dot{ path.find_last_of('.') };

    if (dot != std::string::npos) {
        for (const auto& ext : fontExtensions) {
            if (std::equal(path.begin() + dot, path.end(), ext.begin(), ext.end(),
                    [](char a, char b) noexcept { return tolower(a) == tolower(b); })) {
                return true;
            }
        }
    }

    return false;
}

Napi::Value LinkSceneNode::GetRel(const Napi::CallbackInfo& info) {
    return String::New(info.Env(), ToString(this->relationship));
}

void LinkSceneNode::SetRel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (value.IsString()) {
        auto stringValue{ value.As<String>().Utf8Value() };

        try {
            this->relationship = FromString<LinkRelationship>(stringValue.c_str());
        } catch (const std::invalid_argument& e) {
            throw Error::New(info.Env(), Format("Invalid 'rel' value: %s", stringValue));
        }
    } else if (value.IsUndefined()) {
        this->relationship = LinkRelationshipPreload;
    } else {
        throw Error::New(info.Env(), "Invalid 'rel' value type.");
    }
}

Napi::Value LinkSceneNode::GetAs(const Napi::CallbackInfo& info) {
    return String::New(info.Env(), ToString(this->category));
}

void LinkSceneNode::SetAs(const Napi::CallbackInfo& info, const Napi::Value& value) {
    LinkCategory newCategory;

    if (value.IsString()) {
        auto stringValue{ value.As<String>().Utf8Value() };

        try {
            newCategory = LinkCategoryFromString(stringValue.c_str());
        } catch (const std::invalid_argument& e) {
            throw Error::New(info.Env(), Format("Invalid 'as' value: %s", stringValue));
        }
    } else if (value.IsUndefined()) {
        newCategory = LinkCategoryAuto;
    } else {
        throw Error::New(info.Env(), "Invalid 'as' value type.");
    }

    if (newCategory != this->category) {
        this->category = newCategory;
        this->ClearResource();
    }
}

Napi::Value LinkSceneNode::GetHref(const Napi::CallbackInfo& info) {
    return String::New(info.Env(), this->href);
}

void LinkSceneNode::SetHref(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto env{ info.Env() };
    std::string newHref{};

    switch (value.Type()) {
        case napi_string:
            newHref = value.As<String>();

            if (newHref != this->href) {
                this->href = newHref;
                this->ClearResource();
            }
            break;
        case napi_null:
        case napi_undefined:
            this->href.clear();
            this->ClearResource();
            break;
        default:
            throw Error::New(env, "href must be a string");
    }
}

Napi::Value LinkSceneNode::GetOnLoadCallback(const Napi::CallbackInfo& info) {
    return this->resourceProgress.GetOnLoad(info.Env());
}

void LinkSceneNode::SetOnLoadCallback(const Napi::CallbackInfo& info, const Napi::Value& value) {
    this->resourceProgress.SetOnLoad(info.Env(), value);
}

Napi::Value LinkSceneNode::GetOnErrorCallback(const Napi::CallbackInfo& info) {
    return this->resourceProgress.GetOnError(info.Env());
}

void LinkSceneNode::SetOnErrorCallback(const Napi::CallbackInfo& info, const Napi::Value& value) {
    this->resourceProgress.SetOnError(info.Env(), value);
}

void LinkSceneNode::Destroy() {
    this->resourceProgress.Reset();
    this->ClearResource();

    SceneNode::Destroy();
}

void LinkSceneNode::ClearResource() {
    if (this->resource) {
        auto resourcePtr = this->resource.get();
        this->resource->RemoveListener(this);
        this->resource = nullptr;

        this->GetResources()->ReleaseResource(resourcePtr, true);
    }
}

const char* LinkRelationshipToString(LinkRelationship e) noexcept {
    switch (e) {
        case LinkRelationshipPreload:
            return "preload";
    }

    return "unknown";
}

LinkRelationship LinkRelationshipFromString(const char* value) {
    for (int32_t i = 0; i < Count<LinkRelationship>(); i++) {
        auto e{ static_cast<LinkRelationship>(i) };

        if (strcmp(value, ToString(e)) == 0) {
            return e;
        }
    }

    throw std::invalid_argument(value ? value : "null");
}

const char* LinkCategoryToString(LinkCategory e) noexcept {
    switch (e) {
        case LinkCategoryAuto:
            return "auto";
        case LinkCategoryFont:
            return "font";
        case LinkCategoryImage:
            return "image";
    }

    return "unknown";
}

LinkCategory LinkCategoryFromString(const char* value) {
    for (int32_t i = 0; i < Count<LinkCategory>(); i++) {
        auto e{ static_cast<LinkCategory>(i) };

        if (strcmp(value, ToString(e)) == 0) {
            return e;
        }
    }

    throw std::invalid_argument(value ? value : "null");
}

} // namespace ls
