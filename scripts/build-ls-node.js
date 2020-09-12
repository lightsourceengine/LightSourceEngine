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
//       libSDL2.so
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
//     node (shell script with env configuration)
//     *.dll
//
// Frameworks - On mac, SDL2 and SDL2_mixer frameworks are bundled for the runtime. light-source-sdl.node has it's
//              @executable_path set to find this Frameworks directory.
// so         - On linux builds, this folder is the default value for LD_LIBRARY_PATH. This allows the packager
//              flexibility with how the SDL so is loaded on the target system. Also, SDL_mixer is NOT shipped, as
//              there are too many dependencies to manage. User's should install SDL_mixer from apt if streaming
//              audio formats are required.
// node       - This is a shell script that sets NODE_PATH, to find react, light-source and light-source-react
//              javascript packages, and LD_LIBRARY_PATH, to find SDL libraries. The packages are global or builtin
//              to the runtime or developer.
// Release    - Pre-compiled light-source addon are stored here. The light-source runtime can find them using the
//              $LS_ADDON_PATH environment variable.
// Windows    - On Windows, the SDL DLLs are in the bin directory, as that is in Windows default lib search path.
//

const { join, basename, extname } = require('path')
const { spawn } = require('child_process')
const { emptyDirSync, ensureDirSync, pathExistsSync, copy, move, createSymlink, createWriteStream, unlink, writeFile } = require('fs-extra')
const readPkg = require('read-pkg')
const { tmpdir } = require('os')
const commandLineArgs = require('command-line-args')
const fetch = require('node-fetch')
const tar = require('tar')
const AdmZip = require('adm-zip')

