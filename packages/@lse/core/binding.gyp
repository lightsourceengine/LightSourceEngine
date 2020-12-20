{
  "variables": {
    "ls_enable_plugin_platform_sdl%": "1",
    "ls_enable_plugin_platform_ref%": "1",
    "ls_enable_plugin_audio_sdl_audio%": "1",
    "ls_enable_plugin_audio_sdl_mixer%": "1",
    "ls_enable_plugin_audio_ref%": "1",
    "ls_sdl_include%": "addon/deps/SDL/repo/include",
    "ls_sdl_mixer_include%": "addon/deps/SDL_mixer/repo",
    # note: enable_native_tests variable is set in set-enable-native-tests.gypi
    "ls_enable_native_tests%": "0",
  },
  "target_defaults": {
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")",
    ],
    "conditions": [
      [ "ls_enable_plugin_platform_sdl==1", { "defines": [ "LS_ENABLE_PLUGIN_PLATFORM_SDL" ] } ],
      [ "ls_enable_plugin_platform_ref==1", { "defines": [ "LS_ENABLE_PLUGIN_PLATFORM_REF" ] } ],
      [ "ls_enable_plugin_audio_sdl_audio==1", { "defines": [ "LS_ENABLE_PLUGIN_AUDIO_SDL_AUDIO" ] } ],
      [ "ls_enable_plugin_audio_sdl_mixer==1", { "defines": [ "LS_ENABLE_PLUGIN_AUDIO_SDL_MIXER" ] } ],
      [ "ls_enable_plugin_audio_ref==1", { "defines": [ "LS_ENABLE_PLUGIN_AUDIO_REF" ] } ],
    ]
  },
  "includes": [
    # move setting enable_native_tests variable to another file so tests can be disabled in npm package builds
    "addon/set-enable-native-tests.gypi",

    # third party static libraries
    "addon/deps/nanosvg/build.gypi",
    "addon/deps/stb/build.gypi",
    "addon/deps/asmjit/build.gypi",
    "addon/deps/blend2d/build.gypi",
    "addon/deps/yoga/build.gypi",

    # static libraries
    "addon/lse-lib-napi-ext/build.gypi",
    "addon/lse-lib-audio/build.gypi",
    "addon/lse-lib-platform/build.gypi",
    "addon/lse-lib-util/build.gypi",
    "addon/lse-lib-logger/build.gypi",
    "addon/lse-plugin-ref/build.gypi",
    "addon/lse-plugin-ref-audio/build.gypi",
    "addon/lse-plugin-sdl/build.gypi",
    "addon/lse-plugin-sdl-audio/build.gypi",
    "addon/lse-plugin-sdl-mixer/build.gypi",

    # node modules
    "addon/lse-core/build.gypi"
  ]
}
