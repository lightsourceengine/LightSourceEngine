{
  "targets": [
    {
      "target_name": "nanosvg",
      "type": "static_library",
      "includes": [
        "../../addon/common.gypi",
      ],
      "include_dirs": [
        "include"
      ],
      "sources": [
        "src/nanosvg.cc",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "include",
        ],
      },
    }
  ]
}
