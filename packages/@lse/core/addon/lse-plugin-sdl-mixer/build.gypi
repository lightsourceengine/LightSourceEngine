{
  "targets": [
    {
      "target_name": "lse-plugin-sdl-mixer",
      "type": "static_library",
      "includes": [
        "../common.gypi"
      ],
      "include_dirs": [
        "."
      ],
      "dependencies": [
          "lse-lib-napi-ext",
          "lse-lib-logger",
          "lse-lib-audio",
          "lse-lib-util",
          "lse-lib-habitat",
      ],
      "sources": [
        "lse/SDLMixerAudioPlugin.cc",
        "lse/SDLMixerAudioSource.cc",
        "lse/SDLMixerAudioDestination.cc",
        "lse/bindings/SDLMixerExports.cc",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
        ]
      }
    }
  ]
}
