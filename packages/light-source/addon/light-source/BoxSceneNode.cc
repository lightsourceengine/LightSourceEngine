/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "BoxSceneNode.h"
#include "ImageResource.h"
#include "ImageStore.h"
#include "Scene.h"
#include "Style.h"
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

    const auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (!this->InitLayerRenderTarget(renderer, static_cast<int32_t>(dest.width), static_cast<int32_t>(dest.height))) {
        SceneNode::Paint(renderer);
        return;
    }

    if (!boxStyle->backgroundColor.empty()) {
        renderer->FillRenderTarget(boxStyle->backgroundColor.value);
    }

    if (this->backgroundImage && this->backgroundImage->Sync(renderer)) {
        if (boxStyle->backgroundRepeat == StyleBackgroundRepeatXY) {
            for (float y = 0.f; y < dest.height; y+=this->backgroundImage->GetHeightF()) {
                for (float x = 0.f; x < dest.width; x+=this->backgroundImage->GetWidthF()) {
                    renderer->DrawImage(this->backgroundImage->GetTexture(),
                        { x, y, this->backgroundImage->GetWidthF(), this->backgroundImage->GetHeightF() },
                        this->backgroundImage->GetCapInsets(), RGB(255, 255, 255));
                }
            }
        }
//        if (this->backgroundImage->HasCapInsets()) {
//            renderer->DrawImage(this->backgroundImage->GetTexture(), dest,
//                this->backgroundImage->GetCapInsets(), RGB(255, 255, 255));
//        } else {
//            const auto tintColor{ boxStyle->tintColor() ? boxStyle->tintColor()->Get() : RGB(255, 255, 255) };
//
//            renderer->DrawImage(this->backgroundImage->GetTexture(), dest, tintColor);
//        }
    }

    if (!boxStyle->borderColor.empty()) {
        renderer->DrawBorder(dest, YGNodeLayoutGetBorderRect(this->ygNode), boxStyle->borderColor.value);
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

void BoxSceneNode::OnPropertyChanged(StyleProperty property) {
    if (property == StyleProperty::backgroundImage) {
        this->SetBackgroundImage(this->style->backgroundImage);
    }
}

void BoxSceneNode::SetBackgroundImage(const std::string& imageUri) {
    if (imageUri.empty()) {
        this->backgroundImage = nullptr;
        return;
    }

    if (this->backgroundImage && this->backgroundImage->GetUri().GetId() == imageUri) {
        return;
    }

    this->backgroundImage = this->scene->GetImageStore()->GetOrLoadImage({ imageUri });

    // TODO: listen for load event and mark dirty
}

void BoxSceneNode::DestroyRecursive() {
    this->backgroundImage = nullptr;

    SceneNode::DestroyRecursive();
}

} // namespace ls
