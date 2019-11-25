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
#include <ls/PaintContext.h>
#include <ls/Math.h>
#include <ls/Renderer.h>
#include <ls/Log.h>
#include <ls/PixelConversion.h>
#include <ls-ctx.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::SafeObjectWrap;

namespace ls {

BoxSceneNode::BoxSceneNode(const CallbackInfo& info) : SafeObjectWrap<BoxSceneNode>(info), SceneNode(info) {
}

void BoxSceneNode::Constructor(const Napi::CallbackInfo& info) {
    SceneNode::BaseConstructor(info);
}

Function BoxSceneNode::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "BoxSceneNode", true, SceneNode::Extend<BoxSceneNode>(env, {
            InstanceValue("waypoint", env.Null(), napi_writable),
        }));
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
        case StyleProperty::backgroundClip:
        case StyleProperty::backgroundSize:
        case StyleProperty::backgroundPositionX:
        case StyleProperty::backgroundPositionY:
        case StyleProperty::backgroundWidth:
        case StyleProperty::backgroundHeight:
        case StyleProperty::borderRadius:
        case StyleProperty::borderRadiusBottomRight:
        case StyleProperty::borderRadiusBottomLeft:
        case StyleProperty::borderRadiusTopRight:
        case StyleProperty::borderRadiusTopLeft:
            this->QueuePaint();
            break;
        case StyleProperty::transform:
        case StyleProperty::transformOriginX:
        case StyleProperty::transformOriginY:
        case StyleProperty::opacity:
            this->QueueComposite();
            break;
        default:
            SceneNode::OnPropertyChanged(property);
            break;
    }
}

void BoxSceneNode::BeforeLayout() {
    this->UpdateBackgroundImage(this->style->backgroundImage.value);
}

void BoxSceneNode::AfterLayout() {
    // TODO: a layout change might mean a position change and only a composite is necessary
    this->QueuePaint();
}

void BoxSceneNode::Paint(PaintContext* paint) {
    const auto boxStyle{ this->GetStyleOrEmpty() };

    this->isImmediate = false;

    if (boxStyle->IsLayoutOnly()) {
        return;
    } else if (this->IsBackgroundOnly(boxStyle)) {
        this->isImmediate = true;
    } else if (boxStyle->HasBorderRadius()) {
        this->PaintRoundedRect(paint, boxStyle);
    } else {
        this->PaintBackgroundStack(paint->renderer, boxStyle);
    }

    this->QueueComposite();
}

void BoxSceneNode::Composite(CompositeContext* composite) {
    if (this->isImmediate) {
        const auto boxStyle{ this->GetStyleOrEmpty() };
        const auto rect{ YGNodeLayoutGetRect(this->ygNode) };
        const auto transform{
            ComputeTransform(
                composite->CurrentMatrix(),
                boxStyle->transform,
                boxStyle->transformOriginX,
                boxStyle->transformOriginY,
                rect,
                this->scene)
        };

        composite->renderer->DrawFillRect(rect, transform,
            MixAlpha(boxStyle->backgroundColor.ValueOr(ColorTransparent), composite->CurrentOpacity()));
    } else if (this->layer) {
        const auto boxStyle{ this->GetStyleOrEmpty() };
        const auto rect{ YGNodeLayoutGetRect(this->ygNode) };
        const auto transform{
            ComputeTransform(
                composite->CurrentMatrix(),
                boxStyle->transform,
                boxStyle->transformOriginX,
                boxStyle->transformOriginY,
                rect,
                this->scene)
        };

        composite->renderer->DrawImage(this->layer, rect, transform,
            ARGB(composite->CurrentOpacityAlpha(), 255, 255, 255));
    }

    SceneNode::Composite(composite);
}

bool BoxSceneNode::IsBackgroundOnly(Style* boxStyle) const noexcept {
    return !boxStyle->backgroundColor.empty() && boxStyle->borderColor.empty() && boxStyle->backgroundImage.empty();
}

void BoxSceneNode::PaintBackgroundStack(Renderer* renderer, Style* boxStyle) {
    const auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (!this->InitLayerRenderTarget(renderer, static_cast<int32_t>(dest.width), static_cast<int32_t>(dest.height))) {
        return;
    }

    if (!boxStyle->backgroundColor.empty()) {
        renderer->FillRenderTarget(boxStyle->backgroundColor.value);
    }

    if (this->backgroundImage && this->backgroundImage->Sync(renderer)) {
        this->PaintBackgroundImage(renderer, boxStyle);
    }

    if (!boxStyle->borderColor.empty()) {
        renderer->DrawBorder(dest, YGNodeLayoutGetBorderRect(this->ygNode), boxStyle->borderColor.value);
    }

    renderer->SetRenderTarget(nullptr);
}

