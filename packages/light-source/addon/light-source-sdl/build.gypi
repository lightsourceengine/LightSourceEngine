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
        "../deps/fmt/repo/include",
      ],
      "dependencies": [
        "fmt",
        "napi-ext-lib",
        "light-source-graphics-lib",
        "light-source-audio-lib",
        "light-source-stage-lib",
        "light-source-util-lib",
        "logger-lib",
      ],
      "sources": [
        "SDLAudioAdapter.cc",
        "SDLRenderer.cc",
        "SDLSceneAdapter.cc",
        "SDLStageAdapter.cc",
        "InputDevice.cc",
        "SDLKeyboard.cc",
        "SDLGamepad.cc",
        "Init.cc",
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