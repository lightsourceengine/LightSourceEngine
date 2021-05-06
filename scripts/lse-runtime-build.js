#!/usr/bin/env node

/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

const { join, basename, dirname } = require('path')
const fetch = require('node-fetch')
const cl = require('command-line-args')
const { EOL } = require('os')
const Handlebars = require('handlebars')
const Spinnies = require('spinnies')
const YAML = require('yaml')
const {
  copy,
  createSymlink,
  emptyDir,
  ensureDir,
  readdir,
  readFile,
  remove,
  rename,
  writeFile
} = require('fs-extra')
const {
  copyTo,
  createZip,
  createTarGz,
  exec,
  exit,
  extract,
  extractTemp,
  group,
  writeFileWithTemplate
} = require('./lib/lse-util.js')

const Format = {
  tgz: 'tgz',
  hmod: 'hmod',
  zip: 'zip',
  appimage: 'appimage'
}

const Platform = {
  windows: 'win32',
  linux: 'linux',
  macos: 'darwin'
}

const PlatformType = {
  pi: 'pi',
  nclassic: 'nclassic'
}

const Arch = {
  x64: 'x64',
  armv6l: 'armv6l',
  armv7l: 'armv7l',
  arm64: 'arm64'
}

const SDLPlatformMap = {
  darwin: 'platform:Mac OS X',
  win32: 'platform:Windows',
  linux: 'platform:Linux'
}

const NodeSourceAlias = {
  nodejs: 'https://nodejs.org/download/release/',
  unofficial: 'https://unofficial-builds.nodejs.org/download/release/',
  ci: 'https://github.com/lightsourceengine/custom-node-builds/releases/download/'
}

const kCiUrlDefault = 'https://github.com/lightsourceengine/ci/releases/download/v1.1.0/'

const kGameControllerDbUrl = 'https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt'

const arg = (name, type) => ({ name, type, multiple: false })

const kArgSpec = [
  arg('recipe', String),
  arg('skip-compile', Boolean),
  arg('install-lib-cpp', Boolean),
  arg('install-game-controller-db', Boolean),
  arg('platform', String),
  arg('platform-type', String),
  arg('target-arch', String),
  arg('sdl-install', Boolean),
  arg('sdl-runtime', String),
  arg('sdl-mixer-runtime', String),
  arg('node-bin-compress', Boolean),
  arg('node-bin-strip', Boolean),
  arg('node-minimal', Boolean),
  arg('node-src', String),
  arg('node-custom-tag', String),
  arg('lse-bin-strip', Boolean),
  arg('format', String),
]

// due to the --recipe arg being overridable by other args, the cl tool default options
// do not work. so, store the default values here and apply them AFTER recipe and cl args
// are merged
const kArgDefaults = {
  recipe: '',
  skipCompile: false,
  installLibCpp: false,
  installGameControllerDb: false,
  platform: process.platform,
  platformType: '',
  targetArch: process.arch,
  sdlInstall: false,
  sdlRuntime: '',
  sdlMixerRuntime: '',
  nodeBinCompress: false,
  nodeBinStrip: false,
  nodeMinimal: true,
  nodeSrc: NodeSourceAlias.nodejs,
  nodeCustomTag: '',
  lseBinStrip: false,
  format: Format.tgz
}

const kFriendlyPlatformName = {
  [Platform.windows]: 'windows',
  [Platform.macos]: 'macos'
}

const kSupportedLinuxTargets = [
  Arch.x64,
  Arch.armv6l,
  Arch.armv7l,
  Arch.arm64
]

const status = {}

const readFileYaml = async (filename) => YAML.parse(await readFile(filename, 'utf8'))

const srcroot = (...args) => join(__dirname, '..', ...args)

const createStagingFolder = async (options) => {
  const roots = {
    staging: srcroot('build', options.id)
  }

  let share
  let bin

  switch(options.format) {
    case Format.hmod:
      share = join(roots.staging, 'usr', 'share', 'lse')
      bin = join(roots.staging, 'usr', 'bin')
      break
    case Format.appimage:
      share = join(roots.staging, 'share', 'lse')
      bin = join(roots.staging)
      break
    default:
      share = join(roots.staging, 'share', 'lse')
      bin = join(roots.staging, 'bin')
      break
  }

  await emptyDir(roots.staging)

  await group(
    ensureDir(roots.bin = bin),
    ensureDir(roots.assets = join(share, 'assets')),
    ensureDir(roots.lib = join(share, 'lib')),
    ensureDir(roots.node = join(share, 'node')),
    ensureDir(roots.builtin = join(share, 'builtin')),
    ensureDir(roots.licenses = join(share, 'licenses'))
  )

  await group(
    ensureDir(roots.licenses_lse = join(roots.licenses, 'lse')),
    ensureDir(roots.licenses_node = join(roots.licenses, 'node')),
    ensureDir(roots.licenses_sdl = join(roots.licenses, 'sdl2')),
    ensureDir(roots.licenses_fonts = join(roots.licenses, 'fonts'))
  )

  roots.meta = roots.staging
  roots.node_home = join(roots.node, process.version)

  options.roots = roots
}

