/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/BoxSceneNode.h>

#include <lse/Scene.h>
#include <lse/Stage.h>
#include <lse/Style.h>
#include <lse/CompositeContext.h>
#include <lse/Renderer.h>

namespace lse {

BoxSceneNode::BoxSceneNode(napi_env env, Scene* scene) : SceneNode(env, scene) {
}

void BoxSceneNode::OnComputeStyle() {
  this->MarkCompositeDirty();
}

void BoxSceneNode::OnComposite(CompositeContext* ctx) {
  auto boxStyle{Style::Or(this->style)};

  this->DrawBackground(ctx, boxStyle->GetEnum<StyleBackgroundClip>(StyleProperty::backgroundClip));

  this->DrawBorder(ctx);
}

void BoxSceneNode::OnStylePropertyChanged(StyleProperty property) {
  switch (property) {
    case StyleProperty::backgroundImage:
      this->UpdateBackgroundImage(this->style->GetString(StyleProperty::backgroundImage));
      break;
    case StyleProperty::backgroundRepeat:
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
      this->MarkComputeStyleDirty();
      break;
    case StyleProperty::backgroundColor:
    case StyleProperty::borderColor:
      // TODO: check layout only
      this->MarkCompositeDirty();
      break;
    default:
      SceneNode::OnStylePropertyChanged(property);
      break;
  }
}

void BoxSceneNode::OnFlexBoxLayoutChanged() {
  this->MarkComputeStyleDirty();
}

void BoxSceneNode::UpdateBackgroundImage(const std::string& backgroundUri) {
  if (backgroundUri.empty()) {
    if (this->backgroundImage && this->backgroundImage->HasTexture()) {
      this->ClearBackgroundImageResource();
      this->MarkComputeStyleDirty();
    }
    return;
  }

  this->ClearBackgroundImageResource();
  this->backgroundImage = this->GetResources()->AcquireImage(backgroundUri);

  auto listener{ [this](Resource::Owner owner, Resource* res) {
    if (this != owner || this->backgroundImage.get() != res) {
      return;
    }

    res->RemoveListener(owner);
    this->MarkComputeStyleDirty();
  }};

  switch (this->backgroundImage->GetState()) {
    case Resource::State::Init:
      this->backgroundImage->AddListener(this, listener);
      this->backgroundImage->Load(this->env);
      break;
    case Resource::State::Loading:
      this->backgroundImage->AddListener(this, listener);
      break;
    case Resource::State::Ready:
    case Resource::State::Error:
      listener(this, this->backgroundImage.get());
      break;
  }
}

void BoxSceneNode::ClearBackgroundImageResource() {
  if (this->backgroundImage) {
    auto resource = this->backgroundImage.get();
    this->backgroundImage->RemoveListener(this);
    this->backgroundImage = nullptr;

    this->GetResources()->ReleaseResource(resource);
  }
}

void BoxSceneNode::OnDestroy() {
  this->ClearBackgroundImageResource();
}

// TODO: temporary hack due to scene and renderer shutdown conflicts
void BoxSceneNode::OnDetach() {
  this->ClearBackgroundImageResource();
}

} // namespace lse
