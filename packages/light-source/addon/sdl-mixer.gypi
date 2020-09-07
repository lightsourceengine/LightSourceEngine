{
  "conditions": [
    [
      "OS==\"linux\"", {
        "libraries": [
          "-L<(ls_sdl_mixer_lib)"
        ]
      }
    ],
    [
      "OS==\"mac\" and ls_framework_path!=\"@none\"", {
        "defines": [
          "LS_USING_SDL2_MIXER_FRAMEWORK"
        ],
        "libraries": [
          "-framework SDL2_mixer"
        ]
      },
      {
        "include_dirs": [
          "<(ls_sdl_mixer_include)"
        ],
        "libraries": [
          "-lSDL2_mixer"
        ]
      }
    ]
  ],
  "msvs_settings": {
    "VCLinkerTool": {
      "AdditionalLibraryDirectories": [
        "<(ls_sdl_mixer_lib)"
      ]
    }
  }
}
