#!/bin/bash

set -e

case "$1" in
  aarch64 | arm64)
    APPIMAGE_ARCH=aarch64
    ;;
  armhf | armv6l | armv7l)
    APPIMAGE_ARCH=armhf
    ;;
  *)
    APPIMAGE_ARCH=x86_64
    ;;
esac

APPIMAGE_HOME="${GITHUB_WORKSPACE:-${HOME}}/.appimage"
APPIMAGE_URL=https://github.com/AppImage/AppImageKit/releases/download/continuous
APPIMAGE_TOOL="appimagetool-x86_64.AppImage"
APPIMAGE_RUNTIME="runtime-${APPIMAGE_ARCH}"

if [ ! -d "${APPIMAGE_HOME}" ]; then
  mkdir -p "${APPIMAGE_HOME}"
fi

download_tool() {
  local TOOL_NAME="$1"
  local TOOL_FILE="${APPIMAGE_HOME}/${TOOL_NAME}"

  if [ ! -e "${TOOL_FILE}" ] ; then
    wget "${APPIMAGE_URL}/${TOOL_NAME}" -O "${TOOL_FILE}"
    chmod a+x "${TOOL_FILE}"
  fi
}

download_tool ${APPIMAGE_TOOL}
download_tool ${APPIMAGE_RUNTIME}

ARCH=${APPIMAGE_ARCH} "${APPIMAGE_HOME}/${APPIMAGE_TOOL}" \
  --runtime-file "${APPIMAGE_HOME}/${APPIMAGE_RUNTIME}" \
  "$2" \
  "$3"
