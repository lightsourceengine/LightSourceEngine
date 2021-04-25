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
        "../../third-party/cpp-next/include",
        "../../third-party/utfcpp/repo/source",
        "../../third-party/concurrentqueue/repo",
        "../../third-party/filesystem/include",
        "../../third-party/parallel-hashmap/repo/parallel_hashmap",
      ],
      "dependencies": [
          "lse-lib-napi-ext",
          "lse-lib-logger",
      ],
      "sources": [
        "lse/Timer.cc",
        "lse/System.cc",
        "lse/fs-ext.cc",
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
          "../../third-party/cpp-next/include",
          "../../third-party/utfcpp/repo/source",
          "../../third-party/concurrentqueue/repo",
          "../../third-party/filesystem/include",
          "../../third-party/parallel-hashmap/repo/parallel_hashmap",
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
            "<!@(node -p \"require('napi-unit').include\")",
            "<!@(node -p \"require('node-addon-api').include\")",
          ],
          "dependencies": [
            "lse-lib-napi-ext",
            "lse-lib-logger",
            "lse-lib-util"
          ],
          "sources": [
            "test/FormatSpec.cc",
            "test/FsExtSpec.cc",
            "test/LRUCacheSpec.cc",
            "test/StringExtSpec.cc",
            "test/LightSourceUtilTestSuite.cc",
          ]
        }]
      }
    ]
  ]
}
