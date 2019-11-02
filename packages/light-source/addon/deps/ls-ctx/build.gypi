{
  "targets": [
    {
      "target_name": "ls-ctx",
      "type": "static_library",
      "includes": [
        "../../common.gypi",
      ],
      "include_dirs": [
        "repo/include",
        "repo/ctx",
        "../stb/repo",
      ],
      "sources": [
        "repo/src/ls-ctx.c",
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
