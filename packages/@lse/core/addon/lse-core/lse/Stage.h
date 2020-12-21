/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/Resources.h>
#include <lse/ThreadPool.h>

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
class Stage {
 public:
  void Update();
  void Destroy();
  ThreadPool* GetThreadPool() const noexcept { return &this->threadPool; }
  Resources* GetResources() const noexcept { return &this->resources; }

 private:
  mutable ThreadPool threadPool;
  mutable Resources resources;
};

} // namespace lse
