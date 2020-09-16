#!/bin/sh

set -e
set -x

for repo in "$@"
do
  if [ ! -d "${HOME}/${repo}" ]; then
    git clone --depth 1 https://github.com/lightsourceengine/${repo}.git "${HOME}/${repo}"

    if [ ! -d "${HOME}/${repo}" ]; then
      echo "Failed to clone ${repo}."
      exit 1
    fi
  fi
done
