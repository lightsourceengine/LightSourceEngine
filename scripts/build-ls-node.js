#!/usr/bin/env node

//
// Create a Light Source Engine runtime package.
//
// Usage:
//
// build-ls-node.js [args]      Build a runtime using the current node environment os + arch.
//
// Arguments:
//
// --platform value [Default: process.platform Values: darwin, linux]
// The target platform id.
//
// --arch value [Default: process.arch Values: armv7l, x64]
// The target arch id.
//
// --profile value [Default: none Value: none, pi, nclassic, psclassic]
// The platform profile. An additional specifier of the platform to help the package builder tailor the build for
// a specific target.
//
// --sdl-profile value [Default: system Values: system, rpi, kmsdrm, framework]
// The SDL profile is a hint about how SDL should be linked and possibly packaged for the target platform.
//
// --framework-path path
// If --profile framework is specified, the SDL2.framework and SDL2_mixer.framework in this path will be used
// for building and packaging.
//
// --node-binary-cache path
// Directory where the official node packages are unarchived and stored on disk. If not specified, a temporary
// folder is used.
//
// --crosstools-home path
// If cross compiling to arm, this path points to the source root of the crosstools project.
//
// --sdl-root path
// If an SDL video profile of rpi or kmsdrm is specified, this is the path is used. The path points to a directory
// containing an include (SDL2 headers) and lib (pre-compiled SDL2 shared object for the target). Light Source Engine
// will be built against this SDL2 version and the shared object will be included in the final package.
//
// --skip-build
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
// Shell Dependencies:
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
// The node version that invokes this scripts will be the node version of the built package. This restriction is
// to prevent node ABI conflicts with light-source native modules and the mode runtime on the target system.
//
// Package Structure:
//
// The Light Source Engine is just a standard NodeJS distribution with a pre-compiled light-source installed as a
// global module (via $NODE_PATH). For some builds, npm and extraneous files have been removed to reduce size on disk
// for the target system (when --minimal-node-install is specified).
//
// Here is the general structure of where files end up:
//
// root
//   lib/
//     Frameworks/
//       SDL2.framework
//       SDL2_mixer.framework
//     so/
//     node_modules/
//       light-source/
//         index.js
//         Release/
//       light-source-react
//         index.js
//       react
//         index.js
//   bin/
//     __node
//     -node (shell script with env configuration)
//
// Frameworks - On mac, SDL2 and SDL2_mixer frameworks are bundled for the runtime. light-source-sdl.node has it's
//              @executable_path set to find this Frameworks directory.
// so         - On linux builds, this folder is the default value for LD_LIBRARY_PATH. This allows the packager
//              flexibility with how the SDL so is loaded on the target system.
// node       - This is a shell script that sets NODE_PATH, to find react, light-source and light-source-react
//              javascript packages, and LD_LIBRARY_PATH, to find SDL libraries. The packages are global or builtin
//              to the runtime or developer.
// Release/   - Pre-compiled light-source addon are stored here. The light-source runtime can find them using the
//              $LS_ADDON_PATH environment variable.
//

const { join, delimiter, basename } = require('path')
const { spawn } = require('child_process')
const { emptyDirSync, ensureDirSync, pathExistsSync, copy, move, createSymlink } = require('fs-extra')
const readPkg = require('read-pkg')
const { tmpdir } = require('os')
const commandLineArgs = require('command-line-args')

const command = async (...args) => {
  return new Promise((resolve, reject) => {
    const subprocess = spawn(...args)
    let out = ''

    subprocess.on('close', (code) => {
      if (code !== 0) {
        console.log(args[0] + ' command failed with code ' + code)
        console.log(out)
        process.exit(1)
      } else {
        resolve()
      }
    });

    subprocess.stderr.setEncoding('utf8');
    subprocess.stderr.on('data', (data) => {
      out += data.toString()
    })

    subprocess.stdout.setEncoding('utf8');
    subprocess.stdout.on('data', (data) => {
      out += data.toString()
    })

    subprocess.on('error', (err) => {
      console.log(args[0] + ' command failed with unexpected error')
      console.log(err.message)
      process.exit(1)
    })
  })
}

const Platform = {
  linux: 0,
  darwin: 1,
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
  framework: 'framework'
}

