#!/bin/sh

set -e
set -x

if [ ! -d "${LS_CACHE}/upx" ]; then
  wget -qO- https://github.com/upx/upx/releases/download/v3.96/upx-3.96-amd64_linux.tar.xz | tar xJ -C "${HOME}"
  mv "${HOME}/upx-3.96-amd64_linux" "${LS_CACHE}/upx"
fi
