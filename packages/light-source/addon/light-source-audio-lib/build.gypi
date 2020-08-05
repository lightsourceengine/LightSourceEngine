{
  "targets": [
    {
      "target_name": "light-source-audio-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
      ],
      "sources": [
        "ls/AudioPlugin.cc",
        "ls/BaseAudioSource.cc",
        "ls/BaseAudioDestination.cc",
      ]
    },
  ]
}