#!/bin/sh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
LIB_DIR="${SCRIPT_DIR}/../lib"

# mac only
export LS_RUNTIME_FRAMEWORK_PATH="${LS_RUNTIME_FRAMEWORK_PATH:-"${LIB_DIR}/Frameworks"}"

# linux only
export LD_LIBRARY_PATH="${LIB_DIR}/so:${LD_LIBRARY_PATH}"

$SCRIPT_DIR/__node --loader "${LIB_DIR}/builtin/loader.mjs" "$@"
