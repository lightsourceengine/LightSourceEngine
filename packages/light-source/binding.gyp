{
  "variables": {
    "ls_enable_plugin_platform_sdl%": "1",
    "ls_enable_plugin_platform_ref%": "1",
    "ls_enable_plugin_audio_sdl_audio%": "1",
    "ls_enable_plugin_audio_sdl_mixer%": "1",
    "ls_enable_plugin_audio_ref%": "1",
    "ls_enable_native_tests%": "0",
    "ls_sdl_include%": "addon/deps/SDL/repo/include",
    "ls_sdl_mixer_include%": "addon/deps/SDL_mixer/repo",
  },
  "target_defaults": {
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")",
    ],
    "conditions": [
      [ "ls_enable_native_tests==1", { "defines": [ "LS_ENABLE_NATIVE_TESTS" ] } ],
      [ "ls_enable_plugin_platform_sdl==1", { "defines": [ "LS_ENABLE_PLUGIN_PLATFORM_SDL" ] } ],
      [ "ls_enable_plugin_platform_ref==1", { "defines": [ "LS_ENABLE_PLUGIN_PLATFORM_REF" ] } ],
      [ "ls_enable_plugin_audio_sdl_audio==1", { "defines": [ "LS_ENABLE_PLUGIN_AUDIO_SDL_AUDIO" ] } ],
      [ "ls_enable_plugin_audio_sdl_mixer==1", { "defines": [ "LS_ENABLE_PLUGIN_AUDIO_SDL_MIXER" ] } ],
      [ "ls_enable_plugin_audio_ref==1", { "defines": [ "LS_ENABLE_PLUGIN_AUDIO_REF" ] } ],
    ]
  },
  "includes": [
    # third party static libraries
    "addon/deps/nanosvg/build.gypi",
    "addon/deps/stb/build.gypi",
    "addon/deps/asmjit/build.gypi",
    "addon/deps/blend2d/build.gypi",
    "addon/deps/yoga/build.gypi",

    # static libraries
    "addon/napi-ext-lib/build.gypi",
    "addon/light-source-audio-lib/build.gypi",
    "addon/light-source-platform-lib/build.gypi",
    "addon/light-source-util-lib/build.gypi",
    "addon/logger-lib/build.gypi",
    "addon/light-source-ref/build.gypi",
    "addon/light-source-ref-audio/build.gypi",
    "addon/light-source-sdl/build.gypi",
    "addon/light-source-sdl-audio/build.gypi",
    "addon/light-source-sdl-mixer/build.gypi",

    # node modules
    "addon/light-source/build.gypi"
  ]
}
