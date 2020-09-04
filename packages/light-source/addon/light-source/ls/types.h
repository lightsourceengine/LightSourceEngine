/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <memory>

#define LS_EXPECT_NULL(EXPR, MESSAGE) if ((EXPR) != nullptr) { throw std::runtime_error(MESSAGE); }
#define LS_EXPECT_NOT_NULL(EXPR, MESSAGE) if ((EXPR) == nullptr) { throw std::runtime_error(MESSAGE); }

namespace ls {

class Stage;
using StageRef = std::shared_ptr<Stage>;

class Scene;
using SceneRef = std::shared_ptr<Scene>;

class Style;
using StyleRef = std::shared_ptr<Style>;

} // namespace ls
