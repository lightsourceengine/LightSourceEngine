{
  "variables": {
    "with_sdl_mixer%": "false",
    "sdl_library_path%": "/usr/local/lib",
    "sdl_include_path%": "/usr/local/include/SDL2",
    "sdl_mixer_include_path%": "<(sdl_include_path)",
    "sdl_mixer_library_path%": "<(sdl_library_path)",
    "enable_native_tests%": "false",
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
        "deps/fmt/include",
      ],
      "dependencies": [
        "addon/deps/fmt/fmt.gyp:fmt",
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
        "deps/yoga/lib",
        "include",
        "deps/fmt/include",
        "deps/stb_image/include",
        "deps/nanosvg/include",
        "deps/stb_truetype/include",
        "deps/utf8_v2_3_4/",
        "light-source-util",
        "deps/concurrentqueue/include",
      ],
      "dependencies": [
        "addon/deps/yoga/yoga.gyp:yoga",
        "addon/deps/fmt/fmt.gyp:fmt",
        "addon/deps/stb_image/stb_image.gyp:stb_image",
        "addon/deps/nanosvg/nanosvg.gyp:nanosvg",
        "addon/deps/stb_truetype/stb_truetype.gyp:stb_truetype",
        "light-source-util",
      ],
      "sources": [
        "light-source/AsyncTaskQueue.cc",
        "light-source/Style.cc",
        "light-source/StyleEnumMappings.cc",
        "light-source/StyleEnums.cc",
        "light-source/StyleValue.cc",
        "light-source/Surface.cc",
        "light-source/Font.cc",
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
        "light-source/Init.cc",
      ],
      "conditions": [
        ["enable_native_tests==\"true\"", {
          "include_dirs": [
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "defines": ["LIGHT_SOURCE_ENABLE_NATIVE_TESTS"],
          "sources": [
            "light-source/test/FileSystemSpec.cc",
            "light-source/test/SurfaceSpec.cc",
          ]
        }]
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
        "deps/fmt/include",
        "light-source-util",
      ],
      "dependencies": [
        "addon/deps/fmt/fmt.gyp:fmt",
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
        ["OS==\"mac\" or OS==\"linux\"", {
          "libraries": [
            "-L<(sdl_library_path)"
          ]
        }],
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
        "deps/fmt/include",
        "light-source-util",
      ],
      "dependencies": [
        "addon/deps/fmt/fmt.gyp:fmt",
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
              "deps/fmt/include",
              "light-source-util",
            ],
            "dependencies": [
                "addon/deps/fmt/fmt.gyp:fmt",
                "light-source-util",
            ],
            "sources": [
              "light-source-sdl-mixer/SDLMixerAudioAdapter.cc",
              "light-source-sdl-mixer/Init.cc",
            ],
            "conditions": [
              ["OS==\"mac\" or OS==\"linux\"", {
                "libraries": [
                  "-L<(sdl_mixer_library_path)"
                ]
              }],
            ],
            "msvs_settings": {
              "VCLinkerTool": {
                "AdditionalLibraryDirectories": [
                  "<(sdl_mixer_library_path)"
              ]
            }
          },
            "libraries": [
              "-lSDL2_mixer",
            ]
          }
        ]
      }
    ]
  ]
}
