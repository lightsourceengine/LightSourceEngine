/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageSceneNode.h"
#include "YGNode.h"
#include "Scene.h"
#include "ImageResource.h"
#include "Style.h"
#include "StyleUtils.h"
#include <fmt/format.h>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

ImageSceneNode::ImageSceneNode(const CallbackInfo& info) : ObjectWrap<ImageSceneNode>(info), SceneNode(info) {
    this->isLeaf = true;

    YGNodeSetContext(this->ygNode, this);

    YGNodeSetMeasureFunc(
        this->ygNode,
        [](YGNodeRef nodeRef, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) -> YGSize {
            auto self { static_cast<ImageSceneNode *>(YGNodeGetContext(nodeRef)) };

            if (self && self->image && self->image->IsReady()) {
                return {
                    static_cast<float>(self->image->GetWidth()),
                    static_cast<float>(self->image->GetHeight()),
                };
            }

            return { 0.f, 0.f };
    });
}

Function ImageSceneNode::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "ImageSceneNode", {
            InstanceAccessor("x", &SceneNode::GetX, nullptr),
            InstanceAccessor("y", &SceneNode::GetY, nullptr),
            InstanceAccessor("width", &SceneNode::GetWidth, nullptr),
            InstanceAccessor("height", &SceneNode::GetHeight, nullptr),

            InstanceAccessor("parent", &SceneNode::GetParent, nullptr),
            InstanceAccessor("style", &SceneNode::GetStyle, &SceneNode::SetStyle),

            InstanceMethod("appendChild", &SceneNode::AppendChild),
            InstanceMethod("removeChild", &SceneNode::RemoveChild),

            InstanceAccessor("src", &ImageSceneNode::GetSource, &ImageSceneNode::SetSource),
            InstanceAccessor("onLoad", &ImageSceneNode::GetOnLoadCallback, &ImageSceneNode::SetOnLoadCallback),
            InstanceAccessor("onError", &ImageSceneNode::GetOnErrorCallback, &ImageSceneNode::SetOnErrorCallback),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value ImageSceneNode::GetSource(const CallbackInfo& info) {
    return this->uri.empty() ? info.Env().Undefined() : String::New(info.Env(), this->uri);
}

void ImageSceneNode::SetSource(const CallbackInfo& info, const Napi::Value& value) {
    if (!value.IsString()) {
        this->ClearImage();
        YGNodeMarkDirty(this->ygNode);

        return;
    }

    auto newUri{ value.As<String>().Utf8Value() };

    if (newUri == this->uri) {
        return;
    }

    this->ClearImage();
    this->uri = newUri;
    this->image = this->scene->GetResourceManager()->GetImage(newUri);

    if (!this->image) {
        this->ClearImage();
        YGNodeMarkDirty(this->ygNode);

        return;
    }

    if (this->DoCallbacks()) {
        YGNodeMarkDirty(this->ygNode);

        return;
    }

    this->imageListenerId = this->image->AddListener([&]() {
        YGNodeMarkDirty(this->ygNode);
        this->image->RemoveListener(this->imageListenerId);
        this->imageListenerId = 0;
        this->DoCallbacks();
    });
}

Value ImageSceneNode::GetOnLoadCallback(const CallbackInfo& info) {
    return this->onLoadCallback.Value();
}

void ImageSceneNode::SetOnLoadCallback(const CallbackInfo& info, const Napi::Value& value) {
    if (info[0].IsFunction()) {
        this->onLoadCallback.Reset(info[0].As<Function>(), 1);
    } else {
        this->onLoadCallback.Reset();
    }
}

Value ImageSceneNode::GetOnErrorCallback(const CallbackInfo& info) {
    return this->onErrorCallback.Value();
}

void ImageSceneNode::SetOnErrorCallback(const CallbackInfo& info, const Napi::Value& value) {
    if (info[0].IsFunction()) {
        this->onErrorCallback.Reset(info[0].As<Function>(), 1);
    } else {
        this->onErrorCallback.Reset();
    }
}

void ImageSceneNode::Paint(Renderer* renderer) {
    if (!this->image || !this->image->IsReady()) {
        return;
    }

    auto boxStyle{ this->GetStyleOrEmpty() };
    auto left{ YGNodeLayoutGetBorder(this->ygNode, YGEdgeLeft) + YGNodeLayoutGetPadding(this->ygNode, YGEdgeLeft) };
    auto top{ YGNodeLayoutGetBorder(this->ygNode, YGEdgeTop) + YGNodeLayoutGetPadding(this->ygNode, YGEdgeTop) };
    auto x{ YGNodeLayoutGetLeft(this->ygNode) + left };
    auto y{ YGNodeLayoutGetTop(this->ygNode) + top };
    auto width{ YGNodeLayoutGetWidth(this->ygNode)
        - (left + YGNodeLayoutGetBorder(this->ygNode, YGEdgeRight)
        + YGNodeLayoutGetPadding(this->ygNode, YGEdgeRight)) };
    auto height{ YGNodeLayoutGetHeight(this->ygNode)
        - (top + YGNodeLayoutGetBorder(this->ygNode, YGEdgeBottom)
        + YGNodeLayoutGetPadding(this->ygNode, YGEdgeBottom)) };
    float fitWidth;
    float fitHeight;

    CalculateObjectFitDimensions(boxStyle->objectFit(), this->image, width, height, &fitWidth, &fitHeight);

    auto clip{ fitWidth > width || fitHeight > height };

    if (clip) {
        renderer->PushClipRect({ x, y, width, height });
    }

    renderer->DrawImage(
        // TODO: GetTexture exception
        this->image->GetTexture(renderer),
        {
            YGRoundValueToPixelGrid(x + CalculateObjectPosition(
                boxStyle->objectPositionX(), true, width, fitWidth, 0.5f, this->scene), 1.f, false, false),
            YGRoundValueToPixelGrid(y + CalculateObjectPosition(
                boxStyle->objectPositionY(), false, height, fitHeight, 0.5f, this->scene), 1.f, false, false),
            YGRoundValueToPixelGrid(fitWidth, 1.f, false, false),
            YGRoundValueToPixelGrid(fitHeight, 1.f, false, false),
        },
        boxStyle->tintColor() ? *boxStyle->tintColor() : 0xFFFFFFFF);

    if (clip) {
        renderer->PopClipRect();
    }
}

void ImageSceneNode::DestroyRecursive() {
    this->ClearImage();
    this->onLoadCallback.Reset();
    this->onErrorCallback.Reset();

    SceneNode::DestroyRecursive();
}

void ImageSceneNode::ClearImage() {
    if (this->image) {
        this->image->RemoveListener(this->imageListenerId);
        this->imageListenerId = 0;
        this->image->RemoveRef();
        this->image = nullptr;
        this->uri.clear();
    }
}

bool ImageSceneNode::DoCallbacks() {
    if (this->image) {
        if (this->image->HasError()) {
            if (!this->onErrorCallback.IsEmpty()) {
                 this->onErrorCallback.Call({});
            }

            return true;
        }

        if (this->image->IsReady()) {
            if (!this->onLoadCallback.IsEmpty()) {
                 this->onLoadCallback.Call({});
            }

            return true;
        }
    }

    return false;
}

} // namespace ls
