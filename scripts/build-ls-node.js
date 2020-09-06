#!/usr/bin/env node

//
// Create a Light Source Engine runtime package.
//
// Usage:
//
// build-ls-node.js                      Build a runtime using the current node environment os + arch.
// build-ls-node.js <platform-arch-tag>  Build a runtime for a specific platform. The current node environment or a
//                                       tag supported by crosstools is supported.
//
// <platform-arch-tag> is a string generated in node by: process.platform + '-' + process.arch. The cross compilation
// tags are:
//
//   linux-armv7l - Raspberry Pi 2/3/4, PlayStation Classic and more
//   nesc-armv7l  - S/NES Classic
//   linux-armv6l - Raspberry Pi Zero (Note: after version 10, NodeJS no longer publishes armv6l builds!)
//
// Environment Variables:
//
// CROSSTOOLS_HOME     - Path to crosstools directory. Defaults to ${HOME}/crosstools
//
// Shell Dependencies:
//
// - node
// - patchelf
// - python 2.7 (for node-gyp)
// - crosstools (https://github.com/lightsourceengine/crosstools)
// - C++ 14 toolchain (gcc or clang)
// - SDL 2.0.4+ development libraries
// - SDL Mixer 2.0.0+ development libraries
//
// The environment that invokes this script will be the node version of the built package. This restriction is
// to avoid potential node ABI conflicts when building light-source native modules.
//
// light-source depends on several npm_config environment variables that describe SDL paths and other options. For
// cross compile builds, these environment variables are managed by THIS script. For local builds, these environment
// variables must be exported by the caller of this script.
//
// Package Structure:
//
// The Light Source Engine is just a pre-compiled NodeJS distribution with light-source installed as a global
// module. npm and extraneous files have been removed.
//
// root
//   lib/
//     node/
//       light-source/
//         index.js
//         Release
//           <light-source .node files>
//       light-source-react
//         index.js
//       react
//         index.js
//   bin/
//     __node
//     ls-node (shell script with env configuration)
//
// lib/node/light-source/package.json - This file exists to coerce bindings into loading node module files from
//     the light-source directory.
// lib/node/lib/rpath - This directory is in the runpath for node and node module files on Linux. Any platform specific
//     shared objects can be placed here.
// lib/node/light-source-react/index.js - Contains react-reconciler and scheduler.
// lib/node/react - Standalone version of react.
//

const { join, delimiter } = require('path')
const { spawnSync } = require('child_process')
const { emptyDirSync, ensureDirSync, pathExistsSync, copySync, symlinkSync, existsSync } = require('fs-extra')
const readPkg = require('read-pkg')
const { tmpdir, homedir } = require('os')
const commandLineArgs = require('command-line-args')

let sTargetArch
let sTargetPlatform
let sTargetPlatformAlias
let sStagingPath
let sLightSourceVersion
let sLsNodeName
let sFrameworkPath
let sSkipBuild = false
const sCrossToolsHome = process.env.CROSSTOOLS_HOME || join(homedir(), 'crosstools')
const sCrossToolsSysroot =`${sCrossToolsHome}/x64-gcc-6.3.1/arm-rpi-linux-gnueabihf/arm-rpi-linux-gnueabihf/sysroot`
const sNodeDownloadsPath = process.env.NODE_DOWNLOADS || tmpdir()
const sCrossProfileMapping = {
  armv6l: 'rpizero',
  armv7l: 'rpi'
}
const sSupportedPlatforms = new Set([
  'linux',
  'darwin',
  'nesc',
  'psc'
])
const optionDefinitions = [
  { name: 'tag', type: String, multiple: false, defaultOption: true, defaultValue: `${process.platform}-${process.arch}` },
  { name: 'skip-build', type: Boolean, defaultValue: false },
  { name: 'framework-path', type: String, defaultValue: '' }
]

const run = (...args) => {
  const result = spawnSync(...args)

  if (result.status !== 0) {
    result.stdout && console.log(result.stdout.toString())
    result.stderr && console.error(result.stderr.toString())
    process.exit(1)
  }
}

