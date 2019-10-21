/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <StyleUtils.h>

using Napi::Assert;
using Napi::TestSuite;

namespace ls {

void StyleUtilsSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("StyleUtils") };
    auto assert{ Assert(env) };

    spec->Describe("CreateRoundedRectangleUri()")->tests = {
        {
            "should create rounded rectangle svg xml",
            [=](const Napi::CallbackInfo& info) {
                auto svg{ CreateRoundedRectangleUri(10, 10, 10, 10, 4) };

                // TODO: validate svg string
                assert.IsFalse(svg.empty());
            }
        },
    };

    // TODO: implement tests
//    spec->Describe("CalculateBackgroundDimension()")->tests = {
//        {
//            "should ...",
//            [=](const Napi::CallbackInfo& info) {
//            }
//        },
//    };
//
//    spec->Describe("CalculateObjectFitDimensions()")->tests = {
//        {
//            "should ...",
//            [=](const Napi::CallbackInfo& info) {
//            }
//        },
//    };
//
//    spec->Describe("CalculateObjectPosition()")->tests = {
//        {
//            "should ...",
//            [=](const Napi::CallbackInfo& info) {
//            }
//        },
//    };
//
//    spec->Describe("ComputeIntegerPointValue()")->tests = {
//        {
//            "should ...",
//            [=](const Napi::CallbackInfo& info) {
//            }
//        },
//    };
//
//    spec->Describe("CalculateLineHeight()")->tests = {
//        {
//            "should ...",
//            [=](const Napi::CallbackInfo& info) {
//            }
//        },
//    };
}

} // namespace ls
