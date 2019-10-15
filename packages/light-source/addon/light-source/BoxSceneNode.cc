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
#include <ls/Renderer.h>
#include <ls/Format.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;

namespace ls {

constexpr int64_t white{ 0xFFFFFFFF };

static bool GetBorderRadiusState(const Style* style) noexcept;

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

void BoxSceneNode::Paint(Renderer* renderer) {
    const auto boxStyle{ this->GetStyleOrEmpty() };

    if (boxStyle->IsLayoutOnly()) {
        SceneNode::Paint(renderer);
        return;
    }

    const auto x{ YGNodeLayoutGetLeft(this->ygNode) };
    const auto y{ YGNodeLayoutGetTop(this->ygNode) };
    const auto width{ YGNodeLayoutGetWidth(this->ygNode) };
    const auto height{ YGNodeLayoutGetHeight(this->ygNode) };

    auto dx{ 0.f };
    auto dy{ 0.f };
    auto dw{ 0.f };
    auto dh{ 0.f };

    if (boxStyle->backgroundClip() == StyleBackgroundClipPaddingBox && boxStyle->HasBorder()) {
        const auto borderLeft{ YGNodeLayoutGetBorder(this->ygNode, YGEdgeLeft) };
        const auto borderTop { YGNodeLayoutGetBorder(this->ygNode, YGEdgeTop) };

        dx = borderLeft;
        dy = borderTop;
        dw = -(borderLeft + YGNodeLayoutGetBorder(this->ygNode, YGEdgeRight));
        dh = -(borderTop + YGNodeLayoutGetBorder(this->ygNode, YGEdgeBottom));
    }

    if (this->roundedRectImage) {
        if (style->backgroundColor() && this->roundedRectImage->Sync(renderer)) {
            renderer->DrawImage(this->roundedRectImage->GetTextureId(),
                                { x + dx + 1, y + dy + 1, width + dw - 2, height + dh - 2 },
                                this->roundedRectImage->GetCapInsets(),
                                style->backgroundColor()->Get());
        }
    } else if (this->backgroundImage && this->backgroundImage->Sync(renderer)) {
        YGSize fitDimensions;

        if (boxStyle->backgroundWidth() || boxStyle->backgroundHeight()) {
            fitDimensions = {
                CalculateBackgroundDimension(
                    boxStyle->backgroundWidth(), backgroundImage->GetWidth(), width, this->scene),
                CalculateBackgroundDimension(
                    boxStyle->backgroundHeight(), backgroundImage->GetHeight(), height, this->scene)
            };
        } else {
            fitDimensions = CalculateObjectFitDimensions(
                boxStyle->backgroundFit(), this->backgroundImage.get(), width, height);
        }

        const auto positionX{ x + dx + CalculateObjectPosition(
            boxStyle->backgroundPositionX(), true, width, fitDimensions.width, 0, this->scene) };
        const auto positionY{ y + dy + CalculateObjectPosition(
            boxStyle->backgroundPositionY(), false, height, fitDimensions.height, 0, this->scene) };

        renderer->PushClipRect({ x + dx, y + dy, width + dw, height + dh });

        const auto textureId{ this->backgroundImage->GetTextureId() };
        const Rect destRect{
            YGRoundValueToPixelGrid(positionX),
            YGRoundValueToPixelGrid(positionY),
            YGRoundValueToPixelGrid(fitDimensions.width),
            YGRoundValueToPixelGrid(fitDimensions.height),
        };

        // TODO: mix opacity

        if (this->backgroundImage->HasCapInsets()) {
            renderer->DrawImage(textureId, destRect, this->backgroundImage->GetCapInsets(), white);
        } else {
            renderer->DrawImage(textureId, destRect, white);
        }

        renderer->PopClipRect();
    } else if (boxStyle->backgroundColor()) {
        renderer->DrawFillRect({ x + dx, y + dy, width + dw, height + dh }, *boxStyle->backgroundColor());
    }

    if (this->roundedRectStrokeImage) {
        if (style->borderColor() && this->roundedRectStrokeImage->Sync(renderer)) {
            renderer->DrawImage(
                this->roundedRectStrokeImage->GetTextureId(),
                { x + dx, y + dy, width + dw, height + dh },
                this->roundedRectStrokeImage->GetCapInsets(),
                *style->borderColor());
        }
    } else if (boxStyle->borderColor()) {
        renderer->DrawBorder(
            { x, y, width, height },
            {
                static_cast<int32_t>(YGNodeLayoutGetBorder(this->ygNode, YGEdgeTop)),
                static_cast<int32_t>(YGNodeLayoutGetBorder(this->ygNode, YGEdgeRight)),
                static_cast<int32_t>(YGNodeLayoutGetBorder(this->ygNode, YGEdgeBottom)),
                static_cast<int32_t>(YGNodeLayoutGetBorder(this->ygNode, YGEdgeLeft)),
            },
            *boxStyle->borderColor());
    }

    SceneNode::Paint(renderer);
}

void BoxSceneNode::UpdateStyle(Style* newStyle, Style* oldStyle) {
    SceneNode::UpdateStyle(newStyle, oldStyle);

    this->SetBackgroundImage(newStyle);
    this->SetBorderRadiusImages(newStyle);
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

void BoxSceneNode::SetBorderRadiusImages(const Style* style) {
    if (!GetBorderRadiusState(style)) {
        this->roundedRectImage = this->roundedRectStrokeImage = nullptr;
        return;
    }

    const auto imageStore{ this->scene->GetImageStore() };
    const auto borderRadius{ ComputeIntegerPointValue(
        style->borderRadius(), this->scene, 0) };
    const auto borderRadiusTopLeft{ ComputeIntegerPointValue(
        style->borderRadiusTopLeft(), this->scene, borderRadius) };
    const auto borderRadiusTopRight{ ComputeIntegerPointValue(
        style->borderRadiusTopRight(), this->scene, borderRadius) };
    const auto borderRadiusBottomLeft{ ComputeIntegerPointValue(
        style->borderRadiusBottomLeft(), this->scene, borderRadius) };
    const auto borderRadiusBottomRight{ ComputeIntegerPointValue(
        style->borderRadiusBottomRight(), this->scene, borderRadius) };

    if (style->backgroundColor()) {
        const auto borderRadiusImageId{
            Format("@border-radius:%i,%i,%i,%i",
                borderRadiusTopLeft,
                borderRadiusTopRight,
                borderRadiusBottomRight,
                borderRadiusBottomLeft)
        };

        if (imageStore->HasImage(borderRadiusImageId)) {
            this->roundedRectImage = imageStore->GetImage(borderRadiusImageId);
        } else {
            const auto uri{ CreateRoundedRectangleUri(borderRadiusTopLeft, borderRadiusTopRight,
                borderRadiusBottomRight, borderRadiusBottomLeft, 0) };
            const EdgeRect capInsets{
                std::max(borderRadiusTopLeft, borderRadiusTopRight),
                std::max(borderRadiusTopRight, borderRadiusBottomRight),
                std::max(borderRadiusBottomLeft, borderRadiusBottomRight),
                std::max(borderRadiusTopLeft, borderRadiusBottomLeft)
            };

            this->roundedRectImage = imageStore->LoadImage({ uri, borderRadiusImageId, 0, 0, capInsets });
        }
    } else {
        this->roundedRectImage = nullptr;
    }

    if (style->borderColor() && style->border()) {
        const auto stroke{ ComputeIntegerPointValue(style->border(), this->scene, 0) };
        const auto borderRadiusStrokeImageId{
            Format("@border-radius-stroke:%i,%i,%i,%i,%i",
                borderRadiusTopLeft,
                borderRadiusTopRight,
                borderRadiusBottomRight,
                borderRadiusBottomLeft,
                stroke)
        };

        if (imageStore->Has(borderRadiusStrokeImageId)) {
            this->roundedRectStrokeImage = imageStore->GetImage(borderRadiusStrokeImageId);
        } else {
            const auto uri{ CreateRoundedRectangleUri(borderRadiusTopLeft, borderRadiusTopRight,
                borderRadiusBottomRight, borderRadiusBottomLeft, stroke) };
            const EdgeRect capInsets{
                std::max(borderRadiusTopLeft, borderRadiusTopRight),
                std::max(borderRadiusTopRight, borderRadiusBottomRight),
                std::max(borderRadiusBottomLeft, borderRadiusBottomRight),
                std::max(borderRadiusTopLeft, borderRadiusBottomLeft)
            };

            this->roundedRectStrokeImage = imageStore->LoadImage({ uri, borderRadiusStrokeImageId, 0, 0, capInsets });
        }
    } else {
        this->roundedRectStrokeImage = nullptr;
    }
}

void BoxSceneNode::DestroyRecursive() {
    this->backgroundImage = nullptr;
    this->roundedRectImage = nullptr;
    this->roundedRectStrokeImage = nullptr;

    SceneNode::DestroyRecursive();
}

static bool GetBorderRadiusState(const Style* style) noexcept {
    return style && style->HasBorderRadius() && (style->backgroundColor() || style->borderColor());
}

} // namespace ls
