/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "LinkSceneNode.h"
#include <ls/Log.h>

using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectReference;
using Napi::String;

namespace ls {

ObjectReference CreateMap(Napi::Env env, const std::map <uint32_t, std::string>& entries);

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
            "LinkSceneNode", true,
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
    SceneNode::BaseConstructor(info);
}

void LinkSceneNode::Fetch(const Napi::CallbackInfo& info) {
    // TODO: load font or image
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

    if (value.IsString()) {
        HandleScope scope(env);
        auto enumValue{ LinkSceneNode::categoryMap.Value().Get(value) };

        if (enumValue.IsNumber()) {
            this->category = static_cast<LinkCategory>(enumValue.As<Number>().Int32Value());
        } else {
            throw Error::New(env, "Invalid 'as' value.");
        }
    } else if (value.IsUndefined()) {
        this->category = LinkCategoryImage;
    } else {
        throw Error::New(env, "Invalid 'as' value type.");
    }
}

Napi::Value LinkSceneNode::GetHref(const Napi::CallbackInfo& info) {
    return String::New(info.Env(), this->href);
}

void LinkSceneNode::SetHref(const Napi::CallbackInfo& info, const Napi::Value& value) {
    auto env{ info.Env() };

    if (value.IsString()) {
        this->href = value.As<String>();
    } else if (value.IsNull() || value.IsUndefined()) {
        this->href.clear();
    } else {
        throw Error::New(env, "Invalid 'href' value type.");
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

    SceneNode::DestroyRecursive();
}

ObjectReference CreateMap(Napi::Env env, const std::map <uint32_t, std::string>& entries) {
    auto map{ Object::New(env) };

    for (const auto &p : entries) {
        auto stringValue{ String::New(env, p.second) };

        map.Set(p.first, stringValue);
        map.Set(stringValue, Number::New(env, p.first));
    }

    return Napi::Permanent(map);
}

} // namespace ls
