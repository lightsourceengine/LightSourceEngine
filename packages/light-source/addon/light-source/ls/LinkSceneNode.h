/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <ls/ResourceProgress.h>
#include <ls/Resources.h>
#include <ls/SceneNode.h>
#include <napi-ext.h>

namespace ls {

enum LinkRelationship {
    LinkRelationshipPreload
};

enum LinkCategory {
    LinkCategoryImage,
    LinkCategoryFont
};

class LinkSceneNode : public Napi::SafeObjectWrap<LinkSceneNode>, public SceneNode {
 public:
    LinkSceneNode(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<LinkSceneNode>(info) {}
    ~LinkSceneNode() override = default;

    static Napi::Function GetClass(Napi::Env env);
    void Constructor(const Napi::CallbackInfo& info) override;
    void Fetch(const Napi::CallbackInfo& info);
    Napi::Value GetRel(const Napi::CallbackInfo& info);
    void SetRel(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetAs(const Napi::CallbackInfo& info);
    void SetAs(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetHref(const Napi::CallbackInfo& info);
    void SetHref(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetOnLoadCallback(const Napi::CallbackInfo& info);
    void SetOnLoadCallback(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetOnErrorCallback(const Napi::CallbackInfo& info);
    void SetOnErrorCallback(const Napi::CallbackInfo& info, const Napi::Value& value);

    void Paint(GraphicsContext* graphicsContext) override {}
    void Composite(CompositeContext* composite) override {}

 private:
    void DestroyRecursive() override;
    void ClearResource();
    void ResourceListener(Res::Owner owner, Res* res);

 private:
    static Napi::FunctionReference constructor;
    static Napi::ObjectReference relationshipMap;
    static Napi::ObjectReference categoryMap;

    LinkRelationship relationship{ LinkRelationshipPreload };
    LinkCategory category{ LinkCategoryImage };
    std::string href{};
    Res* resource{};
    ResourceProgress resourceProgress;
};

} // namespace ls
