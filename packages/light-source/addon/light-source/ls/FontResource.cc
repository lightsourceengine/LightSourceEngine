/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontResource.h"
#include "Font.h"
#include "Stage.h"
#include <ls/FileSystem.h>
#include <ls/Log.h>
#include <ls/Format.h>
#include <cstring>
#include <ls-ctx.h>

namespace ls {

static std::shared_ptr<Font> LoadFont(const std::vector<std::string>&, const std::string&, const int32_t);
static std::string FontId(const std::string& family, StyleFontStyle style, StyleFontWeight weight);

FontResource::FontResource(const std::string& family, StyleFontStyle style, StyleFontWeight weight,
                           const std::string& uri, int32_t index) noexcept
: Resource(FontId(family, style, weight)), family(family), style(style), weight(weight), uri(uri), index(index) {
}

FontResource::~FontResource() noexcept {
    this->CtxUninstall();
    this->fontLoadingTask.Cancel();
}

bool FontResource::IsSame(const std::string& family, StyleFontStyle style, StyleFontWeight weight) const noexcept {
    return this->family == family && this->style == style && this->weight == weight;
}

void FontResource::Load(Stage* stage) {
    this->fontLoadingTask.Cancel();

    auto execute = [
        path = stage->GetResourcePath(),
        uri = this->uri,
        index = this->index]() -> std::shared_ptr<Font> {
        return LoadFont({ path }, uri, index);
    };

    auto callback = [this](std::shared_ptr<Font>& font, const std::exception_ptr& eptr) {
        constexpr auto LAMBDA_FUNCTION = "FontResourceLoadCallback";

        if (this->resourceState != ResourceStateLoading) {
            return;
        }

        ResourceState nextState;

        if (eptr) {
            try {
                std::rethrow_exception(eptr);
            } catch (const std::exception& e) {
                LOG_ERROR_LAMBDA(e);
            }

            nextState = ResourceStateError;
        } else {
            this->font = font;
            ctx_load_font_info(this->id.c_str(), font->Info());
            this->ctxInstalled = true;
            LOG_INFO_LAMBDA("%s: %s", ResourceStateToString(ResourceStateReady), this->id);

            nextState = ResourceStateReady;
        }

        this->SetState(nextState, true);
    };

    try {
        this->fontLoadingTask = stage->GetTaskQueue()->Async<std::shared_ptr<Font>>(
            std::move(execute), std::move(callback));
    } catch (const std::exception& e) {
        LOG_ERROR(e);
        this->SetState(ResourceStateError, true);

        return;
    }

    LOG_INFO("%s: %s", ResourceStateToString(ResourceStateLoading), std::string(this->GetId()));
    this->SetState(ResourceStateLoading, true);
}

void FontResource::Reset() {
    this->CtxUninstall();
    this->fontLoadingTask.Cancel();
    this->font.reset();
    this->SetState(ResourceStateInit, false);
}

void FontResource::CtxUninstall() {
    if (this->ctxInstalled) {
        ctx_unload_font(this->id.c_str());
        this->ctxInstalled = false;
    }
}

static std::string FontId(const std::string& family, StyleFontStyle style, StyleFontWeight weight) {
    return Format("%s:%i:%i", family, style, weight);
}

static std::shared_ptr<Font> LoadFont(const std::vector<std::string>& path, const std::string& filename,
        const int32_t index) {
    const auto resolvedFilename{
        IsResourceUri(filename) ? FindFile(GetResourceUriPath(filename), {}, path) : filename
    };
    const auto file{ CFile::Open(resolvedFilename) };
    const auto size{ file.GetSize() };
    auto ttf{ std::make_unique<uint8_t[]>(size) };

    file.Read(ttf.get(), size);

    return std::make_shared<Font>(std::move(ttf), index);
}

} // namespace ls
