#!/bin/sh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
LIB_DIR="${SCRIPT_DIR}/../lib"
NODE_MODULES_DIR="${LIB_DIR}/node_modules"

export NODE_PATH="${NODE_MODULES_DIR}:${NODE_PATH}"
export LS_RUNTIME_FRAMEWORK_PATH="${LS_RUNTIME_FRAMEWORK_PATH:-"${LIB_DIR}/Frameworks"}"
export LD_LIBRARY_PATH="${LIB_DIR}/so:${LD_LIBRARY_PATH}"

$SCRIPT_DIR/__node --loader "${NODE_MODULES_DIR}/light-source-loader/builtin.mjs" "$@"
