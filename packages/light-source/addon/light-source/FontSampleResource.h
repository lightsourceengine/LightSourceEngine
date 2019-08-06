/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "Resource.h"
#include "StyleEnums.h"
#include "FontMetrics.h"

namespace ls {

class ResourceManager;
class FontResource;

class FontSampleResource : public Resource {
 public:
    explicit FontSampleResource(Napi::Env env, FontResource* fontResource, int32_t fontSize);
    virtual ~FontSampleResource();

    static std::string MakeId(const std::string& fontFamily, StyleFontStyle fontStyle, StyleFontWeight fontWeight,
        int32_t fontSize);

    std::shared_ptr<FontMetrics> GetFontMetrics() { return this->fontMetrics; }

 private:
    FontResource* fontResource{};
    int32_t fontSize{};
    uint32_t fontResourceListenerId{0};
    std::shared_ptr<FontMetrics> fontMetrics{};

    friend ResourceManager;
};

} // namespace ls
