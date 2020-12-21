{
  "targets": [
    {
      "target_name": "lse-plugin-ref-audio",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../lse-lib-napi-ext",
        "../lse-lib-logger",
        "../lse-lib-audio",
        "../lse-lib-util",
        "../deps/cpp17_headers/include",
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
        "lse-lib-audio",
        "lse-lib-util",
      ],
      "sources": [
        "lse/RefAudioPluginImpl.cc"
      ]
    }
  ]
}