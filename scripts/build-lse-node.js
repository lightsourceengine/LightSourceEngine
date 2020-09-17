#!/usr/bin/env node

//
// Create a Light Source Engine runtime package.
//
// Usage
//
// build-lse-node.js [args]      Build a runtime using the current node environment os + arch.
//
// Arguments
//
// --platform value [Default: process.platform Values: darwin, linux, win32]
// The target platform id.
//
// --arch value [Default: process.arch Values: armv7l, x64]
// The target arch id.
//
// --profile value [Default: none Value: none, pi, nclassic, psclassic]
// The platform profile. An additional specifier of the platform to help the package builder tailor the build for
// a specific target.
//
// --sdl-profile value [Default: system Values: system, rpi, kmsdrm, framework, dll]
// The SDL profile is a hint about how SDL should be linked and possibly packaged for the target platform.
//
// --node-binary-cache path
// Directory where the official node packages are unarchived and stored on disk. If not specified, a temporary
// folder is used.
//
// --crosstools-home path
// If cross compiling to arm, this path points to the source root of the crosstools project.
//
// --sdl-runtime-pkg path
// SDL2 runtime binaries to be included in the final package.
//
// --sdl-mixer-runtime-pkg path
// SDL2_mixer runtime binaries to be included in the final package.
//
// --skip-compile
// If specified, yarn --force is not run and this script assumes LightSourceEngine has been successfully build.
//
// --compress-node-binary
// If specified, the node executable is compressed with upx. This is useful for systems with limited disk space.
//
// --strip-node-binary
//
// If specified, the node executable will be stripped of debug symbols.
//
// --minimal-node-install
// If specified, nearly everything except the node binary is stripped from the official node package. The resulting
// package will not be functional for node development, but the package can run Light Source Engine apps. This is
// available for systems with limited disk space.
//
// Shell Dependencies
//
// This script is intended to run on a x86_64 Ubuntu system. It can run on Mac, but cross compiling to arm is not
// supported. The following shell dependencies are required, depending on the arguments specified.
//
// - node
// - upx
// - tar
// - wget
// - patchelf
// - python 2.7 (for node-gyp)
// - crosstools (https://github.com/lightsourceengine/crosstools)
// - C++ 14 toolchain (gcc or clang)
// - SDL 2.0.4+ development libraries
// - SDL Mixer 2.0.0+ development libraries
//
// Node Version
//
// The node version that invokes this script will be the node version of the built package. This restriction is
// to prevent node ABI conflicts with light-source native modules and the mode runtime on the target system.
//
// Package Structure
//
// The Light Source Engine is just a standard NodeJS distribution with a pre-compiled light-source, light-source-react
// and react installed as 'builtin' modules.
//
// Pre-compiled SDL libraries are included, if available on the system.
//  - Windows and Mac contain SDL2 and SDL2_mixer.
//  - Linux builds use the system SDL libraries (use apt to install).
//  - Linux (armv7l) builds for specific systems typically have a pre-compiled SDL2, but use the system SDL_mixer (if
//   available).
//
// For 'minimal' builds, npm and extraneous files have been removed to reduce size on disk for the target system
// (use --minimal-node-install).
//
// Here is the general structure of where files end up:
//
// root
//   lib/
//     builtin/
//       loader.mjs
//       light-source/
//         index.mjs
//         Release/
//       light-source-react
//         index.mjs
//       react
//         index.cjs
//     Frameworks/
//       SDL2.framework
//       SDL2_mixer.framework
//     native/
//       libSDL2.so
//   bin/
//     node
//     lse-node
//     SDL2.dll
//     SDL2_mixer.dll
//
// Frameworks - SDL2 and SDL2_mixer frameworks for Mac.
// native     - This folder is in the LD_LIBRARY_PATH for Linux builds. This allows the packager
//              flexibility with how the SDL shared object is loaded on the target system..
// node       - Standard node binary. If this is launched on it's own, import will not be able to find Light Source
//              Engine builtin modules.
// lse-node   - Script that configures the Light Source Engine environment and launches node. The configuration will
//              set up builtins and other platform specific configurations. This script should be used to launch
//              LSE apps.
// Release    - The pre-compiled light-source addon are stored here.
// Windows    - On Windows, the SDL DLLs appear in the same directory as node.exe, as that is in Windows default lib
//              search path.
//

