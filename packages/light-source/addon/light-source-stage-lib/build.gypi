{
  "targets": [
    {
      "target_name": "light-source-stage-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../light-source-graphics-lib",
        "../light-source-stage-lib",
        "../napi-ext-lib",
        "../logger-lib",
      ],
      "dependencies": [
        "napi-ext-lib",
        "light-source-graphics-lib",
      ],
      "sources": [
        "ls/PlatformPlugin.cc",
        "ls/CapabilitiesView.cc",
      ]
    }
  ]
}