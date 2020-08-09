/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/BoxSceneNode.h>

#include <ls/Scene.h>
#include <ls/Stage.h>
#include <ls/Style.h>
#include <ls/StyleUtils.h>
#include <ls/CompositeContext.h>
#include <ls/Math.h>
#include <ls/Renderer.h>
#include <ls/Log.h>
#include <ls/PixelConversion.h>
#include <ls-ctx.h>
#include <ls/yoga-ext.h>
#include <ls/GraphicsContext.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::SafeObjectWrap;

namespace ls {

void BoxSceneNode::Constructor(const Napi::CallbackInfo& info) {
    this->SceneNodeConstructor(info, SceneNodeTypeBox);
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
            // this->QueueBeforeLayout();
            if (!this->style->backgroundImage.empty()) {
                this->UpdateBackgroundImage(this->style->backgroundImage);
            }
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

void BoxSceneNode::Paint(GraphicsContext* graphicsContext) {
    const auto boxStyle{ this->style };

    if (boxStyle == nullptr || boxStyle->IsLayoutOnly()) {
        return;
    }

    if (boxStyle->backgroundRepeat == StyleBackgroundRepeatOff) {
        this->QueueComposite();
        return;
    }

    this->QueueComposite();
}

void BoxSceneNode::Composite(CompositeContext* composite) {
    const auto boxStyle{ this->style };

    if (boxStyle == nullptr || boxStyle->IsLayoutOnly()) {
        return;
    }

    const auto rect{ YGNodeLayoutGetRect(this->ygNode) };
    const auto transform{ ComputeTransform(composite->CurrentMatrix(), boxStyle->transform,
        boxStyle->transformOriginX, boxStyle->transformOriginY, rect, this->scene) };

    if (!boxStyle->backgroundColor.empty()) {
        composite->renderer->DrawFillRect(
            rect,
            transform,
            MixAlpha(boxStyle->backgroundColor.value, composite->CurrentOpacity()));
    }

    if (this->backgroundImage) {
        if (!this->backgroundImage->HasTexture()) {
            this->backgroundImage->LoadTexture(composite->renderer);
        }

        // TODO: use background position

        if (this->backgroundImage->HasTexture()) {
            composite->renderer->DrawImage(this->backgroundImage->GetTexture(), rect, transform,
                     MixAlpha(ColorWhite, composite->CurrentOpacity()));
        }
    }

    if (!boxStyle->borderColor.empty()) {
        composite->renderer->DrawBorder(
            rect,
            YGNodeLayoutGetBorderRect(this->ygNode),
            transform,
            MixAlpha(boxStyle->borderColor.value, composite->CurrentOpacity()));
    }

    SceneNode::Composite(composite);
}

//void BoxSceneNode::PaintRoundedRect(PaintContext* paint, Style* boxStyle) {
//    const auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };
//
//    this->QueueComposite();
//
//    if (!this->InitLayerSoftwareRenderTarget(paint->renderer,
//            static_cast<int32_t>(dest.width), static_cast<int32_t>(dest.height))) {
//        return;
//    }
//
//    auto surface{ this->layer->Lock() };
//
//    if (surface.IsEmpty()) {
//        return;
//    }
//
//    auto ctx{ paint->Context2D(surface) };
//
//    const auto radius{ ComputeBorderWidth(boxStyle->borderRadius, 0, this->scene) };
//    const auto radiusTopLeft{ ComputeBorderWidth(boxStyle->borderRadiusTopLeft, radius, this->scene) };
//    const auto radiusBottomLeft{ ComputeBorderWidth(boxStyle->borderRadiusBottomLeft, radius, this->scene) };
//    const auto radiusTopRight{ ComputeBorderWidth(boxStyle->borderRadiusTopRight, radius, this->scene) };
//    const auto radiusBottomRight{ ComputeBorderWidth(boxStyle->borderRadiusBottomRight, radius, this->scene) };
//    const auto border{ ComputeBorderWidth(boxStyle->border, 0, this->scene) };
//    const auto x{ border / 2.f };
//    const auto y{ border / 2.f };
//    const auto width{ dest.width - 1.f - border };
//    const auto height{ dest.height - border };
//    auto roundedRect = [&](Ctx* ctx, uint32_t color) {
//        ctx_set_rgba_u8(ctx, GetR(color), GetG(color), GetB(color), GetA(color));
//        ctx_move_to(ctx, x + radiusTopLeft, y);
//        ctx_line_to(ctx, x + width - radiusTopRight, y);
//        ctx_quad_to(ctx, x + width, y, x + width, y + radiusTopRight);
//        ctx_line_to(ctx, x + width, y + height - radiusBottomRight);
//        ctx_quad_to(ctx, x + width, y + height, x + width - radiusBottomRight, y + height);
//        ctx_line_to(ctx, x + radiusBottomLeft, y + height);
//        ctx_quad_to(ctx, x, y + height, x, y + height - radiusBottomLeft);
//        ctx_line_to(ctx, x, y + radiusTopLeft);
//        ctx_quad_to(ctx, x, y, x + radiusTopLeft, y);
//    };
//
//    surface.FillTransparent();
//
//    if (!boxStyle->backgroundColor.empty()) {
//        roundedRect(ctx, boxStyle->backgroundColor.value);
//        ctx_fill(ctx);
//    }
//
//    if (!boxStyle->borderColor.empty() && !boxStyle->border.empty()) {
//        ctx_set_line_width(ctx, border);
//        roundedRect(ctx, boxStyle->borderColor.value);
//        ctx_stroke(ctx);
//    }
//}

void BoxSceneNode::PaintBackgroundImage(Renderer* renderer, Style* boxStyle) {
//    const auto dest{
//        boxStyle->backgroundClip == StyleBackgroundClipBorderBox ?
//            YGNodeLayoutGetRect(this->ygNode, 0, 0) : YGNodeLayoutGetInnerRect(this->ygNode)
//    };
//
//    if (!this->InitLayerRenderTarget(renderer, static_cast<int32_t>(dest.width), static_cast<int32_t>(dest.height))) {
//        return;
//    }
//
//    const auto imageRect{
//        ComputeBackgroundImageRect(
//            boxStyle->backgroundPositionX,
//            boxStyle->backgroundPositionY,
//            boxStyle->backgroundWidth,
//            boxStyle->backgroundHeight,
//            boxStyle->backgroundSize,
//            dest,
//            this->backgroundImage.Get(),
//            this->scene)
//    };
//    auto drawBackgroundImage = [&](float x, float y) {
//        renderer->DrawImage(this->backgroundImage->GetTexture(),
//            { x + imageRect.x, y + imageRect.y, imageRect.width, imageRect.height }, ColorWhite);
//    };
//    float x{ dest.x };
//    float y{ dest.y };
//    const float x2{ x + dest.width };
//    const float y2{ y + dest.height };
//
//    renderer->SetClipRect(dest);
//
//    switch (boxStyle->backgroundRepeat) {
//        case StyleBackgroundRepeatXY:
//            while (y < y2) {
//                x = 0;
//
//                while (x < x2) {
//                    drawBackgroundImage(x, y);
//                    x+=imageRect.width;
//                }
//
//                y+=imageRect.height;
//            }
//            break;
//        case StyleBackgroundRepeatX:
//            while (x < x2) {
//                drawBackgroundImage(x, y);
//                x+=imageRect.width;
//            }
//            break;
//        case StyleBackgroundRepeatY:
//            while (y < y2) {
//                drawBackgroundImage(x, y);
//                y+=imageRect.height;
//            }
//            break;
//        case StyleBackgroundRepeatOff:
//            drawBackgroundImage(x, y);
//            break;
//        default:
//            break;
//    }
//
//    renderer->ClearClipRect();
//    renderer->SetRenderTarget(nullptr);
}

void BoxSceneNode::UpdateBackgroundImage(const std::string& backgroundUri) {
    if (backgroundUri.empty()) {
        if (this->backgroundImage) {
            this->QueuePaint();
        }
        this->ClearBackgroundImageResource();
        return;
    }

    if (this->backgroundImage && this->backgroundImage->GetId() == backgroundUri) {
        return;
    }

    if (this->backgroundImage) {
        this->QueuePaint();
    }

    this->ClearBackgroundImageResource();
    this->backgroundImage = this->GetStage()->GetResources()->AcquireImage(backgroundUri);

    auto listener{ [this](Res::Owner owner, Res* res) {
        if (this != owner || this->backgroundImage != res) {
            return;
        }

        this->QueuePaint();
        res->RemoveListener(owner);
    }};

    switch (this->backgroundImage->GetState()) {
        case Res::State::Init:
            this->backgroundImage->AddListener(this, listener);
            this->backgroundImage->Load(this->Env());
            break;
        case Res::State::Loading:
            this->backgroundImage->AddListener(this, listener);
            break;
        case Res::State::Ready:
        case Res::State::Error:
            listener(this, this->backgroundImage);
            break;
    }
}

void BoxSceneNode::ClearBackgroundImageResource() {
    if (this->backgroundImage) {
        this->backgroundImage->RemoveListener(this);
        this->GetStage()->GetResources()->ReleaseResource(this->backgroundImage);
        this->backgroundImage = nullptr;
    }
}

void BoxSceneNode::DestroyRecursive() {
    this->ClearBackgroundImageResource();
    SceneNode::DestroyRecursive();
}

} // namespace ls
