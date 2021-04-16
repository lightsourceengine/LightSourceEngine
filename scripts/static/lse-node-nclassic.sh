#!/bin/sh

LIB_DIR="/usr/share/lse/lib"

export LSE_ENV="lse-node"
export LSE_PATH="${LIB_DIR}/node"
export LSE_FONT_PATH="${LSE_FONT_PATH:-"${LIB_DIR}/../assets"}"
export LD_LIBRARY_PATH="${LIB_DIR}/native:${LD_LIBRARY_PATH}"

/usr/share/lse/bin/node --loader "${LSE_PATH}/@lse/loader/index.mjs" "$@"
