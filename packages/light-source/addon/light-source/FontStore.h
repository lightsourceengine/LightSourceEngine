/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "FontResource.h"
#include "ResourceStore.h"

namespace ls {

class FontStore : public ResourceStore<FontResource, FontId, FontId::Hash> {
 public:
    explicit FontStore(Stage* stage);

    std::shared_ptr<FontResource> Find(const std::string& family, StyleFontStyle style, StyleFontWeight weight) const;
};

} // namespace ls
