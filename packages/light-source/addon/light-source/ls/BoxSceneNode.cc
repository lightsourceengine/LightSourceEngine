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
#include <ls/CompositeContext.h>
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

void BoxSceneNode::OnPropertyChanged(StyleProperty property) {
    switch (property) {
        case StyleProperty::backgroundImage:
            this->QueueBeforeLayout();
            break;
        case StyleProperty::backgroundColor:
        case StyleProperty::borderColor:
            this->QueuePaint();
            break;
        // TODO: add background fields
        default:
            if (IsYogaLayoutProperty(property)) {
                this->QueueAfterLayout();
            }
            break;
    }
}

void BoxSceneNode::BeforeLayout() {
    this->UpdateBackgroundImage(this->style->backgroundImage.value);
}

void BoxSceneNode::AfterLayout() {
    if (YGNodeGetHasNewLayout(this->ygNode)) {
        YGNodeSetHasNewLayout(this->ygNode, false);
        // TODO: a layout change might mean a position change and only a composite is necessary
        this->QueuePaint();
    }
}

void BoxSceneNode::Composite(CompositeContext* context) {
    if (!this->layer) {
        SceneNode::Composite(context);
        return;
    }

    const auto& transform{ context->CurrentMatrix() };
    auto rect{ YGNodeLayoutGetRect(this->ygNode) };

    rect.x += transform.GetTranslateX();
    rect.y += transform.GetTranslateY();

    context->renderer->DrawImage(this->layer, rect, RGB(255, 255, 255));

    SceneNode::Composite(context);
}

void BoxSceneNode::Paint(Renderer* renderer) {
    const auto boxStyle{ this->GetStyleOrEmpty() };

    if (boxStyle->IsLayoutOnly()) {
        return;
    }

    const auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (!this->InitLayerRenderTarget(renderer, static_cast<int32_t>(dest.width), static_cast<int32_t>(dest.height))) {
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
    this->QueueComposite();
}

void BoxSceneNode::UpdateBackgroundImage(const ImageUri& imageUri) {
    if (imageUri.IsEmpty()) {
        if (this->backgroundImage) {
            this->QueuePaint();
        }

        this->backgroundImage = nullptr;

        return;
    }

    if (this->backgroundImage && this->backgroundImage->GetUri() == imageUri) {
        return;
    }

    this->backgroundImage = this->scene->GetImageStore()->GetOrLoadImage({ imageUri });

    if (this->backgroundImage) {
        if (this->backgroundImage->IsReady()) {
            this->QueuePaint();
        } else {
            this->backgroundImage.Listen([this]() {
                this->QueuePaint();
            });
        }
    }
}

void BoxSceneNode::DestroyRecursive() {
    this->backgroundImage = nullptr;

    SceneNode::DestroyRecursive();
}

} // namespace ls
