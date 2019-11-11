/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "Resource.h"
#include "SceneNode.h"
#include "ImageUri.h"

namespace ls {

class ImageResource;

class BoxSceneNode : public Napi::ObjectWrap<BoxSceneNode>, public SceneNode {
 public:
    explicit BoxSceneNode(const Napi::CallbackInfo& info);
    virtual ~BoxSceneNode() = default;

    static Napi::Function Constructor(Napi::Env env);

    void OnPropertyChanged(StyleProperty property) override;
    void BeforeLayout() override;
    void AfterLayout() override;
    void Paint(Renderer* renderer) override;
    void Composite(CompositeContext* context) override;

    Napi::Reference<Napi::Object>* AsReference() noexcept override { return this; }

 private:
    void DestroyRecursive() override;
    void UpdateBackgroundImage(const ImageUri& imageUri);

 private:
    ImageUri backgroundImageUri;
    ResourceLink<ImageResource> backgroundImage;
};

} // namespace ls
