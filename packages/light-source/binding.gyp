{
  "variables": {
    "ls_with_sdl_mixer%": "false",
    "ls_with_tests%": "false",
    "ls_framework_path%": "@none",
    "conditions": [
      [
        "OS==\"mac\"", {
          "ls_sdl_include%": "/usr/local/include",
          "ls_sdl_lib%": "/usr/local/lib",
          "ls_sdl_mixer_include%": "/usr/local/include",
          "ls_sdl_mixer_lib%": "/usr/local/lib"
        }
      ],
      [
        "OS==\"linux\"", {
          "ls_sdl_include%": "/usr/include",
          "ls_sdl_lib%": "/usr/lib/<!@(gcc -dumpmachine)",
          "ls_sdl_mixer_include%": "/usr/include",
          "ls_sdl_mixer_lib%": "/usr/lib/<!@(gcc -dumpmachine)",
        }
      ],
      [
        "OS==\"win\"", {
          "ls_sdl_include%": "<!@(echo %USERPROFILE%)\\SDL2\\include",
          "ls_sdl_lib%": "<!@(echo %USERPROFILE%)\\SDL2\\lib\\x64",
          "ls_sdl_mixer_include%": "<!@(echo %USERPROFILE%)\\SDL2_mixer\\include",
          "ls_sdl_mixer_lib%": "<!@(echo %USERPROFILE%)\\SDL2_mixer\\lib\\x64",
        }
      ]
    ]
  },
  "target_defaults": {
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")",
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

    # node modules
    "addon/light-source/build.gypi",
    "addon/light-source-ref/build.gypi",
    "addon/light-source-ref-audio/build.gypi",
    "addon/light-source-sdl/build.gypi",
    "addon/light-source-sdl-audio/build.gypi",
    "addon/light-source-sdl-mixer/build.gypi",
  ]
}
