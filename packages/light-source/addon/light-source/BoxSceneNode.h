/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "Resource.h"
#include "SceneNode.h"

namespace ls {

class ImageResource;
class Texture;

class BoxSceneNode : public Napi::ObjectWrap<BoxSceneNode>, public SceneNode {
 public:
    explicit BoxSceneNode(const Napi::CallbackInfo& info);
    virtual ~BoxSceneNode() = default;

    static Napi::Function Constructor(Napi::Env env);

    void ComputeStyle() override;
    void Paint(Renderer* renderer) override;
    void Composite(CompositeContext* context, Renderer* renderer) override;

    Napi::Reference<Napi::Object>* AsReference() noexcept override { return this; }

 private:
    void DestroyRecursive() override;
    void UpdateStyle(Style* newStyle, Style* oldStyle) override;
    void SetBackgroundImage(const Style* style);

 private:
    std::string backgroundImageUri;
    std::shared_ptr<ImageResource> backgroundImage;
    std::shared_ptr<Texture> layer;
};

} // namespace ls
