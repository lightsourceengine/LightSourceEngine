/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/bindings/JSStyleTransformSpec.h>

#include <lse/bindings/JSStyleValue.h>

static Napi::FunctionReference jsStyleTransformSpecClass;
static Napi::ObjectReference jsStyleTransformSpecUndefined;

namespace lse {
namespace bindings {

Napi::Function JSStyleTransformSpec::GetClass(const Napi::Env& env) {
  if (jsStyleTransformSpecClass.IsEmpty()) {
    const char* script = R"(
            const StyleTransformSpec = class StyleTransformSpec {
                constructor(transform, ...args) {
                    switch (transform) {
                        case 0:
                            this.x = this.y = this.angle = undefined
                            break;
                        case 2:
                            this.x = this.y = undefined
                            this.angle = Object.freeze(args[0])
                            break;
                        case 1:
                        case 3:
                            this.x = Object.freeze(args[0])
                            this.y = Object.freeze(args[1])
                            this.angle = undefined
                            break;
                        default:
                            this.x = this.y = this.angle = undefined
                            transform = -1
                            break;
                    }

                    this.transform = transform
                    
                    Object.freeze(this)
                }

                isUndefined() {
                    return this.transform < 0
                }

                static rotate(angle) {
                    const spec = new StyleTransformSpec(2, angle)
                    return StyleTransformSpec.validate(spec) ? spec : undefined
                }

                static identity(angle) {
                    return new StyleTransformSpec(0)
                }

                static translate(x, y) {
                    const spec = new StyleTransformSpec(1, x, y)
                    return StyleTransformSpec.validate(spec) ? spec : undefined
                }

                static scale(x, y) {
                    const spec = new StyleTransformSpec(3, x, y)
                    return StyleTransformSpec.validate(spec) ? spec : undefined
                }
            }

            /*return*/ StyleTransformSpec
        )";

    auto result{ Napi::RunScript(env, script) };

    if (!result.IsFunction()) {
      throw Napi::Error::New(env, "Failed to parse StyleTransformSpec class source.");
    }

    auto function = result.As<Napi::Function>();

    function.DefineProperty(Napi::PropertyDescriptor::Function(
        "validate",
        [](const Napi::CallbackInfo& info) -> Napi::Value {
          auto env = info.Env();
          const auto list = JSStyleTransformSpec::ToStyleTransformSpecList(info[0]);

          return Napi::Boolean::New(env, !list.empty());
        },
        napi_default,
        nullptr));

    jsStyleTransformSpecClass.Reset(function, 1);
    jsStyleTransformSpecClass.SuppressDestruct();
  }

