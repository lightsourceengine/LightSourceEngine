This document covers how to configure an environment to build Light Source Engine (Light Source Developer) or include 
light-source/light-source-react in your project (Light Source User). The npm packages do not support
node-pre-gyp right now, so a Light Source User will have to configure a full build environment.

## Requirements

- Linux or MacOS or Windows (1)
- NodeJS 12+
- node-gyp (2)
- npm or yarn (3)
- C++ 14 toolchain, such as gcc, clang or MSVC
- SDL 2.0.4+ development libraries
- Optional: SDL Mixer 2.0.0+ development libraries

1. On Windows with PowerShell, Light Source Engine is known to compile and run. Windows should be fine as a development environment, but it has not been fully vetted (YMMV).
2. node-gyp should be installed automatically with NodeJS.
3. yarn is required for Light Source Engine Developers. 

## Environment Variables

| Name                            | Description                    | Default Value                       |
|---------------------------------|--------------------------------|-------------------------------------|
| npm_config_ls_install_opts      | node-gyp options               | Empty                               |
| npm_config_ls_with_tests        | Build native unit tests        | false                               |
| npm_config_ls_with_sdl_mixer    | Build SDL2 Mixer audio adapter | false                               |
| npm_config_ls_sdl_include       | SDL2 native include path       | Default OS path (see Dependencies). |
| npm_config_ls_sdl_lib           | SDL2 native library path       | Default OS path (see Dependencies). |
| npm_config_ls_sdl_mixer_include | SDL2 Mixer native include path | Default OS path (see Dependencies). |
| npm_config_ls_sdl_mixer_lib     | SDL2 Mixer native library path | Default OS path (see Dependencies). |

## Dependencies

<details>
<summary>Linux (Debian/Ubuntu)</summary>
<p>
  
### Install

```
sudo apt-get install libsdl2-dev libsdl2-2.0-0
```
```
# codecs
sudo apt-get install libmikmod-dev libfishsound1-dev libsmpeg-dev liboggz2-dev libflac-dev libfluidsynth-dev
# SDL Mixer
sudo apt-get install libsdl2-mixer-dev libsdl2-mixer-2.0-0
```

### Environment Variable Defaults

| Environment Variable            | Value                        |
|---------------------------------|------------------------------|
| npm_config_ls_sdl_include       | /usr/include/SDL2            |
| npm_config_ls_sdl_lib           | /usr/lib/$(gcc -dumpmachine) |
| npm_config_ls_sdl_mixer_include | /usr/include/SDL2            |
| npm_config_ls_sdl_mixer_lib     | /usr/lib/$(gcc -dumpmachine) |
</p>
</details>

<details>
<summary>MacOS</summary>
<p>

### Install

```
brew install sdl2
```
```
brew install sdl2_mixer
```

### Environment Variable Defaults

| Environment Variable            | Value                        |
|---------------------------------|------------------------------|
| npm_config_ls_sdl_include       | /usr/local/include/SDL2      |
| npm_config_ls_sdl_lib           | /usr/local/lib               |
| npm_config_ls_sdl_mixer_include | /usr/local/include/SDL2      |
| npm_config_ls_sdl_mixer_lib     | /usr/local/lib               |
</p>
</details>

<details>
<summary>Windows</summary>
<p>

### Install

- [Download SDL2 MSVC development libraries](https://www.libsdl.org/download-2.0.php)
- Extract to user home.
- [Download SDL Mixer MSVC development libraries](https://www.libsdl.org/projects/SDL_mixer/)
- Extract to user home.

### Environment Variable Defaults

| Environment Variable            | Value                               |
|---------------------------------|-------------------------------------|
| npm_config_ls_sdl_include       | %USERPROFILE%\\SDL2\\include        |
| npm_config_ls_sdl_lib           | %USERPROFILE%\\SDL2\\lib\\x64       |
| npm_config_ls_sdl_mixer_include | %USERPROFILE%\\SDL2_mixer\\include  |
| npm_config_ls_sdl_mixer_lib     | %USERPROFILE%\\SDL2_mixer\\lib\\x64 |
</p>
</details>

## Typical Configurations

#### Light Source Engine User

```
export npm_config_ls_install_opts="--jobs max"
```

#### Light Source Engine Developer (Release)

```
export npm_config_ls_install_opts="--jobs max"
export npm_config_ls_with_unit_tests="false"
export npm_config_ls_with_sdl_mixer="true"
```
#### Light Source Engine Developer (Debug)

```
export npm_config_ls_install_opts="--jobs max --debug"
export npm_config_ls_with_unit_tests="true"
export npm_config_ls_with_sdl_mixer="true"
```

## Build

### Light Source Engine User

```
yarn add light-source-react
```
or
```
npm install light-source-react
```

### Light Source Engine Developer

```
git clone --recurse-submodules https://github.com/lightsourceengine/LightSourceEngine.git
cd LightSourceEngine
yarn
```