const command = async (...args) => {
  return new Promise((resolve) => {
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
  framework: 'framework',
  dll: 'dll',
}

const getCommandLineOptions = () => {
  const commandLineArgSpec = [
    { name: 'platform', type: String, multiple: false, defaultValue: process.platform },
    { name: 'arch', type: String, multiple: false, defaultValue: process.arch },
    { name: 'profile', type: String, multiple: false, defaultValue: 'none' },
    { name: 'sdl-profile', type: String, multiple: false, defaultValue: 'system' },
    { name: 'node-binary-cache', type: String, multiple: false, defaultValue: '' },
    { name: 'crosstools-home', type: String, multiple: false, defaultValue: '' },
    { name: 'sdl-runtime-pkg', type: String, multiple: false, defaultValue: '' },
    { name: 'sdl-mixer-runtime-pkg', type: String, multiple: false, defaultValue: '' },
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
    await this.#createPackage()
  }

  async #stagingFirstPass () {
    const nodeHomeCache = join(this.options.nodeBinaryCache, this.nodeTag)
    const exe = (program) => this.options.platform === 'win' ? program + '.exe' : program

    // Download node to cache.
    if (!pathExistsSync(nodeHomeCache)) {
      await this.#downloadNode()
    } else {
      console.log(`Using node (${this.nodeTag}) from cache...`)
    }

    // Clear staging directory.
    emptyDirSync(this.staging.home)
    ensureDirSync(this.staging.nodeHome)

    // Copy entire node install or a minimal install with just the node executable.
    if (this.options.minimalNodeInstall) {
      const nodeExecutable = exe('node')

      console.log('staging: adding minimal node package...')
      ensureDirSync(this.staging.nodeLib)
      ensureDirSync(this.staging.nodeBin)
      ensureDirSync(this.staging.nodeModules)
      await copy(join(nodeHomeCache, 'bin', nodeExecutable), join(this.staging.nodeBin, nodeExecutable))
      await copy(join(nodeHomeCache, 'LICENSE'), join(this.staging.nodeHome, 'LICENSE-node'))
      console.log('staging: adding minimal node package... complete')
    } else {
      console.log('staging: adding full node package...')
      await copy(nodeHomeCache, this.staging.nodeHome)
      console.log('staging: adding full node package... complete')
    }

    // Copy license.
    const lightSourceEngineLicense = 'LICENSE-LightSourceEngine'

    await copy(join(this.srcRoot, 'scripts', 'static', lightSourceEngineLicense),
      join(this.staging.nodeHome, lightSourceEngineLicense))

    // Rename node to __node. node will be invoked by a wrapped script.
    const nodeBinaryPath = join(this.staging.nodeBin, exe('__node'))

    console.log('staging: wrapping node binary...')
    await move(join(this.staging.nodeBin, exe('node')), nodeBinaryPath)
    console.log('staging: wrapping node binary... complete')

    // Copy wrapper script (this sets up the environment to find Light Source Engine built-ins)
    if (this.options.platform === 'win') {
      await copy(join(this.srcRoot, 'scripts', 'static', 'ls-node.cmd'), join(this.staging.nodeBin, 'node.cmd'))
    } else {
      await copy(join(this.srcRoot, 'scripts', 'static', 'ls-node.sh'), join(this.staging.nodeBin, 'node'))
    }

    // Set the node runpath.
    // TODO: set to pick up a bundled c++ lib, but not sure if this is necessary
    if (this.options.platform === 'linux') {
      console.log('staging: patching node binary rpath...')
      ensureDirSync(join(this.staging.nodeLdLibraryPath))
      await command ('patchelf', [ '--set-rpath', '$ORIGIN/../lib/so', nodeBinaryPath ])
      console.log('staging: patching node binary rpath... complete')
    }

    // Copy c++ lib from cross compiler.
    // TODO: some platforms have an earlier version of c++. seems to work, but more investigation is needed
    if (this.options.isArmCrossCompile) {
      const cpp = 'libstdc++.so.6.0.22'

      console.log('staging: adding cpp 6.0.22 library...')
      await copy(join(this.crosstoolsSysroot, 'lib', cpp), join(this.staging.nodeLdLibraryPath, cpp))
      await createSymlink(cpp, join(this.staging.nodeLdLibraryPath, 'libstdc++.so.6'))
      console.log('staging: adding cpp 6.0.22 library... complete')
    }

    // Strip symbols. Saves several megs on arm builds.
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

    // Compress node binary with upx. 66% size reduction on arm builds.
    if (this.options.compressNodeBinary) {
      console.log('staging: compressing node binary...')
      await command ('upx', [ '-q', nodeBinaryPath ])
      console.log('staging: compressing node binary... complete')
    }

    // Copy pre-compiled SDL binaries according to the selected SDL profile.
    if (this.options.sdlProfile === SDLProfile.rpi || this.options.sdlProfile === SDLProfile.kmsdrm) {
      // TODO: untar single file?
      await copy(join(this.options.sdlRuntimePkg, 'lib', 'libSDL2-2.0.so.0'),
        join(this.staging.nodeLdLibraryPath, 'libSDL2.so'))
    } else if (this.options.sdlProfile === SDLProfile.framework) {
      const sdl2 = join(this.staging.nodeFrameworks, 'SDL2.framework')
      const sdl2Mixer = join(this.staging.nodeFrameworks, 'SDL2_mixer.framework')

      ensureDirSync(sdl2)
      ensureDirSync(sdl2Mixer)

      console.log('staging: adding Mac SDL Frameworks...')
      await copy(join(this.options.sdlRuntimePkg), sdl2)
      await copy(join(this.options.sdlMixerRuntimePkg), sdl2Mixer)
      console.log('staging: adding Mac SDL Frameworks... complete')
    } else if (this.options.sdlProfile === SDLProfile.dll) {
      const processZipEntries = (entries, rename) => {
        const files = []

        for (const entry of entries.filter(e => e.endsWith('.dll') || e.endsWith('.txt'))) {
          const readAndWrite = new Promise((resolve, reject) => {
            entry.getDataAsync((buffer, err) => {
              err ? reject(Error(err)) : resolve(buffer)
            })
          }).then((buffer) => writeFile(join(this.options.nodeBin, rename(entry.entryName)), buffer))

          files.push(readAndWrite)
        }

        return files
      }

      const sdl = new AdmZip(this.options.sdlRuntimePkg)
      const sdlMixer = new AdmZip(this.options.sdlMixerRuntimePkg)

      await Promise.all([
        ...processZipEntries(sdl.getEntries(), (name) => name),
        ...processZipEntries(sdlMixer.getEntries(), (name) => name === 'README.txt' ? 'README-SDL_mixer.txt' : name)
      ])
    }
  }

  async #downloadNode () {
    const downloadArchiveFromNodeJs = async (outputStream, archiveExt) => {
      const url = `https://nodejs.org/download/release/${this.nodeVersion}/${this.nodeTag}${archiveExt}`

      console.log(`staging: ${url}`)

      const response = await fetch(url)

      response.body.pipe(outputStream)

      await new Promise((resolve, reject) => {
        outputStream.on('end', () => resolve())
        outputStream.on('error', reject)
      })
    }

    console.log('staging: downloading node...')

    if (this.options.platform === 'win') {
      // zip library does not support streams, so download to temp, extract and delete temp
      const tempFile = join(this.options.nodeBinaryCache, this.nodeTag + '.zip')

      await downloadArchiveFromNodeJs(createWriteStream(tempFile), extname(tempFile))

      await new Promise((resolve, reject) => {
        (new AdmZip(tempFile)).extractAllToAsync(
          this.options.nodeBinaryCache, false, (err) => {
            err ? reject(err) : resolve()
          })
      })

      await unlink(tempFile)
    } else {
      await downloadArchiveFromNodeJs(tar.x({ C: this.options.nodeBinaryCache }), '.tar.gz')
    }

    console.log('staging: downloading node... complete')
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
    if (this.options.isArmCrossCompile) {
      const crossProfile = { armv6l: 'rpizero', armv7l: 'rpi' }[this.options.arch]

      if (crossProfile === undefined) {
        throw Error('cross: unsupported arch' + this.options.arch)
      }

      program = join(this.options.crosstoolsHome, 'bin', 'cross')
      programArgs = [crossProfile, 'yarn']
    } else {
      program = 'yarn'
      programArgs = []
    }

    console.log('compile: running yarn...')

    const compileInterval = setInterval(() => { console.log('compile: still working...') }, 5000)

    try {
      await command(program, [...programArgs, '--force'], {shell: true, cwd: this.srcRoot, env})
    } finally {
      clearInterval(compileInterval)
    }

    console.log('compile: running yarn... complete')
  }

  async #createPackage () {
    console.log(`package: creating package...`)

    const packageName = basename(this.staging.home)
    const packagePathNoExt = this.staging.home

    if (this.options.platform === 'win') {
      const zip = new AdmZip(packagePathNoExt + '.zip')

      zip.addLocalFolder(this.staging.home, packageName)

      await new Promise((resolve, reject) => {
        zip.writeZip((err) => { err ? reject(err) : resolve() })
      })
    } else if (this.options.profile === Profile.psclassic) {
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
      await tar.create({
        file: packagePathNoExt + 'tar.gz',
        gzip: true,
        C: join(this.srcRoot, 'build')
      }, [packageName])
    }

    console.log(`package: creating package... complete`)
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
