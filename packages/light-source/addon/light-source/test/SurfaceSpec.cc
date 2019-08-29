/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "LightSourceSpecList.h"
#include "TestGroup.h"
#include "../Surface.h"

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

void SurfaceSpec(TestGroup* parent) {
    auto suite{ parent->Describe("Surface") };
    auto assert{ Assert(parent->Env()) };

    suite->Describe("constructor")->tests = {
        {
            "should create a single channel surface of 10x10 pixels",
            [assert](const Napi::CallbackInfo& info) {
                auto surface{ Surface(10, 10) };

                assert.Equal(surface.Width(), 10);
                assert.Equal(surface.Height(), 10);
                assert.Equal(surface.Pitch(), 10);
                assert.Equal(surface.Format(), PixelFormatAlpha);
                assert.IsTrue(surface.Pixels() != nullptr);
                assert.IsFalse(surface.IsEmpty());
            }
        },
        {
            "should create a surface from a single channel buffer",
            [assert](const Napi::CallbackInfo& info) {
                auto buffer{ NewBytes(10 * 10) };
                auto surface{ Surface(buffer, 10, 10) };

                assert.Equal(surface.Width(), 10);
                assert.Equal(surface.Height(), 10);
                assert.Equal(surface.Pitch(), 10);
                assert.Equal(surface.Format(), PixelFormatAlpha);
                assert.Equal(surface.Pixels(), buffer.get());
                assert.IsFalse(surface.IsEmpty());
            }
        },
        {
            "should create a surface from a 4 channel buffer",
            [assert](const Napi::CallbackInfo& info) {
                auto buffer{ NewBytes(10 * 10 * 4) };
                auto surface{ Surface(buffer, 10, 10, 10 * 4, PixelFormatRGBA) };

                assert.Equal(surface.Width(), 10);
                assert.Equal(surface.Height(), 10);
                assert.Equal(surface.Pitch(), 10 * 4);
                assert.Equal(surface.Format(), PixelFormatRGBA);
                assert.Equal(surface.Pixels(), buffer.get());
                assert.IsFalse(surface.IsEmpty());
            }
        },
    };

    suite->Describe("FillTransparent()")->tests = {
        {
            "should clear single channel surface",
            [assert](const Napi::CallbackInfo& info) {
                uint8_t full{255};
                uint8_t zero{0};
                int32_t len{10 * 10};
                auto surface{ Surface(NewBytes(len, full), 10, 10) };

                assert.IsTrue(BytesEqualTo(surface.Pixels(), len, full));

                surface.FillTransparent();

                assert.IsTrue(BytesEqualTo(surface.Pixels(), len, zero));
            }
        },
        {
            "should clear 4 channel surface",
            [assert](const Napi::CallbackInfo& info) {
                uint8_t full{255};
                uint8_t zero{0};
                int32_t len{10 * 10 * 4};
                auto surface{ Surface(NewBytes(len, full), 10, 10, 10 * 4, PixelFormatRGBA) };

                assert.IsTrue(BytesEqualTo(surface.Pixels(), len, full));

                surface.FillTransparent();

                assert.IsTrue(BytesEqualTo(surface.Pixels(), len, zero));
            }
        },
    };
}

} // namespace ls
