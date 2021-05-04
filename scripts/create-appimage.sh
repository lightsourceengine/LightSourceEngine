#!/bin/bash

set -e
set -x

export APPIMAGE_HOME="${GITHUB_WORKSPACE:-${HOME}}/.appimage"
export APPIMAGETOOL="${APPIMAGE_HOME}/appimagetool"
export APPIMAGE_URL=https://github.com/AppImage/AppImageKit/releases/download/continuous

if [ ! -e "${APPIMAGETOOL}" ] ; then
  mkdir -p ${APPIMAGE_HOME}
  wget "${APPIMAGE_URL}/appimagetool-x86_64.AppImage" -O "${APPIMAGETOOL}"
  chmod a+x "${APPIMAGETOOL}"
fi

V_RUNTIME_FILE_ARG=

case "$1" in
  arm64)
    V_RUNTIME="${APPIMAGE_HOME}/appimagetool-aarch64.AppImage"
    if [ ! -e "${APPIMAGETOOL}" ] ; then
      wget "${APPIMAGE_URL}/${V_RUNTIME}" -O "${V_RUNTIME}"
      V_RUNTIME_FILE_ARG="--runtime-file=${V_RUNTIME}"
    fi
    ;;
  armv6l | armv7l)
    V_RUNTIME="${APPIMAGE_HOME}/appimagetool-armhf.AppImage"
    if [ ! -e "${APPIMAGETOOL}" ] ; then
      wget "${APPIMAGE_URL}/${V_RUNTIME}" -O "${V_RUNTIME}"
      V_RUNTIME_FILE_ARG="--runtime-file=${V_RUNTIME}"
    fi
    ;;
esac

${APPIMAGETOOL} ${V_RUNTIME_FILE_ARG} "$2" "$3"
