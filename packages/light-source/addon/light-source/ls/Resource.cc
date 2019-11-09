/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Resource.h"

namespace ls {

const char* ResourceStateToString(ResourceState state) {
    switch (state) {
        case ResourceStateInit:
            return "init";
        case ResourceStateReady:
            return "ready";
        case ResourceStateError:
            return "error";
        case ResourceStateLoading:
            return "loading";
    }
    return "unknown";
}

} // namespace ls
