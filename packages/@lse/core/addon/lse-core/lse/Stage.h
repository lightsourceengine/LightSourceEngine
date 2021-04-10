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

#pragma once

#include <lse/Reference.h>

namespace lse {

/**
 * Application instance for a Light Source Engine app.
 *
 * Stage is analogous to a desktop and a Scene is analogous to a window owned by desktop. The Stage API is split
 * between native (this class) and javascript (Stage.js). This class contains the API required by the native
 * layer, including the functionality to expose to javascript. The javascript bindings can be found in JSStage.
 *
 * The Stage abject can be instanced, but in practice Light Source Engine has a single instance of a Stage. The
 * instance is controlled by a shared_ptr that is owned by JSStage and Scene.
 */
class Stage : public Reference {
 public:
  void Destroy();
};

} // namespace lse