const compile = async (options) => {
  if (options.skipCompile) {
    return
  }

  let program
  let programArgs

  // Setup compile command. Either "cross [profile] yarn --force" or "yarn --force"
  if (options.platform === Platform.linux && options.targetArch !== process.arch) {
    process.env.CROSS_TOOLCHAIN_PREFIX || exit('CROSS_TOOLCHAIN_PREFIX must be set before cross compiler can run.')

    let crossTarget

    if (options.platformType === PlatformType.nclassic) {
      crossTarget = options.platformType
    } else {
      crossTarget = options.targetArch
    }

    program = join('cross')
    programArgs = [crossTarget, 'yarn']
  } else {
    program = 'yarn'
    programArgs = []
  }

  programArgs = [...programArgs, '--force']

  const spawnOpts = {
    shell: true,
    cwd: srcroot(),
    env: {
      // inherit the environment
      ...process.env,
      // use all the power
      npm_config_lse_install_opts: '--jobs max',
      // Build SDL plugins
      // TODO: this list must be kept in sync!
      lse_enable_plugin_platform_sdl: 1,
      lse_enable_plugin_audio_sdl_audio: 1,
      lse_enable_plugin_audio_sdl_mixer: 1,
      // disable tests
      npm_config_lse_enable_native_tests: 0
    }
  }

  status.compile.update('running')

  await exec(program, programArgs, spawnOpts)
}

const withToolchainPrefix = (program, {targetArch}) => {
  const { CROSS_TOOLCHAIN_PREFIX } = process.env

  if (CROSS_TOOLCHAIN_PREFIX && [Arch.armv6l, Arch.armv7l, Arch.arm64].includes(targetArch)) {
    return `${CROSS_TOOLCHAIN_PREFIX}-${program}`
  }

  return program
}

const stripNode = async (options) => {
  if (options.nodeBinStrip) {
    status.node.update('stripping executable symbols')
    await exec(withToolchainPrefix('strip', options), [join(options.roots.node_home, 'bin', 'node')])
  }
}

const stripBin = async (options) => {
  if (options.lseBinStrip) {
    status.compile.update('stripping executable symbols')
    await exec(
      withToolchainPrefix('strip', options),
      [join(options.roots.builtin, '@lse', 'core', 'build', 'lse-core.node')])
  }
}

const compressNode = async (options) => {
  if (options.nodeBinCompress) {
    status.node.update('compressing executable with upx')

    await exec('upx', ['-q', join(options.roots.node_home, 'bin', 'node')])
  }
}

const installGameControllerDb = async (options) => {
  const { roots, installGameControllerDb } = options

  if (installGameControllerDb) {
    status.meta.update('installing gamecontrollerdb.txt')

    // TODO: consider adding SDL_GameControllerDB as a submodule in the project
    const response = await fetch(kGameControllerDbUrl, { redirect: 'follow' })
    const csv = await response.text()

    await writeFile(
      join(roots.assets, basename(kGameControllerDbUrl)),
      // constrain entries to target platform
      csv.split(/\r?\n/)
        .filter(line => line.indexOf(SDLPlatformMap[options.platform]) !== -1)
        .join(EOL))
  }
}

const installFontLicense = async (options) => {
  status.meta.update('installing font license')

  await copyTo(srcroot('packages', '@lse', 'core', 'src', 'font', 'LICENSE-Roboto.txt'), options.roots.licenses_fonts)
}

const installSDL = async (options) => {
  const { platform, platformType, sdlInstall, sdlRuntime, sdlMixerRuntime } = options

  if (!sdlInstall || platform === Platform.windows) {
    return
  }

  if (platformType === PlatformType.pi) {
    await installSDLForPi(options)
  } else if (platform === Platform.macos) {
    await group(
      installSDLFramework(sdlRuntime, options),
      sdlMixerRuntime ? installSDLFramework(sdlMixerRuntime, options) : Promise.resolve()
    )
  } else {
    exit(`no SDL install for platform: ${platform}-${platformType ?? 'any'}`)
  }
}