void BoxSceneNode::PaintRoundedRect(PaintContext* paint, Style* boxStyle) {
    const auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (!this->InitLayerSoftwareRenderTarget(
        paint->renderer, static_cast<int32_t>(dest.width), static_cast<int32_t>(dest.height))) {
        return;
    }

    auto surface{ this->layer->Lock() };

    // TODO: surface valid?

    auto ctx{ paint->Context2D(surface) };

    // TODO: radius %
    const auto radius{ boxStyle->borderRadius.AsFloat(0) };
    const auto radiusTopLeft{ boxStyle->borderRadiusTopLeft.AsFloat(radius) };
    const auto radiusBottomLeft{ boxStyle->borderRadiusBottomLeft.AsFloat(radius) };
    const auto radiusTopRight{ boxStyle->borderRadiusTopRight.AsFloat(radius) };
    const auto radiusBottomRight{ boxStyle->borderRadiusBottomRight.AsFloat(radius) };
    const auto border = boxStyle->border.AsFloat(0);
    const auto x = border / 2.f;
    const auto y = border / 2.f;
    const auto width = dest.width - 1.f - border;
    const auto height = dest.height - border;
    auto roundedRect = [&](Ctx* ctx, uint32_t color) {
        ctx_set_rgba_u8(ctx, GetR(color), GetG(color), GetB(color), GetA(color));
        ctx_move_to(ctx, x + radiusTopLeft, y);
        ctx_line_to(ctx, x + width - radiusTopRight, y);
        ctx_quad_to(ctx, x + width, y, x + width, y + radiusTopRight);
        ctx_line_to(ctx, x + width, y + height - radiusBottomRight);
        ctx_quad_to(ctx, x + width, y + height, x + width - radiusBottomRight, y + height);
        ctx_line_to(ctx, x + radiusBottomLeft, y + height);
        ctx_quad_to(ctx, x, y + height, x, y + height - radiusBottomLeft);
        ctx_line_to(ctx, x, y + radiusTopLeft);
        ctx_quad_to(ctx, x, y, x + radiusTopLeft, y);
    };

    surface.FillTransparent();

    if (!boxStyle->backgroundColor.empty()) {
        roundedRect(ctx, boxStyle->backgroundColor.value);
        ctx_fill(ctx);
    }

    if (!boxStyle->borderColor.empty() && !boxStyle->border.empty()) {
        ctx_set_line_width(ctx, border);
        roundedRect(ctx, boxStyle->borderColor.value);
        ctx_stroke(ctx);
    }

    this->QueueComposite();
}

void BoxSceneNode::PaintBackgroundImage(Renderer* renderer, Style* boxStyle) {
    const auto dest{
        boxStyle->backgroundClip == StyleBackgroundClipBorderBox ?
            YGNodeLayoutGetRect(this->ygNode, 0, 0) : YGNodeLayoutGetInnerRect(this->ygNode)
    };

    if (!this->InitLayerRenderTarget(renderer, static_cast<int32_t>(dest.width), static_cast<int32_t>(dest.height))) {
        return;
    }

    const auto imageRect{
        ComputeBackgroundImageRect(
            boxStyle->backgroundPositionX,
            boxStyle->backgroundPositionY,
            boxStyle->backgroundWidth,
            boxStyle->backgroundHeight,
            boxStyle->backgroundSize,
            dest,
            this->backgroundImage.Get(),
            this->scene)
    };
    auto drawBackgroundImage = [&](float x, float y) {
        renderer->DrawImage(this->backgroundImage->GetTexture(),
            { x + imageRect.x, y + imageRect.y, imageRect.width, imageRect.height }, ColorWhite);
    };
    float x{ dest.x };
    float y{ dest.y };
    const float x2{ x + dest.width };
    const float y2{ y + dest.height };

    renderer->SetClipRect(dest);

    switch (boxStyle->backgroundRepeat) {
        case StyleBackgroundRepeatXY:
            while (y < y2) {
                x = 0;

                while (x < x2) {
                    drawBackgroundImage(x, y);
                    x+=imageRect.width;
                }

                y+=imageRect.height;
            }
            break;
        case StyleBackgroundRepeatX:
            while (x < x2) {
                drawBackgroundImage(x, y);
                x+=imageRect.width;
            }
            break;
        case StyleBackgroundRepeatY:
            while (y < y2) {
                drawBackgroundImage(x, y);
                y+=imageRect.height;
            }
            break;
        case StyleBackgroundRepeatOff:
            drawBackgroundImage(x, y);
            break;
        default:
            break;
    }

    renderer->ClearClipRect();
    renderer->SetRenderTarget(nullptr);
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
        if (this->backgroundImage->IsReady() || this->backgroundImage->HasError()) {
            this->QueuePaint();
        } else {
            this->backgroundImage.Listen([this]() {
                this->QueuePaint();
                this->backgroundImage.Unlisten();
            });
        }
    }
}

void BoxSceneNode::DestroyRecursive() {
    this->backgroundImage = nullptr;

    SceneNode::DestroyRecursive();
}

} // namespace ls
