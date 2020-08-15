/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/BoxSceneNode.h>

#include <cstring>
#include <ls/Scene.h>
#include <ls/Stage.h>
#include <ls/Style.h>
#include <ls/CompositeContext.h>
#include <ls/Math.h>
#include <ls/Renderer.h>
#include <ls/Log.h>
#include <ls/Color.h>
#include <ls/yoga-ext.h>
#include <ls/GraphicsContext.h>
#include <ls/Timer.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::SafeObjectWrap;

namespace ls {

void BoxSceneNode::Constructor(const Napi::CallbackInfo& info) {
    this->SceneNodeConstructor(info);
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

void BoxSceneNode::OnStylePropertyChanged(StyleProperty property) {
    switch (property) {
        case StyleProperty::backgroundImage:
            this->UpdateBackgroundImage(this->style->backgroundImage);
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
            this->RequestStyleLayout();
            break;
        default:
            SceneNode::OnStylePropertyChanged(property);
            break;
    }
}

void BoxSceneNode::OnStyleLayout() {
    auto boxStyle{ Style::OrEmpty(this->style) };

    if (!boxStyle->backgroundColor.empty() && this->scene->GetStyleResolver().HasBorderRadius(boxStyle)) {
        this->RequestPaint();
        return;
    }

    // TODO: if background repeat -> paint

    if (this->backgroundImage && this->backgroundImage->HasDimensions()) {
        auto bounds{
            boxStyle->backgroundClip == StyleBackgroundClipBorderBox ?
                YGNodeLayoutGetRect(this->ygNode, 0, 0) : YGNodeLayoutGetInnerRect(this->ygNode)
        };

        if (!IsEmpty(bounds)) {
            auto fit{this->scene->GetStyleResolver().ResolveBackgroundFit(this->style, bounds, this->backgroundImage)};

            this->backgroundImageRect = ClipImage(bounds, fit,
                    this->backgroundImage->WidthF(), this->backgroundImage->HeightF());
        }
    }

    this->RequestComposite();
}

void BoxSceneNode::OnBoundingBoxChanged() {
    this->RequestStyleLayout();
}

void BoxSceneNode::Paint(RenderingContext2D* context) {
    const auto boxStyle{ Style::OrEmpty(this->style) };
    const auto dest{ YGNodeLayoutGetRect(this->ygNode, 0, 0) };
    // TODO: limit radius to 50% bounds
    const auto borderRadius{ this->scene->GetStyleResolver().ResolveBorderRadius(boxStyle) };
    const auto x{ 0 };
    const auto y{ 0 };
    const auto width{ dest.width };
    const auto height{ dest.height };

    if (!this->paintTarget || !this->paintTarget.IsLockable()) {
        this->paintTarget.Destroy();
        this->paintTarget = this->scene->GetRenderer()->CreateTexture(width, height, Texture::Lockable);

        if (!this->paintTarget) {
            LOG_ERROR("Failed to create paint texture.");
            return;
        }
    }

    auto pixels{ this->paintTarget.Lock() };
    Timer t("border radius render");

    context->Begin(pixels.Data(), pixels.Width(), pixels.Height(), pixels.Pitch());

    // Fill entire texture surface with transparent to start from a known state.
    context->SetColor(0);
    context->FillAll();

    context->SetColor(boxStyle->backgroundColor.value);

    context->BeginPath();
    context->MoveTo(x + borderRadius.topLeft, y);
    context->LineTo(x + width - borderRadius.topRight, y);
    context->QuadTo(x + width, y, x + width, y + borderRadius.topRight);
    context->LineTo(x + width, y + height - borderRadius.bottomRight);
    context->QuadTo(x + width, y + height, x + width - borderRadius.bottomRight, y + height);
    context->LineTo(x + borderRadius.bottomLeft, y + height);
    context->QuadTo(x, y + height, x, y + height - borderRadius.bottomLeft);
    context->LineTo(x, y + borderRadius.topLeft);
    context->QuadTo(x, y, x + borderRadius.topLeft, y);
    context->ClosePath();

    context->FillPath();

    context->End();

    pixels.Release();

    // TODO: convert context pixels, if necessary

    this->RequestComposite();
}

void BoxSceneNode::Composite(CompositeContext* composite) {
    const auto boxStyle{ this->style };

    if (boxStyle == nullptr || boxStyle->IsLayoutOnly()) {
        SceneNode::Composite(composite);
        return;
    }

    const auto rect{ YGNodeLayoutGetRect(this->ygNode) };

    if (IsEmpty(rect)) {
        SceneNode::Composite(composite);
        return;
    }

    const auto transform{
        composite->CurrentMatrix() * this->scene->GetStyleResolver().ResolveTransform(boxStyle, rect)
    };

    if (this->paintTarget) {
        composite->renderer->DrawImage(this->paintTarget, rect, transform,
                ColorWhite.MixAlpha(composite->CurrentOpacity()));
        SceneNode::Composite(composite);
        return;
    }

    if (!boxStyle->backgroundColor.empty()) {
        composite->renderer->DrawFillRect(
            rect,
            transform,
            boxStyle->backgroundColor.value.MixAlpha(composite->CurrentOpacity()));
    }

    if (this->backgroundImage) {
        if (!this->backgroundImage->HasTexture()) {
            this->backgroundImage->LoadTexture(composite->renderer);

            if (!this->backgroundImage) {
                LOG_ERROR("Failed to create background image texture.");
                return;
            }
        }

        if (this->backgroundImage->HasTexture() && !IsEmpty(this->backgroundImageRect.dest)) {
            const auto backgroundImageDestRect{Translate(this->backgroundImageRect.dest, rect.x, rect.y)};

            composite->renderer->DrawImage(
                this->backgroundImage->GetTexture(),
                this->backgroundImageRect.src,
                backgroundImageDestRect,
                transform,
                boxStyle->tintColor.ValueOr(ColorWhite).MixAlpha(composite->CurrentOpacity()));
        }
    }

    if (!boxStyle->borderColor.empty()) {
        composite->renderer->DrawBorder(
            rect,
            YGNodeLayoutGetBorderRect(this->ygNode),
            transform,
            boxStyle->borderColor.value.MixAlpha(composite->CurrentOpacity()));
    }

    SceneNode::Composite(composite);
}

//void BoxSceneNode::PaintBackgroundImage(Renderer* renderer, Style* boxStyle) {
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
//}

void BoxSceneNode::UpdateBackgroundImage(const std::string& backgroundUri) {
    auto clearBackgroundImageResource = [](BoxSceneNode* node) {
        if (node->backgroundImage && node->backgroundImage->HasTexture()) {
            node->RequestComposite();
        }
        node->ClearBackgroundImageResource();
    };

    clearBackgroundImageResource(this);

    if (backgroundUri.empty()) {
        return;
    }

    this->backgroundImage = this->GetStage()->GetResources()->AcquireImage(backgroundUri);

    auto listener{ [this](Res::Owner owner, Res* res) {
        if (this != owner || this->backgroundImage != res) {
            return;
        }

        res->RemoveListener(owner);
        this->RequestStyleLayout();
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

void BoxSceneNode::Destroy() {
    this->paintTarget.Destroy();
    this->ClearBackgroundImageResource();
    SceneNode::Destroy();
}

} // namespace ls
