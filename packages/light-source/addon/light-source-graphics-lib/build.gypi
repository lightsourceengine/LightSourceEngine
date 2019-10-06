{
  "targets": [
    {
      "target_name": "light-source-graphics-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        "../napi-ext-lib",
        "../logger",
        ".",
      ],
      "sources": [
        "ls/PixelConversion.cc",
      ]
    }
  ]
}
