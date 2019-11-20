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
            this->QueuePaint();
            break;
        case StyleProperty::transform:
        case StyleProperty::transformOriginX:
        case StyleProperty::transformOriginY:
        case StyleProperty::opacity:
            this->QueueComposite();
            break;
        // TODO: add background fields
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

void BoxSceneNode::Paint(Renderer* renderer) {
    const auto boxStyle{ this->GetStyleOrEmpty() };

    if (boxStyle->IsLayoutOnly()) {
        return;
    }

    if (boxStyle->HasBorderRadius()) {
        this->PaintRoundedRect(renderer, boxStyle);
        this->QueueComposite();
        return;
    }

    // TODO: special cases:
    // TODO: background image + no border + no background + repeat off + size [cover or contain] -> composite
    // TODO: background only -> composite

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

    this->QueueComposite();
    renderer->SetRenderTarget(nullptr);
}

void BoxSceneNode::Composite(CompositeContext* context) {
    if (!this->layer) {
        SceneNode::Composite(context);
        return;
    }

    const auto boxStyle{ this->GetStyleOrEmpty() };
    const auto rect{ YGNodeLayoutGetRect(this->ygNode) };

    context->renderer->DrawImage(
        this->layer,
        rect,
        context->CurrentMatrix() * boxStyle->transform.ToMatrix(rect.width, rect.height),
        ARGB(context->CurrentOpacityAlpha(), 255, 255, 255));

    // TODO: special case - image only
    // TODO: speical case - background only

    SceneNode::Composite(context);
}

void BoxSceneNode::PaintRoundedRect(Renderer* renderer, Style* boxStyle) {
    const auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };

    if (!this->InitLayerSoftwareRenderTarget(
        renderer, static_cast<int32_t>(dest.width), static_cast<int32_t>(dest.height))) {
        return;
    }

    auto surface{ this->layer->Lock() };

    // TODO: surface valid?

    auto ctx = ctx_new_for_framebuffer(
        surface.Pixels(), surface.Width(), surface.Height(), surface.Pitch(), CTX_FORMAT_RGBA8);

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

    ctx_free(ctx);
    this->QueueComposite();
}

void BoxSceneNode::PaintBackgroundImage(Renderer* renderer, Style* boxStyle) {
    const auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };
    const auto width{ this->backgroundImage->GetWidthF() };
    const auto height{ this->backgroundImage->GetHeightF() };

    switch (boxStyle->backgroundRepeat) {
        case StyleBackgroundRepeatXY:
            for (float y = 0.f; y < dest.height; y+=height) {
                for (float x = 0.f; x < dest.width; x+=width) {
                    renderer->DrawImage(this->backgroundImage->GetTexture(),
                        { x, y, width, height },
                        this->backgroundImage->GetCapInsets(),
                        ColorWhite);
                }
            }
            break;
        case StyleBackgroundRepeatX:
            for (float x = 0.f; x < dest.width; x+=width) {
                renderer->DrawImage(this->backgroundImage->GetTexture(),
                    { x, 0, width, height },
                    this->backgroundImage->GetCapInsets(),
                    ColorWhite);
            }
            break;
        case StyleBackgroundRepeatY:
            for (float y = 0.f; y < dest.height; y+=height) {
                renderer->DrawImage(this->backgroundImage->GetTexture(),
                    { 0, y, width, height },
                    this->backgroundImage->GetCapInsets(),
                    ColorWhite);
            }
            break;
        case StyleBackgroundRepeatOff:
            renderer->DrawImage(this->backgroundImage->GetTexture(),
                { 0, 0, width, height },
                this->backgroundImage->GetCapInsets(),
                ColorWhite);
            break;
        default:
            break;
    }
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