const getCommandLineOptions = () => {
  const commandLineArgSpec = [
    { name: 'platform', type: String, multiple: false, defaultValue: process.platform },
    { name: 'arch', type: String, multiple: false, defaultValue: process.arch },
    { name: 'profile', type: String, multiple: false, defaultValue: 'none' },
    { name: 'sdl-profile', type: String, multiple: false, defaultValue: 'system' },
    { name: 'framework-path', type: String, multiple: false, defaultValue: '/Library/Frameworks' },
    { name: 'node-binary-cache', type: String, multiple: false, defaultValue: '' },
    { name: 'crosstools-home', type: String, multiple: false, defaultValue: '' },
    { name: 'sdl-root', type: String, multiple: false, defaultValue: '' },
    { name: 'skip-build', type: Boolean, defaultValue: false },
    { name: 'compress-node-binary', type: Boolean, defaultValue: false },
    { name: 'strip-node-binary', type: Boolean, defaultValue: false },
    { name: 'minimal-node-install', type: Boolean, defaultValue: false }
  ]

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

  options.isArmCrossCompile = (process.platform === 'linux' && process.arch === 'x64'
      && options.platform === 'linux' && (options.arch === Architecture.armv7l))

  if (!options.isArmCrossCompile && (options.platform !== process.platform || options.arch !== process.arch)) {
    throw Error(`Host ${process.platform}-${process.arch} cannot compile to target ${options.platform}-${options.arch}`)
  }

  if (options.isArmCrossCompile && !pathExistsSync(options.crosstoolsHome)) {
    throw Error('--crosstool-home does not specify a directory')
  }

  if ((options.sdlProfile === SDLProfile.rpi || options.sdlProfile === SDLProfile.kmsdrm)
      && !pathExistsSync(options.sdlRoot)) {
    throw Error('--sdl-root does not specify a directory')
  }

  if (!options.nodeBinaryCache) {
    options.nodeBinaryCache = tmpdir()
  }

  if (!pathExistsSync(options.nodeBinaryCache)) {
    throw Error('--node-binary-cache does not exist')
  }

  if (options.sdlProfile === 'framework' && !pathExistsSync(options.frameworkPath)) {
    throw Error('--framework-path must specify a valid directory when --sdl-profile framework is enabled')
  }

  return options
}

const getPackageName = ({platform, arch, profile, sdlProfile}, lightSourceVersion) => {
  let postFix = ''

  if (platform === 'darwin') {
    platform = 'mac'
  } else if (profile === Profile.pi) {
    if (sdlProfile === SDLProfile.rpi) {
      postFix = "-rpi"
    } else if (sdlProfile === SDLProfile.kmsdrm) {
      postFix = "-kmsdrm"
    }
  } else if (profile === Profile.psclassic || profile === Profile.nclassic) {
    platform = profile
  }

  return `ls-node-v${lightSourceVersion}-${platform}-${arch}${postFix}`
}

class LightSourceBundle {
  constructor (srcRoot, options) {
    const lightSourceVersion = readPkg.sync({ cwd: join(srcRoot, 'packages', 'light-source') }).version
    const packageName = getPackageName(options, lightSourceVersion)
    const home = join(srcRoot, 'build', packageName)
    let nodeHome

    if (options.profile === Profile.nclassic || options.profile === Profile.psclassic) {
      nodeHome = join(home, 'usr', 'share', 'ls-node')
    } else {
      nodeHome = home
    }

    const nodeModules = join(nodeHome, 'lib', 'node_modules')

    this.staging = {
      home,
      nodeHome,
      nodeLib: join(nodeHome, 'lib'),
      nodeFrameworks: join(nodeHome, 'lib', 'Frameworks'),
      nodeLdLibraryPath: join(nodeHome, 'lib', 'so'),
      nodeBin: join(nodeHome, 'bin'),
      nodeModules,
      nodeLightSource: join(nodeModules, 'light-source'),
      nodeLightSourceAddon: join(nodeModules, 'light-source', 'Release'),
      nodeLightSourceReact: join(nodeModules, 'light-source-react'),
      nodeReact: join(nodeModules, 'react'),
    }

    this.options = options
    this.srcRoot = srcRoot
    this.nodeVersion = process.version
    this.nodeTag = `node-${this.nodeVersion}-${this.options.platform}-${this.options.arch}`

    if (this.options.isArmCrossCompile) {
      this.crosstoolsSysroot = `${this.options.crosstoolsHome}/x64-gcc-6.3.1/arm-rpi-linux-gnueabihf/arm-rpi-linux-gnueabihf/sysroot`
    }
  }

