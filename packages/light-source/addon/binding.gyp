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
      "target_name": "light-source-util",
      "type": "static_library",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "include",
        "deps/filesystem/include",
        "deps/fmt/repo/include",
      ],
      "dependencies": [
      ],
      "sources": [
        "light-source-util/BaseAudioSource.cc",
        "light-source-util/BaseAudioDestination.cc",
        "light-source-util/BaseAudioAdapter.cc",
        "light-source-util/PixelConversion.cc",
        "light-source-util/FileSystem.cc",
        "light-source-util/Timer.cc",
        "light-source-util/napi-ext.cc",
      ]
    },
    {
      "target_name": "light-source",
      "includes": [
        "common.gypi",
      ],
      "include_dirs": [
        "light-source",
        "deps/yoga/lib",
        "include",
        "deps/fmt/repo/include",
        "deps/stb/repo",
        "deps/nanosvg/include",
        "deps/utfcpp/repo/source",
        "light-source-util",
        "deps/concurrentqueue/repo",
        "deps/filesystem/include",
      ],
      "dependencies": [
        "addon/deps/yoga/yoga.gyp:*",
        "addon/deps/stb/stb.gyp:*",
        "addon/deps/nanosvg/nanosvg.gyp:*",
        "light-source-util",
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
        "include",
        "deps/fmt/repo/include",
        "light-source-util",
      ],
      "dependencies": [
        "light-source-util",
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
        "include",
        "deps/fmt/repo/include",
        "light-source-util",
      ],
      "dependencies": [
        "light-source-util",
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
              "include",
              "deps/fmt/repo/include",
              "light-source-util",
            ],
            "dependencies": [
                "light-source-util",
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
