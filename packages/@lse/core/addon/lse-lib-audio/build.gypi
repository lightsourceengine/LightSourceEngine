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
        "../lse-lib-util",
        "../lse-lib-habitat",
        "../deps/cpp17_headers/include",
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
        "lse-lib-util",
        "lse-lib-habitat"
      ],
      "sources": [
        "lse/AudioPlugin.cc",
        "lse/AudioPluginConfig.cc",
        "lse/AudioUtil.cc",
        "lse/bindings/AudioExports.cc",
        "lse/bindings/CAudioPlugin.cc",
        "lse/bindings/CAudioSource.cc",
        "lse/bindings/CAudioDestination.cc",
      ]
    },
  ]
}