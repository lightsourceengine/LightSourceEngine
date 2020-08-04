/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include "Resources.h"
#include "SceneNode.h"

namespace ls {

class Style;

class BoxSceneNode : public Napi::SafeObjectWrap<BoxSceneNode>, public SceneNode {
 public:
    explicit BoxSceneNode(const Napi::CallbackInfo& info);

    void OnPropertyChanged(StyleProperty property) override;

    void BeforeLayout() override;
    void AfterLayout() override;
    void Paint(PaintContext* paint) override;
    void Composite(CompositeContext* composite) override;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;

 private:
    void DestroyRecursive() override;
    void UpdateBackgroundImage(const std::string& backgroundUri);
    void PaintRoundedRect(PaintContext* paint, Style* boxStyle);
    void PaintBackgroundImage(Renderer* renderer, Style* boxStyle);
    void PaintBackgroundStack(Renderer* renderer, Style* boxStyle);
    bool IsBackgroundOnly(Style* boxStyle) const noexcept;
    void ClearBackgroundImageResource();

 private:
    ImageData* backgroundImage{};
    bool isImmediate{ false };

    friend Napi::SafeObjectWrap<BoxSceneNode>;
};

} // namespace ls
