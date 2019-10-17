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
    const auto assert{ Assert(env) };

    spec->Describe("CFile")->tests = {
        {
            "should get file size",
            [assert](const Napi::CallbackInfo& info) {
                auto file{ CFile::Open("test/resources/300x300.svg") };

                assert.Equal(file.GetSize(), 166UL);
            }
        },
        {
            "should read bytes from file",
            [assert](const Napi::CallbackInfo& info) {
                uint8_t buffer[1];
                auto file{ CFile::Open("test/resources/300x300.svg") };

                auto result{ file.Read(buffer, 1) };
                auto firstByte{ buffer[0] };

                assert.Equal(result, 1UL);
                file.Reset();
                result = file.Read(buffer, 1);
                assert.Equal(result, 1UL);
                assert.Equal(buffer[0], firstByte);
            }
        }
    };

    spec->Describe("HasExtension()")->tests = {
        {
            "should return true when file has an extension",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(HasExtension(P("file.txt")));
                assert.IsTrue(HasExtension(P("path/file.txt")));
                assert.IsTrue(HasExtension(P("path/file.txt")));
                assert.IsTrue(HasExtension(P("path/file.tar.gz")));
            }
        },
        {
            "should return false when file has no extension",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsFalse(HasExtension(P("file")));
                assert.IsFalse(HasExtension(P("path/file")));
                assert.IsFalse(HasExtension(P("path.d/file")));
                assert.IsFalse(HasExtension(""));
            }
        }
    };

    spec->Describe("IsDataUri()")->tests = {
        {
            "should return true for a valid data uri",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(IsDataUri("data:text/html,<script>alert('hi');</script>"));
            }
        },
        {
            "should return false for an invalid data uri",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsFalse(IsDataUri("file.txt"));
                assert.IsFalse(IsDataUri(""));
            }
        }
    };

    spec->Describe("IsResourceUri()")->tests = {
        {
            "should return true for a valid resource uri",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(IsResourceUri("file://resource/file.txt"));
            }
        },
        {
            "should return false for an invalid resource uri",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsFalse(IsResourceUri("file.txt"));
                assert.IsFalse(IsResourceUri(""));
            }
        }
    };

    spec->Describe("IsSvgDataUri()")->tests = {
        {
            "should return true for a valid svg data uri",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(IsSvgDataUri("data:image/svg+xml,<svg/>"));
            }
        },
        {
            "should return false for invalid svg data uri",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsFalse(IsSvgDataUri("file.txt"));
                assert.IsFalse(IsSvgDataUri(""));
            }
        }
    };

    spec->Describe("GetResourceUriPath()")->tests = {
        {
            "should return relative path from file uri",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(GetResourceUriPath("file://resource/file.txt"), P("file.txt"));
                assert.Equal(GetResourceUriPath("file://resource/path/file.txt"), P("path/file.txt"));
            }
        }
    };

    spec->Describe("GetSvgUriData()")->tests = {
        {
            "should return svg xml from svg data uri",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(GetSvgUriData("data:image/svg+xml,<svg/>"), "<svg/>");
            }
        }
    };

    spec->Describe("FindFile()")->tests = {
        {
            "should find file by filename",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(FindFile("test/resources/600x400.jpg", {}), P("test/resources/600x400.jpg"));
            }
        },
        {
            "should throw exception if file not found",
            [assert](const Napi::CallbackInfo& info) {
                assert.Throws([]() { FindFile("does-not-exist", {}); });
            }
        },
        {
            "should find file using extension search",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(FindFile("test/resources/600x400", { ".jpg" }), P("test/resources/600x400.jpg"));
            }
        },
        {
            "should find file using extension search and resource paths",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(FindFile("600x400", { ".jpg" }, { "test/resources" }), P("test/resources/600x400.jpg"));
            }
        },
        {
            "should throw exception if file not found in resource paths",
            [assert](const Napi::CallbackInfo& info) {
                assert.Throws([](){ FindFile("does-not-exist", {}, { "test/resources" }); });
            }
        }
    };
}

} // namespace ls
