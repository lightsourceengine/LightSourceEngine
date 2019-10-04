{
  "targets": [
    {
      "target_name": "stb_image",
      "type": "static_library",
      "includes": [
        "../../common.gypi",
      ],
      "include_dirs": [
        "repo"
      ],
      "sources": [
        "src/stb_image.cc",
      ],
      "cflags_cc+": [
        "-Wno-unused-function",
      ],
      "xcode_settings": {
        "OTHER_CPLUSPLUSFLAGS": [
            "-Wno-unused-function"
        ],
      },
    },
    {
      "target_name": "stb_truetype",
      "type": "static_library",
      "includes": [
        "../../common.gypi",
      ],
      "include_dirs": [
        "repo"
      ],
      "sources": [
        "src/stb_truetype.cc",
      ]
    }
  ]
}
