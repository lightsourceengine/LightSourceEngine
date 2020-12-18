{
  "targets": [
    {
      "target_name": "logger-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
      ],
      "sources": [
        "ls/Log.cc",
      ]
    },
    {
      "target_name": "logger-bindings-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
      ],
      "dependencies": [
          "napi-ext-lib",
      ],
      "sources": [
        "ls/bindings/Logger.cc",
      ]
    }
  ]
}
