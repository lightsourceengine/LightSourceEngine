#!/bin/sh
set -e

# test-npm-packages-install.sh
#
# Tests Light Source Engine npm packages created by create-npm-packages.sh.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
SOURCE_ROOT="${SCRIPT_DIR}/.."
TEST_DIR="${SOURCE_ROOT}/build/npm/test"

mkdir -p "${TEST_DIR}"
cd "${TEST_DIR}"

PUBLISHING_VERSION=$(node -p "JSON.parse(require('fs').readFileSync('../../../publishing/version.json', 'utf8')).version")
PUBLISHABLE_DIR="../publishable"

rm -rf test*.mjs package*.json node_modules

echo "{}" > package.json
npm install \
  "${PUBLISHABLE_DIR}/lse-core-${PUBLISHING_VERSION}.tgz" \
  "${PUBLISHABLE_DIR}/lse-react-${PUBLISHING_VERSION}.tgz" \
  "${PUBLISHABLE_DIR}/lse-loader-${PUBLISHING_VERSION}.tgz"

echo 'import { version } from "@lse/core"; console.log(version);' > test-lse-core-version.mjs
# XXX: react is not being shutting down properly, force exit
echo 'import { version } from "@lse/react"; console.log(version); process.exit(0);' > test-lse-react-version.mjs

CORE_VERSION=$(node test-lse-core-version.mjs)
if [ ${CORE_VERSION} != ${PUBLISHING_VERSION} ]; then
  echo "@lse/core version '$CORE_VERSION' does not match publishing version '$PUBLISHING_VERSION"
  exit 1
fi

REACT_VERSION=$(node test-lse-react-version.mjs)
if [ ${REACT_VERSION} != ${PUBLISHING_VERSION} ]; then
  echo "@lse/react version '$REACT_VERSION' does not match publishing version '$PUBLISHING_VERSION"
  exit 1
fi
