/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontSampleResource.h"
#include "FontResource.h"
#include <fmt/format.h>

namespace ls {

FontSampleResource::FontSampleResource(FontResource* fontResource, int32_t fontSize)
: Resource(MakeId(fontResource->GetFontFamily(), fontResource->GetFontStyle(), fontResource->GetFontWeight(),
    fontSize)),
  fontResource(fontResource),
  fontSize(fontSize) {
    this->fontResource->AddRef();

    if (this->fontResource->IsReady()) {
        this->fontMetrics = std::make_shared<FontMetrics>(this->fontResource->GetFontInfo(), this->fontSize);
        this->SetState(ResourceStateReady);
    } else if (this->fontResource->HasError()) {
        this->SetState(ResourceStateError);
    } else {
        this->fontResourceListenerId = this->fontResource->AddListener([this]() {
            // TODO: should be in loading state
            if (this->fontResource->IsReady()) {
                this->fontMetrics = std::make_shared<FontMetrics>(this->fontResource->GetFontInfo(), this->fontSize);
                this->SetStateAndNotifyListeners(ResourceStateReady);
            } else if (this->fontResource->HasError()) {
                this->SetStateAndNotifyListeners(ResourceStateError);
            } else {
                return;
            }

            this->fontResource->RemoveListener(this->fontResourceListenerId);
            this->fontResourceListenerId = 0;
        });
    }
}

FontSampleResource::~FontSampleResource() {
    if (this->fontResource) {
        this->fontResource->RemoveListener(this->fontResourceListenerId);
        this->fontResourceListenerId = 0;
        this->fontResource->RemoveRef();
        this->fontResource = nullptr;
    }
}

std::string FontSampleResource::MakeId(const std::string& fontFamily, StyleFontStyle fontStyle,
        StyleFontWeight fontWeight, int32_t fontSize) {
    return fmt::format("{}:{}:{}:{}", fontFamily, fontStyle, fontWeight, fontSize);
}

} // namespace ls
