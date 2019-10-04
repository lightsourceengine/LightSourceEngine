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
#include <fmt/println.h>
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
    const auto uri{ this->uri };
    const auto index{ this-> index };
    const auto path{ stage->GetResourcePath() };

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

                const auto fontTaskResult{ std::static_pointer_cast<FontTaskResult>(taskResult) };

                this->fontInfo = fontTaskResult->fontInfo;
                this->task = nullptr;

                this->SetState(ResourceStateReady, true);
            },
            [this](const std::string& errorMessage) {
                fmt::println("FontResource.Load(): Error: {}", errorMessage);
                this->SetState(ResourceStateError, true);
            });
    } catch (const std::exception&) {
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

    const auto it{ this->fontsBySize.find(fontSize) };

    if (it == this->fontsBySize.end()) {
        try {
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

std::shared_ptr<TaskResult> LoadFont(const std::vector<std::string>& path, const std::string& filename,
        const int32_t index) {
    const auto result{ std::make_shared<FontTaskResult>() };
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

    if (stbtt_InitFont(result->fontInfo.get(), ttf.get(), offset) == 0) {
        throw std::runtime_error("Failed to init font.");
    }

    ttf.release();

    return result;
}

} // namespace ls
