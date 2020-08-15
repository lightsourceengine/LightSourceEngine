/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/Renderer.h>
#include <ls/RenderingContext2D.h>
#include <ls/Resources.h>
#include <ls/Style.h>
#include <ls/StyleResolver.h>
#include <ls/TextBlock.h>
#include <ls/Timer.h>
#include <math.h>

namespace ls {

void TextBlock::Shape(const std::string& utf8, FontFace* fontFace, Style* style, const StyleResolver& resolver,
        float maxWidth, float maxHeight) {
    this->Invalidate();

    if (utf8.empty() || !fontFace || fontFace->GetState() != Res::Ready || !style) {
        return;
    }

    const auto fontSize = resolver.ResolveFontSize(style->fontSize);

    if (fontSize <= 0 || style->fontFamily.empty()) {
        return;
    }

    auto f{ fontFace->GetFont(fontSize) };

    this->font = f;

    if (this->font.empty()) {
        return;
    }

    this->glyphBuffer.setUtf8Text(utf8.c_str(), utf8.size());

    this->font.shape(this->glyphBuffer);

    BLTextMetrics tm{};

    if (this->font.getTextMetrics(this->glyphBuffer, tm) != BL_SUCCESS) {
        return;
    }

    this->calculatedWidth = ::ceilf(tm.advance.x);
    this->calculatedHeight
        = ::ceilf(this->font.metrics().ascent + this->font.metrics().descent + this->font.metrics().lineGap);
}

void TextBlock::Paint(RenderingContext2D* context) {
    if (this->IsEmpty()) {
        return;
    }

    auto target{ this->EnsureLockableTexture(context->renderer, this->calculatedWidth, this->calculatedHeight) };

    if (!target) {
        LOG_ERROR("Failed to create paint texture.");
        return;
    }

    auto pixels{ target.Lock() };
    Timer t("text render");

    context->Begin(pixels.Data(), pixels.Width(), pixels.Height(), pixels.Pitch());

    // Fill entire texture surface with transparent to start from a known state.
    context->SetColor(0);
    context->FillAll();

    float x;
    float y = 0 + font.metrics().ascent;

    context->SetColor(ColorWhite);
    context->SetFont(this->font);

    switch (this->textAlign) {
        case StyleTextAlignCenter:
            x = static_cast<float>(target.Width() - this->calculatedWidth) / 2.f;
            break;
        case StyleTextAlignRight:
            x = static_cast<float>(target.Width() - this->calculatedWidth);
            break;
        case StyleTextAlignLeft:
        default:
            x = 0;
            break;
    }

    context->FillText(x, y, this->glyphBuffer.glyphRun());

    context->End();

    pixels.Release();
}

int32_t TextBlock::Width() const noexcept {
    return this->calculatedWidth;
}

int32_t TextBlock::Height() const noexcept {
    return this->calculatedHeight;
}

float TextBlock::WidthF() const noexcept {
    return this->calculatedWidth;
}

float TextBlock::HeightF() const noexcept {
    return this->calculatedHeight;
}

void TextBlock::Invalidate() noexcept {
    this->calculatedWidth = this->calculatedHeight = 0;
}

bool TextBlock::IsEmpty() const noexcept {
    return this->calculatedWidth <= 0 || this->calculatedHeight <= 0;
}

} // namespace ls

//static std::string TextTransform(const Napi::Env& env, const StyleTextTransform transform, const std::string& text) {
//    HandleScope scope(env);
//
//    switch (transform) {
//        case StyleTextTransformUppercase:
//            return ToUpperCase(String::New(env, text));
//        case StyleTextTransformLowercase:
//            return ToLowerCase(String::New(env, text));
//        case StyleTextTransformNone:
//        default:
//            return text;
//    }
//}
