/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontResource.h"
#include "Font.h"
#include "Stage.h"
#include <ls/FileSystem.h>
#include <stb_truetype.h>
#include <fmt/println.h>
#include <cstring>

namespace ls {

std::shared_ptr<stbtt_fontinfo> LoadFont(const std::vector<std::string>&, const std::string&, const int32_t);

FontResource::FontResource(const FontId& fontId, const std::string& uri, const int32_t index) noexcept
: Resource(fontId), uri(uri), index(index) {
}

FontResource::~FontResource() noexcept {
    this->fontLoadingTask.Cancel();
}

void FontResource::Load(Stage* stage) {
    this->fontLoadingTask.Cancel();

    auto initialState{ ResourceStateLoading };
    const auto uri{ this->uri };
    const auto index{ this-> index };
    const auto path{ stage->GetResourcePath() };

    auto execute = [path, uri, index]() -> std::shared_ptr<stbtt_fontinfo> {
        return LoadFont({ path }, uri, index);
    };

    auto callback = [this](std::shared_ptr<stbtt_fontinfo>&& fontInfo, const std::exception_ptr& eptr) {
        if (this->resourceState != ResourceStateLoading) {
            return;
        }

        ResourceState nextState;

        if (eptr) {
            nextState = ResourceStateError;
        } else {
            this->fontInfo = fontInfo;
            nextState = ResourceStateReady;
        }

        this->SetState(nextState, true);
    };

    try {
        this->fontLoadingTask = stage->GetTaskQueue()->Async<std::shared_ptr<stbtt_fontinfo>>(
            std::move(execute), std::move(callback));
    } catch (const std::exception&) {
        initialState = ResourceStateError;
    }

    this->SetState(initialState, true);
}

void FontResource::Reset() {
    this->fontLoadingTask.Cancel();

    this->fontInfo.reset();
    this->fontsBySize.clear();

    this->SetState(ResourceStateInit, false);
}

std::shared_ptr<Font> FontResource::GetFont(int32_t fontSize) const {
    // TODO: assert font size
    // TODO: assert fontInfo / ready

    const auto it{ this->fontsBySize.find(fontSize) };

    if (it == this->fontsBySize.end()) {
        try {
            // TODO: font info might be null...
            const auto font{ std::make_shared<Font>(this->fontInfo, fontSize) };

            this->fontsBySize.insert(std::make_pair(fontSize, font));

            return font;
        } catch (std::exception& e) {
            fmt::println("Failed to create font {} @ {}px. Error: {}", this->id.family, fontSize, e.what());

            return {};
        }
    }

    return it->second;
}

std::shared_ptr<stbtt_fontinfo> LoadFont(const std::vector<std::string>& path, const std::string& filename,
        const int32_t index) {
    const std::shared_ptr<stbtt_fontinfo> result(
        new stbtt_fontinfo(), [](stbtt_fontinfo* p) { if (p->data) { delete [] p->data; } delete p; });
    const auto resolvedFilename{
        IsResourceUri(filename) ? FindFile(GetResourceUriPath(filename), {}, path) : fs::path(filename)
    };
    const FileHandle file(fopen(resolvedFilename.c_str(), "rb"), fclose);

    if (!file) {
        throw std::runtime_error(fmt::format("Failed to open ttf file: {}", filename));
    }

    fseek(file.get(), 0, SEEK_END);
    const auto size{ static_cast<size_t>(ftell(file.get())) };
    fseek(file.get(), 0, SEEK_SET);

    std::unique_ptr<uint8_t[]> ttf(new uint8_t[size]);

    if (fread(ttf.get(), 1, size, file.get()) != size) {
        throw std::runtime_error(fmt::format("Failed to read file: {}", filename));
    }

    const auto offset{ stbtt_GetFontOffsetForIndex(ttf.get(), index) };

    if (offset == -1) {
        throw std::runtime_error("Cannot find font at index.");
    }

    if (stbtt_InitFont(result.get(), ttf.get(), offset) == 0) {
        throw std::runtime_error("Failed to init font.");
    }

    ttf.release();

    return result;
}

} // namespace ls
