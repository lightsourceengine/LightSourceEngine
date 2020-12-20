/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/Math.h>
#include <napi-unit.h>

#include <std20/numbers>

#include "../../light-source-platform-lib/ls/Matrix.h"

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

constexpr auto PI = std20::pi_v<float>;

namespace ls {

void MatrixSpec(TestSuite* parent) {
    auto spec{ parent->Describe("Matrix") };

    spec->Describe("Identity()")->tests = {
        {
            "should create an identity matrix",
            [](const TestInfo&) {
                auto m{ Matrix::Identity() };

                Assert::Equal(m.a, 1);
                Assert::Equal(m.b, 0);
                Assert::Equal(m.x, 0);
                Assert::Equal(m.c, 0);
                Assert::Equal(m.d, 1);
                Assert::Equal(m.y, 0);
            }
        },
    };

    spec->Describe("Rotate()")->tests = {
        {
            "should create a rotation matrix",
            [](const TestInfo&) {
                auto m{ Matrix::Rotate(PI / 4.f) };

                Assert::IsTrue(m.a != 0.f);
                Assert::IsTrue(m.b != 0.f);
                Assert::Equal(m.x, 0);
                Assert::IsTrue(m.c != 0.f);
                Assert::IsTrue(m.d != 0.f);
                Assert::Equal(m.y, 0);
            }
        },
    };

    spec->Describe("Scale()")->tests = {
        {
            "should create a scale matrix",
            [](const TestInfo&) {
                auto m{ Matrix::Scale(3, 3) };

                Assert::Equal(m.a, 3);
                Assert::Equal(m.b, 0);
                Assert::Equal(m.x, 0);
                Assert::Equal(m.c, 0);
                Assert::Equal(m.d, 3);
                Assert::Equal(m.y, 0);
            }
        },
    };

    spec->Describe("Translate()")->tests = {
        {
            "should create a translation",
            [](const TestInfo&) {
                auto m{ Matrix::Translate(50, 50) };

                Assert::Equal(m.a, 1);
                Assert::Equal(m.b, 0);
                Assert::Equal(m.x, 50);
                Assert::Equal(m.c, 0);
                Assert::Equal(m.d, 1);
                Assert::Equal(m.y, 50);
            }
        },
    };

    spec->Describe("GetAxisAngle()")->tests = {
        {
            "should get axis angle [+] (in radians) of rotation matrix",
            [](const TestInfo&) {
                auto radians{ PI / 4.f };
                auto m{ Matrix::Rotate(radians) };

                Assert::IsTrue(Equals(m.GetAxisAngle(), radians));
            }
        },
        {
            "should get axis angle [-] (in radians) of rotation matrix",
            [](const TestInfo&) {
                auto radians{ -PI / 4.f };
                auto m{ Matrix::Rotate(radians) };

                Assert::IsTrue(Equals(m.GetAxisAngle(), radians));
            }
        },
        {
            "should return zero for non-rotation matrix",
            [](const TestInfo&) {
                Assert::IsTrue(Equals(Matrix::Identity().GetAxisAngle(), 0));
            }
        },
    };

    spec->Describe("GetAxisAngleDeg()")->tests = {
        {
            "should get axis angle [+] (in degrees) of rotation matrix",
            [](const TestInfo&) {
                auto radians{ PI / 4.f };
                auto m{ Matrix::Rotate(radians) };

                Assert::IsTrue(Equals(m.GetAxisAngleDeg(), 45.f));
            }
        },
        {
            "should get axis angle [-] (in degrees) of rotation matrix",
            [](const TestInfo&) {
                auto radians{ -PI / 4.f };
                auto m{ Matrix::Rotate(radians) };

                Assert::IsTrue(Equals(m.GetAxisAngleDeg(), 360.f - 45.f));
            }
        },
        {
            "should return zero for non-rotation matrix",
            [](const TestInfo&) {
                Assert::IsTrue(Equals(Matrix::Identity().GetAxisAngleDeg(), 0));
            }
        },
    };

    spec->Describe("GetScale()")->tests = {
        {
            "should get scale x and y",
            [](const TestInfo&) {
                auto m{ Matrix::Scale(10, 100) };

                Assert::IsTrue(Equals(m.GetScaleX(), 10.f));
                Assert::IsTrue(Equals(m.GetScaleY(), 100.f));
            }
        },
        {
            "should return 1 for identity matrix",
            [](const TestInfo&) {
                auto m{ Matrix::Identity() };
                Assert::IsTrue(Equals(m.GetScaleX(), 1.f));
                Assert::IsTrue(Equals(m.GetScaleY(), 1.f));
            }
        },
    };

    spec->Describe("GetTranslate()")->tests = {
        {
            "should get translate x and y",
            [](const TestInfo&) {
                auto m{ Matrix::Translate(10, 100) };

                Assert::IsTrue(Equals(m.GetTranslateX(), 10.f));
                Assert::IsTrue(Equals(m.GetTranslateY(), 100.f));
            }
        },
        {
            "should return 0 for identity matrix",
            [](const TestInfo&) {
                auto m{ Matrix::Identity() };
                Assert::IsTrue(Equals(m.GetTranslateX(), 0.f));
                Assert::IsTrue(Equals(m.GetTranslateY(), 0.f));
            }
        },
    };

    spec->Describe("multiplication")->tests = {
        {
            "should multiply identity x translate(10, 100) = translate(10, 100)",
            [](const TestInfo&) {
                auto a{ Matrix::Identity() };
                auto b{ Matrix::Translate(10, 100) };
                auto m{ a * b };

                Assert::IsTrue(Equals(m.GetTranslateX(), 10.f));
                Assert::IsTrue(Equals(m.GetTranslateY(), 100.f));
            }
        },
        {
            "should multiply translate(10, 10)  x translate(10, 10) = translate(20, 20)",
            [](const TestInfo&) {
                auto a{ Matrix::Translate(10, 10) };
                auto b{ Matrix::Translate(10, 10) };
                auto m{ a * b };

                Assert::IsTrue(Equals(m.GetTranslateX(), 20.f));
                Assert::IsTrue(Equals(m.GetTranslateY(), 20.f));
            }
        },
    };
}

} // namespace ls
