{
  "targets": [
    {
      "target_name": "light-source-platform-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../light-source-graphics-lib",
        "../light-source-util-lib",
        "../napi-ext-lib",
        "../logger-lib",
        "../deps/cpp17_headers/include",
      ],
      "dependencies": [
        "napi-ext-lib",
        "light-source-util-lib",
        "light-source-graphics-lib",
      ],
      "sources": [
        "ls/GraphicsContext.cc",
        "ls/PlatformPlugin.cc",
        "ls/CapabilitiesView.cc",
      ]
    }
  ]
}