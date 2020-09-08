#!/bin/sh

SCRIPT_DIR=$(dirname "$0")
if [ $SCRIPT_DIR = "." ]; then
  SCRIPT_DIR=`pwd`
fi

export NODE_PATH ="${LS_NODE_PATH:-"${SCRIPT_DIR}/../lib/node_modules"}:${NODE_PATH}"
export LD_LIBRARY_PATH ="${LS_LD_LIBRARY_PATH:-"${SCRIPT_DIR}/../lib/so"}:${LD_LIBRARY_PATH}"

$SCRIPT_DIR/__node "$@"
