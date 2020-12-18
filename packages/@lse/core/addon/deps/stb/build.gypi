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
        "-w",
      ],
      "xcode_settings": {
        'WARNING_CFLAGS': [
            "-w"
        ],
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
            "WarningLevel": "0",
        },
      }
    }
  ]
}
