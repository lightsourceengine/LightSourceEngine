/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi.h>

namespace ls {

struct DisplayMode {
    int width{};
    int height{};
};

struct Display {
    std::string name;
    DisplayMode defaultMode{};
    std::vector<DisplayMode> modes;
};

struct Capabilities {
    std::vector<Display> displays;
};

Napi::Object ToCapabilitiesView(Napi::Env env, const Capabilities& caps);

} // namespace ls
