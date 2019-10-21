/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <ls/Matrix.h>

using Napi::Assert;
using Napi::TestSuite;

constexpr auto PI = static_cast<float>(M_PI);

namespace ls {

static bool eq(float a, float b) noexcept {
    return fabs(a - b) < 0.001f;
}

void MatrixSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("Matrix") };
    auto assert{ Assert(env) };

    spec->Describe("Identity()")->tests = {
        {
            "should create an identity matrix",
            [assert](const Napi::CallbackInfo& info) {
                auto m{ Matrix::Identity() };

                assert.Equal(m.a, 1);
                assert.Equal(m.b, 0);
                assert.Equal(m.x, 0);
                assert.Equal(m.c, 0);
                assert.Equal(m.d, 1);
                assert.Equal(m.y, 0);
            }
        },
    };

    spec->Describe("Rotate()")->tests = {
        {
            "should create a rotation matrix",
            [assert](const Napi::CallbackInfo& info) {
                auto m{ Matrix::Rotate(PI / 4) };

                assert.IsTrue(m.a != 0.f);
                assert.IsTrue(m.b != 0.f);
                assert.Equal(m.x, 0);
                assert.IsTrue(m.c != 0.f);
                assert.IsTrue(m.d != 0.f);
                assert.Equal(m.y, 0);
            }
        },
    };

    spec->Describe("Scale()")->tests = {
        {
            "should create a scale matrix",
            [assert](const Napi::CallbackInfo& info) {
                auto m{ Matrix::Scale(3, 3) };

                assert.Equal(m.a, 3);
                assert.Equal(m.b, 0);
                assert.Equal(m.x, 0);
                assert.Equal(m.c, 0);
                assert.Equal(m.d, 3);
                assert.Equal(m.y, 0);
            }
        },
    };

    spec->Describe("Translate()")->tests = {
        {
            "should create a translation",
            [assert](const Napi::CallbackInfo& info) {
                auto m{ Matrix::Translate(50, 50) };

                assert.Equal(m.a, 1);
                assert.Equal(m.b, 0);
                assert.Equal(m.x, 50);
                assert.Equal(m.c, 0);
                assert.Equal(m.d, 1);
                assert.Equal(m.y, 50);
            }
        },
    };

    spec->Describe("GetAxisAngle()")->tests = {
        {
            "should get axis angle (in radians) of rotation matrix",
            [assert](const Napi::CallbackInfo& info) {
                auto radians{ PI / 4 };
                auto m{ Matrix::Rotate(radians) };

                assert.IsTrue(eq(m.GetAxisAngle(), radians));
            }
        },
        {
            "should return zero for non-rotation matrix",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(eq(Matrix::Identity().GetAxisAngle(), 0));
            }
        },
    };

    spec->Describe("GetAxisAngleDeg()")->tests = {
        {
            "should get axis angle (in degrees) of rotation matrix",
            [assert](const Napi::CallbackInfo& info) {
                auto radians{ PI / 4 };
                auto m{ Matrix::Rotate(radians) };

                assert.IsTrue(eq(m.GetAxisAngleDeg(), 45.f));
            }
        },
        {
            "should return zero for non-rotation matrix",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(eq(Matrix::Identity().GetAxisAngleDeg(), 0));
            }
        },
    };

    spec->Describe("GetScale()")->tests = {
        {
            "should get scale x and y",
            [assert](const Napi::CallbackInfo& info) {
                auto m{ Matrix::Scale(10, 100) };

                assert.IsTrue(eq(m.GetScaleX(), 10.f));
                assert.IsTrue(eq(m.GetScaleY(), 100.f));
            }
        },
        {
            "should return 1 for identity matrix",
            [assert](const Napi::CallbackInfo& info) {
                auto m{ Matrix::Identity() };
                assert.IsTrue(eq(m.GetScaleX(), 1.f));
                assert.IsTrue(eq(m.GetScaleY(), 1.f));
            }
        },
    };

    spec->Describe("GetTranslate()")->tests = {
        {
            "should get translate x and y",
            [assert](const Napi::CallbackInfo& info) {
                auto m{ Matrix::Translate(10, 100) };

                assert.IsTrue(eq(m.GetTranslateX(), 10.f));
                assert.IsTrue(eq(m.GetTranslateY(), 100.f));
            }
        },
        {
            "should return 0 for identity matrix",
            [assert](const Napi::CallbackInfo& info) {
                auto m{ Matrix::Identity() };
                assert.IsTrue(eq(m.GetTranslateX(), 0.f));
                assert.IsTrue(eq(m.GetTranslateY(), 0.f));
            }
        },
    };

    spec->Describe("multiplication")->tests = {
        {
            "should multiply identity x translate(10, 100) = translate(10, 100)",
            [assert](const Napi::CallbackInfo& info) {
                auto a{ Matrix::Identity() };
                auto b{ Matrix::Translate(10, 100) };
                auto m{ a * b };

                assert.IsTrue(eq(m.GetTranslateX(), 10.f));
                assert.IsTrue(eq(m.GetTranslateY(), 100.f));
            }
        },
        {
            "should multiply translate(10, 10)  x translate(10, 10) = translate(20, 20)",
            [assert](const Napi::CallbackInfo& info) {
                auto a{ Matrix::Translate(10, 10) };
                auto b{ Matrix::Translate(10, 10) };
                auto m{ a * b };

                assert.IsTrue(eq(m.GetTranslateX(), 20.f));
                assert.IsTrue(eq(m.GetTranslateY(), 20.f));
            }
        },
    };
}

} // namespace ls
