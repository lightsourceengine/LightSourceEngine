{
  "targets": [
    {
      "target_name": "lse-lib-habitat",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        "."
      ],
      "dependencies": [
        "lse-lib-logger",
      ],
      "sources": [
        "lse/Habitat.cc",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
        ],
      }
    }
  ]
}
