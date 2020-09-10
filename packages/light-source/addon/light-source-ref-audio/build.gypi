{
  "targets": [
    {
      "target_name": "light-source-ref-audio",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
        "../light-source-audio-lib",
        "../light-source-util-lib",
        "../deps/cpp17_headers/include",
      ],
      "dependencies": [
        "napi-ext-lib",
        "logger-lib",
        "light-source-audio-lib",
        "light-source-util-lib",
      ],
      "sources": [
        "ls/RefAudioPluginImpl.cc"
      ]
    }
  ]
}