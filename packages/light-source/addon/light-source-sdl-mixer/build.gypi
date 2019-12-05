{
  "conditions": [
    [
      "ls_with_sdl_mixer==\"true\"",
      {
        "targets": [
          {
            "target_name": "light-source-sdl-mixer",
            "includes": [
              "../common.gypi",
            ],
            "include_dirs": [
              "<(ls_sdl_mixer_include)",
              "<(ls_sdl_include)",
              ".",
              "../napi-ext-lib",
              "../logger-lib",
              "../light-source-audio-lib",
              "../light-source-util-lib",
              "../deps/cpp17_headers/include",
            ],
            "dependencies": [
                "napi-ext-lib",
                "logger-lib",
                "light-source-audio-lib",
                "light-source-util-lib",
            ],
            "sources": [
              "ls/SDLMixerAudioAdapter.cc",
              "ls/Init.cc",
            ],
            "conditions": [
              [
                "OS==\"mac\" or OS==\"linux\"", {
                  "libraries": [
                    "-L<(ls_sdl_lib)",
                    "-L<(ls_sdl_mixer_lib)",
                  ]
                }
              ]
            ],
            "msvs_settings": {
              "VCLinkerTool": {
                "AdditionalLibraryDirectories": [
                  "<(ls_sdl_mixer_lib)",
                  "<(ls_sdl_lib)",
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