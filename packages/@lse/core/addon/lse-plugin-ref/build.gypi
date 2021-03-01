{
  "targets": [
    {
      "target_name": "lse-plugin-ref",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        "."
      ],
      "dependencies": [
        "lse-lib-logger",
        "lse-lib-platform",
        "lse-lib-util"
      ],
      "sources": [
        "lse/RefRenderer.cc",
        "lse/RefGraphicsContext.cc"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
        ]
      }
    }
  ]
}
