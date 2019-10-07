#!/usr/bin/env bash
#
# Create a Light Source Engine runtime package.
#
# Usage:
#
# package-ls-node.sh                      Build a runtime using the current node environment os + arch.
# package-ls-node.sh <platform-arch-tag>  Build a runtime for a specific platform. The current node environment or a
#                                         tag supported by crosstools is supported.
#
# <platform-arch-tag> is a string generated in node by: process.platform + '-' + process.arch. The cross compilation
# tags are:
#
#   linux-armv7l - Raspberry Pi 2/3/4, PlayStation Classic and more
#   nesc-armv7l  - S/NES Classic
#   linux-armv6l - Raspberry Pi Zero (Note: after version 10, NodeJS no longer publishes armv6l builds!)
#
# Environment Variables:
#
# CROSSTOOLS_HOME     - Path to crosstools directory. Defaults to ${HOME}/crosstools
#
# Shell Dependencies:
#
# - node
# - patchelf
# - python 2.7 (for node-gyp)
# - crosstools (https://github.com/dananderson/crosstools)
# - C++ 11 toolchain (gcc or clang)
# - SDL 2.0.4 development libraries
# - SDL Mixer 2.0.0 development libraries
#
# The environment that invokes this script will be the node version of the built package. This restriction is
# to avoid potential node ABI conflicts when building light-source native modules.
#
# light-source depends on seeral npm_config environment variables that describe SDL paths and other options. For
# cross compile builds, these environment variables are managed by THIS script. For local builds, these environment
# variables must be exported by the caller of this script.
#
# Pacakge Structure:
#
# The Light Source Engine is just a pre-compiled NodeJS distribution with light-source installed as a global
# module. npm and extraneous files have been removed.
#
# root
#   lib/
#     node_modules/
#       light-source/
#         package.json
#         index.js
#         build
#           <light-source .node files>
#       light-source-react
#         index.js
#       react
#         index.js
#   bin/
#     node
#     ls-node -> node
#
# lib/node_modules/light-source/package.json - This file exists to coerce bindings into loading node module files from
#                                              the light-source directory.
# lib/node_modules/lib - This directory is in the runpath for node and node module files on Linux. Any platform specific
#                        shared objects can be placed here.
# lib/node_modules/light-source-react/index.js - Contains react-reconciler and scheduler.
# lib/node_modules/react - Standalone version of react.
#

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd "${SCRIPT_DIR}/.."

bail() {
  echo $1
  exit 1
}

get_node_platform_arch() {
  node -p '`${process.platform}-${process.arch}`'
}

assert_node_platform_arch() {
  if [[ $1 -ne $(get_node_platform_arch) ]]; then
    bail "Cannot cross-compile light-source for $1."
  fi
}

get_node_version() {
  node --version
}

get_light_source_version() {
  node -p "JSON.parse(require('fs').readFileSync('packages/light-source/package.json')).version"
}

install_bin_node() {
  local NODE_PLATFORM_ARCH
  local BIN
  local LOCAL_BIN
  local NODE_VERSION
  local URL

  NODE_PLATFORM_ARCH=$1
  NODE_VERSION=$(get_node_version)
  NODE_RELEASE_ID=node-${NODE_VERSION}-${NODE_PLATFORM_ARCH}
  BIN="${NODE_DOWNLOADS}/${NODE_RELEASE_ID}/bin/node"

  if [[ ! -f "${BIN}" ]]; then
    if [[ $(get_node_platform_arch) = ${NODE_PLATFORM_ARCH} ]]; then
      LOCAL_BIN=$(which node)

      if [[ -f "$LOCAL_BIN" ]]; then
        mkdir -p "$(dirname ${BIN})"
        cp ${LOCAL_BIN} ${BIN}
      fi
    else
        URL=https://nodejs.org/download/release/${NODE_VERSION}/${NODE_RELEASE_ID}.tar.gz
        echo "Downloading node from ${URL}"
        wget -qO- "${URL}" | tar -C "${NODE_DOWNLOADS}" -xvz ${NODE_RELEASE_ID}/bin/node > /dev/null
    fi
  fi

  mkdir -p "${STAGING_DIR}/bin"

  cp ${BIN} "${STAGING_DIR}/bin"
  ln -s node "${STAGING_DIR}/bin/ls-node"

  if [[ "${NODE_PLATFORM_ARCH}" = linux-* ]]; then
    assert_patchelf
    patchelf --set-rpath '$ORIGIN/../lib' "${STAGING_DIR}/bin/node" > /dev/null
  fi
}

