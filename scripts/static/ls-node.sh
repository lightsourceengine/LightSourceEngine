#!/bin/sh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"

export NODE_PATH="${SCRIPT_DIR}/../lib/node_modules:${NODE_PATH}"
export LS_RUNTIME_FRAMEWORK_PATH="${LS_RUNTIME_FRAMEWORK_PATH:-"${SCRIPT_DIR}/../lib/Frameworks"}:${LS_RUNTIME_FRAMEWORK_PATH}"
export LD_LIBRARY_PATH="${LS_LD_LIBRARY_PATH:-"${SCRIPT_DIR}/../lib/so"}:${LD_LIBRARY_PATH}"

$SCRIPT_DIR/__node "$@"
