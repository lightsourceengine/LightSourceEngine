{
  "targets": [
    {
      "target_name": "light-source-audio-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        "../light-source-audio-lib",
        "../napi-ext-lib",
        "../logger",
      ],
      "sources": [
        "ls/BaseAudioSource.cc",
        "ls/BaseAudioDestination.cc",
        "ls/BaseAudioAdapter.cc",
      ]
    },
  ]
}