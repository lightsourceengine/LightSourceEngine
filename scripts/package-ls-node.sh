#!/usr/bin/env bash
#
# Create a Light Source Engine runtime tar.gz package.
#
# The runtime is a standard node binary packaged with pre-compiled, standalone versions of light-source,
# react-light-source, react and bindings.
#
# Packages are stored in the build directory.
#
# The pacakager requires some setup.
#
# For packages matching the build machine's architecture, the following is required:
#
# - The environment must have node version 12.0.0.
# - The SDL npm_comfig variables must be set for the current environment.
# - gcc or clang C++ toolchain
# - python 2.7 (for node-gyp)
#
# For cross compiled packages, the following is required:
#
# - The environment must have node version 12.0.0. For armv6 builds, node version must be 10.16.3.
# - crosstools installed at CROSSTOOLS_HOME environment variable.
# - python 2.7 (for node-gyp)
#
# Usage:
#
# package-ls-node.sh [platform-arch]
#
# Examples:
#
# Compile for build machine:  package-ls-node.sh
# Cross compile for armv7:     package-ls-node.sh linux-armv6l
# Cross compile for armv7:     package-ls-node.sh linux-armv7l

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd "${SCRIPT_DIR}/.."

bail() {
  echo $1
  exit 1
}

get_node_platform_arch() {
  echo $(node -e 'console.log(`${process.platform}-${process.arch}`)')
}

assert_node_platform_arch() {
  if [[ $1 -ne $(get_node_platform_arch) ]]; then
    bail "Cannot cross-compile light-source for $1."
  fi
}

get_node_version() {
  echo $(node --version)
}

assert_node_version() {
  local VERSION

  VERSION=$(node -e 'console.log(process.version)')

  if [[ "$VERSION" != "$1" ]]; then
    bail "Installed node version @ $VERSION but build requires $1"
  fi  
}

get_light_source_version() {
  echo $(node -e "console.log(JSON.parse(require('fs').readFileSync('packages/light-source/package.json')).version)")
}