const build = () => {
  let command = 'yarn'
  let args = [ '--force' ]
  let env = {
    ...process.env,
    npm_config_ls_install_opts: '--jobs max',
    npm_config_ls_with_tests: 'false',
    npm_config_ls_with_sdl_mixer: 'true'
  }

  switch (sTargetArch) {
    case 'armv6l':
    case 'armv7l':
      env.npm_config_ls_sdl_include = `${sCrossToolsSysroot}/usr/include`
      env.npm_config_ls_sdl_lib = `${sCrossToolsSysroot}/usr/lib`
      env.npm_config_ls_sdl_mixer_include = `${sCrossToolsSysroot}/usr/include`
      env.npm_config_ls_sdl_mixer_lib = `${sCrossToolsSysroot}/usr/lib`
      // TODO: use this environment variable when the arm backend is ready
      // env.npm_config_ls_asmjit_build = 'arm'
      args.unshift(sCrossProfileMapping[sTargetArch], command)
      env.PATH = `${env.PATH}${delimiter}${join(sCrossToolsHome, 'bin')}` 
      command = 'cross'
      break
    default:
      break
  }

  if (sFrameworkPath) {
    env.npm_config_ls_framework_path = sFrameworkPath
  }

  console.log(`Building [${sTargetPlatform}-${sTargetArch}]...`)

  run(command, args, { env, shell: true })
}

const prepareStagingDirectory = () => {
  emptyDirSync(sStagingPath)
  ensureDirSync(join(sStagingPath, 'lib', 'node_modules'))
  ensureDirSync(join(sStagingPath, 'lib', 'rpath'))
  ensureDirSync(join(sStagingPath, 'bin'))
}

const installNodeBin = () => {
  const node = `node-${process.version}-${sTargetPlatform}-${sTargetArch}`
  const nodeBinaryPath = join(sNodeDownloadsPath, node, 'bin', 'node')
  const stagingNodeBinaryPath = join(sStagingPath, 'bin', '__node')

  if (sTargetPlatform === process.platform && sTargetArch === process.arch) {
    copySync(process.execPath, stagingNodeBinaryPath)
  } else if (pathExistsSync(nodeBinaryPath)) {
    copySync(nodeBinaryPath, stagingNodeBinaryPath)
  } else {
    const url = `https://nodejs.org/download/release/${process.version}/${node}.tar.gz`

    console.log(`Downloading [${url}]...`)

    run(`wget -qO- "${url}" | tar -C "${sNodeDownloadsPath}" -xvz ${node}/bin/node`, { shell: true })
    copySync(nodeBinaryPath, stagingNodeBinaryPath)
  }

  copySync(join(__dirname, 'static', 'ls-node.sh'), join(sStagingPath, 'bin', 'ls-node'))

  if (sTargetPlatform === 'linux') {
    run('patchelf', [ '--set-rpath', '$ORIGIN/../lib/rpath', stagingNodeBinaryPath ], {})
  }
}

