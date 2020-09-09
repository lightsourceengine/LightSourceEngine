{
  "targets": [
    {
      "target_name": "light-source-sdl-audio",
      "includes": [
        "../common.gypi",
        "../sdl.gypi"
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
        "../light-source-audio-lib",
        "../light-source-util-lib",
        "../deps/cpp17_headers/include",
        "../deps/SDL2/include"
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
      ]
    }
  ]
}