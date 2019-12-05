{
  "variables": {
    "ls_with_sdl_mixer%": "false",
    "ls_with_tests%": "false",
    "conditions": [
      [
        "OS==\"mac\"", {
          "ls_sdl_include%": "/usr/local/include/SDL2",
          "ls_sdl_lib%": "/usr/local/lib",
          "ls_sdl_mixer_include%": "/usr/local/include/SDL2",
          "ls_sdl_mixer_lib%": "/usr/local/lib",
        }
      ],
      [
        "OS==\"linux\"", {
          "ls_sdl_include%": "/usr/include/SDL2",
          "ls_sdl_lib%": "/usr/lib/<!@(gcc -dumpmachine)",
          "ls_sdl_mixer_include%": "/usr/include/SDL2",
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
    "addon/deps/yoga/build.gypi",
    "addon/deps/ls-ctx/build.gypi",

    # static libraries
    "addon/napi-ext-lib/build.gypi",
    "addon/light-source-graphics-lib/build.gypi",
    "addon/light-source-audio-lib/build.gypi",
    "addon/light-source-stage-lib/build.gypi",
    "addon/light-source-util-lib/build.gypi",
    "addon/logger-lib/build.gypi",

    # node modules
    "addon/light-source/build.gypi",
    "addon/light-source-ref/build.gypi",
    "addon/light-source-sdl/build.gypi",
    "addon/light-source-sdl-mixer/build.gypi",
  ]
}
