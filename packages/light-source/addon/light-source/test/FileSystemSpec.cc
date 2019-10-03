/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "LightSourceSpec.h"
#include <FileSystem.h>

using Napi::Assert;
using Napi::TestSuite;

namespace ls {

void FileSystemSpec(Napi::Env env, TestSuite* parent) {
    const auto spec{ parent->Describe("FileSystem") };
    const auto assert{ Assert(env) };

    spec->Describe("HasExtension()")->tests = {
        {
            "should return true when file has an extension",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(HasExtension("file.txt"));
                assert.IsTrue(HasExtension(fs::path("path") / "file.txt"));
                assert.IsTrue(HasExtension(fs::path("path") / "file.txt"));
                assert.IsTrue(HasExtension(fs::path("path") / "file.tar.gz"));
            }
        },
        {
            "should return false when file has no extension",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsFalse(HasExtension("file"));
                assert.IsFalse(HasExtension(fs::path("path") / "file"));
                assert.IsFalse(HasExtension(fs::path("path.d") / "file"));
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
                assert.Equal(GetResourceUriPath("file://resource/file.txt"), fs::path("file.txt"));
                assert.Equal(GetResourceUriPath("file://resource/path/file.txt"), fs::path("path/file.txt"));
            }
        }
    };

    spec->Describe("GetSvgUriData()")->tests = {
        {
            "should return svg xml from svg data uri",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(GetSvgUriData("data:image/svg+xml,<svg/>"), std::string("<svg/>"));
            }
        }
    };

    spec->Describe("FindFile()")->tests = {
        {
            "should find file by filename",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(FindFile(fs::path("test") / "resources" / "600x400.jpg", {}),
                    fs::path("test") / "resources" / "600x400.jpg");
            }
        },
        {
            "should throw exception if file not found",
            [assert](const Napi::CallbackInfo& info) {
                assert.Throws([]() { FindFile(fs::path("does-not-exist"), {}); });
            }
        },
        {
            "should find file using extension search",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(FindFile(fs::path("test") / "resources" / "600x400", { ".jpg" }),
                    fs::path("test") / "resources" / "600x400.jpg");
            }
        },
        {
            "should find file using extension search and resource paths",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(FindFile(fs::path("600x400"), { ".jpg" }, { fs::path("test") / "resources" }),
                    fs::path("test") / "resources" / "600x400.jpg");
            }
        },
        {
            "should throw exception if file not found in resource paths",
            [assert](const Napi::CallbackInfo& info) {
                assert.Throws([](){ FindFile(fs::path("does-not-exist"), {}, { fs::path("test") / "resources" }); });
            }
        }
    };
}

} // namespace ls
