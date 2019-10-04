/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "BoxSceneNode.h"
#include "ImageResource.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "StyleUtils.h"
#include <fmt/format.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;

namespace ls {

void AssignImage(ImageResource** targetImage, ImageResource* newImage);
constexpr int64_t white{ 0xFFFFFFFF };

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
                boxStyle->backgroundFit(), this->backgroundImage, width, height);
        }

        const auto positionX{ x + dx + CalculateObjectPosition(
            boxStyle->backgroundPositionX(), true, width, fitDimensions.width, 0, this->scene) };
        const auto positionY{ y + dy + CalculateObjectPosition(
            boxStyle->backgroundPositionY(), false, height, fitDimensions.height, 0, this->scene) };

        renderer->PushClipRect({ x + dx, y + dy, width + dw, height + dh });

        const auto textureId{ this->backgroundImage->GetTextureId() };
        const Rect destRect{
            YGRoundValueToPixelGrid(positionX, 1.f, false, false),
            YGRoundValueToPixelGrid(positionY, 1.f, false, false),
            YGRoundValueToPixelGrid(fitDimensions.width, 1.f, false, false),
            YGRoundValueToPixelGrid(fitDimensions.height, 1.f, false, false),
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

    if (newStyle->backgroundImage() != this->backgroundImageUri) {
        ImageResource* newBackgroundImage;

        if (!newStyle->backgroundImage().empty()) {
            this->backgroundImageUri = newStyle->backgroundImage();
            // TODO: support full uri objects
            newBackgroundImage = this->scene->GetResourceManager()->LoadImage(ImageUri(this->backgroundImageUri));
        } else {
            newBackgroundImage = nullptr;
        }

        AssignImage(&this->backgroundImage, newBackgroundImage);
    }

    if (newStyle->HasBorderRadius() && (newStyle->backgroundColor() || newStyle->borderColor())) {
        const auto borderRadius{ ComputeIntegerPointValue(
            newStyle->borderRadius(), this->scene, 0) };
        const auto borderRadiusTopLeft{ ComputeIntegerPointValue(
            newStyle->borderRadiusTopLeft(), this->scene, borderRadius) };
        const auto borderRadiusTopRight{ ComputeIntegerPointValue(
            newStyle->borderRadiusTopRight(), this->scene, borderRadius) };
        const auto borderRadiusBottomLeft{ ComputeIntegerPointValue(
            newStyle->borderRadiusBottomLeft(), this->scene, borderRadius) };
        const auto borderRadiusBottomRight{ ComputeIntegerPointValue(
            newStyle->borderRadiusBottomRight(), this->scene, borderRadius) };

        ImageResource* newRoundedRectImage;
        EdgeRect capInsets{
            std::max(borderRadiusTopLeft, borderRadiusTopRight),
            std::max(borderRadiusTopRight, borderRadiusBottomRight),
            std::max(borderRadiusBottomLeft, borderRadiusBottomRight),
            std::max(borderRadiusTopLeft, borderRadiusBottomLeft)
        };

        if (newStyle->backgroundColor()) {
            const auto borderRadiusImageId{
                fmt::format("@border-radius:{},{},{},{}",
                    borderRadiusTopLeft,
                    borderRadiusTopRight,
                    borderRadiusBottomRight,
                    borderRadiusBottomLeft)
            };

            newRoundedRectImage = this->scene->GetResourceManager()->GetImage(borderRadiusImageId);

            if (!newRoundedRectImage) {
                auto uri{ CreateRoundedRectangleUri(borderRadiusTopLeft, borderRadiusTopRight,
                    borderRadiusBottomRight, borderRadiusBottomLeft, 0) };

                newRoundedRectImage = this->scene->GetResourceManager()->LoadImage(
                    { uri, borderRadiusImageId, 0, 0, capInsets });
            }
        } else {
            newRoundedRectImage = nullptr;
        }

        AssignImage(&this->roundedRectImage, newRoundedRectImage);

        ImageResource* newRoundedRectStrokeImage;

        if (newStyle->borderColor() && newStyle->border()) {
            const auto stroke{ ComputeIntegerPointValue(newStyle->border(), this->scene, 0) };

            const auto borderRadiusStrokeImageId{
                fmt::format("@border-radius-stroke:{},{},{},{},{}",
                    borderRadiusTopLeft,
                    borderRadiusTopRight,
                    borderRadiusBottomRight,
                    borderRadiusBottomLeft,
                    stroke)
            };

            newRoundedRectStrokeImage = this->scene->GetResourceManager()->GetImage(borderRadiusStrokeImageId);

            if (!newRoundedRectStrokeImage) {
                const auto uri{ CreateRoundedRectangleUri(borderRadiusTopLeft, borderRadiusTopRight,
                    borderRadiusBottomRight, borderRadiusBottomLeft, stroke) };

                newRoundedRectStrokeImage = this->scene->GetResourceManager()->LoadImage(
                    { uri, borderRadiusStrokeImageId, 0, 0, capInsets });
            }
        } else {
            newRoundedRectStrokeImage = nullptr;
        }

        AssignImage(&this->roundedRectStrokeImage, newRoundedRectStrokeImage);
    } else {
        AssignImage(&this->roundedRectImage, nullptr);
        AssignImage(&this->roundedRectStrokeImage, nullptr);
    }
}

void BoxSceneNode::DestroyRecursive() {
    AssignImage(&this->backgroundImage, nullptr);
    AssignImage(&this->roundedRectImage, nullptr);
    AssignImage(&this->roundedRectStrokeImage, nullptr);

    SceneNode::DestroyRecursive();
}

void AssignImage(ImageResource** targetImage, ImageResource* newImage) {
    auto image = *targetImage;

    if (image != newImage) {
        if (image) {
            image->RemoveRef();
            *targetImage = nullptr;
        }

        *targetImage = newImage;
    }
}

} // namespace ls