  async build() {
    await Promise.all([
      this.#compile(),
      this.#stagingFirstPass()
    ])
    
    await this.#stagingSecondPass()

    console.log(`package: creating package...`)

    if (this.options.profile === Profile.psclassic) {
      // TODO: this should be in deb package format, not tar.gz
      const mod = `${basename(this.staging.home)}.mod`

      console.log(`package: ${mod}`)

      await command('tar', ['-cvzf', `../${mod}`, '.'],
        {shell: true, cwd: this.staging.home})
    } else if (this.options.profile === Profile.nclassic) {
      // TODO: add install/uninstall script, bin link
      const hmod = `${basename(this.staging.home)}.hmod`

      console.log(`package: ${hmod}`)

      await command('tar', ['-cvzf', `../${hmod}`, '.'],
        {shell: true, cwd: this.staging.home})
    } else {
      const tar = `${basename(this.staging.home)}.tar.gz`

      console.log(`package: ${tar}`)

      await command('tar', ['-cvzf', tar, basename(this.staging.home)],
        {shell: true, cwd: join(this.srcRoot, 'build')})
    }

    console.log(`package: creating package... complete`)
  }

  async #stagingFirstPass () {
    const nodeHomeCache = join(this.options.nodeBinaryCache, this.nodeTag)

    if (!pathExistsSync(nodeHomeCache)) {
      const url = `https://nodejs.org/download/release/${this.nodeVersion}/${this.nodeTag}.tar.gz`

      console.log('staging: downloading node...')
      console.log(`staging: ${url}`)
      await command(`wget -qO- "${url}" | tar -C "${this.options.nodeBinaryCache}" -xzf -`, { shell: true })
      console.log('staging: downloading node... complete')
    } else {
      console.log(`Using node (${this.nodeTag}) from cache...`)
    }

    emptyDirSync(this.staging.home)
    ensureDirSync(this.staging.nodeHome)

    if (this.options.minimalNodeInstall) {
      console.log('staging: adding minimal node package...')
      ensureDirSync(this.staging.nodeLib)
      ensureDirSync(this.staging.nodeBin)
      ensureDirSync(this.staging.nodeModules)
      await copy(join(nodeHomeCache, 'bin', 'node'), join(this.staging.nodeBin, 'node'))
      console.log('staging: adding minimal node package... complete')
    } else {
      console.log('staging: adding full node package...')
      await copy(nodeHomeCache, this.staging.nodeHome)
      console.log('staging: adding full node package... complete')
    }

    const nodeBinaryPath = join(this.staging.nodeBin, '__node')

    console.log('staging: wrapping node binary...')
    await move(join(this.staging.nodeBin, 'node'), nodeBinaryPath)
    console.log('staging: wrapping node binary... complete')

    await copy(join(this.srcRoot, 'scripts', 'static', 'ls-node.sh'), join(this.staging.nodeBin, 'node'))

    if (this.options.platform === 'linux') {
      console.log('staging: patching node binary rpath...')
      ensureDirSync(join(this.staging.nodeLdLibraryPath))
      await command ('patchelf', [ '--set-rpath', '$ORIGIN/../lib/so', nodeBinaryPath ])
      console.log('staging: patching node binary rpath... complete')
    }

    if (this.options.stripNodeBinary) {
      console.log('staging: stripping node binary...')
      if (this.options.isArmCrossCompile) {
        const strip = join(
          this.options.crosstoolsHome, 'x64-gcc-6.3.1/arm-rpi-linux-gnueabihf/bin/arm-rpi-linux-gnueabihf-strip')
        await command (strip, [nodeBinaryPath])
      } else {
        await command ('strip', [ nodeBinaryPath ])
      }
      console.log('staging: stripping node binary... complete')
    }

    if (this.options.compressNodeBinary) {
      console.log('staging: compressing node binary...')
      await command ('upx', [ '-q', nodeBinaryPath ])
      console.log('staging: compressing node binary... complete')
    }

    ensureDirSync(this.staging.nodeLightSource)
    ensureDirSync(this.staging.nodeLightSourceReact)
    ensureDirSync(this.staging.nodeReact)

