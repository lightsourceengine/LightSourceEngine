/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontStore.h"

namespace ls {

FontStore::FontStore(Stage* stage) : ResourceStore<FontResource, FontId, FontId::Hash>(stage) {
}

std::shared_ptr<FontResource> FontStore::Find(const std::string& family,
        StyleFontStyle style, StyleFontWeight weight) const {
    return this->Get({ family, style, weight });
}

} // namespace ls
