/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "BoxSceneNode.h"
#include "ImageResource.h"
#include "Scene.h"
#include "StyleUtils.h"
#include <fmt/format.h>

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

        auto func = DefineClass(env, "BoxSceneNode", {
            InstanceAccessor("x", &SceneNode::GetX, nullptr),
            InstanceAccessor("y", &SceneNode::GetY, nullptr),
            InstanceAccessor("width", &SceneNode::GetWidth, nullptr),
            InstanceAccessor("height", &SceneNode::GetHeight, nullptr),

            InstanceAccessor("parent", &SceneNode::GetParent, nullptr),
            InstanceAccessor("style", &SceneNode::GetStyle, &SceneNode::SetStyle),

            InstanceMethod("appendChild", &SceneNode::AppendChild),
            InstanceMethod("removeChild", &SceneNode::RemoveChild),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

void BoxSceneNode::Paint(Renderer* renderer) {
    auto boxStyle{ this->GetStyleOrEmpty() };

    auto x{ YGNodeLayoutGetLeft(this->ygNode) };
    auto y{ YGNodeLayoutGetTop(this->ygNode) };
    auto width{ YGNodeLayoutGetWidth(this->ygNode) };
    auto height{ YGNodeLayoutGetHeight(this->ygNode) };

    auto dx{ 0.f };
    auto dy{ 0.f };
    auto dw{ 0.f };
    auto dh{ 0.f };

    if (boxStyle->backgroundClip() == StyleBackgroundClipPaddingBox) {
        auto borderLeft{ YGNodeLayoutGetBorder(this->ygNode, YGEdgeLeft) };
        auto borderTop { YGNodeLayoutGetBorder(this->ygNode, YGEdgeTop) };

        dx = borderLeft;
        dy = borderTop;
        dw = -(borderLeft + YGNodeLayoutGetBorder(this->ygNode, YGEdgeRight));
        dh = -(borderTop + YGNodeLayoutGetBorder(this->ygNode, YGEdgeBottom));
    }

    if (this->roundedRectImage) {
        if (this->roundedRectImage->IsReady() && style->backgroundColor()) {
            // TODO: GetTexture() exception
            renderer->DrawImage(this->roundedRectImage->GetTexture(renderer),
                                { x + dx + 1, y + dy + 1, width + dw - 2, height + dh - 2 },
                                this->roundedRectImage->GetCapInsets(),
                                style->backgroundColor()->Value());
        }
    } else if (this->backgroundImage && this->backgroundImage->IsReady()) {
        renderer->PushClipRect({ x + dx, y + dy, width + dw, height + dh });

        float fitWidth;
        float fitHeight;

        if (boxStyle->backgroundWidth() || boxStyle->backgroundHeight()) {
            fitWidth = CalculateBackgroundDimension(
                boxStyle->backgroundWidth(), backgroundImage->GetWidth(), width, this->scene);
            fitHeight = CalculateBackgroundDimension(
                boxStyle->backgroundHeight(), backgroundImage->GetHeight(), height, this->scene);
        } else {
            CalculateObjectFitDimensions(
                boxStyle->backgroundFit(), this->backgroundImage, width, height, &fitWidth, &fitHeight);
        }

        float positionX{ x + dx
            + CalculateObjectPosition(boxStyle->backgroundPositionX(), true, width, fitWidth, 0, this->scene) };
        float positionY{ y + dy
            + CalculateObjectPosition(boxStyle->backgroundPositionY(), false, height, fitHeight, 0, this->scene) };

        // TODO: mix opacity
        // TODO: GetTexture() exception
        renderer->DrawImage(this->backgroundImage->GetTexture(renderer),
                            {
                                YGRoundValueToPixelGrid(positionX, 1.f, false, false),
                                YGRoundValueToPixelGrid(positionY, 1.f, false, false),
                                YGRoundValueToPixelGrid(fitWidth, 1.f, false, false),
                                YGRoundValueToPixelGrid(fitHeight, 1.f, false, false),
                            },
                            0xFFFFFFFF);

        renderer->PopClipRect();
    } else if (boxStyle->backgroundColor()) {
        renderer->DrawFillRect({ x + dx, y + dy, width + dw, height + dh }, boxStyle->backgroundColor()->Value());
    }

    if (this->roundedRectBorderImage) {
        if (this->roundedRectBorderImage->IsReady() && style->borderColor()) {
            // TODO: GetTexture() exception
            renderer->DrawImage(this->roundedRectBorderImage->GetTexture(renderer),
                                { x + dx, y + dy, width + dw, height + dh },
                                this->roundedRectBorderImage->GetCapInsets(),
                                style->borderColor()->Value());
        }
    } else if (boxStyle->borderColor()) {
        renderer->DrawBorder(
            {
                x,
                y,
                width,
                height,
            },
            {
                static_cast<int32_t>(YGNodeLayoutGetBorder(this->ygNode, YGEdgeTop)),
                static_cast<int32_t>(YGNodeLayoutGetBorder(this->ygNode, YGEdgeRight)),
                static_cast<int32_t>(YGNodeLayoutGetBorder(this->ygNode, YGEdgeBottom)),
                static_cast<int32_t>(YGNodeLayoutGetBorder(this->ygNode, YGEdgeLeft)),
            },
            boxStyle->borderColor()->Value());
    }

    SceneNode::Paint(renderer);
}

void BoxSceneNode::ApplyStyle(Style* style) {
    SceneNode::ApplyStyle(style);

    if (style->backgroundImage() != this->backgroundImageUri) {
        this->ClearBackgroundImage();

        if (!style->backgroundImage().empty()) {
            this->backgroundImageUri = style->backgroundImage();
            this->backgroundImage = this->scene->GetResourceManager()->GetImage(style->backgroundImage());
        }
    }

    if (style->borderRadius() || style->borderRadiusTopLeft() || style->borderRadiusTopRight()
            || style->borderRadiusBottomLeft() || style->borderRadiusBottomRight()) {
        auto borderRadius{ ComputeIntegerPointValue(
            style->borderRadius(), this->scene, 0) };
        auto borderRadiusTopLeft{ ComputeIntegerPointValue(
            style->borderRadiusTopLeft(), this->scene, borderRadius) };
        auto borderRadiusTopRight{ ComputeIntegerPointValue(
            style->borderRadiusTopRight(), this->scene, borderRadius) };
        auto borderRadiusBottomLeft{ ComputeIntegerPointValue(
            style->borderRadiusBottomLeft(), this->scene, borderRadius) };
        auto borderRadiusBottomRight{ ComputeIntegerPointValue(
            style->borderRadiusBottomRight(), this->scene, borderRadius) };

        if (style->backgroundColor()) {
            auto uri = CreateRoundedRectangleUri(
                borderRadiusTopLeft, borderRadiusTopRight, borderRadiusBottomRight, borderRadiusBottomLeft, 0);
            // TODO: auto id = fmt::format("@border-radius:{},{},{},{},{}",
            //    borderRadiusTopLeft, borderRadiusTopRight, borderRadiusBottomRight, borderRadiusBottomLeft, 0);
            EdgeRect capInsets{
                std::max(borderRadiusTopLeft, borderRadiusTopRight),
                std::max(borderRadiusTopRight, borderRadiusBottomRight),
                std::max(borderRadiusBottomLeft, borderRadiusBottomRight),
                std::max(borderRadiusTopLeft, borderRadiusBottomLeft)
            };
            auto image = this->scene->GetResourceManager()->GetImage(uri);

            SetRoundedRectImage(image);
        }

//        if (style->borderColor() && style->border()) {
//            auto border{ ComputeIntegerPointValue(style->border(), this->scene, 1) };
//            auto uri = CreateRoundedRectangleUri(
//                borderRadiusTopLeft, borderRadiusTopRight, borderRadiusBottomRight, borderRadiusBottomLeft, border);
//
//            fmt::println(uri);
//
//            auto image = this->scene->GetImageStore()->Acquire(uri);
//
//            SetRoundedRectBorderImage(image);
//        }
    }
}

void BoxSceneNode::SetRoundedRectImage(ImageResource* image) {
    if (this->roundedRectImage) {
        this->roundedRectImage->RemoveRef();
        this->roundedRectImage = nullptr;
    }

    this->roundedRectImage = image;
}

void BoxSceneNode::SetRoundedRectBorderImage(ImageResource* image) {
    if (this->roundedRectBorderImage) {
        this->roundedRectBorderImage->RemoveRef();
        this->roundedRectBorderImage = nullptr;
    }

    this->roundedRectBorderImage = image;
}

void BoxSceneNode::ClearBackgroundImage() {
    if (this->backgroundImage) {
        this->backgroundImage->RemoveRef();
        this->backgroundImageUri.clear();
        this->backgroundImage = nullptr;
    }
}

void BoxSceneNode::DestroyRecursive() {
    this->ClearBackgroundImage();
    this->SetRoundedRectBorderImage(nullptr);
    this->SetRoundedRectImage(nullptr);

    SceneNode::DestroyRecursive();
}

} // namespace ls
