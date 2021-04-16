#!/bin/sh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
LIB_DIR="${SCRIPT_DIR}/../lib"

export LSE_ENV="lse-node"
export LSE_PATH="${LIB_DIR}/node"
export LSE_FONT_PATH="${LSE_FONT_PATH:-"${LIB_DIR}/../assets"}"
export LSE_GAME_CONTROLLER_DB="${LSE_GAME_CONTROLLER_DB:-"${LIB_DIR}/../assets/gamecontrollerdb.txt"}"

# mac only
export LSE_RUNTIME_FRAMEWORK_PATH="${LSE_RUNTIME_FRAMEWORK_PATH:-"${LIB_DIR}/Frameworks"}"

# linux only
export LD_LIBRARY_PATH="${LIB_DIR}/native:${LD_LIBRARY_PATH}"

$SCRIPT_DIR/node --loader "${LSE_PATH}/@lse/loader/index.mjs" "$@"
