#!/bin/bash

set -e
set -x

export APPIMAGE_HOME="${GITHUB_WORKSPACE:-${HOME}}/.appimage"
export APPIMAGE_URL=https://github.com/AppImage/AppImageKit/releases/download/continuous

download_appimagetool() {
  local RUNTIME_NAME="appimagetool-${1}.AppImage"
  local RUNTIME_FILE="${APPIMAGE_HOME}/${RUNTIME_NAME}"

  if [ ! -e "${RUNTIME_FILE}" ] ; then
    mkdir -p ${APPIMAGE_HOME}
    wget "${APPIMAGE_URL}/${RUNTIME_NAME}" -O "${RUNTIME_FILE}"
    chmod a+x "${RUNTIME_FILE}"
  fi
}

download_appimagetool x86_64

case "$1" in
  aarch64 | arm64)
    V_ARCH=aarch64
    download_appimagetool ${V_ARCH}
    ;;
  armhf | armv6l | armv7l)
    V_ARCH=armhf
    download_appimagetool ${V_ARCH}
    ;;
  *)
    V_ARCH=x86_64
    ;;
esac

ARCH=${V_ARCH} "${APPIMAGE_HOME}/appimagetool-x86_64.AppImage" \
  --runtime-file "${APPIMAGE_HOME}/appimagetool-${V_ARCH}.AppImage" \
  "$2" \
  "$3"
