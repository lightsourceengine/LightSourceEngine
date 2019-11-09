{
  "targets": [
    {
      "target_name": "light-source-sdl",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        "<(sdl_include_path)",
        "../napi-ext-lib",
        "../logger-lib",
        "../light-source-audio-lib",
        "../light-source-graphics-lib",
        "../light-source-stage-lib",
        "../light-source-util-lib",
        "../deps/cpp17_headers/include",
      ],
      "dependencies": [
        "napi-ext-lib",
        "light-source-graphics-lib",
        "light-source-audio-lib",
        "light-source-stage-lib",
        "light-source-util-lib",
        "logger-lib",
      ],
      "sources": [
        "ls/SDLAudioAdapter.cc",
        "ls/SDLRenderer.cc",
        "ls/SDLTexture.cc",
        "ls/SDLSceneAdapter.cc",
        "ls/SDLStageAdapter.cc",
        "ls/InputDevice.cc",
        "ls/SDLKeyboard.cc",
        "ls/SDLGamepad.cc",
        "ls/Init.cc",
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
    }
  ]
}