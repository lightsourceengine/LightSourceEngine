#!/bin/sh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
LIB_DIR="${SCRIPT_DIR}/../lib"

export LSE_PATH="${LIB_DIR}/builtin"
export LSE_ENV="lse-node"

# mac only
export LSE_RUNTIME_FRAMEWORK_PATH="${LSE_RUNTIME_FRAMEWORK_PATH:-"${LIB_DIR}/Frameworks"}"

# linux only
export LD_LIBRARY_PATH="${LIB_DIR}/native:${LD_LIBRARY_PATH}"

$SCRIPT_DIR/node --loader "${LSE_PATH}/@lse/loader/index.mjs" "$@"
