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
    auto spec{ parent->Describe("FileSystem") };
    auto assert{ Assert(env) };

    spec->Describe("HasExtension()")->tests = {
        {
            "should return true when file has an extension",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(HasExtension("file.txt"));
                assert.IsTrue(HasExtension("path/file.txt"));
                assert.IsTrue(HasExtension("path.d/file.txt"));
                assert.IsTrue(HasExtension("path/file.tar.gz"));
            }
        },
        {
            "should return false when file has no extension",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsFalse(HasExtension("file"));
                assert.IsFalse(HasExtension("path/file"));
                assert.IsFalse(HasExtension("path.d/file"));
                assert.IsFalse(HasExtension(""));
            }
        }
    };

    spec->Describe("FileExists()")->tests = {
        {
            "should return true when the file exists",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(FileExists("test/resources/300x300.svg"));
            }
        },
        {
            "should return false when file cannot be found",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsFalse(HasExtension("file"));
            }
        }
    };
}

} // namespace ls
