{
  "targets": [
    {
      "target_name": "light-source-sdl-mixer",
      "type": "static_library",
      "includes": [
        "../common.gypi"
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
        "../light-source-audio-lib",
        "../light-source-util-lib",
        "../deps/cpp17_headers/include",
        "<(ls_sdl_include)",
        "<(ls_sdl_mixer_include)"
      ],
      "dependencies": [
          "napi-ext-lib",
          "logger-lib",
          "light-source-audio-lib",
          "light-source-util-lib",
      ],
      "sources": [
        "ls/SDLMixerAudioPluginImpl.cc"
      ]
    }
  ]
}
