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

    if (!boxStyle->backgroundImage.empty() && boxStyle->backgroundRepeat != StyleBackgroundRepeatOff) {
        this->RequestPaint();
        return;
    }

    // if repeat && background image
    // TODO: if background repeat -> paint

    if (this->backgroundImage && this->backgroundImage->HasDimensions()) {
        const auto box{ this->GetBackgroundClipBox(boxStyle->backgroundClip) };

        if (!IsEmpty(box)) {
            auto fit{this->scene->GetStyleResolver().ResolveBackgroundFit(this->style, box, this->backgroundImage)};

            this->backgroundImageRect = ClipImage(box, fit,
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

    if (!boxStyle->backgroundColor.empty() && this->scene->GetStyleResolver().HasBorderRadius(boxStyle)) {
        this->PaintBorderRadius(context);
    } else if (this->backgroundImage && this->backgroundImage->HasDimensions()
            && !boxStyle->backgroundImage.empty() && boxStyle->backgroundRepeat != StyleBackgroundRepeatOff) {
        this->PaintBackgroundRepeat(context);
    }
}

void BoxSceneNode::PaintBorderRadius(RenderingContext2D* context) {
    const auto boxStyle{ Style::OrEmpty(this->style) };
    const auto box{ this->GetBackgroundClipBox(boxStyle->backgroundClip) };
    const auto borderRadius{ this->scene->GetStyleResolver().ResolveBorderRadius(boxStyle, box) };

    if (!this->paintTarget
            || !this->paintTarget.IsLockable()
            || this->paintTarget.Width() != static_cast<int32_t>(box.width)
            || this->paintTarget.Height() != static_cast<int32_t>(box.height)) {
        this->paintTarget.Destroy();
        this->paintTarget = this->scene->GetRenderer()->CreateTexture(box.width, box.height, Texture::Lockable);

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
    context->MoveTo(box.x + borderRadius.topLeft, box.y);
    context->LineTo(box.x + box.width - borderRadius.topRight, box.y);
    context->QuadTo(box.x + box.width, box.y, box.x + box.width, box.y + borderRadius.topRight);
    context->LineTo(box.x + box.width, box.y + box.height - borderRadius.bottomRight);
    context->QuadTo(box.x + box.width, box.y + box.height,
                    box.x + box.width - borderRadius.bottomRight, box.y + box.height);
    context->LineTo(box.x + borderRadius.bottomLeft, box.y + box.height);
    context->QuadTo(box.x, box.y + box.height, box.x, box.y + box.height - borderRadius.bottomLeft);
    context->LineTo(box.x, box.y + borderRadius.topLeft);
    context->QuadTo(box.x, box.y, box.x + borderRadius.topLeft, box.y);
    context->ClosePath();

    context->FillPath();

    context->End();

    pixels.Release();

    // TODO: convert context pixels, if necessary

    this->RequestComposite();
}

void BoxSceneNode::PaintBackgroundRepeat(RenderingContext2D* context) {
    const auto boxStyle{ Style::OrEmpty(this->style) };
    const auto box{ this->GetBackgroundClipBox(boxStyle->backgroundClip) };
    const auto boxWidthI{ static_cast<int32_t>(box.width) };
    const auto boxHeightI{ static_cast<int32_t>(box.height) };

    if (!this->paintTarget || !this->paintTarget.IsLockable()
            || this->paintTarget.Width() != boxWidthI || this->paintTarget.Height() != boxHeightI) {
        this->paintTarget.Destroy();
        this->paintTarget = this->scene->GetRenderer()->CreateTexture(
                boxWidthI, boxHeightI, Texture::RenderTarget);

        if (!this->paintTarget) {
            LOG_ERROR("Failed to create paint texture.");
            return;
        }
    }

    if (!this->backgroundImage->HasTexture() && this->backgroundImage->GetState() == Resource::Ready) {
        if (!this->backgroundImage->LoadTexture(context->renderer)) {
            return;
        }
    }

    auto x{ box.x };
    auto y{ box.y };
    const auto x2{ box.x + box.width };
    const auto y2{ box.y + box.height };
    Rect imageRect{ 0, 0, this->backgroundImage->WidthF(), this->backgroundImage->HeightF() };

    context->renderer->SetRenderTarget(this->paintTarget);
    context->renderer->FillRenderTarget(0);

    switch (boxStyle->backgroundRepeat) {
        case StyleBackgroundRepeatXY:
            while (y < y2) {
                x = 0;

                while (x < x2) {
                    context->renderer->DrawImage(this->backgroundImage->GetTexture(),
                            Translate(imageRect, x, y), Matrix::Identity(), ColorWhite);
                    x += imageRect.width;
                }

                y += imageRect.height;
            }
            break;
        case StyleBackgroundRepeatX:
            while (x < x2) {
                context->renderer->DrawImage(
                    this->backgroundImage->GetTexture(), Translate(imageRect, x, y), Matrix::Identity(), ColorWhite);
                x += imageRect.width;
            }
            break;
        case StyleBackgroundRepeatY:
            while (y < y2) {
                context->renderer->DrawImage(
                    this->backgroundImage->GetTexture(), Translate(imageRect, x, y), Matrix::Identity(), ColorWhite);
                y += imageRect.height;
            }
            break;
        default:
            break;
    }

    context->renderer->Reset();

    this->RequestComposite();
}

void BoxSceneNode::Composite(CompositeContext* composite) {
    const auto boxStyle{ this->style };

    if (boxStyle == nullptr || boxStyle->IsLayoutOnly()) {
        return;
    }

    const auto box{ YGNodeGetBox(this->ygNode, 0, 0) };
    const auto& transform{ composite->CurrentMatrix() };

    if (this->paintTarget) {
        composite->renderer->DrawImage(this->paintTarget, box, transform,
                ColorWhite.MixAlpha(composite->CurrentOpacity()));
        return;
    }

    if (!boxStyle->backgroundColor.empty()) {
        composite->renderer->DrawFillRect(
            box,
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
            const auto backgroundImageDestRect{Translate(this->backgroundImageRect.dest, box.x, box.y)};

            composite->renderer->DrawImage(
                this->backgroundImage->GetTexture(),
                this->backgroundImageRect.src,
                backgroundImageDestRect,
                transform,
                boxStyle->tintColor.ValueOr(ColorWhite).MixAlpha(composite->CurrentOpacity()));
        }
    }

    if (!boxStyle->borderColor.empty()) {
        composite->renderer->DrawBorder(box, YGNodeGetBorderEdges(this->ygNode),
            transform,
            boxStyle->borderColor.value.MixAlpha(composite->CurrentOpacity()));
    }
}

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

    this->backgroundImage = this->GetResources()->AcquireImage(backgroundUri);

    auto listener{ [this](Resource::Owner owner, Resource* res) {
        if (this != owner || this->backgroundImage != res) {
            return;
        }

        res->RemoveListener(owner);
        this->RequestStyleLayout();
    }};

    switch (this->backgroundImage->GetState()) {
        case Resource::State::Init:
            this->backgroundImage->AddListener(this, listener);
            this->backgroundImage->Load(this->Env());
            break;
        case Resource::State::Loading:
            this->backgroundImage->AddListener(this, listener);
            break;
        case Resource::State::Ready:
        case Resource::State::Error:
            listener(this, this->backgroundImage);
            break;
    }
}

void BoxSceneNode::ClearBackgroundImageResource() {
    if (this->backgroundImage) {
        this->backgroundImage->RemoveListener(this);
        this->GetResources()->ReleaseResource(this->backgroundImage);
        this->backgroundImage = nullptr;
    }
}

Rect BoxSceneNode::GetBackgroundClipBox(StyleBackgroundClip value) const noexcept {
    switch (value) {
        case StyleBackgroundClipBorderBox:
            return YGNodeGetBox(this->ygNode, 0, 0);
        case StyleBackgroundClipPaddingBox:
            return YGNodeGetBorderBox(this->ygNode);
        case StyleBackgroundClipContentBox:
            return YGNodeGetPaddingBox(this->ygNode);
    }

    return {};
}

void BoxSceneNode::Destroy() {
    this->paintTarget.Destroy();
    this->ClearBackgroundImageResource();
    SceneNode::Destroy();
}

} // namespace ls
