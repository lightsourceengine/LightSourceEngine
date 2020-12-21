/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <lse/Uri.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

void UriSpec(TestSuite* parent) {
  const auto spec{ parent->Describe("Uri") };

  spec->Describe("GetUriScheme()")->tests = {
      {
          "should return UriSchemeFile for absolute path file uri",
          [](const TestInfo&) {
            Assert::Equal(GetUriScheme("file:/path/to/file.txt"), UriSchemeFile);
          }
      },
      {
          "should return UriSchemeFile for relative path file uri",
          [](const TestInfo&) {
            Assert::Equal(GetUriScheme("file:path/to/file.txt"), UriSchemeFile);
          }
      },
      {
          "should return return UriSchemeUnknown for non-file uri paths",
          [](const TestInfo&) {
            Assert::Equal(GetUriScheme("/path/to/file.txt"), UriSchemeUnknown);
            Assert::Equal(GetUriScheme("path/to/file.txt"), UriSchemeUnknown);
          }
      }
  };

  spec->Describe("GetQueryParam()")->tests = {
      {
          "should return value of parameter p",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:/path/to/file.txt?p=just%20a%20param",
                "file:/path/to/file.txt?p=just%20a%20param&",
                "file:/path/to/file.txt?p=just+a+param",
            };

            for (const auto& value : values) {
              Assert::Equal(GetQueryParam(value, "p"), "just a param");
            }
          }
      },
      {
          "should return empty string when parameter p is not present",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:/path/to/file.txt?p=",
                "file:/path/to/file.txt?p =",
                "file:/path/to/file.txt?p",
                "file:/path/to/file.txt?p=&",
                "file:/path/to/file.txt?",
                "file:/path/to/file.txt"
            };

            for (const auto& value : values) {
              Assert::Equal(GetQueryParam(value, "p"), "");
            }
          }
      }
  };

  spec->Describe("GetQueryParamView()")->tests = {
      {
          "should return value of parameter width",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:/path/to/file.txt?width=123",
                "file:/path/to/file.txt?width=123&",
                "file:/path/to/file.txt?width=123&width=999",
                "file:/path/to/file.txt?width=123&height=999"
            };

            for (const auto& value : values) {
              Assert::Equal(GetQueryParamView(value, "width"), std17::string_view("123"));
            }
          }
      },
      {
          "should return empty string when parameter width is not present",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:/path/to/file.txt?width=",
                "file:/path/to/file.txt?width =",
                "file:/path/to/file.txt?width",
                "file:/path/to/file.txt?width=&",
                "file:/path/to/file.txt?",
                "file:/path/to/file.txt"
            };

            for (const auto& value : values) {
              Assert::Equal(GetQueryParamView(value, "width"), std17::string_view(""));
            }
          }
      }
  };

  spec->Describe("GetQueryParamInteger()")->tests = {
      {
          "should return value of parameter width",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:/path/to/file.txt?width=123",
                "file:/path/to/file.txt?width=123&",
                "file:/path/to/file.txt?width=123&width=999",
                "file:/path/to/file.txt?width=123&height=999"
            };

            for (const auto& value : values) {
              Assert::Equal(GetQueryParamInteger(value, "width", 0), 123);
            }
          }
      },
      {
          "should return default value when parameter width is not present",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:/path/to/file.txt?width=",
                "file:/path/to/file.txt?width =",
                "file:/path/to/file.txt?width",
                "file:/path/to/file.txt?width=&",
                "file:/path/to/file.txt?",
                "file:/path/to/file.txt"
            };

            for (const auto& value : values) {
              Assert::Equal(GetQueryParamInteger(value, "width", 111), 111);
            }
          }
      }
  };

  spec->Describe("GetPathFromFileUri()")->tests = {
      {
          "should return file path for valid absolute file uri values",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:/path/to/file.txt",
                "file:/path/to/file.txt?param=value",
                "file:/path/to/file.txt?"
            };

            for (const auto& value : values) {
              Assert::Equal(GetPathFromFileUri(value), "/path/to/file.txt");
            }
          }
      },
      {
          "should return file path for valid relative file uri values",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:path/to/file.txt",
                "file:path/to/file.txt?param=value",
                "file:path/to/file.txt?"
            };

            for (const auto& value : values) {
              Assert::Equal(GetPathFromFileUri(value), "path/to/file.txt");
            }
          }
      },
      {
          "should return empty string for invalid file uri values",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:",
                "file:?",
                "file:?param=value"
            };

            for (const auto& value : values) {
              Assert::Equal(GetPathFromFileUri(value), "");
            }
          }
      },
      {
          "should return url decoded file path",
          [](const TestInfo&) {
            std::vector<std::string> values = {
                "file:/this+is+a+file.txt",
                "file:/this%20is%20a%20file.txt",
                "file:%2Fthis%20is%20a%20file.txt",
                "file:%2fthis%20is%20a%20file.txt",
            };

            for (const auto& value : values) {
              Assert::Equal(GetPathFromFileUri(value), "/this is a file.txt");
            }
          }
      }
  };
}

} // namespace lse
