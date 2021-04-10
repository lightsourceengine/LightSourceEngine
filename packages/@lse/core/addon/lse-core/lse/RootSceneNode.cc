/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include <lse/RootSceneNode.h>

#include <lse/Scene.h>
#include <lse/Style.h>
#include <lse/Renderer.h>
#include <lse/CompositeContext.h>

namespace lse {

RootSceneNode::RootSceneNode(Scene* scene) : SceneNode(scene) {
}

void RootSceneNode::OnStylePropertyChanged(StyleProperty property) {
  switch (property) {
    case StyleProperty::backgroundColor:
    case StyleProperty::opacity:
      this->MarkCompositeDirty();
      break;
    case StyleProperty::fontSize:
      this->scene->OnRootFontSizeChange();
      break;
    default:
      break;
  }
}

void RootSceneNode::OnComposite(CompositeContext* composite) {
  if (this->style && !this->style->IsEmpty(StyleProperty::backgroundColor)) {
    composite->renderer->Clear(
        this->style->GetColor(StyleProperty::backgroundColor)->MixAlpha(composite->CurrentOpacity()));
  }
}

} // namespace lse