    if (this.options.sdlProfile === SDLProfile.framework) {
      const sdl2 = join(this.staging.nodeFrameworks, 'SDL2.framework')
      const sdl2Mixer = join(this.staging.nodeFrameworks, 'SDL2_mixer.framework')

      ensureDirSync(sdl2)
      ensureDirSync(sdl2Mixer)

      console.log('staging: adding Mac SDL Frameworks...')
      await copy(join(this.options.frameworkPath, 'SDL2.framework'), sdl2)
      await copy(join(this.options.frameworkPath, 'SDL2_mixer.framework'), sdl2Mixer)
      console.log('staging: adding Mac SDL Frameworks... complete')
    }

    if (this.options.isArmCrossCompile) {
      const cpp = 'libstdc++.so.6.0.22'

      console.log('staging: adding cpp 6.0.22 library...')
      await copy(join(this.crosstoolsSysroot, 'lib', cpp), join(this.staging.nodeLdLibraryPath, cpp))
      await createSymlink(cpp, join(this.staging.nodeLdLibraryPath, 'libstdc++.so.6'))
      console.log('staging: adding cpp 6.0.22 library... complete')
    }

    if (this.options.sdlProfile === SDLProfile.rpi || this.options.sdlProfile === SDLProfile.kmsdrm) {
      await copy(join(this.options.sdlRoot, 'lib', 'libSDL2-2.0.so.0'), 
        join(this.staging.nodeLdLibraryPath, 'libSDL2.so'))
    }
  }

  async #stagingSecondPass () {
    ensureDirSync(this.staging.nodeLightSourceAddon)
    ensureDirSync(this.staging.nodeLightSourceReact)
    ensureDirSync(this.staging.nodeReact)

    console.log('staging: adding LightSourceEngine files from source root...')

    const copies = await Promise.allSettled([
      copy(join(this.srcRoot, 'packages/light-source/build/Release/light-source.node'),
        join(this.staging.nodeLightSourceAddon, 'light-source.node')),
      copy(join(this.srcRoot, 'packages/light-source/dist/light-source.standalone.cjs'),
        join(this.staging.nodeLightSource, 'index.js')),
      copy(join(this.srcRoot, 'packages/light-source-react/dist/light-source-react.standalone.cjs'),
        join(this.staging.nodeLightSourceReact, 'index.js')),
      copy(join(this.srcRoot, 'packages/light-source-node/dist/react.standalone.cjs'),
        join(this.staging.nodeReact, 'index.js'))
    ])

    const firstRejected = copies.find(value => value.status === 'rejected')

    if (firstRejected) {
      throw firstRejected.reason
    }

    console.log('staging: adding LightSourceEngine files from source root... complete')
  }

  async #compile () {
    if (this.options.skipBuild) {
      console.log('compile: skipping, using files already in source root')
      return
    }

    let program
    let programArgs
    const env = {
      ...process.env,
      npm_config_ls_install_opts: '--jobs max',
      npm_config_ls_enable_native_tests: '0',
    }

    if (this.options.isArmCrossCompile) {
      const crossProfile = { armv6l: 'rpizero', armv7l: 'rpi' }[this.options.arch]

      if (crossProfile === undefined) {
        throw Error('cross: unsupported arch' + this.options.arch)
      }

      program = 'cross'
      programArgs = [crossProfile, 'yarn']

      // TODO: env.npm_config_ls_asmjit_build = 'arm'

      if (process.env.PATH) {
        env.PATH += delimiter
      } else {
        env.PATH = ''
      }
      env.PATH += join(this.options.crosstoolsHome, 'bin')
    } else {
      program = 'yarn'
      programArgs = []
    }

    console.log('compile: running yarn...')

    const compileInterval = setInterval(() => { console.log('compile: still working...') }, 5000)

    await command(program, [...programArgs, '--force'], { shell: true, cwd: this.srcRoot, env })

    clearInterval(compileInterval)
    console.log('compile: running yarn... complete')
  }
}

let cloptions

try {
  cloptions = getCommandLineOptions()
} catch (e) {
  console.log(e.message)
  process.exit(1)
}

const bundle = new LightSourceBundle(join(__dirname, '..'), cloptions)

bundle.build()
  .then(() => {
    console.log('done')
  })
  .catch((e) => {
    console.log(e.message)
    process.exit(1)
  })
