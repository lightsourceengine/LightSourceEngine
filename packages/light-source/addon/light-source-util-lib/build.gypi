{
  "targets": [
    {
      "target_name": "light-source-util-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
        "../deps/filesystem/include",
        "../deps/cpp17_headers/include",
        "<(ls_sdl_include)",
        "<(ls_sdl_mixer_include)",
      ],
      "dependencies": [
          "napi-ext-lib",
          "logger-lib",
      ],
      "sources": [
        "ls/Uri.cc",
        "ls/Timer.cc",
        "ls/System.cc",
        "ls/string-ext.cc",
        "ls/SDL2.cc",
        "ls/SDL2_mixer.cc",
        "ls/internal/SharedLibrary.cc"
      ]
    }
  ],
  "conditions": [
    [
      "enable_native_tests==1", {
        "targets": [{
          "target_name": "light-source-util-lib-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
            ".",
            "../napi-ext-lib",
            "../logger-lib",
            "../deps/filesystem/include",
            "../deps/cpp17_headers/include",
            "<(ls_sdl_include)",
            "<(ls_sdl_mixer_include)",
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "dependencies": [
            "napi-ext-lib",
            "logger-lib",
            "light-source-util-lib"
          ],
          "sources": [
            "test/FormatSpec.cc",
            "test/StringExtSpec.cc",
            "test/UriSpec.cc",
            "test/LightSourceUtilTestSuite.cc",
          ]
        }]
      }
    ]
  ]
}
