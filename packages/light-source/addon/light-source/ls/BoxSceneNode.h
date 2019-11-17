/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include "Resource.h"
#include "SceneNode.h"
#include "ImageUri.h"

namespace ls {

class ImageResource;

class BoxSceneNode : public Napi::SafeObjectWrap<BoxSceneNode>, public SceneNode {
 public:
    explicit BoxSceneNode(const Napi::CallbackInfo& info);
    virtual ~BoxSceneNode() = default;

    void OnPropertyChanged(StyleProperty property) override;

    void BeforeLayout() override;
    void AfterLayout() override;
    void Paint(Renderer* renderer) override;
    void Composite(CompositeContext* context) override;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;

 private:
    void DestroyRecursive() override;
    void UpdateBackgroundImage(const ImageUri& imageUri);

 private:
    ImageUri backgroundImageUri;
    ResourceLink<ImageResource> backgroundImage;

    friend Napi::SafeObjectWrap<BoxSceneNode>;
};

} // namespace ls
