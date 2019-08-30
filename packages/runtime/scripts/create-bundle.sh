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
# create-bundle.sh [platform-arch]
#
# Examples:
#
# Compile for build machine:  create-bundle.sh
# Cross compile for armv7:     create-bundle.sh linux-armv6l
# Cross compile for armv7:     create-bundle.sh linux-armv7l

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
  echo $(node -e 'console.log(process.version)')
}

assert_node_version() {
  local VERSION

  VERSION=$(node -e 'console.log(process.version)')

  if [[ "$VERSION" != "$1" ]]; then
    bail "Installed node version @ $VERSION but build requires $1"
  fi  
}

get_light_source_version() {
  echo $(node -e "console.log(JSON.parse(require('fs').readFileSync('package.json')).devDependencies['light-source'])")
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
  local LIGHT_SOURCE_BUILD_DIR

  NODE_PLATFORM_ARCH=$1

  echo "****** Start create bundle for ${NODE_PLATFORM_ARCH}"

  assert_python2
  export npm_config_enable_native_tests=false

  echo "****** Compiling..."

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

  echo "****** Minifying javscript..."

  yarn run bundle

  echo "****** Staging bundle..."

  LIGHT_SOURCE_VERSION=$(get_light_source_version)
  STAGING_DIR="build/${NODE_PLATFORM_ARCH}"
  LIGHT_SOURCE_BUILD_DIR=${STAGING_DIR}/lib/node_modules/light-source/build
  NODE_BIN=$(get_node_bin "${NODE_PLATFORM_ARCH}")

  clear_staging_dir
  mkdir -p "${STAGING_DIR}/bin" "${STAGING_DIR}/lib"

  (cd ${STAGING_DIR}/bin && cp ${NODE_BIN} . && ln -s node lsnode)
  mv build/node_modules ${STAGING_DIR}/lib

  mkdir "${LIGHT_SOURCE_BUILD_DIR}"
  cp node_modules/light-source/build/Release/*.node "${LIGHT_SOURCE_BUILD_DIR}"
  rm "${LIGHT_SOURCE_BUILD_DIR}/light-source-ref.node"

  echo "****** Creating bundle..."

  (cd build && tar -cvzf lsnode-v${LIGHT_SOURCE_VERSION}-${NODE_PLATFORM_ARCH}.tar.gz ${NODE_PLATFORM_ARCH})

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