  return jsStyleTransformSpecClass.Value();
}

Napi::Object JSStyleTransformSpec::New(const Napi::Env& env, const StyleTransformSpec& value) {
  auto styleTransformSpecClass{ GetClass(env) };

  switch (value.transform) {
    case StyleTransformIdentity:
      return styleTransformSpecClass.New({ Napi::Number::New(env, value.transform) });
    case StyleTransformRotate:
      return styleTransformSpecClass.New({ Napi::Number::New(env, value.transform),
                                           JSStyleValue::New(env, value.angle) });
    case StyleTransformScale:
    case StyleTransformTranslate:
      return styleTransformSpecClass.New({ Napi::Number::New(env, value.transform),
                                           JSStyleValue::New(env, value.x), JSStyleValue::New(env, value.y) });
    default:
      assert(false);
      return Napi::Object();
  }
}

Napi::Array JSStyleTransformSpec::New(const Napi::Env& env, const std::vector<StyleTransformSpec>& value) {
  auto list{ Napi::Array::New(env, value.size()) };
  uint32_t i = 0;

  for (const auto& spec : value) {
    auto specObject = JSStyleTransformSpec::New(env, spec);

    if (specObject.IsEmpty()) {
      return Napi::Array::New(env);
    }

    list[i] = specObject;
  }

  return list;
}

Napi::Value JSStyleTransformSpec::Undefined(const Napi::Env& env) {
  if (jsStyleTransformSpecUndefined.IsEmpty()) {
    jsStyleTransformSpecUndefined.Reset(
        Napi::RunScript(env, "/*return*/ Object.freeze([])").As<Napi::Object>(), 1);
    jsStyleTransformSpecUndefined.SuppressDestruct();
  }

  return jsStyleTransformSpecUndefined.Value();
}

bool IsValidAngle(const StyleValue& styleValue) {
  switch (styleValue.unit) {
    case StyleNumberUnitPoint:
    case StyleNumberUnitRadian:
    case StyleNumberUnitGradian:
    case StyleNumberUnitDegree:
    case StyleNumberUnitTurn:
      return true;
    default:
      return false;
  }
}

bool IsValidTranslate(const StyleValue& styleValue) {
  switch (styleValue.unit) {
    case StyleNumberUnitPoint:
    case StyleNumberUnitPercent:
    case StyleNumberUnitViewportWidth:
    case StyleNumberUnitViewportHeight:
    case StyleNumberUnitViewportMin:
    case StyleNumberUnitViewportMax:
    case StyleNumberUnitRootEm:
      return true;
    default:
      return false;
  }
}

bool IsValidScale(const StyleValue& styleValue) {
  switch (styleValue.unit) {
    case StyleNumberUnitPoint:
      return true;
    default:
      return false;
  }
}

std17::optional<StyleTransformSpec> ToStyleTransformSpec(const Napi::Value& value) {
  if (!value.IsObject()) {
    return {};
  }

  auto obj = value.As<Napi::Object>();

  if (!obj.InstanceOf(JSStyleTransformSpec::GetClass(value.Env()))) {
    return {};
  }

  auto transformRaw = Napi::ObjectGetNumberOrDefault(obj, "transform", -1);

  if (!IsValidEnum<StyleTransform>(transformRaw)) {
    return {};
  }

  switch (static_cast<StyleTransform>(transformRaw)) {
    case StyleTransformIdentity:
      return { StyleTransformSpec::OfIdentity() };
    case StyleTransformTranslate: {
      auto x = JSStyleValue::ToStyleValue(obj["x"]);
      auto y = JSStyleValue::ToStyleValue(obj["y"]);

      if (IsValidTranslate(x) && IsValidTranslate(y)) {
        return StyleTransformSpec::OfTranslate(x, y);
      } else {
        return {};
      }
    }
    case StyleTransformRotate: {
      auto angle = JSStyleValue::ToStyleValue(obj["angle"]);

      if (IsValidAngle(angle)) {
        return StyleTransformSpec::OfRotate(angle);
      } else {
        return {};
      }
    }
    case StyleTransformScale: {
      auto x = JSStyleValue::ToStyleValue(obj["x"]);
      auto y = JSStyleValue::ToStyleValue(obj["y"]);

      if (IsValidScale(x) && IsValidScale(y)) {
        return StyleTransformSpec::OfScale(x, y);
      } else {
        return {};
      }
    }
    default:
      assert(false);
      return {};
  }
}

std::vector<StyleTransformSpec> JSStyleTransformSpec::ToStyleTransformSpecList(const Napi::Value& value) {
  if (value.IsArray()) {
    auto array = value.As<Napi::Array>();
    auto len = array.Length();

    if (len == 0) {
      return {};
    }

    std::vector<StyleTransformSpec> result;

    result.reserve(len);

    for (uint32_t i = 0; i < len; i++) {
      auto spec = ToStyleTransformSpec(array[i]);

      if (spec.has_value()) {
        result.emplace_back(*spec);
      } else {
        return {};
      }
    }

    return result;
  }

  auto spec = ToStyleTransformSpec(value);

  if (spec.has_value()) {
    return { spec.value() };
  }

  return {};
}

} // namespace bindings
} // namespace lse
