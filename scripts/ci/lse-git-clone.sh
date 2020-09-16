#!/bin/sh

for repo in "$@"
do
  if [ ! -d "${HOME}/${repo}" ]; then
    git clone --depth 1 https://github.com/lightsourceengine/${repo}.git "${HOME}/${repo}"
  fi
done
