{
  "targets": [
    {
      "target_name": "lse-plugin-ref",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../lse-lib-napi-ext",
        "../lse-lib-logger",
        "../lse-lib-platform",
        "../lse-lib-util",
        "../deps/cpp17_headers/include",
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
        "lse-lib-platform",
        "lse-lib-util"
      ],
      "sources": [
        "lse/RefRenderer.cc",
        "lse/RefGraphicsContextImpl.cc",
        "lse/RefPlatformPluginImpl.cc"
      ]
    }
  ]
}