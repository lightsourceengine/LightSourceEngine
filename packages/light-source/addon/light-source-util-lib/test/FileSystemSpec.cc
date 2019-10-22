/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <ls/FileSystem.h>
#include <compat/filesystem>

using Napi::Assert;
using Napi::TestSuite;
using ls::internal::HasExtension;
namespace fs = std::filesystem;

namespace ls {

// Create a lexically_normal path so the tests run on windows and linux.
static fs::path P(const std::string& filename) {
    fs::path path{ filename };

    path.lexically_normal();

    return path;
}

void FileSystemSpec(Napi::Env env, TestSuite* parent) {
    const auto spec{ parent->Describe("FileSystem") };

    spec->Describe("CFile")->tests = {
        {
            "should get file size",
            [](const Napi::CallbackInfo& info) {
                auto file{ CFile::Open("test/resources/300x300.svg") };

                Assert::Equal(file.GetSize(), 166UL);
            }
        },
        {
            "should read bytes from file",
            [](const Napi::CallbackInfo& info) {
                uint8_t buffer[1];
                auto file{ CFile::Open("test/resources/300x300.svg") };

                auto result{ file.Read(buffer, 1) };
                auto firstByte{ buffer[0] };

                Assert::Equal(result, 1UL);
                file.Reset();
                result = file.Read(buffer, 1);
                Assert::Equal(result, 1UL);
                Assert::Equal(buffer[0], firstByte);
            }
        }
    };

    spec->Describe("HasExtension()")->tests = {
        {
            "should return true when file has an extension",
            [](const Napi::CallbackInfo& info) {
                Assert::IsTrue(HasExtension(P("file.txt")));
                Assert::IsTrue(HasExtension(P("path/file.txt")));
                Assert::IsTrue(HasExtension(P("path/file.txt")));
                Assert::IsTrue(HasExtension(P("path/file.tar.gz")));
            }
        },
        {
            "should return false when file has no extension",
            [](const Napi::CallbackInfo& info) {
                Assert::IsFalse(HasExtension(P("file")));
                Assert::IsFalse(HasExtension(P("path/file")));
                Assert::IsFalse(HasExtension(P("path.d/file")));
                Assert::IsFalse(HasExtension(""));
            }
        }
    };

    spec->Describe("IsDataUri()")->tests = {
        {
            "should return true for a valid data uri",
            [](const Napi::CallbackInfo& info) {
                Assert::IsTrue(IsDataUri("data:text/html,<script>alert('hi');</script>"));
            }
        },
        {
            "should return false for an invalid data uri",
            [](const Napi::CallbackInfo& info) {
                Assert::IsFalse(IsDataUri("file.txt"));
                Assert::IsFalse(IsDataUri(""));
            }
        }
    };

    spec->Describe("IsResourceUri()")->tests = {
        {
            "should return true for a valid resource uri",
            [](const Napi::CallbackInfo& info) {
                Assert::IsTrue(IsResourceUri("file://resource/file.txt"));
            }
        },
        {
            "should return false for an invalid resource uri",
            [](const Napi::CallbackInfo& info) {
                Assert::IsFalse(IsResourceUri("file.txt"));
                Assert::IsFalse(IsResourceUri(""));
            }
        }
    };

    spec->Describe("IsSvgDataUri()")->tests = {
        {
            "should return true for a valid svg data uri",
            [](const Napi::CallbackInfo& info) {
                Assert::IsTrue(IsSvgDataUri("data:image/svg+xml,<svg/>"));
            }
        },
        {
            "should return false for invalid svg data uri",
            [](const Napi::CallbackInfo& info) {
                Assert::IsFalse(IsSvgDataUri("file.txt"));
                Assert::IsFalse(IsSvgDataUri(""));
            }
        }
    };

    spec->Describe("GetResourceUriPath()")->tests = {
        {
            "should return relative path from file uri",
            [](const Napi::CallbackInfo& info) {
                Assert::Equal(GetResourceUriPath("file://resource/file.txt"), P("file.txt"));
                Assert::Equal(GetResourceUriPath("file://resource/path/file.txt"), P("path/file.txt"));
            }
        }
    };

    spec->Describe("GetSvgUriData()")->tests = {
        {
            "should return svg xml from svg data uri",
            [](const Napi::CallbackInfo& info) {
                Assert::Equal(GetSvgUriData("data:image/svg+xml,<svg/>"), "<svg/>");
            }
        }
    };

    spec->Describe("FindFile()")->tests = {
        {
            "should find file by filename",
            [](const Napi::CallbackInfo& info) {
                Assert::Equal(FindFile("test/resources/600x400.jpg", {}), P("test/resources/600x400.jpg"));
            }
        },
        {
            "should throw exception if file not found",
            [](const Napi::CallbackInfo& info) {
                Assert::Throws([]() { FindFile("does-not-exist", {}); });
            }
        },
        {
            "should find file using extension search",
            [](const Napi::CallbackInfo& info) {
                Assert::Equal(FindFile("test/resources/600x400", { ".jpg" }), P("test/resources/600x400.jpg"));
            }
        },
        {
            "should find file using extension search and resource paths",
            [](const Napi::CallbackInfo& info) {
                Assert::Equal(FindFile("600x400", { ".jpg" }, { "test/resources" }), P("test/resources/600x400.jpg"));
            }
        },
        {
            "should throw exception if file not found in resource paths",
            [](const Napi::CallbackInfo& info) {
                Assert::Throws([](){ FindFile("does-not-exist", {}, { "test/resources" }); });
            }
        }
    };
}

} // namespace ls
