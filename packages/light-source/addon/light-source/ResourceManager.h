/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include "FontResource.h"
#include "ImageResource.h"

namespace ls {

class Renderer;

class ResourceManager : public Napi::ObjectWrap<ResourceManager> {
 public:
    explicit ResourceManager(const Napi::CallbackInfo& info);
    virtual ~ResourceManager() = default;

    static Napi::Function Constructor(Napi::Env env);

    void RegisterImage(const Napi::CallbackInfo& info);
    void RegisterFont(const Napi::CallbackInfo& info);

    Napi::Value GetImageExtensions(const Napi::CallbackInfo& info);
    void SetImageExtensions(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Value GetPath(const Napi::CallbackInfo& info);
    void SetPath(const Napi::CallbackInfo& info, const Napi::Value& value);

    void Attach(Renderer* renderer);
    void Detach();

    void ProcessEvents();

    ImageResource* GetImage(const std::string& id);

    FontResource* FindFont(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);

 private:
    Renderer* renderer{};
    std::unordered_map<std::string, std::shared_ptr<ImageResource>> images;
    std::unordered_map<std::string, std::shared_ptr<FontResource>> fonts;
    std::vector<std::string> imageExtensions;
    Napi::ObjectReference imageExtensionsObject;
    std::vector<std::string> path;
    Napi::ObjectReference pathObject;
};

} // namespace ls
