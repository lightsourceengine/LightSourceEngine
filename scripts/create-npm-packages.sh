#!/bin/sh
set -e

# create-npm-packages.sh
#
# Creates npm publishable tarballs for light-source, light-source-react and light-source-loader packages. The packages
# are saved to $SRCROOT/build/npm/publishable/*.tgz.
#
# Before this script is run, the version in $SRCROOT/publishing/version should be manually changed. After this script
# is run, the created packages can be published with npm/yarn publish command.
#
# This script will build the Light Source Engine project and run tests to verify a valid state. Then, npm pack is
# run on each package. The package.json files field performs filtering of files. .npmignore is not used. The resulting
# tarball contains a package.json with monorepo configuration that should not be published. Also, the package may
# require additional hand patching that cannot be described with package.json files. The tarball is extracted to
# disk and the patches are made. Finally, the files are tarballed into the final package and stored in
# $SCROOT/build/npm/publishable.

_pushd () {
  cd "$@" > /dev/null 2>&1
}

_popd () {
  cd - > /dev/null 2>&1
}

get_version() {
  echo $(node -p "JSON.parse(require('fs').readFileSync('$1', 'utf8')).version")
}

create_npm_package() {
  TARGET_DIR="${SOURCE_ROOT}/build/npm/$1"
  SOURCE_DIR="${SOURCE_ROOT}/packages/@lse/$1"
  
  mkdir -p "${TARGET_DIR}"
  VERSION=$(get_version "${SOURCE_DIR}/package.json")

  _pushd "${SOURCE_DIR}"
  npm pack > /dev/null 2>&1
  tar -xzf lse-${1}-${VERSION}.tgz -C "${TARGET_DIR}"
  rm lse-${1}-${VERSION}.tgz
  _popd

  OVERLAY="${SOURCE_DIR}/publishing/package-json.overlay"

  if [ -f "${OVERLAY}" ]; then
    node "${SOURCE_ROOT}/scripts/apply-package-json-overlay.js" \
      "${SOURCE_DIR}/package.json" \
      "${OVERLAY}" \
      "${TARGET_DIR}/package/package.json"
  fi

  PATCH_DIR="${SOURCE_DIR}/publishing/patch"

  if [ -d "${PATCH_DIR}" ]; then
    cp -rf "${PATCH_DIR}/." "${TARGET_DIR}/package"
  fi

  cp "${SOURCE_ROOT}/LICENSE" "${TARGET_DIR}/package"

  _pushd "${TARGET_DIR}/package"
  tar -czf "${PUBLISHABLE_DIR}/lse-${1}-${PUBLISHING_VERSION}.tgz" .
  _popd

  # TODO: cleanup
  # rm -rf "${TARGET_DIR}"
}

test_npm_install() {
  TEST_DIR="${SOURCE_ROOT}/build/npm/test"
  mkdir -p "${TEST_DIR}"

  _pushd "${TEST_DIR}"
  export npm_config_lse_install_opts="--jobs max"
  echo "{\"dependencies\": { \"@lse/core\": \"../publishable/lse-core-${PUBLISHING_VERSION}.tgz\" }}" > package.json
  rm -rf ./empty-cache
  npm install --cache ./empty-cache
  _popd

  rm -rf "${TEST_DIR}"
}

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
SOURCE_ROOT="${SCRIPT_DIR}/.."
PUBLISHABLE_DIR="${SOURCE_ROOT}/build/npm/publishable"
PUBLISHING_VERSION=$(get_version "${SOURCE_ROOT}/publishing/version.json")

_pushd "${SOURCE_ROOT}"

if [ "$1" = "--skip-yarn-install" ]; then
  yarn run bundle
else
  yarn --force
fi

_popd

rm -rf "${SOURCE_ROOT}/build/npm"
mkdir -p "${PUBLISHABLE_DIR}"

create_npm_package "core"
create_npm_package "react"
create_npm_package "loader"

test_npm_install

rm -r "${SOURCE_ROOT}/build/npm/core" "${SOURCE_ROOT}/build/npm/react" "${SOURCE_ROOT}/build/npm/loader"
