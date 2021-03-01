{
  "targets": [
    {
      "target_name": "lse-plugin-sdl-audio",
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
        "lse/SDLAudioPlugin.cc",
        "lse/SDLAudioSource.cc",
        "lse/SDLAudioDestination.cc",
        "lse/bindings/SDLAudioExports.cc",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
        ]
      }
    }
  ]
}