const installSDLWindows = async (options) => {
  const { roots, platform, sdlInstall, sdlRuntime, sdlMixerRuntime } = options

  if (!sdlInstall || platform !== Platform.windows) {
    return
  }

  await extract(sdlRuntime, roots.node_home)
  await rename(join(roots.node_home, 'README-SDL.txt'), join(roots.licenses_sdl, 'README-SDL.txt'))

  if (!sdlMixerRuntime) {
    return
  }

  await extract(sdlMixerRuntime, roots.node_home)

  const ops = []
  let files = await readdir(roots.node_home)

  files.forEach(file => {
    if (file.startsWith('LICENSE')) {
      ops.push(rename(join(roots.node_home, file), join(roots.licenses_sdl, file)))
    }
  })

  ops.push(rename(join(roots.node_home, 'README.txt'), join(roots.licenses_sdl, 'README-sdl_mixer.txt')))

  await Promise.all(ops)
}

const installSDLFramework = async (runtime, options) => {
  const { lib } = options.roots

  if (runtime.endsWith('.framework')) {
    const frameworkPath = join(lib, basename(runtime))

    await ensureDir(frameworkPath)
    await copy(runtime, frameworkPath)
  } else {
    await extract(runtime, lib)
  }

  // TODO: install sdl license!
}

const installSDLForPi = async (options) => {
  const { sdlRuntime, roots } = options
  const root = await extractTemp(sdlRuntime)
  const sharedObject = join(root, 'lib', 'libSDL2-2.0.so.0')

  await copyTo(sharedObject, join(roots.lib, 'libSDL2-2.0.so.0.14.0'))
  await createSymlink('libSDL2-2.0.so.0.14.0', join(roots.lib, 'libSDL2-2.0.so.0'))
  await createSymlink('libSDL2-2.0.so.0.14.0', join(roots.lib, 'libSDL2-2.0.so'))
  await remove(root)

  // TODO: install sdl license!
}

const installLibCpp = async (options) => {
  if (options.installLibCpp) {
    await exec('copy-stdcpp', [options.roots.lib], { shell: true })
  }
}

const installNodePackages = async (options) => {
  const { roots } = options

  const src_standalone = srcroot('packages', 'standalone', 'dist')
  const src_lse_react = srcroot('packages', '@lse', 'react', 'dist')
  const src_lse_core = srcroot('packages', '@lse', 'core', 'dist')
  const src_lse_loader = srcroot('packages', '@lse', 'loader', 'dist')

  let react
  let bindings
  let lse_react
  let lse_core
  let lse_loader
  
  await group(
    ensureDir(react = join(roots.builtin, 'react')),
    ensureDir(bindings = join(roots.builtin, 'bindings')),
    ensureDir(lse_react = join(roots.builtin, '@lse', 'react')),
    ensureDir(lse_core = join(roots.builtin, '@lse', 'core')),
    ensureDir(lse_loader = join(roots.builtin, '@lse', 'loader')),
  )
  
  await group(
    // react
    copyTo(join(src_standalone, 'react.cjs'), react, 'index.cjs'),
    copyTo(join(src_standalone, 'jsx-runtime.cjs'), react),
    copyTo(join(src_standalone, 'jsx-dev-runtime.cjs'), react),
    copyTo(join(src_standalone, 'react-package.json'), react, 'package.json'),
    // bindings
    copyTo(join(src_standalone, 'bindings.cjs'), bindings, 'index.cjs'),
    copyTo(join(src_standalone, 'bindings-package.json'), bindings, 'package.json'),
    // @lse/core
    copyTo(join(src_lse_core, 'lse-core-standalone.cjs'), lse_core, 'index.cjs'),
    copyTo(join(src_lse_core, 'standalone-package.json'), lse_core, 'package.json'),
    copyTo(join(src_lse_core, 'Roboto-Regular-Latin.woff'), lse_core),
    copyTo(srcroot('packages', '@lse', 'core', 'build', 'Release', 'lse-core.node'), join(lse_core, 'build')),
    // @lse/react
    copyTo(join(src_lse_react, 'lse-react-standalone.cjs'), lse_react, 'index.cjs'),
    copyTo(join(src_lse_react, 'jsx-runtime.cjs'), lse_react, 'jsx-runtime.cjs'),
    copyTo(join(src_lse_react, 'standalone-package.json'), lse_react, 'package.json'),
    // @lse/loader
    copyTo(join(src_lse_loader, 'index-standalone.mjs'), lse_loader, 'index.mjs'),
    // licenses
    copyTo(srcroot('LICENSE'), roots.licenses_lse),
    copyTo(srcroot('NOTICE'), roots.licenses_lse),
  )
}

