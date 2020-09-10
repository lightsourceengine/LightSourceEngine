{
  "targets": [
    {
      "target_name": "light-source-ref",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
        "../light-source-platform-lib",
        "../light-source-util-lib",
        "../deps/cpp17_headers/include",
      ],
      "dependencies": [
        "napi-ext-lib",
        "logger-lib",
        "light-source-platform-lib",
        "light-source-util-lib"
      ],
      "sources": [
        "ls/RefRenderer.cc",
        "ls/RefGraphicsContextImpl.cc",
        "ls/RefPlatformPluginImpl.cc"
      ]
    }
  ]
}