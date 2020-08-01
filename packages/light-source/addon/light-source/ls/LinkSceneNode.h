/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include "SceneNode.h"

namespace ls {

class Style;

enum LinkRelationship {
    LinkRelationshipPreload
};

enum LinkCategory {
    LinkCategoryImage,
    LinkCategoryFont
};

class LinkSceneNode : public Napi::SafeObjectWrap<LinkSceneNode>, public SceneNode {
 public:
    explicit LinkSceneNode(const Napi::CallbackInfo& info);

    static Napi::Function GetClass(Napi::Env env);

    void OnPropertyChanged(StyleProperty property) override {}
    void BeforeLayout() override {}
    void AfterLayout() override {}
    void Paint(PaintContext* paint) override {}
    void Composite(CompositeContext* composite) override {}

 private: // javascript bindings
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

 private:
    void AppendChild(SceneNode* child) override;
    void DestroyRecursive() override;

 private:
    static Napi::FunctionReference constructor;
    static Napi::ObjectReference relationshipMap;
    static Napi::ObjectReference categoryMap;

    LinkRelationship relationship{ LinkRelationshipPreload };
    LinkCategory category{ LinkCategoryImage };
    std::string href;
    Napi::FunctionReference onLoadCallback;
    Napi::FunctionReference onErrorCallback;

    friend Napi::SafeObjectWrap<LinkSceneNode>;
};

} // namespace ls
