/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontResource.h"
#include "FileSystem.h"
#include "AsyncTaskQueue.h"
#include "Font.h"
#include "Stage.h"
#include <stb_truetype.h>
#include <fmt/format.h>
#include <cstring>

namespace ls {

std::shared_ptr<TaskResult> LoadFont(
    const std::vector<std::string>& path, const std::string& filename, const int32_t index);

struct FontTaskResult : public TaskResult {
    FontTaskResult()
    : fontInfo(new stbtt_fontinfo(), [](stbtt_fontinfo* p) { if (p->data) { delete [] p->data; } delete p; }) {
        this->fontInfo->data = nullptr;
    }

    std::shared_ptr<stbtt_fontinfo> fontInfo;
};

FontResource::FontResource(const FontId& fontId, const std::string& uri, const int32_t index)
        : BaseResource(fontId), uri(uri), index(index) {
}

void FontResource::Load(Stage* stage) {
    auto initialState{ ResourceStateLoading };
    auto uri{ this->uri };
    auto index{ this-> index };
    auto path{ stage->GetResourcePath() };

    assert(!this->task);

    try {
        this->task = std::make_shared<Task>(
            [path, uri, index]() -> std::shared_ptr<TaskResult> {
                return LoadFont({ path }, uri, index);
            },
            [this](std::shared_ptr<TaskResult> taskResult) {
                assert(taskResult != nullptr);

                // TODO: assert(this->resourceState != ResourceStateLoading)
                // TODO: assert(this->GetRefCount() > 0)

                auto fontTaskResult{ std::static_pointer_cast<FontTaskResult>(taskResult) };

                this->fontInfo = fontTaskResult->fontInfo;
                this->task = nullptr;

                this->SetState(ResourceStateReady, true);
            },
            [this](const std::string& errorMessage) {
                fmt::println("FontResource.Load(): Error: {}", errorMessage);
                this->SetState(ResourceStateError, true);
            });
    } catch (std::exception& e) {
        initialState = ResourceStateError;
    }

    try {
        if (this->task) {
            stage->GetAsyncTaskQueue()->Submit(this->task);
        }
    } catch (std::exception& e) {
        initialState = ResourceStateError;
    }

    this->SetState(initialState, true);
}

void FontResource::Attach(Stage* stage, Scene* scene) {
    this->Load(stage);
}

std::shared_ptr<Font> FontResource::GetFont(int32_t fontSize) const {
    // TODO: assert font size
    // TODO: assert fontInfo / ready

    auto it{ this->fontsBySize.find(fontSize) };

    if (it == this->fontsBySize.end()) {
        std::shared_ptr<Font> font;

        try {
            font = std::make_shared<Font>(this->fontInfo, fontSize);
            this->fontsBySize.insert(std::make_pair(fontSize, font));
        } catch (std::exception& e) {
            fmt::println("Failed to create font {} @ {}px. Error: {}", this->id.family, fontSize, e.what());
            font.reset();
        }

        return font;
    }

    return it->second;
}

std::shared_ptr<TaskResult> LoadFont(const std::vector<std::string>& path, const std::string& filename,
        const int32_t index) {
    auto result{ std::make_shared<FontTaskResult>() };
    std::string resolvedFilename;

    if (IsResourceUri(filename)) {
        resolvedFilename = FindFile(GetResourceUriPath(filename), {}, path);
    } else {
        resolvedFilename = filename;
    }

    FileHandle file(fopen(resolvedFilename.c_str(), "rb"), fclose);

    if (!file) {
        throw std::runtime_error(fmt::format("Failed to open ttf file: {}", filename));
    }

    fseek(file.get(), 0, SEEK_END);
    auto size{ static_cast<size_t>(ftell(file.get())) };
    fseek(file.get(), 0, SEEK_SET);

    std::unique_ptr<uint8_t[]> ttf(new uint8_t[size]);

    if (fread(ttf.get(), 1, size, file.get()) != size) {
        throw std::runtime_error(fmt::format("Failed to read file: {}", filename));
    }

    auto offset{ stbtt_GetFontOffsetForIndex(ttf.get(), index) };

    if (offset == -1) {
        throw std::runtime_error("Cannot find font at index.");
    }

    if (stbtt_InitFont(result->fontInfo.get(), ttf.get(), offset) == 0) {
        throw std::runtime_error("Failed to init font.");
    }

    ttf.release();

    return result;
}

} // namespace ls