const installMeta = async (options) => {
  const { format, platformType, roots } = options
  const src_meta = srcroot('runtime', 'meta')

  status.meta.update('installing')

  if (format === Format.hmod) {
    await copyTo(join(src_meta, `uninstall-${platformType}.sh`), join(roots.meta), 'uninstall')
  } else if (format === Format.appimage) {
    await group(
      copyTo(join(src_meta, 'LightSourceEngine.desktop'), roots.meta),
      copyTo(join(src_meta, 'lse-logo.svg'), roots.meta)
    )
  }
}

const installEntryPoint = async (options) => {
  const { platform, platformType, roots, installGameControllerDb } = options
  const src_entry = srcroot('runtime', 'entry')
  const input = {
    node_version: process.version,
    platform,
    platform_type: platformType,
    install_game_controller_db: installGameControllerDb
  }

  status.bin.update('installing')

  if (platform === Platform.windows) {
    const file = 'LightSourceEngine.cmd'

    await writeFileWithTemplate(join(src_entry, file), join(roots.bin, file), input)
  } else {
    let app = 'LightSourceEngine'

    switch (options.format) {
      case Format.hmod:
        input.share_home = '/usr/share/lse'
        break
      case Format.appimage:
        input.share_home = './share/lse'
        app = 'AppRun'
        break
      default:
        input.share_home = '../share/lse'
        break
    }

    const destFile = join(roots.bin, app)

    await writeFileWithTemplate(join(src_entry, 'LightSourceEngine.sh'), destFile, input)
    await exec('chmod', [ 'a+x', destFile ], {shell: true})
  }
}

const installNode = async (options) => {
  const { roots, nodeMinimal, platform, targetArch, nodeSrc, nodeCustomTag } = options
  const { version } = process
  const custom = nodeCustomTag ? `-${nodeCustomTag}` : ''
  let tag
  let files
  let ext

  if (platform === Platform.windows) {
    tag = `node-${version}${custom}-win-${targetArch}`
    ext = '.zip'
    files = [
      `${tag}/node.exe`,
      `${tag}/LICENSE`
    ]
  } else {
    tag = `node-${version}${custom}-${platform}-${targetArch}`
    ext = '.tar.gz'
    files = [
      `${tag}/bin/node`,
      `${tag}/LICENSE`
    ]
  }

  const url = `${nodeSrc}${version}/${tag}${ext}`

  status.node.update('installing')

  await extract(
    url,
    join(roots.node),
    nodeMinimal ? { files } : {})

  await rename(join(roots.node, tag, 'LICENSE'), join(roots.licenses_node, 'LICENSE'))
  await rename(join(roots.node, tag), join(roots.node_home))
}

const createPackage = async (options) => {
  const { id, roots, format } = options
  const { staging } = roots
  const build = dirname(staging)
  let file = join(build, id + '.' + format)
  let removeStagingFolder = true

  status.package.update('creating')

  switch (format) {
    case Format.tgz:
      await createTarGz(build, [id], file)
      break
    case Format.hmod:
      await createTarGz(staging, ['usr', 'uninstall'], file)
      break
    case Format.zip:
      await createZip(staging, id, file)
      break
    case Format.appimage:
      file = join(build, id)
      removeStagingFolder = false
      break
    default:
      exit(`unsupported format: ${format}`)
  }

  if (removeStagingFolder) {
    await remove(staging)
  }

  return file
}

const resolveProtocol = (value) => {
  if (value?.startsWith('ci://')) {
    let base = process.env.CI_URL || kCiUrlDefault
    let file = (new URL(value)).host

    if (!base.endsWith('/')) {
      base += '/'
    }

    return (new URL(file, base)).href
  }

  return value
}

