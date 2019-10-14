{
  "conditions": [
    [
      "with_sdl_mixer==\"true\"",
      {
        "targets": [
          {
            "target_name": "light-source-sdl-mixer",
            "includes": [
              "../common.gypi",
            ],
            "include_dirs": [
              "<(sdl_mixer_include_path)",
              "<(sdl_include_path)",
              ".",
              "../napi-ext-lib",
              "../logger-lib",
              "../light-source-audio-lib",
              "../light-source-util-lib",
              "../deps/fmt/repo/include",
            ],
            "dependencies": [
                "fmt",
                "napi-ext-lib",
                "light-source-audio-lib",
                "light-source-util-lib",
            ],
            "sources": [
              "SDLMixerAudioAdapter.cc",
              "Init.cc",
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