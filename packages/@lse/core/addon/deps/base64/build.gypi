{
  "targets": [
    {
      "target_name": "base64",
      "type": "static_library",
      "includes": [
        "../../common.gypi",
      ],
      "include_dirs": [
        "include"
      ],
      "sources": [
        "src/base64.cc",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "include",
        ],
      },
    }
  ]
}
