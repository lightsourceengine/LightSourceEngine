#!/bin/bash

set -e
set -x

export APPIMAGE_HOME="${GITHUB_WORKSPACE:-${HOME}}/.appimage"
export APPIMAGETOOL="${APPIMAGE_HOME}/appimagetool"

if [ ! -e "${APPIMAGETOOL}" ] ; then
  mkdir -p ${APPIMAGE_HOME}
  sudo apt-get install -y wget
  wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-${1}.AppImage -O "${APPIMAGETOOL}"
  chmod a+x "${APPIMAGETOOL}"
fi

${APPIMAGETOOL} "$2" "$3"
