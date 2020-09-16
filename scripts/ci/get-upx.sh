#!/bin/sh

if [ ! -d "${HOME}/upx" ]; then
  wget -qO- https://github.com/upx/upx/releases/download/v3.96/upx-3.96-amd64_linux.tar.xz | tar xJ -C "${HOME}"
  mv "${HOME}/upx-3.96-amd64_linux" "${HOME}/upx"
fi
