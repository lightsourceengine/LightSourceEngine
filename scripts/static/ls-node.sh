#!/bin/sh

SCRIPT_DIR=$(dirname "$0")
if [ $SCRIPT_DIR = "." ]; then
  SCRIPT_DIR=`pwd`
fi

export NODE_PATH="${NODE_PATH:-"${SCRIPT_DIR}/../lib/node_modules"}"
export LS_ADDON_PATH="${LS_ADDON_PATH:-"${SCRIPT_DIR}/../lib/node_modules/light-source/Release"}"

$SCRIPT_DIR/__node "$@"
