/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/RootSceneNode.h>

#include <lse/Scene.h>
#include <lse/Style.h>
#include <lse/Renderer.h>
#include <lse/CompositeContext.h>

namespace lse {

RootSceneNode::RootSceneNode(napi_env env, Scene* scene) : SceneNode(env, scene) {
}

void RootSceneNode::OnStylePropertyChanged(StyleProperty property) {
  switch (property) {
    case StyleProperty::backgroundColor:
    case StyleProperty::opacity:
      this->RequestComposite();
      break;
    case StyleProperty::fontSize:
      this->scene->OnRootFontSizeChange();
      break;
    default:
      break;
  }
}

void RootSceneNode::Composite(CompositeContext* composite) {
  const auto style = Style::Or(this->style);
  const auto backgroundColor = style->GetColor(StyleProperty::backgroundColor);

  if (backgroundColor.has_value()) {
    composite->renderer->FillRenderTarget(backgroundColor.value());
  }
}

} // namespace lse