const { join, basename } = require('path')
const { spawn } = require('child_process')
const { emptyDir, ensureDir, lstat, pathExists, pathExistsSync, copy, move, createSymlink, createWriteStream, unlink, writeFile, readFile } = require('fs-extra')
const { tmpdir } = require('os')
const commandLineArgs = require('command-line-args')
const fetch = require('node-fetch')
const tar = require('tar')
const AdmZip = require('adm-zip')

const Platform = {
  linux: 'linux',
  darwin: 'darwin',
  win32: 'win32'
}

const Architecture = {
  armv7l: 'armv7l',
  x64: 'x64'
}

const Profile = {
  none: 'none',
  pi: 'pi',
  nclassic: 'nclassic',
  psclassic: 'psclassic'
}

const SDLProfile = {
  system: 'system',
  rpi: 'rpi',
  kmsdrm: 'kmsdrm',
  framework: 'framework',
  dll: 'dll',
}

const commandLineArgSpec = [
  { name: 'platform', type: String, multiple: false, defaultValue: process.platform },
  { name: 'arch', type: String, multiple: false, defaultValue: process.arch },
  { name: 'profile', type: String, multiple: false, defaultValue: 'none' },
  { name: 'sdl-profile', type: String, multiple: false, defaultValue: 'system' },
  { name: 'node-binary-cache', type: String, multiple: false, defaultValue: '' },
  { name: 'crosstools-home', type: String, multiple: false, defaultValue: '' },
  { name: 'sdl-runtime-pkg', type: String, multiple: false, defaultValue: '' },
  { name: 'sdl-mixer-runtime-pkg', type: String, multiple: false, defaultValue: '' },
  { name: 'skip-compile', type: Boolean, defaultValue: false },
  { name: 'compress-node-binary', type: Boolean, defaultValue: false },
  { name: 'strip-node-binary', type: Boolean, defaultValue: false },
  { name: 'minimal-node-install', type: Boolean, defaultValue: false }
]

const getCommandLineOptions = () => {
  const options = commandLineArgs(commandLineArgSpec, { camelCase: true })
  const validateStringArg = (options, name, legalValues) => {
    if (legalValues[options[name]] === undefined) {
      throw Error(`Invalid ${name} value of ${options[name]}. Legal values: ${Object.keys(legalValues).join(', ')}`)
    }
  }

  validateStringArg(options, 'platform', Platform)
  validateStringArg(options, 'arch', Architecture)
  validateStringArg(options, 'profile', Profile)
  validateStringArg(options, 'sdlProfile', SDLProfile)

  options.isArmCrossCompile = (isLinux(process.platform) && process.arch === Architecture.x64
      && isLinux(options.platform) && (options.arch === Architecture.armv7l))

  // TODO: re-enable
  // if (!options.isArmCrossCompile && (options.platform !== process.platform || options.arch !== process.arch)) {
  //   throw Error(`Host ${process.platform}-${process.arch} cannot compile to target ${options.platform}-${options.arch}`)
  // }

  if (options.isArmCrossCompile && !pathExistsSync(options.crosstoolsHome)) {
    throw Error('--crosstool-home does not specify a directory')
  }

  if (!options.nodeBinaryCache) {
    options.nodeBinaryCache = tmpdir()
  }

  if (!pathExistsSync(options.nodeBinaryCache)) {
    throw Error('--node-binary-cache does not exist')
  }

  const sdlRuntimePkgExists = pathExistsSync(options.sdlRuntimePkg)
  const sdlMixerRuntimePkgExists = pathExistsSync(options.sdlMixerRuntimePkg)
  const sdlProfileError = (pkgFlag) => {
    if (!sdlRuntimePkgExists) {
      throw Error(`--sdl-profile ${options.sdlProfile} requires ${pkgFlag} to be a valid path`)
    }
  }

  switch (options.sdlProfile) {
    case SDLProfile.rpi:
    case SDLProfile.kmsdrm:
      if (!sdlRuntimePkgExists) {
        sdlProfileError('--sdl-runtime-pkg')
      }
      break
    case SDLProfile.framework:
      if (!sdlRuntimePkgExists || !options.sdlRuntimePkg.endsWith('SDL2.framework')) {
        sdlProfileError('--sdl-runtime-pkg')
      }
      if (!sdlMixerRuntimePkgExists || !options.sdlMixerRuntimePkg.endsWith('SDL2_mixer.framework')) {
        sdlProfileError('--sdl-mixer-runtime-pkg')
      }
      break
    case SDLProfile.dll:
      if (!sdlRuntimePkgExists) {
        sdlProfileError('--sdl-runtime-pkg')
      }
      if (!sdlMixerRuntimePkgExists) {
        sdlProfileError('--sdl-mixer-runtime-pkg')
      }
      break
    default:
      break
  }

  options.nodeVersion = process.version

  return options
}

