{
  "targets": [
    {
      "target_name": "lse-plugin-sdl-mixer",
      "type": "static_library",
      "includes": [
        "../common.gypi"
      ],
      "include_dirs": [
        ".",
        "../lse-lib-napi-ext",
        "../lse-lib-logger",
        "../lse-lib-audio",
        "../lse-lib-util",
        "../deps/cpp17_headers/include",
        "<(lse_sdl_include)",
        "<(lse_sdl_mixer_include)"
      ],
      "dependencies": [
          "lse-lib-napi-ext",
          "lse-lib-logger",
          "lse-lib-audio",
          "lse-lib-util",
      ],
      "sources": [
        "lse/SDLMixerAudioPluginImpl.cc"
      ]
    }
  ]
}