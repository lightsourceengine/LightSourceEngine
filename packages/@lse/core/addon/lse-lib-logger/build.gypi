{
  "targets": [
    {
      "target_name": "lse-lib-logger",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
      ],
      "sources": [
        "lse/Log.cc",
      ]
    },
    {
      "target_name": "lse-lib-logger-bindings",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../lse-lib-napi-ext",
      ],
      "dependencies": [
          "lse-lib-napi-ext",
      ],
      "sources": [
        "lse/bindings/Logger.cc",
      ]
    }
  ]
}
