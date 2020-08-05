{
  "targets": [
    {
      "target_name": "light-source-sdl-audio",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
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
        "ls/SDLAudioPluginImpl.cc",
        "ls/Init.cc",
      ],
      "conditions": [
        [
          "OS==\"mac\" or OS==\"linux\"", {
            "libraries": [
              "-L<(ls_sdl_lib)"
            ]
          }
        ]
      ],
      "msvs_settings": {
        "VCLinkerTool": {
          "AdditionalLibraryDirectories": [
            "<(ls_sdl_lib)",
          ]
        }
    },
    "libraries": [
      "-lSDL2"
    ]
  }
]
}