{
  "targets": [
    {
      "target_name": "nanosvg",
      "type": "static_library",
      "includes": [
        "../../common.gypi",
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
