{
  "variables": {
    "with_sdl_mixer%": "false",
    "with_native_tests%": "false",
    "conditions": [
      [
        "OS==\"mac\"", {
          "sdl_include_path%": "/usr/local/include/SDL2",
          "sdl_library_path%": "/usr/local/lib",
          "sdl_mixer_include_path%": "/usr/local/include/SDL2",
          "sdl_mixer_library_path%": "/usr/local/lib",
        }
      ],
      [
        "OS==\"linux\"", {
          "sdl_include_path%": "/usr/include/SDL2",
          "sdl_library_path%": "/usr/lib/<!@(gcc -dumpmachine)",
          "sdl_mixer_include_path%": "/usr/include/SDL2",
          "sdl_mixer_library_path%": "/usr/lib/<!@(gcc -dumpmachine)",
        }
      ],
      [
        "OS==\"win\"", {
          "sdl_include_path%": "<!@(echo %USERPROFILE%)\\SDL2\\include",
          "sdl_library_path%": "<!@(echo %USERPROFILE%)\\SDL2\\lib\\x64",
          "sdl_mixer_include_path%": "<!@(echo %USERPROFILE%)\\SDL2_mixer\\include",
          "sdl_mixer_library_path%": "<!@(echo %USERPROFILE%)\\SDL2_mixer\\lib\\x64",
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
    "addon/deps/fmt/build.gypi",
    "addon/deps/nanosvg/build.gypi",
    "addon/deps/stb/build.gypi",
    "addon/deps/yoga/build.gypi",

    # static libraries
    "addon/napi-ext-lib/build.gypi",
    "addon/light-source-graphics-lib/build.gypi",
    "addon/light-source-audio-lib/build.gypi",
    "addon/light-source-stage-lib/build.gypi",
    "addon/light-source-util-lib/build.gypi",

    # node modules
    "addon/light-source/build.gypi",
    "addon/light-source-ref/build.gypi",
    "addon/light-source-sdl/build.gypi",
    "addon/light-source-sdl-mixer/build.gypi",
  ]
}