install_lib_node_modules() {
  local GLOBAL_REACT_MODULE
  local GLOBAL_LIGHT_SOURCE_MODULE
  local GLOBAL_REACT_LIGHT_SOURCE_MODULE

  GLOBAL_REACT_MODULE=${STAGING_DIR}/lib/node_modules/react
  GLOBAL_LIGHT_SOURCE_MODULE=${STAGING_DIR}/lib/node_modules/light-source
  GLOBAL_REACT_LIGHT_SOURCE_MODULE=${STAGING_DIR}/lib/node_modules/light-source-react

  mkdir -p "${STAGING_DIR}/lib" "${GLOBAL_REACT_MODULE}" "${GLOBAL_REACT_LIGHT_SOURCE_MODULE}" "${GLOBAL_LIGHT_SOURCE_MODULE}"

  cp node_modules/light-source/build/standalone/cjs/light-source.min.js "${GLOBAL_LIGHT_SOURCE_MODULE}/index.js"
  echo '{ "description": "the precense of an empty package.json coerces bindings to load .node files from this directory" }' > "${GLOBAL_LIGHT_SOURCE_MODULE}/package.json"

  cp node_modules/light-source-react/build/standalone/cjs/react.min.js "${GLOBAL_REACT_MODULE}/index.js"
  cp node_modules/light-source-react/build/standalone/cjs/light-source-react.min.js "${GLOBAL_REACT_LIGHT_SOURCE_MODULE}/index.js"

  mkdir "${GLOBAL_LIGHT_SOURCE_MODULE}/build"
  cp node_modules/light-source/build/Release/*.node "${GLOBAL_LIGHT_SOURCE_MODULE}/build"

  # Do not ship the test only reference renderer.
  rm "${GLOBAL_LIGHT_SOURCE_MODULE}/build/light-source-ref.node"
}

install_platform_specific_patches() {
  local NODE_PLATFORM_ARCH
  local PROFILE

  NODE_PLATFORM_ARCH=$1
  PROFILE=$2

  # Some platforms are compiled with gcc 4/5 (S/NES Classic, older Raspian images, etc), which does not support
  # C++ 11. Patch in libstdc++ to allow these older platforms to run.
  if [[ "${NODE_PLATFORM_ARCH}" = linux-arm* ]]; then
    cp -a "${CROSSTOOLS_SYSROOT}/lib/libstdc++.so.6" "${STAGING_DIR}/lib"
    cp "${CROSSTOOLS_SYSROOT}/lib/libstdc++.so.6.0.22" "${STAGING_DIR}/lib"
  fi

  # On S/NES Classic, SDL2 is installed as libSDL2.so, but light-source-sdl links libSDL2-2.0.so.0. Add a symlink
  # to resolve SDL2 properly.
  if [[ "${PROFILE}" = nesc-* ]]; then
    ln -s /usr/lib/libSDL2.so "${STAGING_DIR}/lib/libSDL2-2.0.so.0"
  fi
}

assert_python2() {
  if ! [ -x "$(command -v python2.7)" ]; then
    if [ -x "$(command -v python)" ]; then
      if python -c "import platform;import sys;int(platform.python_version_tuple()[0]) == 2 or sys.exit(1)"; then
        return
      fi
    fi

    bail "python 2 not available (required by node-gyp)"
  else
    export PYTHON=python2.7
  fi
}

assert_patchelf() {
  if ! [ -x "$(command -v patchelf)" ]; then
    bail "patchelf command not installed"
  fi
}

configure_crosstools() {
  local TOOLCHAIN_TYPE

  if [[ ! -f "${CROSSTOOLS_HOME}/bin/cross" ]]; then
    bail "crosstools (https://github.com/dananderson/crosstools) is not found via CROSSTOOLS_HOME environment variable."
  fi

  TOOLCHAIN_TYPE="arm-rpi-linux-gnueabihf"
  CROSSTOOLS_SYSROOT="${CROSSTOOLS_HOME}/x64-gcc-6.3.1/${TOOLCHAIN_TYPE}/${TOOLCHAIN_TYPE}/sysroot"

  export npm_config_with_sdl_mixer=true
  export npm_config_with_native_tests=false
  export npm_config_sdl_include_path="${CROSSTOOLS_SYSROOT}/usr/include/SDL2"
  export npm_config_sdl_library_path="${CROSSTOOLS_SYSROOT}/usr/lib"
  export npm_config_sdl_mixer_include_path="${CROSSTOOLS_SYSROOT}/usr/include/SDL2"
  export npm_config_sdl_mixer_library_path="${CROSSTOOLS_SYSROOT}/usr/lib"
}

clear_staging_dir() {
  if [[ -d "$STAGING_DIR" ]]; then
    rm -rf "$STAGING_DIR"
  fi
}

create_package() {
  local NODE_PLATFORM_ARCH
  local PROFILE

  NODE_PLATFORM_ARCH=$1
  PROFILE=$2

  echo "****** Creating package for ${PROFILE:-${NODE_PLATFORM_ARCH}}..."

  assert_python2
  export npm_config_enable_native_tests=false

  echo "****** Building LightSourceEngine..."

  case ${NODE_PLATFORM_ARCH} in
    darwin-x64|linux-x64)
      assert_node_platform_arch "${NODE_PLATFORM_ARCH}"

      yarn --force
    ;;
    linux-armv7l)
      configure_crosstools

      ${CROSSTOOLS_HOME}/bin/cross "rpi" yarn --force
    ;;
    linux-armv6l)
      configure_crosstools

      ${CROSSTOOLS_HOME}/bin/cross "rpizero" yarn --force
    ;;
    *)
      bail "Unknown target: '${NODE_PLATFORM_ARCH}'"
    ;;
  esac

  LIGHT_SOURCE_PACKAGE_NAME=ls-node-v$(get_light_source_version)-${NODE_PLATFORM_ARCH}
  STAGING_DIR="build/${LIGHT_SOURCE_PACKAGE_NAME}"

  echo "****** Copying build artifacts to staging directory (${STAGING_DIR})..."

  clear_staging_dir
  rm -f build/${LIGHT_SOURCE_PACKAGE_NAME}.tar.gz

  install_bin_node ${NODE_PLATFORM_ARCH}
  install_lib_node_modules
  install_platform_specific_patches ${NODE_PLATFORM_ARCH} ${PROFILE}

  echo "****** Creating package (${LIGHT_SOURCE_PACKAGE_NAME}.tar.gz)..."

  (cd build && tar -cvzf ${LIGHT_SOURCE_PACKAGE_NAME}.tar.gz ${LIGHT_SOURCE_PACKAGE_NAME})

  echo "****** Done"
}

# Delete the temporary staging directory on exit (on success, the tar.gz remains in the build directory).
trap clear_staging_dir EXIT

STAGING_DIR=
CROSSTOOLS_HOME="${CROSSTOOLS_HOME:-${HOME}/crosstools}"
CROSSTOOLS_SYSROOT=
NODE_DOWNLOADS="${NODE_DOWNLOADS:-/tmp}"
NODE_DOWNLOADS="${NODE_DOWNLOADS:-/tmp}"

case $1 in
  linux-armv7l|linux-armv6l)
    time create_package $1
  ;;
  nesc-armv7l)
    time create_package linux-armv7l $1
  ;;
  *)
    if [ -z "$1" ]; then
      time create_package $(get_node_platform_arch)
    elif [[ "$(get_node_platform_arch)" = "$1" ]]; then
      time create_package $1
    else
      bail "Unknown target: '$1'"
    fi
  ;;
esac
