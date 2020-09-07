{
  "conditions": [
    [
      "OS==\"linux\"", {
        "libraries": [
          "-L<(ls_sdl_lib)"
        ]
      }
    ],
    [
      "OS==\"mac\" and ls_framework_path!=\"@none\"", {
        "libraries": [
          "-framework SDL2"
        ]
      },
      {
        "include_dirs": [
          "<(ls_sdl_include)"
        ],
        "libraries": [
          "-lSDL2"
        ]
      }
    ]
  ],
  "msvs_settings": {
    "VCLinkerTool": {
      "AdditionalLibraryDirectories": [
        "<(ls_sdl_lib)"
      ]
    }
  }
}
