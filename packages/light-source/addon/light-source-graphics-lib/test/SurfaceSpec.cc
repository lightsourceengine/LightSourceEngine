/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <ls/Surface.h>

using Napi::Assert;
using Napi::TestSuite;

namespace ls {

std::shared_ptr<uint8_t> NewBytes(int len, uint8_t fill = 0) {
    auto buffer{ new uint8_t[len] };

    std::fill_n(buffer, len, fill);

    return std::shared_ptr<uint8_t>(buffer, [](uint8_t* data) { delete [] data; });
}

bool BytesEqualTo(uint8_t* bytes, int32_t bytesLen, uint8_t value) {
    for (int32_t i = 0; i < 10 * 10; i++) {
        if (bytes[i] != value) {
            return false;
        }
    }

    return true;
}

void SurfaceSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("Surface") };

    spec->Describe("constructor")->tests = {
        {
            "should create a surface from a single channel buffer",
            [](const Napi::CallbackInfo& info) {
                auto buffer{ NewBytes(10 * 10) };
                auto surface{ Surface(buffer, 10, 10) };

                Assert::Equal(surface.Width(), 10);
                Assert::Equal(surface.Height(), 10);
                Assert::Equal(surface.Pitch(), 10);
                Assert::Equal(surface.Format(), PixelFormatAlpha);
                Assert::Equal(surface.Pixels(), buffer.get());
                Assert::IsFalse(surface.IsEmpty());
            }
        },
        {
            "should create a surface from a 4 channel buffer",
            [](const Napi::CallbackInfo& info) {
                auto buffer{ NewBytes(10 * 10 * 4) };
                auto surface{ Surface(buffer, 10, 10, 10 * 4, PixelFormatRGBA) };

                Assert::Equal(surface.Width(), 10);
                Assert::Equal(surface.Height(), 10);
                Assert::Equal(surface.Pitch(), 10 * 4);
                Assert::Equal(surface.Format(), PixelFormatRGBA);
                Assert::Equal(surface.Pixels(), buffer.get());
                Assert::IsFalse(surface.IsEmpty());
            }
        },
    };

    spec->Describe("FillTransparent()")->tests = {
        {
            "should clear single channel surface",
            [](const Napi::CallbackInfo& info) {
                uint8_t full{255};
                uint8_t zero{0};
                int32_t len{10 * 10};
                auto surface{ Surface(NewBytes(len, full), 10, 10) };

                Assert::IsTrue(BytesEqualTo(surface.Pixels(), len, full));

                surface.FillTransparent();

                Assert::IsTrue(BytesEqualTo(surface.Pixels(), len, zero));
            }
        },
        {
            "should clear 4 channel surface",
            [](const Napi::CallbackInfo& info) {
                uint8_t full{255};
                uint8_t zero{0};
                int32_t len{10 * 10 * 4};
                auto surface{ Surface(NewBytes(len, full), 10, 10, 10 * 4, PixelFormatRGBA) };

                Assert::IsTrue(BytesEqualTo(surface.Pixels(), len, full));

                surface.FillTransparent();

                Assert::IsTrue(BytesEqualTo(surface.Pixels(), len, zero));
            }
        },
    };
}

} // namespace ls
