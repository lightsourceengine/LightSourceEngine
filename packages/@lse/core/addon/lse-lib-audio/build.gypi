{
  "targets": [
    {
      "target_name": "lse-lib-audio",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../lse-lib-napi-ext",
        "../lse-lib-logger",
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
      ],
      "sources": [
        "lse/AudioPlugin.cc",
        "lse/AudioSource.cc",
        "lse/AudioDestination.cc",
      ]
    },
  ]
}