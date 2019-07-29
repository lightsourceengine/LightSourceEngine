/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontResource.h"
#include "FileSystem.h"
#include <fmt/format.h>

namespace ls {

std::vector<uint8_t> LoadFont(const std::string filename, const int32_t index, stbtt_fontinfo* info);

FontResource::FontResource(const std::string& id, const std::string& uri, const int32_t index,
        const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight)
    : Resource(id), uri(uri), index(index), family(family), fontStyle(fontStyle), fontWeight(fontWeight) {
}

std::string FontResource::MakeId(const std::string& family, StyleFontStyle fontStyle, StyleFontWeight fontWeight) {
    return fmt::format("{}:{}:{}", family, fontStyle, fontWeight);
}

void FontResource::Load(Napi::Env env) {
    auto initialState{ ResourceStateLoading };

    this->AddRef();

    try {
        this->work = std::make_unique<AsyncWork>(
            env,
            this->id,
            [this](Napi::Env env) {
                // TODO: use resource path
                this->fontInfoData = LoadFont(this->uri, this->index, &this->fontInfo);
            },
            [this](Napi::Env env, napi_status status, const std::string& message) {
                this->RemoveRef();

                if (this->resourceState != ResourceStateLoading) {
                    return;
                }

                ResourceState nextState;

                if (status != napi_ok) {
                    nextState = ResourceStateError;
                } else {
                    nextState = ResourceStateReady;
                }

                this->SetStateAndNotifyListeners(nextState);
            });
    } catch (std::exception& e) {
        this->RemoveRef();
        initialState = ResourceStateError;
    }

    this->SetStateAndNotifyListeners(initialState);
}

bool FontResource::IsReady() const {
    return this->resourceState == ResourceStateReady;
}

std::vector<uint8_t> LoadFont(const std::string filename, const int32_t index, stbtt_fontinfo* info) {
    auto buffer{ ReadBytes(filename) };
    auto offset = stbtt_GetFontOffsetForIndex(&buffer[0], index);

    if (offset == -1) {
        throw std::runtime_error("Cannot find font at index.");
    }

    if (stbtt_InitFont(info, &buffer[0], offset) == 0) {
        throw std::runtime_error("Failed to init font.");
    }

    return buffer;
}

} // namespace ls
