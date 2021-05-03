#!/bin/sh

{{#if (isAbsolute share_home)}}
V_SHARE_HOME={{share_home}}
{{else}}
V_SHARE_HOME="$(cd "$(dirname "$0")/{{share_home}}" && pwd -P)"
{{/if}}

export LSE_ENV="runtime"
export LSE_PATH="${V_SHARE_HOME}/builtin"
export LSE_FONT_PATH="${V_SHARE_HOME}/assets"

{{#if with_game_controller_db}}
export LSE_GAME_CONTROLLER_DB="${LSE_GAME_CONTROLLER_DB:-"${V_SHARE_HOME}/assets/gamecontrollerdb.txt"}"
{{/if}}

{{#if (isMacOS platform)}}
export LSE_RUNTIME_FRAMEWORK_PATH="${LSE_RUNTIME_FRAMEWORK_PATH:-"${V_SHARE_HOME}/lib"}"
{{/if}}

{{#if (isLinux platform)}}
export LD_LIBRARY_PATH="${V_SHARE_HOME}/lib:${LD_LIBRARY_PATH}"
{{/if}}

{{#if (isNClassic platform_type)}}
export LSE_SDL_LIB_NAME="libSDL2.so"
{{/if}}

${V_SHARE_HOME}/node/{{node_version}}/bin/node --loader "${LSE_PATH}/@lse/loader/index.mjs" "$@"
