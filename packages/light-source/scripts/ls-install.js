#!/usr/bin/env node

// Script that invokes "node-gyp rebuild $LS_INSTALL_OPTS" as the package install script.
//
// The install command should be specified in package.json scripts with cross-env. In the lightsourceengine workspace,
// cross-env is available and works fine. However, when the light-source package is included as a dependent
// package from the repository, cross-env is not visible. It appears that the .bin directory is not in the path during
// the install phase (at least with yarn).
//
// A solution is to require the user to install cross-env globally, but I opted for this local script to minimize
// user environment configuration of light-source.

const { spawnSync } = require('child_process')
const opts = (process.env.LS_INSTALL_OPTS || '').split(' ')
const result = spawnSync('node-gyp', ['rebuild', ...opts], { stdio: 'inherit' })

if (result.error || result.signal) {
  process.exit(1)
}
