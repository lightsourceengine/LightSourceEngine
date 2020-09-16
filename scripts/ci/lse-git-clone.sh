#!/bin/sh

set -e
set -x

for repo in "$@"
do
  git clone --depth 1 https://github.com/lightsourceengine/${repo}.git "${HOME}/${repo}"
done
