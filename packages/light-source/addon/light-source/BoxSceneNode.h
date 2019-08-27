/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "SceneNode.h"

namespace ls {

class ImageResource;

class BoxSceneNode : public Napi::ObjectWrap<BoxSceneNode>, public SceneNode {
 public:
    explicit BoxSceneNode(const Napi::CallbackInfo& info);
    virtual ~BoxSceneNode() = default;

    static Napi::Function Constructor(Napi::Env env);

    void Paint(Renderer* renderer) override;

    Napi::Reference<Napi::Object>* AsReference() override { return this; }

 private:
    void DestroyRecursive() override;
    void UpdateStyle(Style* newStyle, Style* oldStyle) override;
    void ClearBackgroundImage();
    void SetRoundedRectImage(ImageResource* image);
    void SetRoundedRectStrokeImage(ImageResource* image);

 private:
    std::string backgroundImageUri{};
    ImageResource* backgroundImage{};
    ImageResource* roundedRectImage{};
    ImageResource* roundedRectStrokeImage{};
};

} // namespace ls
