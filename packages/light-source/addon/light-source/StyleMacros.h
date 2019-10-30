/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#define DefineStyleNumberProperty(NAME) \
    private: std::unique_ptr<StyleNumberValue> m_##NAME{}; \
    public: \
    Napi::Value get_##NAME(const Napi::CallbackInfo& info) { \
        return this->JSGetter(this->m_##NAME, info.Env()); \
    } \
    void set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) { \
        JSSetter<StyleNumber>(this->m_##NAME, value, false); \
    } \
    const StyleNumberValue* NAME() const { return this->m_##NAME.get(); }

#define DefineStyleColorProperty(NAME) \
    private: std::unique_ptr<StyleColorValue> m_##NAME{}; \
    public: \
    Napi::Value get_##NAME(const Napi::CallbackInfo& info) { \
        return this->JSGetter(this->m_##NAME, info.Env()); \
    } \
    void set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) { \
        JSSetter<uint32_t>(this->m_##NAME, value, false); \
    } \
    const StyleColorValue* NAME() const { return this->m_##NAME.get(); }

#define DefineStyleEnumProperty(NAME, ENUM) \
    private: std::unique_ptr<StyleEnumValue<ENUM>> m_##NAME{}; \
    public: \
    Napi::Value get_##NAME(const Napi::CallbackInfo& info) { \
        return this->JSGetter(this->m_##NAME, info.Env()); \
    } \
    void set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) { \
        JSSetter<ENUM>(this->m_##NAME, value, false); \
    } \
    ENUM NAME() const { return this->m_##NAME ? this->m_##NAME->Get() : static_cast<ENUM>(0); }

#define DefineStyleEnumPropertyWithDefault(NAME, ENUM, DEFAULT) \
    private: std::unique_ptr<StyleEnumValue<ENUM>> m_##NAME{}; \
    public: \
    Napi::Value get_##NAME(const Napi::CallbackInfo& info) { \
        return this->JSGetter(this->m_##NAME, info.Env()); \
    } \
    void set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) { \
        JSSetter<ENUM>(this->m_##NAME, value, false); \
    } \
    ENUM NAME() const { return this->m_##NAME ? this->m_##NAME->Get() : DEFAULT; }

#define DefineStyleStringProperty(NAME) \
    private: std::string m_##NAME{}; \
    public: \
    Napi::Value get_##NAME(const Napi::CallbackInfo& info) { \
        return m_##NAME.empty() ? info.Env().Undefined() : Napi::String::New(info.Env(), m_##NAME); \
    } \
    void set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) { \
        if (value.IsNull() || value.IsUndefined()) { \
            m_##NAME.clear(); \
        } else { \
            m_##NAME = value.ToString(); \
        } \
    } \
    const std::string& NAME() const { return this->m_##NAME; }

#define DefineYogaStyleEnumProperty(NAME, ENUM, SETTER) \
    private: std::unique_ptr<YogaStyleEnumValue<ENUM, SETTER>> m_##NAME{}; \
    public: \
    Napi::Value get_##NAME(const Napi::CallbackInfo& info) { \
        return this->JSGetter(this->m_##NAME, info.Env()); \
    } \
    void set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) { \
        JSSetter<ENUM>(this->m_##NAME, value, true); \
    } \
    ENUM NAME() const { return this->m_##NAME ? this->m_##NAME->Get() : static_cast<ENUM>(0); }

#define DefineYogaStyleNumberProperty(NAME, ...) \
    private: std::unique_ptr<YogaStyleNumberValue<__VA_ARGS__>> m_##NAME{}; \
    public: \
    Napi::Value get_##NAME(const Napi::CallbackInfo& info) { \
        return this->JSGetter(this->m_##NAME, info.Env()); \
    } \
    void set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) { \
        JSSetter<StyleNumber>(this->m_##NAME, value, true); \
    } \
    const StyleNumberValue* NAME() const { return this->m_##NAME.get(); }

#define DefineYogaEdgeStyleNumberProperty(NAME, EDGE, ...) \
    private: std::unique_ptr<YogaEdgeStyleNumberValue<EDGE, __VA_ARGS__>> m_##NAME{}; \
    public: \
    Napi::Value get_##NAME(const Napi::CallbackInfo& info) { \
        return this->JSGetter(this->m_##NAME, info.Env()); \
    } \
    void set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) { \
        JSSetter<StyleNumber>(this->m_##NAME, value, true); \
    } \
    const StyleNumberValue* NAME() const { return this->m_##NAME.get(); }

#define StylePropertyInstanceAccessor(NAME) InstanceAccessor(#NAME, &Style::get_##NAME, &Style::set_##NAME)
