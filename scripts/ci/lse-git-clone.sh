#!/bin/sh

set -e
set -x

for repo in "$@"
do
  if [ ! -d "${LSE_CACHE}/${repo}" ]; then
    git clone --depth 1 https://github.com/lightsourceengine/${repo}.git "${LSE_CACHE}/${repo}"
  fi
done
