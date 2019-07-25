{
  "variables": {
    "with_sdl_mixer%": "false",
    "sdl_library_path%": "/usr/local/lib",
    "sdl_include_path%": "/usr/local/include/SDL2",
    "sdl_mixer_include_path%": "<(sdl_include_path)",
    "sdl_mixer_library_path%": "<(sdl_library_path)"
  },
  "target_defaults": {
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")",
    ],
  },
  "targets": [
    {
      "target_name": "light-source",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "deps/yoga/lib",
        "include",
        "deps/fmt/include",
      ],
      "dependencies": [
        "addon/deps/yoga/yoga.gyp:yoga",
        "addon/deps/fmt/fmt.gyp:fmt",
      ],
      "sources": [
        "light-source/Style.cc",
        "light-source/StyleEnumMappings.cc",
        "light-source/StyleEnums.cc",
        "light-source/StyleValue.cc",
        "light-source/ResourceManager.cc",
        "light-source/SceneNode.cc",
        "light-source/Init.cc",
      ]
    },
    {
      "target_name": "light-source-sdl",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "<(sdl_include_path)",
        "include",
        "deps/fmt/include",
      ],
      "dependencies": [
        "addon/deps/fmt/fmt.gyp:fmt",
      ],
      "sources": [
        "light-source-sdl/SDLRenderer.cc",
        "light-source-sdl/SDLSceneAdapter.cc",
        "light-source-sdl/SDLStageAdapter.cc",
        "light-source-sdl/SDLRenderer.cc",
        "light-source-sdl/InputDevice.cc",
        "light-source-sdl/SDLKeyboard.cc",
        "light-source-sdl/SDLGamepad.cc",
        "light-source-sdl/Init.cc",
      ],
      "libraries": [
        "-L<(sdl_library_path)",
        "-lSDL2"
      ]
    }
  ],
  "conditions": [
    [
      "with_sdl_mixer==\"true\"",
      {
        "targets": [
          {
            "target_name": "light-source-sdl-mixer",
            "includes": [
              "common.gypi",
            ],
            "include_dirs": [
              "<(sdl_include_path)",
              "<(sdl_mixer_include_path)",
            ],
            "dependencies": [

            ],
            "sources": [
              "light-source-sdl-mixer/Init.cc",
            ],
            "libraries": [
              "-L<(sdl_mixer_library_path)",
              "-lSDL2_mixer"
            ]
          }
        ]
      }
    ]
  ]
}
