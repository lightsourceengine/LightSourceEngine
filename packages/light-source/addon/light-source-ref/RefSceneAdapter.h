/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "RefRenderer.h"
#include <SceneAdapter.h>
#include <memory>

namespace ls {

class RefSceneAdapter : public SceneAdapter {
 public:
    explicit RefSceneAdapter(const SceneAdapterConfig& config);
    virtual ~RefSceneAdapter();

    void Attach() override;
    void Detach() override;
    void Resize(int32_t width, int32_t height, bool fullscreen) override;

    void SetTitle(const std::string& title) override { this->title = title; }
    std::string GetTitle() const override { return this->title; }
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    bool GetFullscreen() const override;
    Renderer* GetRenderer() const override;

 private:
    SceneAdapterConfig config;
    mutable RefRenderer renderer;
    std::string title;
};

} // namespace ls
