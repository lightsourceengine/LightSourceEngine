/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include "AsyncTaskQueue.h"
#include "FontResource.h"
#include "ImageResource.h"

namespace ls {

class Renderer;
class LayerResource;

class ResourceManager : public Napi::ObjectWrap<ResourceManager> {
 public:
    explicit ResourceManager(const Napi::CallbackInfo& info);
    virtual ~ResourceManager() = default;

    static Napi::Function Constructor(Napi::Env env);

    void RegisterImage(const Napi::CallbackInfo& info);
    void AddFont(const Napi::CallbackInfo& info);

    Napi::Value GetImageExtensions(const Napi::CallbackInfo& info);
    void SetImageExtensions(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Value GetPath(const Napi::CallbackInfo& info);
    void SetPath(const Napi::CallbackInfo& info, const Napi::Value& value);

    Napi::Value GetFonts(const Napi::CallbackInfo& info);
    Napi::Value GetImages(const Napi::CallbackInfo& info);

    void PostConstruct(Renderer* renderer);

    void Attach();
    void Detach();
    void ProcessEvents();
    void Destroy();

    ImageResource* GetImage(const std::string& id);
    ImageResource* LoadImage(const ImageUri& uri);

    FontResource* FindFont(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);

    LayerResource* CreateLayerResource();
    void RemoveLayerResource(LayerResource* layerResource);

 private:
    FontResource* FindFontInternal(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);
    void LoadFont(const std::string& id, const std::string& uri, const int32_t index,
        const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight);

 private:
    Renderer* renderer{};
    std::unordered_map<std::string, std::shared_ptr<ImageResource>> images;
    std::unordered_map<std::string, std::shared_ptr<FontResource>> fonts;
    std::unordered_set<LayerResource*> layers;
    std::unordered_map<std::string, ImageUri> registeredImageUris;
    std::vector<std::string> imageExtensions{ ".jpg", ".jpeg", ".png", ".gif", ".svg" };
    std::vector<std::string> path;
    AsyncTaskQueue asyncTaskQueue;
};

} // namespace ls