const isWindows = (platform) => platform === Platform.win32
const isLinux = (platform) => platform === Platform.linux
const isDirectory = async (path) => {
  try {
    return (await lstat(path)).isDirectory()
  } catch (err) {
    return false
  }
}
const log = str => console.log(str)
const logMark = str => { log(str); return () => log(`${str} complete`)}

class NodePackage {
  #cache = ''
  #home = ''
  #version = ''
  #platform = ''
  #arch = ''

  constructor (options) {
    this.#version = options.nodeVersion
    this.#platform = options.platform
    this.#arch = options.arch
    this.#cache = options.nodeBinaryCache
  }

  getHome () {
    return this.#home
  }

  getNodeExecutable () {
    return isWindows(this.#platform) ? join(this.#home, 'node.exe') : join(this.#home, 'bin', 'node')
  }

  getNodeExecutableBasename () {
    return basename(this.getNodeExecutable())
  }

  getNodeLicense () {
    return join(this.#home, 'LICENSE')
  }

  async sync () {
    const windows = isWindows(this.#platform)
    const tag = `node-${this.#version}-${windows ? this.#platform.slice(0, -2) : this.#platform}-${this.#arch}`
    const home = join(this.#cache, tag)
    const homeExists = await pathExists(home)
    const ext = windows ? '.zip' : '.tar.gz'
    const url = `https://nodejs.org/download/release/${this.#version}/${tag}${ext}`

    if (homeExists) {
      log('node: found node package in cache')
      this.#home = home
      return
    }

    const complete = logMark('node: downloading node')

    log('node: ' + url)
    
    const response = await fetch(url)
    const tempFile = join(this.#cache, 'temp')
    let outputStream

    if (ext === '.zip') {
      outputStream = createWriteStream(tempFile)
    } else {
      outputStream = tar.x({ C: this.#cache })
    }

    response.body.pipe(outputStream)

    await new Promise((resolve, reject) => {
      outputStream.on('close', () => resolve())
      outputStream.on('error', reject)
    })

    if (ext === '.zip') {
      await new Promise((resolve, reject) => {
        (new AdmZip(tempFile)).extractAllToAsync(this.#cache, false, (err) => {
            err ? reject(err) : resolve()
          })
      })

      try {
        await unlink(tempFile)
      } catch (e) {
        // ignore
      }
    }

    complete()
    this.#home = home
  }
}

class LightSourceNodePackage {
  #home = ''
  #name = ''
  #nodeHome = ''
  #nodeBin = ''
  #nodeLib = ''
  #nodeBuiltin = ''

  async prepare (options, sourceRoot) {
    let postFix = ''
    let { platform } = options
    let lightSourceVersion = await sourceRoot.getVersion()

    if (options.platform === Platform.darwin) {
      platform = 'mac'
    } else if (options.platform === Platform.win32) {
      platform = 'win'
    } else if (options.profile === Profile.pi) {
      if (options.sdlProfile === SDLProfile.rpi) {
        postFix = "-rpi"
      } else if (options.sdlProfile === SDLProfile.kmsdrm) {
        postFix = "-kmsdrm"
      }
    } else if (options.profile === Profile.psclassic || options.profile === Profile.nclassic) {
      platform = options.profile
    }

    this.#name = `ls-node-v${lightSourceVersion}-${platform}-${options.arch}${postFix}`
    this.#home = join(sourceRoot.getBuildPath(), this.#name)

    await emptyDir(this.#home)
    await ensureDir(this.#home)

    if (options.profile === Profile.nclassic || options.profile === Profile.psclassic) {
      this.#nodeHome = join(this.#home, 'usr/share/LightSourceEngine')
    } else {
      this.#nodeHome = this.#home
    }

    if (isWindows(options.platform)) {
      this.#nodeBin = this.#nodeHome
      this.#nodeLib = this.#nodeHome
    } else {
      this.#nodeBin = join(this.#nodeHome, 'bin')
      this.#nodeLib = join(this.#nodeHome, 'lib')
    }

    this.#nodeBuiltin = join(this.#nodeLib, 'builtin')
  }

  async installNode (nodePackage, minimal) {
    const complete = logMark('staging: installing node...')

    if (minimal) {
      await ensureDir(this.#nodeBuiltin)
      await ensureDir(this.#nodeBin)
      await ensureDir(this.#nodeLib)

      const copyComplete = logMark('staging: minimal node copy...')

      await copy(nodePackage.getNodeExecutable(), join(this.#nodeBin, nodePackage.getNodeExecutableBasename()))
      await copy(nodePackage.getNodeLicense(), join(this.#nodeHome, 'LICENSE-node'))

      copyComplete()
    } else {
      const copyComplete = logMark('staging: full node copy...')

      await copy(nodePackage.getHome(), this.#nodeHome)
      await move(join(this.#nodeHome, 'LICENSE'), join(this.#nodeHome, 'LICENSE-node'))

      copyComplete()
    }

    complete()
  }

  async installModuleLoader (sourceRoot) {
    await copy(sourceRoot.getBuiltinLoader(), join(this.#nodeBuiltin, 'loader.mjs'))
    log('staging: loader installed')
  }

  async installModule(module) {
    if (module.mjs) {
      const sourceDir = join(this.#nodeBuiltin, module.name)

      await ensureDir(sourceDir)
      await copy(module.mjs, join(sourceDir, 'index.mjs'))
    }

    if (module.cjs) {
      const sourceDir = join(this.#nodeBuiltin, module.name)

      await ensureDir(sourceDir)
      await copy(module.cjs, join(sourceDir, 'index.cjs'))
    }

    if (module.native) {
      const nativeDir = join(this.#nodeBuiltin, module.name, 'Release')

      await ensureDir(nativeDir)
      await copy(module.native, join(nativeDir, `${module.name}.node`))
    }

    log(`staging: ${module.name} module installed`)
  }

  async configureNodeExecutable (nodePackage, options) {
    const executable = join(this.#nodeBin, nodePackage.getNodeExecutableBasename())

    if (isLinux(options.platform)) {
      const complete = logMark('node: patchelf...')

      await runCommand ('patchelf', [ '--set-rpath', '$ORIGIN/../lib/native', executable ])

      complete()
    }

    if (options.stripNodeBinary) {
      const complete = logMark('node: strip...')

      if (options.isArmCrossCompile) {
        const strip = join(options.crosstoolsHome,
          'x64-gcc-6.3.1/arm-rpi-linux-gnueabihf/bin/arm-rpi-linux-gnueabihf-strip')
        await runCommand (strip, [executable])
      } else {
        await runCommand ('strip', [ executable ])
      }

      complete()
    }

    if (options.compressNodeBinary) {
      const complete = logMark('node: compressing with upx...')

      await runCommand ('upx', [ '-q', executable ])

      complete()
    }
  }

  async installNodeWrapperScript (nodePackage, sourceRoot, options) {
    const source = sourceRoot.getNodeWrapperScript(options)
    const scriptName = basename(source)
    const dest = join(this.#nodeBin, scriptName)

    await copy(source, dest)

    if (scriptName.endsWith('.sh')) {
      await move(dest, join(this.#nodeBin, scriptName.slice(0, -3)))
    }

    log(`staging: wrapper script ${scriptName} installed`)
  }

  async installLicense (sourceRoot) {
    const license = sourceRoot.getLicense()
    await copy(license, join(this.#nodeHome, 'LICENSE-lse-node'))

    log(`staging: license installed`)
  }

  async installSDLPackage (sdlPackage, options) {
    let target

    if (!sdlPackage) {
      return
    }

    const complete = logMark('staging: installing SDL binaries...')

    if (sdlPackage.libRoot) {
      target = join(this.#nodeLib, sdlPackage.libRoot)
      await ensureDir(target)
    } else {
      target = this.#nodeLib
    }

    await sdlPackage.install(target, options)

    complete()
  }

  async installCppLib (options) {
    if (options.isArmCrossCompile) {
      const libcpp = process.env.CROSS_LIBCPP
      const libcppBasename = basename(libcpp)
      const ldPath = join(this.#nodeLib, 'native')

      await ensureDir(ldPath)
      await copy(libcpp, join(ldPath, libcppBasename))
      await createSymlink(libcppBasename, join(ldPath, 'libstdc++.so.6'))

      log(`staging: ${libcppBasename} installed`)
    }
  }

  getName () {
    return this.#name
  }

  async package (sourceRoot, options) {
    const packagingComplete = logMark('packaging: bundle staging directory...')

    if (isWindows(options.platform)) {
      await Package.createZip(sourceRoot.getBuildPath(), this.getName())
    } else if (options.profile === Profile.psclassic) {
      await Package.createDeb(sourceRoot.getBuildPath(), this.getName())
    } else if (options.profile === Profile.nclassic) {
      await Package.createTar(sourceRoot.getBuildPath(), this.getName())
    } else {
      await Package.createTar(sourceRoot.getBuildPath(), this.getName())
    }

    packagingComplete()
  }
}

class SourceRoot {
  #root = ''

  constructor () {
    this.#root = join(__dirname, '..')
  }

  async getVersion () {
    const json = await readFile(join(this.#root, 'publishing/version.json'), "utf8")

    return JSON.parse(json).version
  }

  getBuildPath () {
    return join(this.#root, 'build')
  }

  getLightSourceModule () {
    return {
      name: 'light-source',
      mjs: join(this.#root, 'packages/light-source/dist/light-source.standalone.mjs'),
      native: join(this.#root, 'packages/light-source/build/Release/light-source.node')
    }
  }

  getLightSourceReactModule () {
    return {
      name: 'light-source-react',
      mjs: join(this.#root, 'packages/light-source-react/dist/light-source-react.standalone.mjs'),
      native: null
    }
  }

  getReactModule () {
    return {
      name: 'react',
      cjs: join(this.#root, 'packages/light-source-react/dist/react.standalone.cjs'),
      native: null
    }
  }

  getBuiltinLoader () {
    return join(this.#root, 'packages/light-source-loader/dist/builtin.standalone.mjs')
  }

  getLicense () {
    join(this.#root, 'LICENSE')
  }

  getNodeWrapperScript ({ platform }) {
    if (isWindows(platform)) {
      return join(this.#root, 'scripts/static/lse-node.cmd')
    } else {
      return join(this.#root, 'scripts/static/lse-node.sh')
    }
  }

  async compile (options) {
    if (options.skipCompile) {
      log('compile: skipping build due to --skip-compile flag')
      return
    }

    // Set up environment.
    const env = {
      // inherit the environment
      ...process.env,
      // use all the power
      npm_config_ls_install_opts: '--jobs max',
      // enable all plugins, include ref
      // TODO: disable ref for release builds?
      // TODO: this list must be kept in sync!
      ls_enable_plugin_platform_sdl: 1,
      ls_enable_plugin_platform_ref: 1,
      ls_enable_plugin_audio_sdl_audio: 1,
      ls_enable_plugin_audio_sdl_mixer: 1,
      ls_enable_plugin_audio_ref: 1,
      // disable tests
      npm_config_ls_enable_native_tests: 0,
      // TODO: (not supported yet) env.npm_config_ls_asmjit_build = 'arm'
    }

    let program
    let programArgs

    // Setup compile command. Either "cross [profile] yarn --force" or "yarn --force"
    if (options.isArmCrossCompile) {
      // TODO: use profile to choose a device specific cross target
      const crossTarget = options.arch

      program = join(options.crosstoolsHome, 'bin', 'cross')
      programArgs = [crossTarget, 'yarn']
    } else {
      program = 'yarn'
      programArgs = []
    }

    programArgs = [...programArgs, '--force']

    const complete = logMark(`compile: ${program} ${programArgs.join(' ')}`)
    const compileInterval = setInterval(() => { log('compile: still running...') }, 5000)

    try {
      await runCommand(program, programArgs, {shell: true, cwd: this.#root, env})
    } finally {
      clearInterval(compileInterval)
    }

    complete()
  }
}

class SDLFrameworkPackage {
  libRoot = 'Frameworks'

  async install (path, options) {
    const { sdlRuntimePkg, sdlMixerRuntimePkg } = options

    const complete = logMark('staging: copying Frameworks...')

    await Promise.all([
      this.#installFramework(sdlRuntimePkg, path, 'SDL2.framework'),
      this.#installFramework(sdlMixerRuntimePkg, path, 'SDL2_mixer.framework')
    ])

    complete()
  }

  async #installFramework (pkg, libPath, frameworkName) {
    if (pkg.endsWith('.framework')) {
      const frameworkPath = join(libPath, frameworkName)

      await ensureDir(frameworkPath)
      await copy(pkg, frameworkPath)
    } else if (pkg.endsWith('.tar.gz')) {
        await tar.x({
          file: pkg,
          C: libPath
        })
    } else {
      throw Error('sdl-runtime-pkg must be a .framework directory or tarball')
    }
  }
}

class SDLNativePackage {
  libRoot = 'native'

  async install (path, options) {
    const libPath = join(path, this.libRoot)
    const { sdlRuntimePkg } = options
    const pkgIsDirectory = await isDirectory(sdlRuntimePkg)
    const tarballExtension = '.tar.gz'
    const libSDL2Path = join('lib', 'libSDL2-2.0.so.0')
    const complete = logMark('staging: copying native SDL2...')

    if (pkgIsDirectory) {
      await copy(join(sdlRuntimePkg, libSDL2Path), join(libPath, 'libSDL2.so'))
    } else if (sdlRuntimePkg.endsWith(tarballExtension)) {
      await tar.x({
        file: sdlRuntimePkg
      }, [join(basename(sdlRuntimePkg).slice(0, -(tarballExtension.length)), libSDL2Path)])
      await move(join(libPath, 'libSDL2-2.0.so.0'), join(libPath, 'libSDL2.so'))
    } else {
      throw Error('sdl-runtime-pkg must be a directory or tarball')
    }

    complete()
  }
}

class SDLWindowsLibPackage {
  libRoot = ''

  async install (path, options) {
    const { sdlRuntimePkg, sdlMixerRuntimePkg } = options
    const sdl = new AdmZip(sdlRuntimePkg)
    const sdlMixer = new AdmZip(sdlMixerRuntimePkg)

    const complete = logMark('staging: copying SDL2 DLLs...')

    await Promise.all([
      ...this.#processZipEntries(path, sdl, (name) => name),
      ...this.#processZipEntries(path, sdlMixer, (name) => name === 'README.txt' ? 'README-SDL_mixer.txt' : name)
    ])

    complete()
  }

  #processZipEntries (path, zip, rename) {
    const files = []
    const entries = zip.getEntries().filter(({entryName}) => entryName.endsWith('.dll') || entryName.endsWith('.txt'))

    for (const entry of entries) {
      const readAndWrite = new Promise((resolve, reject) => {
        entry.getDataAsync((buffer, err) => {
          err ? reject(Error(err)) : resolve(buffer)
        })
      }).then((buffer) => {
        writeFile(join(path, rename(entry.entryName)), buffer)
      })

      files.push(readAndWrite)
    }

    return files
  }
}

class Package {
  static async createTar(workingDir, archiveDir) {
    const file = archiveDir + '.tar.gz'

    log('packaging: ' + file)

    await tar.create({
      file: join(workingDir, file),
      gzip: true,
      C: workingDir
    }, [archiveDir])
  }

  static async createZip(workingDir, archiveDir) {
    const zip = new AdmZip()
    const file = archiveDir + '.zip'

    log('packaging: ' + file)

    zip.addLocalFolder(join(workingDir, archiveDir), archiveDir)

    await new Promise((resolve, reject) => {
      zip.writeZip(join(workingDir, file), (err) => { err ? reject(err) : resolve() })
    })
  }

  static async createDeb() {
    throw Error('.deb packages not implemented')
  }
}

const runCommand = async (...args) => {
  return new Promise((resolve) => {
    const subprocess = spawn(...args)
    let out = ''

    subprocess.on('close', (code) => {
      if (code !== 0) {
        log(args[0] + ' command failed with code ' + code)
        log(out)
        process.exit(1)
      } else {
        resolve()
      }
    })

    subprocess.stderr.setEncoding('utf8');
    subprocess.stderr.on('data', (data) => {
      out += data.toString()
    })

    subprocess.stdout.setEncoding('utf8');
    subprocess.stdout.on('data', (data) => {
      out += data.toString()
    })

    subprocess.on('error', (err) => {
      log(args[0] + ' command failed with unexpected error')
      log(err.message)
      process.exit(1)
    })
  })
}

const createLseNodePackage = async () => {
  const options = getCommandLineOptions()
  const sourceRoot = new SourceRoot()
  const nodePackage = new NodePackage(options)
  const lightSourceModePackage = new LightSourceNodePackage()

  // Create a fresh staging directory.
  await lightSourceModePackage.prepare(options, sourceRoot)

  log(`Building ${lightSourceModePackage.getName()} with NodeJS ${options.nodeVersion}`)

  // Start compilation.
  const compile = sourceRoot.compile(options)

  // Get node and copy all files to the staging directory that are not compile dependent.
  const staging = nodePackage.sync()
    .then(() => {
      return lightSourceModePackage.installNode(nodePackage, options.minimalNodeInstall)
    })
    .then(() => {
      const { sdlProfile } = options
      let sdlPackage

      if (sdlProfile === SDLProfile.rpi || sdlProfile === SDLProfile.kmsdrm) {
        sdlPackage = new SDLNativePackage()
      } else if (sdlProfile === SDLProfile.framework) {
        sdlPackage = new SDLFrameworkPackage()
      } else if (sdlProfile === SDLProfile.dll) {
        sdlPackage = new SDLWindowsLibPackage()
      } else {
        sdlPackage = null
      }

      return Promise.all([
        lightSourceModePackage.installSDLPackage(sdlPackage, options),
        lightSourceModePackage.installCppLib(options),
        lightSourceModePackage.configureNodeExecutable(nodePackage, options),
        lightSourceModePackage.installNodeWrapperScript(nodePackage, sourceRoot, options),
        lightSourceModePackage.installLicense(sourceRoot)
      ])
    })

  // Wait for compile and staging to complete before proceeding.
  await Promise.all([ compile, staging ])
    .then(() => {
      // These files are dependent on compile completing and staging creating directories.
      return Promise.all([
        lightSourceModePackage.installModuleLoader(sourceRoot),
        lightSourceModePackage.installModule(sourceRoot.getLightSourceModule()),
        lightSourceModePackage.installModule(sourceRoot.getLightSourceReactModule()),
        lightSourceModePackage.installModule(sourceRoot.getReactModule()),
      ])
    })

  // Finally, creating an archive from the staging directory.
  await lightSourceModePackage.package(sourceRoot, options)
}

createLseNodePackage()
  .then(() => {
    log('done')
  })
  .catch((err) => {
    log(err)
    process.exit(1)
  })
