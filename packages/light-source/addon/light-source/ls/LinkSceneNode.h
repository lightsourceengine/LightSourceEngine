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

LS_ENUM_SEQ_DECL(
    LinkRelationship,
    LinkRelationshipPreload
)

LS_ENUM_SEQ_DECL(
    LinkCategory,
    LinkCategoryAuto,
    LinkCategoryImage,
    LinkCategoryFont
)

class LinkSceneNode final : public Napi::SafeObjectWrap<LinkSceneNode>, public SceneNode {
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

    void Paint(RenderingContext2D* context) override {}
    void Composite(CompositeContext* composite) override {}
    void Destroy() override;

 private:
    void ClearResource();
    void ResourceListener(Resource::Owner owner, Resource* res);
    bool HasFontFileExtension(const std::string& path) const noexcept;

 private:
    static Napi::FunctionReference constructor;

    LinkRelationship relationship{ LinkRelationshipPreload };
    LinkCategory category{ LinkCategoryAuto };
    std::string href{};
    Resource* resource{};
    ResourceProgress resourceProgress;
};

} // namespace ls