const installGlobalModules = () => {
  const lib = join(sStagingPath, 'lib')
  const nodeModules = join(lib, 'node_modules')
  const lightSourcePath = join(nodeModules, 'light-source')
  const lightSourceReleasePath = join(lightSourcePath, 'Release')
  const lightSourceReactPath = join(nodeModules, 'light-source-react')
  const reactPath = join(nodeModules, 'react')
  const addonPath = join('packages/light-source/build/Release')

  console.log('Installing global modules...')

  ensureDirSync(lightSourcePath)
  ensureDirSync(lightSourceReleasePath)
  ensureDirSync(lightSourceReactPath)
  ensureDirSync(reactPath)

  const fileList = [
    ['packages/light-source/dist/light-source.standalone.cjs', join(lightSourcePath, 'index.js')],
    ['packages/light-source-react/dist/light-source-react.standalone.cjs', join(lightSourceReactPath, 'index.js')],
    ['packages/light-source-node/dist/react.standalone.cjs', join(reactPath, 'index.js')],

    // TODO: add when toolchain supports modules
    // ['packages/light-source-node/dist/node-path-loader.mjs', join(sStagingPath, 'lib', 'node-path-loader.mjs')],

    [join(addonPath, 'light-source.node'), join(lightSourceReleasePath, 'light-source.node')],
    [join(addonPath, 'light-source-sdl.node'), join(lightSourceReleasePath, 'light-source-sdl.node')],
    [join(addonPath, 'light-source-sdl-mixer.node'), join(lightSourceReleasePath, 'light-source-sdl-mixer.node')],
    [join(addonPath, 'light-source-sdl-audio.node'), join(lightSourceReleasePath, 'light-source-sdl-mixer-audio.node')]
  ]

  for (const entry of fileList) {
    copySync(entry[0], entry[1])
  }

  if (sTargetPlatform === 'darwin') {
    const sdl2 = join(lib, 'Frameworks', 'SDL2.framework')
    const sdl2Mixer = join(lib, 'Frameworks', 'SDL2_mixer.framework')

    // Note: copySync copies directory CONTENTS, not the containing directory. So, we need to create (ensure) the
    // .framework directory we are copying, then copy the files into the new directory.
    emptyDirSync(sdl2)
    emptyDirSync(sdl2Mixer)

    copySync(`${sFrameworkPath}/SDL2.framework`, sdl2)
    copySync(`${sFrameworkPath}/SDL2_mixer.framework`, sdl2Mixer)
  }
}

const installArchSpecificFiles = () => {
  if (sTargetArch.startsWith('arm')) {
    const cpp = 'libstdc++.so.6.0.22'

    console.log('Copying libstdc++...')

    copySync(join(sCrossToolsSysroot, 'lib', cpp), join(sStagingPath, 'lib', 'rpath', cpp))
    symlinkSync(cpp, join(sStagingPath, 'lib', 'rpath', 'libstdc++.so.6'))
  }

  if (sTargetPlatformAlias === 'nesc' || sTargetPlatformAlias === 'psc') {
    console.log('Creating SDL2 symlink...')

    symlinkSync('/usr/lib/libSDL2.so', join(sStagingPath, 'lib', 'rpath', 'libSDL2-2.0.so.0'))
  }
}

const createPackage = () => {
  const tar = `${sLsNodeName}.tar.gz`

  console.log(`Packaging [${tar}]...`)

  run('tar', [ '-C', 'build', '-cvzf', join('build', tar), sLsNodeName ], { shell: true })
}

const load = () => {
  const options = commandLineArgs(optionDefinitions, { camelCase: true })
  const match = /^(\w+)-(\w+)$/.exec(options.tag)

  sSkipBuild = options.skipBuild

  if (match) {
    sTargetPlatformAlias = match[1]
    sTargetArch = match[2]
  } else {
    exit(`Unknown platform/arch: ${options.tag}`)
  }

  switch (sTargetPlatformAlias) {
    case 'nesc':
    case 'psc':
      sTargetPlatform = 'linux'
      break
    default:
      sTargetPlatform = sTargetPlatformAlias
      break
  }

  if (!sSupportedPlatforms.has(sTargetPlatformAlias)) {
    exit(`Unsupported platform ${sTargetPlatformAlias}.`)
  }

  if (sTargetPlatform === 'darwin' && (!sFrameworkPath || !existsSync(sFrameworkPath))) {
    exit('No framework path provided!')
  }

  sLightSourceVersion = readPkg.sync({ cwd: join('packages', 'light-source') }).version
  sLsNodeName = `ls-node-v${sLightSourceVersion}-${sTargetPlatform}-${sTargetArch}`
  sStagingPath = join('build', sLsNodeName)
}

const exit = (message) => {
  console.error(`Error: ${message}`)
  process.exit(1)
}

const main = () => {
  process.chdir(join(__dirname, '..'))

  load()
  prepareStagingDirectory()
  sSkipBuild || build()
  installNodeBin()
  installGlobalModules()
  installArchSpecificFiles()
  createPackage()
}

try {
  main()
} catch (e) {
  exit(e.message)
}
