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
    ],
  },
  "targets": [
    {
      "target_name": "napi-ext-lib",
      "type": "static_library",
      "includes": [
        "common.gypi",
      ],
      "sources": [
        "napi-ext-lib/napi-ext.cc",
      ]
    },
    {
      "target_name": "light-source-audio-lib",
      "type": "static_library",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "light-source-audio-lib",
        "napi-ext-lib",
        "logger",
      ],
      "sources": [
        "light-source-audio-lib/ls/BaseAudioSource.cc",
        "light-source-audio-lib/ls/BaseAudioDestination.cc",
        "light-source-audio-lib/ls/BaseAudioAdapter.cc",
      ]
    },
    {
      "target_name": "light-source-stage-lib",
      "type": "static_library",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "light-source-graphics-lib",
        "light-source-stage-lib",
        "napi-ext-lib",
        "logger",
      ],
      "sources": [
        "light-source-stage-lib/ls/BaseSceneAdapter.cc",
        "light-source-stage-lib/ls/BaseStageAdapter.cc",
      ]
    },
    {
      "target_name": "light-source-util-lib",
      "type": "static_library",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "napi-ext-lib",
        "logger",
        "deps/filesystem/include",
        "deps/fmt/repo/include",
      ],
      "sources": [
        "light-source-util-lib/ls/FileSystem.cc",
        "light-source-util-lib/ls/Timer.cc",
      ]
    },
    {
      "target_name": "light-source-graphics-lib",
      "type": "static_library",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "napi-ext-lib",
        "logger",
        "light-source-graphics-lib",
      ],
      "sources": [
        "light-source-graphics-lib/ls/PixelConversion.cc",
      ]
    },
    {
      "target_name": "light-source",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "light-source",
        "napi-ext-lib",
        "logger",
        "light-source-graphics-lib",
        "light-source-util-lib",
        "light-source-audio-lib",
        "light-source-stage-lib",
        "deps/yoga/lib",
        "deps/fmt/repo/include",
        "deps/stb/repo",
        "deps/nanosvg/include",
        "deps/utfcpp/repo/source",
        "deps/concurrentqueue/repo",
        "deps/filesystem/include",
      ],
      "dependencies": [
        "addon/deps/yoga/yoga.gyp:*",
        "addon/deps/fmt/fmt.gyp:*",
        "addon/deps/stb/stb.gyp:*",
        "addon/deps/nanosvg/nanosvg.gyp:*",
        "napi-ext-lib",
        "light-source-util-lib",
        "light-source-graphics-lib",
      ],
      "sources": [
        "light-source/yoga-ext.cc",
        "light-source/AsyncTaskQueue.cc",
        "light-source/Style.cc",
        "light-source/StyleEnumMappings.cc",
        "light-source/StyleEnums.cc",
        "light-source/StyleValue.cc",
        "light-source/Surface.cc",
        "light-source/Font.cc",
        "light-source/FontStore.cc",
        "light-source/FontStoreView.cc",
        "light-source/Resource.cc",
        "light-source/FontResource.cc",
        "light-source/ImageResource.cc",
        "light-source/LayerResource.cc",
        "light-source/ResourceManager.cc",
        "light-source/TextBlock.cc",
        "light-source/SceneNode.cc",
        "light-source/RootSceneNode.cc",
        "light-source/BoxSceneNode.cc",
        "light-source/ImageSceneNode.cc",
        "light-source/TextSceneNode.cc",
        "light-source/Scene.cc",
        "light-source/Stage.cc",
        "light-source/Init.cc",
      ],
      "conditions": [
        [
          "with_native_tests==\"true\"", {
            "include_dirs": [
              "<!@(node -p \"require('napi-unit').include\")"
            ],
            "defines": [
              "LIGHT_SOURCE_NATIVE_TESTS"
            ],
            "sources": [
              "light-source/test/FileSystemSpec.cc",
              "light-source/test/SurfaceSpec.cc",
              "light-source/test/AsyncTaskQueueSpec.cc",
            ]
          }
        ]
      ],
    },
    {
      "target_name": "light-source-sdl",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "<(sdl_include_path)",
        "napi-ext-lib",
        "logger",
        "light-source-audio-lib",
        "light-source-graphics-lib",
        "light-source-stage-lib",
        "light-source-util-lib",
        "deps/fmt/repo/include",
      ],
      "dependencies": [
        "addon/deps/fmt/fmt.gyp:*",
        "napi-ext-lib",
        "light-source-graphics-lib",
        "light-source-audio-lib",
        "light-source-stage-lib",
        "light-source-util-lib",
      ],
      "sources": [
        "light-source-sdl/SDLAudioAdapter.cc",
        "light-source-sdl/SDLRenderer.cc",
        "light-source-sdl/SDLSceneAdapter.cc",
        "light-source-sdl/SDLStageAdapter.cc",
        "light-source-sdl/InputDevice.cc",
        "light-source-sdl/SDLKeyboard.cc",
        "light-source-sdl/SDLGamepad.cc",
        "light-source-sdl/Init.cc",
      ],
      "conditions": [
        [
          "OS==\"mac\" or OS==\"linux\"", {
            "libraries": [
              "-L<(sdl_library_path)"
            ]
          }
        ]
      ],
      "msvs_settings": {
        "VCLinkerTool": {
          "AdditionalLibraryDirectories": [
            "<(sdl_library_path)"
          ]
        }
      },
      "libraries": [
        "-lSDL2"
      ]
    },
    {
      "target_name": "light-source-ref",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "napi-ext-lib",
        "logger",
        "light-source-audio-lib",
        "light-source-graphics-lib",
        "light-source-stage-lib",
        "deps/fmt/repo/include",
      ],
      "dependencies": [
        "napi-ext-lib",
        "light-source-audio-lib",
        "light-source-stage-lib",
        "addon/deps/fmt/fmt.gyp:*",
      ],
      "sources": [
        "light-source-ref/RefRenderer.cc",
        "light-source-ref/RefAudioAdapter.cc",
        "light-source-ref/RefSceneAdapter.cc",
        "light-source-ref/RefStageAdapter.cc",
        "light-source-ref/Init.cc",
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
              "<(sdl_mixer_include_path)",
              "<(sdl_include_path)",
              "napi-ext-lib",
              "logger",
              "light-source-audio-lib",
              "light-source-util-lib",
              "deps/fmt/repo/include",
            ],
            "dependencies": [
                "napi-ext-lib",
                "light-source-audio-lib",
                "light-source-util-lib",
                "addon/deps/fmt/fmt.gyp:*",
            ],
            "sources": [
              "light-source-sdl-mixer/SDLMixerAudioAdapter.cc",
              "light-source-sdl-mixer/Init.cc",
            ],
            "conditions": [
              [
                "OS==\"mac\" or OS==\"linux\"", {
                  "libraries": [
                    "-L<(sdl_library_path)",
                    "-L<(sdl_mixer_library_path)",
                  ]
                }
              ]
            ],
            "msvs_settings": {
              "VCLinkerTool": {
                "AdditionalLibraryDirectories": [
                  "<(sdl_mixer_library_path)",
                  "<(sdl_library_path)",
              ]
            }
          },
            "libraries": [
              "-lSDL2",
              "-lSDL2_mixer",
            ]
          }
        ]
      }
    ]
  ]
}
