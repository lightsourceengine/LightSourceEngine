/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/StyleEnums.h>
#include <ls/bindings/JSEnums.h>

using Napi::ClassBuilder;

namespace ls {
namespace bindings {

Napi::Function NewStyleUnitClass(Napi::Env env) {
    return ClassBuilder(env, "StyleUnit")
        .WithStaticValue("Undefined", StyleNumberUnitUndefined)
        .WithStaticValue("Point", StyleNumberUnitPoint)
        .WithStaticValue("Percent", StyleNumberUnitPercent)
        .WithStaticValue("ViewportWidth", StyleNumberUnitViewportWidth)
        .WithStaticValue("ViewportHeight", StyleNumberUnitViewportHeight)
        .WithStaticValue("ViewportMin", StyleNumberUnitViewportMin)
        .WithStaticValue("ViewportMax", StyleNumberUnitViewportMax)
        .WithStaticValue("Auto", StyleNumberUnitAuto)
        .WithStaticValue("Anchor", StyleNumberUnitAnchor)
        .WithStaticValue("RootEm", StyleNumberUnitRootEm)
        .WithStaticValue("Radian", StyleNumberUnitRadian)
        .WithStaticValue("Degree", StyleNumberUnitDegree)
        .WithStaticValue("Gradian", StyleNumberUnitGradian)
        .WithStaticValue("Turn", StyleNumberUnitTurn)
        .ToConstructor();
}

Napi::Function NewStyleTransformClass(Napi::Env env) {
    return ClassBuilder(env, "StyleTransform")
        .WithStaticValue("Identity", StyleTransformIdentity)
        .WithStaticValue("Translate", StyleTransformTranslate)
        .WithStaticValue("Rotate", StyleTransformRotate)
        .WithStaticValue("Scale", StyleTransformScale)
        .ToConstructor();
}

Napi::Function NewStyleAnchorClass(Napi::Env env) {
    return ClassBuilder(env, "StyleAnchor")
        .WithStaticValue("Left", StyleAnchorLeft)
        .WithStaticValue("Right", StyleAnchorRight)
        .WithStaticValue("Center", StyleAnchorCenter)
        .WithStaticValue("Top", StyleAnchorTop)
        .WithStaticValue("Bottom", StyleAnchorBottom)
        .ToConstructor();
}

} // namespace bindings
} // namespace ls
