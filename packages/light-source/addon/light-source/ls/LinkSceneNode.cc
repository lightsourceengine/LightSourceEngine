/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "LinkSceneNode.h"
#include "Scene.h"
#include "Stage.h"
#include <ls/Log.h>
#include <unordered_map>

using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectReference;
using Napi::String;

namespace ls {

ObjectReference CreateMap(Napi::Env env, const std::unordered_map<uint32_t, std::string>& entries);

Napi::FunctionReference LinkSceneNode::constructor;
Napi::ObjectReference LinkSceneNode::relationshipMap;
Napi::ObjectReference LinkSceneNode::categoryMap;

LinkSceneNode::LinkSceneNode(const Napi::CallbackInfo& info) : SafeObjectWrap<LinkSceneNode>(info), SceneNode(info) {
}

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

        LinkSceneNode::relationshipMap = CreateMap(env, {
            { LinkRelationshipPreload, "preload" }
        });

        LinkSceneNode::categoryMap = CreateMap(env, {
            {LinkCategoryImage, "image" },
            {LinkCategoryFont, "font" },
        });
    }

    return constructor.Value();
}

void LinkSceneNode::Constructor(const Napi::CallbackInfo& info) {
    SceneNode::BaseConstructor(info, SceneNodeTypeLink);
}

void LinkSceneNode::Fetch(const Napi::CallbackInfo& info) {
    if (this->href.empty() || this->resource) {
        return;
    }

    auto env{ info.Env() };
    auto resources{ this->GetStage()->GetResources() };

    switch (this->category) {
        case LinkCategoryImage:
            this->resource = resources->AcquireImageData(this->href);
            break;
        case LinkCategoryFont:
            this->resource = resources->AcquireFontFace(this->href);
            break;
    }

    if (!this->resource) {
        LOG_WARN("Failed to acquire resource: %s", this->href);

        return;
    }

    auto listener{ [this](Res::Owner owner, Res* res) { this->ResourceListener(owner, res); } };

    switch (this->resource->GetState()) {
        case Res::State::Init:
            this->resource->AddListener(this, listener);
            this->resource->Load(env);
            break;
        case Res::State::Loading:
            this->resource->AddListener(this, listener);
            break;
        case Res::State::Ready:
        case Res::State::Error:
            listener(this, this->resource);
            break;
    }
}

void LinkSceneNode::ResourceListener(Res::Owner owner, Res* res) {
    if (this != owner || this->resource != res) {
        LOG_WARN("Invalid owner or resource: %s", this->href);
        return;
    }

    Napi::HandleScope scope(this->Env());

    switch (res->GetState()) {
        case Res::Ready:
            if (!this->onLoadCallback.IsEmpty()) {
                try {
                    this->onLoadCallback.Call({this->Value(), res->GetSummary(this->Env())});
                } catch (std::exception& e) {
                    LOG_WARN("onLoad unhandled exception: %s", e);
                }
            }
            break;
        case Res::Error:
            if (!this->onErrorCallback.IsEmpty()) {
                try {
                    this->onErrorCallback.Call({this->Value(), res->GetErrorMessage(this->Env())});
                } catch (std::exception& e) {
                    LOG_WARN("onError unhandled exception: %s", e);
                }
            }
            break;
        default:
            break;
    }

    res->RemoveListener(owner);
}

Napi::Value LinkSceneNode::GetRel(const Napi::CallbackInfo& info) {
    return LinkSceneNode::relationshipMap.Get(this->relationship);
}

void LinkSceneNode::SetRel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto env{ info.Env() };

    if (value.IsString()) {
        HandleScope scope(env);
        auto enumValue{ LinkSceneNode::relationshipMap.Value().Get(value) };

        if (enumValue.IsNumber()) {
            this->relationship = static_cast<LinkRelationship>(enumValue.As<Number>().Int32Value());
        } else {
            throw Error::New(env, "Invalid 'rel' value.");
        }
    } else if (value.IsUndefined()) {
        this->relationship = LinkRelationshipPreload;
    } else {
        throw Error::New(env, "Invalid 'rel' value type.");
    }
}

Napi::Value LinkSceneNode::GetAs(const Napi::CallbackInfo& info) {
    return LinkSceneNode::categoryMap.Get(this->category);
}

void LinkSceneNode::SetAs(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto env{ info.Env() };
    LinkCategory newCategory;

    if (value.IsString()) {
        HandleScope scope(env);
        auto enumValue{ LinkSceneNode::categoryMap.Value().Get(value) };

        if (enumValue.IsNumber()) {
            newCategory = static_cast<LinkCategory>(enumValue.As<Number>().Int32Value());
        } else {
            throw Error::New(env, "Invalid 'as' value.");
        }
    } else if (value.IsUndefined()) {
        newCategory = LinkCategoryImage;
    } else {
        throw Error::New(env, "Invalid 'as' value type.");
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
    return this->onLoadCallback.Value();
}

void LinkSceneNode::SetOnLoadCallback(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (!Napi::AssignFunctionReference(this->onLoadCallback, value)) {
        throw Error::New(info.Env(), "Invalid assignment of onLoad.");
    }
}

Napi::Value LinkSceneNode::GetOnErrorCallback(const Napi::CallbackInfo& info) {
    return this->onErrorCallback.Value();
}

void LinkSceneNode::SetOnErrorCallback(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (!Napi::AssignFunctionReference(this->onErrorCallback, value)) {
        throw Error::New(info.Env(), "Invalid assignment of onError.");
    }
}

void LinkSceneNode::AppendChild(SceneNode* child) {
    throw Error::New(this->Env(), "appendChild() is an unsupported operation on link nodes");
}

void LinkSceneNode::DestroyRecursive() {
    this->onLoadCallback.Reset();
    this->onErrorCallback.Reset();
    this->ClearResource();

    SceneNode::DestroyRecursive();
}

void LinkSceneNode::ClearResource() {
    if (this->resource) {
        this->resource->RemoveListener(this);
        this->GetStage()->GetResources()->ReleaseResource(this->resource, true);
        this->resource = nullptr;
    }
}

ObjectReference CreateMap(Napi::Env env, const std::unordered_map<uint32_t, std::string>& entries) {
    auto map{ Object::New(env) };

    for (const auto &p : entries) {
        auto stringValue{ String::New(env, p.second) };

        map.Set(p.first, stringValue);
        map.Set(stringValue, Number::New(env, p.first));
    }

    return Napi::Permanent(map);
}

} // namespace ls
