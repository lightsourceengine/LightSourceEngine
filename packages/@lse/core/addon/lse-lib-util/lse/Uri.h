/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <string>
#include <std17/string_view>

namespace lse {

enum UriScheme {
    UriSchemeFile,
    UriSchemeUnknown,
};

/**
 * Get the URI scheme (prefix of URI, such as <scheme>:body?query=value).
 *
 * @param uri The URI to inspect
 * @return UriScheme type or UriSchemeUnknown if uri is empty or invalid.
 */
UriScheme GetUriScheme(const std::string& uri) noexcept;

/**
 * Get the decoded file path section of a file URI.
 *
 * The file URI format is: file:<file-path>?query=value. File path is removed, decoded and returned. Note, file path
 * is the system file path, which can be a relative or absolute path. This is NOT a standard file URI format.
 *
 * @param uri The file URI to work on
 * @return File path; otherwise, empty string for an invalid uri/
 */
std::string GetPathFromFileUri(const std::string& uri);

/**
 * Get an encoded query param value as a string_view.
 *
 * Note: No URL decoding is performed on the value.
 *
 * @param uri The file URI to work on
 * @param name The name of the query parameter
 * @return Encoded value of the query param; otherwise, an empty string is returned
 */
std17::string_view GetQueryParamView(const std::string& uri, const char* name) noexcept;

/**
 * Get an decoded query param value as a string.
 *
 * @param uri The file URI to work on
 * @param name The name of the query parameter
 * @return Decoded value of the query param; otherwise, an empty string is returned
 */
std::string GetQueryParam(const std::string& uri, const char* name);

/**
 * Get a query param value as an integer.
 *
 * @param uri The file URI to work on
 * @param name The name of the query parameter
 * @param fallback If the query param value is not present, could not be parsed or empty, the fallback value
 * is returned.
 * @return The integer value of the named query param; otherwise, fallback is returned
 */
int32_t GetQueryParamInteger(const std::string& uri, const char* name, int32_t fallback);

} // namespace lse
