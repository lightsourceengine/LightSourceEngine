/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "BoxSceneNode.h"
#include "ImageResource.h"
#include "ImageStore.h"
#include "Scene.h"
#include "StyleUtils.h"
#include "yoga-ext.h"
#include "CompositeContext.h"
#include <ls/Math.h>
#include <ls/Renderer.h>
#include <ls/Log.h>
#include <ls/PixelConversion.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;

namespace ls {

BoxSceneNode::BoxSceneNode(const CallbackInfo& info) : ObjectWrap<BoxSceneNode>(info), SceneNode(info) {
}

Function BoxSceneNode::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func{ DefineClass(env, "BoxSceneNode", SceneNode::Extend<BoxSceneNode>(env, {
            InstanceValue("waypoint", env.Null(), napi_writable),
        })) };

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void BoxSceneNode::ComputeStyle() {
    SceneNode::ComputeStyle();
}

void BoxSceneNode::Paint(Renderer* renderer) {
    const auto boxStyle{ this->GetStyleOrEmpty() };

    if (boxStyle->IsLayoutOnly()) {
        SceneNode::Paint(renderer);
        return;
    }

    auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (!this->layer) {
        this->layer = renderer->CreateRenderTarget(
            static_cast<int32_t>(dest.width), static_cast<int32_t>(dest.height));
    }

    renderer->SetRenderTarget(this->layer);

    if (boxStyle->backgroundColor()) {
        renderer->FillRenderTarget(boxStyle->backgroundColor()->Get());
    }

    if (this->backgroundImage && this->backgroundImage->GetTexture()) {
        if (this->backgroundImage->HasCapInsets()) {
            renderer->DrawImage(this->backgroundImage->GetTexture(), dest,
                this->backgroundImage->GetCapInsets(), RGB(255, 255, 255));
        } else {
            renderer->DrawImage(this->backgroundImage->GetTexture(), dest, tintColor);
        }
    }

    if (boxStyle->borderColor()) {
        renderer->DrawBorder(dest, YGNodeLayoutGetBorderRect(this->ygNode), boxStyle->borderColor()->Get());
    }

    renderer->SetRenderTarget(nullptr);
    SceneNode::Paint(renderer);
}

void BoxSceneNode::Composite(CompositeContext* context, Renderer* renderer) {
    if (!this->layer) {
        SceneNode::Composite(context, renderer);
        return;
    }

    const auto& transform{ context->CurrentMatrix() };
    auto rect{ YGNodeLayoutGetRect(this->ygNode) };

    rect.x += transform.GetTranslateX();
    rect.y += transform.GetTranslateY();

    renderer->DrawImage(this->layer, rect, RGB(255, 255, 255));

    SceneNode::Composite(context, renderer);
}

void BoxSceneNode::UpdateStyle(Style* newStyle, Style* oldStyle) {
    SceneNode::UpdateStyle(newStyle, oldStyle);

    this->SetBackgroundImage(newStyle);
}

void BoxSceneNode::SetBackgroundImage(const Style* style) {
    if (!style || style->backgroundImage().empty()) {
        this->backgroundImage = nullptr;
        return;
    }

    if (this->backgroundImage && this->backgroundImage->GetUri().GetId() == style->backgroundImage()) {
        return;
    }

    this->backgroundImage = this->scene->GetImageStore()->GetOrLoadImage({ style->backgroundImage() });
}

void BoxSceneNode::DestroyRecursive() {
    this->backgroundImage = nullptr;
    this->layer = nullptr;

    SceneNode::DestroyRecursive();
}

} // namespace ls