const getCommandLineOptions = async () => {
  let options = cl(kArgSpec, { camelCase: true })

  // load a recipe if it was specified
  if (options.recipe) {
    let recipeFile

    // get recipe filename
    if (options.recipe.startsWith('recipe://')) {
      recipeFile = srcroot('runtime', 'recipes', options.recipe.replace('recipe://', ''))

      if (!recipeFile.endsWith('.yml')) {
        recipeFile += '.yml'
      }
    } else {
      recipeFile = options.recipe
    }

    // read in recipe arguments
    const config = await readFileYaml(recipeFile)

    // convert dash format to camel-case
    for (const key of Object.keys(config)) {
      const camelCaseKey = key
        .split('-')
        .map((v, i) => i > 0 ? v[0].toUpperCase() + v.substr(1) : v)
        .join('')

      if (key !== camelCaseKey) {
        config[camelCaseKey] = config[key]
        delete config[key]
      }
    }

    // allow explicitly set args to override the recipe
    options = { ...config, ...options }
  }

  for (const key of Object.keys(kArgDefaults)) {
    if (options[key] === undefined) {
      options[key] = kArgDefaults[key]
    }
  }

  // convert known node src tags to urls
  if (options.nodeSrc in NodeSourceAlias) {
    options.nodeSrc = NodeSourceAlias[options.nodeSrc]
  } else if (!options.nodeSrc.endsWith('/')) {
    options.nodeSrc += '/'
  }

  // allow user to specify alternate names to node's darwin and win32.
  if (['windows', 'win'].includes(options.platform)) {
    options.platform = 'win32'
  } else if (['macos', 'mac'].includes(options.platform)) {
    options.platform = 'darwin'
  }

  // validate enum-like options
  Object.values(Platform).includes(options.platform)  || exit(`invalid platform: ${options.platform}`)
  !options.platformType || options.platformType in PlatformType || exit(`invalid platformType: ${options.platformType}`)
  options.targetArch in Arch || exit(`invalid target arch: ${options.targetArch}`)
  options.format in Format || exit(`invalid format: ${options.format}`)

  // resolve special protocols, like ci://, to a real url.
  options.sdlRuntime = resolveProtocol(options.sdlRuntime)
  options.sdlMixerRuntime = resolveProtocol(options.sdlMixerRuntime)

  const { targetArch, platform } = options

  if (process.platform === Platform.linux && process.arch === Arch.x64) {
    if (!kSupportedLinuxTargets.includes(targetArch)) {
      exit(`cross compiling to ${targetArch} not supported on linux`)
    }
  } else if (platform !== process.platform || targetArch !== process.arch) {
    exit(`cross compiling not supported on platform: ${process.platform}`)
  }

  // name the package, without extension, based on arguments
  const app = 'LightSourceEngine'
  const platformAlias = options.platformType || kFriendlyPlatformName[options.platform] || options.platform

  switch (options.format) {
    case Format.hmod:
      options.id = `${app}-${platformAlias}`
      break
    case Format.appimage:
      options.id = `${app}-${options.targetArch}.AppDir`
      break
    case Format.zip:
    case Format.tgz:
      options.id = `${app}-${platformAlias}-${options.targetArch}`
      break
  }

  return options
}

const setupTemplateEngine = () => {
  Handlebars.registerHelper('isMacOS', (value) => value === Platform.macos)
  Handlebars.registerHelper('isLinux', (value) => value === Platform.linux)
  Handlebars.registerHelper('isWindows', (value) => value === Platform.windows)
  Handlebars.registerHelper('isNClassic', (value) => value === PlatformType.nclassic)
  Handlebars.registerHelper('isAbsolute', (value) => value.startsWith('/'))
}

const setupSpinnies = () => {
  const spinnies = new Spinnies()
  const Status = class {
    constructor (id) {
      this.id = id
      spinnies.add(id, { text: `[${id}]: waiting` })
    }

    update(text) {
      spinnies.update(this.id, { text: `[${this.id}]: ${text}` })
    }

    succeed(text = 'done') {
      spinnies.succeed(this.id, { text: `[${this.id}]: ${text}` })
    }
  }

  status.node = new Status('node')
  status.compile = new Status('compile')
  status.lib = new Status('lib')
  status.assets = new Status('assets')
  status.bin = new Status('bin')
  status.package = new Status('package')
  status.meta = new Status('meta')
}

const main = async () => {
  const options = await getCommandLineOptions()

  setupTemplateEngine()
  setupSpinnies()

  // create the directory structure for the build.
  await createStagingFolder(options)

  // each top level (ish) directory in staging does not depend on another top
  // level directory. processing on each directory can safely be done in parallel.
  await group(
    // bin
    installEntryPoint(options)
      .then(() => status.bin.succeed()),
    // share/lse/lib
    group(
      installSDL(options),
      installLibCpp(options)
    ).then(() => status.lib.succeed()),
    // share/lse/assets
    group(
      installGameControllerDb(options),
      installFontLicense(options)
    ).then(() => status.assets.succeed()),
    // share/lse/node
    installNode(options)
      .then(() => stripNode(options))
      .then(() => compressNode(options))
      .then(() => installSDLWindows(options))
      .then(() => status.node.succeed()),
    // share/lse/builtin
    compile(options)
      .then(() => installNodePackages(options))
      .then(() => stripBin(options))
      .then(() => status.compile.succeed()),
    // share/lse/meta
    installMeta(options)
      .then(() => status.meta.succeed())
  )

  const packageFile = await createPackage(options)

  status.package.succeed()

  console.log(`Successfully creating runtime package: ${basename(packageFile)}`)
}

main().catch(exit)
