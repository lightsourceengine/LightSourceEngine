{
  "targets": [
    {
      "target_name": "lse-lib-util",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../lse-lib-napi-ext",
        "../lse-lib-logger",
        "<(lse_sdl_include)",
        "<(lse_sdl_mixer_include)",
        "../deps/cpp17_headers/include",
        "../deps/utfcpp/repo/source",
        "../deps/concurrentqueue/repo",
        "../deps/filesystem/include",
        "../deps/parallel-hashmap/repo/parallel_hashmap",
      ],
      "dependencies": [
          "lse-lib-napi-ext",
          "lse-lib-logger",
      ],
      "sources": [
        "lse/Uri.cc",
        "lse/Timer.cc",
        "lse/System.cc",
        "lse/string-ext.cc",
        "lse/SDL2.cc",
        "lse/SDL2_mixer.cc",
        "lse/internal/SharedLibrary.cc"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
          "<(lse_sdl_include)",
          "<(lse_sdl_mixer_include)",
          "../deps/cpp17_headers/include",
          "../deps/utfcpp/repo/source",
          "../deps/concurrentqueue/repo",
          "../deps/filesystem/include",
          "../deps/parallel-hashmap/repo/parallel_hashmap",
        ]
      }
    }
  ],
  "conditions": [
    [
      "enable_native_tests==1", {
        "targets": [{
          "target_name": "lse-lib-util-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "dependencies": [
            "lse-lib-napi-ext",
            "lse-lib-logger",
            "lse-lib-util"
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