get_node_bin() {
  local NODE_PLATFORM_ARCH
  local BIN
  local NODE_VERSION
  local URL

  NODE_PLATFORM_ARCH=$1
  NODE_VERSION=$(get_node_version)
  NODE_RELEASE_ID=node-${NODE_VERSION}-${NODE_PLATFORM_ARCH}
  BIN="${NODE_DOWNLOADS}/${NODE_RELEASE_ID}/bin/node"

  if [[ $(get_node_platform_arch) == ${NODE_PLATFORM_ARCH} ]]; then
    BIN=$(which node)

    if [[ ! -f "$BIN" ]]; then
      bail "Cannot find local node binary."
    fi
  elif [[ ! -f "${BIN}" ]]; then
    URL=https://nodejs.org/download/release/${NODE_VERSION}/${NODE_RELEASE_ID}.tar.gz

    wget -qO- "${URL}" | tar -C "${NODE_DOWNLOADS}" -xvz ${NODE_RELEASE_ID}/bin/node > /dev/null
  fi

  echo "$BIN"
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

configure_crosstools() {
  local TOOLCHAIN_TYPE
  local SYSROOT

  if [[ ! -f "${CROSSTOOLS_HOME}/bin/cross" ]]; then
    bail "crosstools (https://github.com/dananderson/crosstools) is not found via CROSSTOOLS_HOME environment variable."
  fi

  TOOLCHAIN_TYPE="arm-rpi-linux-gnueabihf"
  SYSROOT="${CROSSTOOLS_HOME}/x64-gcc-6.3.1/${TOOLCHAIN_TYPE}/${TOOLCHAIN_TYPE}/sysroot"

  export npm_config_with_sdl_mixer=true
  export npm_config_sdl_include_path="${SYSROOT}/usr/include/SDL2"
  export npm_config_sdl_library_path="${SYSROOT}/usr/lib"
  export npm_config_with_cec=false
  export npm_config_cec_include_path="${SYSROOT}/usr/include"
  export npm_config_cec_library_path="${SYSROOT}/usr/lib"
}

clear_staging_dir() {
  if [[ -d "$STAGING_DIR" ]]; then
    rm -rf "$STAGING_DIR"
  fi
}

create_bundle() {
  local NODE_PLATFORM_ARCH
  local NODE_BIN
  local LIGHT_SOURCE_VERSION
  local GLOBAL_BINDINGS_MODULE
  local GLOBAL_REACT_MODULE
  local GLOBAL_LIGHT_SOURCE_MODULE
  local GLOBAL_REACT_LIGHT_SOURCE_MODULE

  NODE_PLATFORM_ARCH=$1

  echo "****** Start create bundle for ${NODE_PLATFORM_ARCH}"

  assert_python2
  export npm_config_enable_native_tests=false

  echo "****** Compiling and minifying javscript..."

  case ${NODE_PLATFORM_ARCH} in
    darwin-x64|linux-x64)
      assert_node_version ${TARGET_NODE_VERSION:-${DEFAULT_NODE_VERSION}}
      assert_node_platform_arch "${NODE_PLATFORM_ARCH}"

      yarn --force
    ;;
    linux-armv7l)
      assert_node_version ${TARGET_NODE_VERSION:-${DEFAULT_NODE_VERSION}}
      configure_crosstools

      ${CROSSTOOLS_HOME}/bin/cross "rpi" yarn --force
    ;;
    linux-armv6l)
      assert_node_version ${TARGET_NODE_VERSION:-${DEFAULT_NODE_VERSION_ARMV6}}
      configure_crosstools

      ${CROSSTOOLS_HOME}/bin/cross "rpizero" yarn --force
    ;;
    *)
      bail "Unknown target: '${NODE_PLATFORM_ARCH}'"
    ;;
  esac

  echo "****** Staging bundle..."

  LIGHT_SOURCE_VERSION=$(get_light_source_version)
  LIGHT_SOURCE_PACKAGE_NAME=ls-node-v${LIGHT_SOURCE_VERSION}-${NODE_PLATFORM_ARCH}
  STAGING_DIR="build/${LIGHT_SOURCE_PACKAGE_NAME}"
  NODE_BIN=$(get_node_bin "${NODE_PLATFORM_ARCH}")

  clear_staging_dir
  mkdir -p "${STAGING_DIR}/bin" "${STAGING_DIR}/lib"

  # Copy node binary and create ls-node symlink.
  (cd ${STAGING_DIR}/bin && cp ${NODE_BIN} . && ln -s node ls-node)

  # Copy standalone react, bindings, light-source and react-light-source modules into global module directory.
  GLOBAL_BINDINGS_MODULE=${STAGING_DIR}/lib/node_modules/bindings
  GLOBAL_REACT_MODULE=${STAGING_DIR}/lib/node_modules/react
  GLOBAL_LIGHT_SOURCE_MODULE=${STAGING_DIR}/lib/node_modules/light-source
  GLOBAL_REACT_LIGHT_SOURCE_MODULE=${STAGING_DIR}/lib/node_modules/react-light-source

  mkdir -p "$GLOBAL_REACT_MODULE" "$GLOBAL_BINDINGS_MODULE" "$GLOBAL_REACT_LIGHT_SOURCE_MODULE" "$GLOBAL_LIGHT_SOURCE_MODULE"

  cp node_modules/light-source/build/standalone/cjs/bindings.min.js "${GLOBAL_BINDINGS_MODULE}/index.js"
  cp node_modules/light-source/build/standalone/cjs/light-source.min.js "${GLOBAL_LIGHT_SOURCE_MODULE}/index.js"
  echo '{ "description": "the precense of an empty package.json coerces bindings to load .node files from this directory" }' > "${GLOBAL_LIGHT_SOURCE_MODULE}/package.json"

  cp node_modules/react-light-source/build/standalone/cjs/react.min.js "${GLOBAL_REACT_MODULE}/index.js"
  cp node_modules/react-light-source/build/standalone/cjs/react-light-source.min.js "${GLOBAL_REACT_LIGHT_SOURCE_MODULE}/index.js"

  # Copy native light-source artifacts to the global light-source module. Note, bindings search algorithm looks
  # at the "build" directory first.
  mkdir "${GLOBAL_LIGHT_SOURCE_MODULE}/build"
  cp node_modules/light-source/build/Release/*.node "${GLOBAL_LIGHT_SOURCE_MODULE}/build"
  # Remove the test only reference renderer.
  rm "${GLOBAL_LIGHT_SOURCE_MODULE}/build/light-source-ref.node"

  echo "****** Creating bundle..."

  (cd build && tar -cvzf ${LIGHT_SOURCE_PACKAGE_NAME}.tar.gz ${LIGHT_SOURCE_PACKAGE_NAME})

  echo "****** Done"
}

# Delete the temporary staging directory on exit (on success, the tar.gz remains in the build directory).
trap clear_staging_dir EXIT

STAGING_DIR=
CROSSTOOLS_HOME="${CROSSTOOLS_HOME:-${HOME}/crosstools}"
NODE_DOWNLOADS="${NODE_DOWNLOADS:-/tmp}"
DEFAULT_NODE_VERSION=v12.0.0
DEFAULT_NODE_VERSION_ARMV6=v10.16.3

case $1 in
  linux-armv7l|linux-armv6l)
    time create_bundle $1
  ;;
  *)
    if [ -z "$1" ]; then
      time create_bundle $(get_node_platform_arch)
    elif [[ "$(get_node_platform_arch)" == "$1" ]]; then
      time create_bundle $1
    else
      bail "Unknown target: '$1'"
    fi
  ;;
esac
