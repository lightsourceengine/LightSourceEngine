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
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
        ],
      }
    },
    {
      "target_name": "lse-lib-logger-bindings",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
      ],
      "dependencies": [
        # TODO: remove dependency
        "lse-lib-napi-ext",
      ],
      "sources": [
        "lse/bindings/LoggerExports.cc",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
        ],
      }
    }
  ]
}
