{
  "targets": [
    {
      "target_name": "lse-lib-font",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
        "lse-lib-util",
        "lse-lib-habitat",
      ],
      "sources": [
        "lse/Font.cc",
        "lse/FontManager.cc",
        "lse/bindings/FontEnums.cc",
        "lse/bindings/CFontManager.cc",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
        ]
      }
    }
  ],
  "conditions": [
    [
      "enable_native_tests==1", {
        "targets": [{
          "target_name": "lse-lib-font-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
            ".",
            "../lse-lib-napi-ext",
            "../lse-lib-util",
            "../lse-lib-habitat",
            "../lse-lib-logger",
            "../deps/cpp17_headers/include",
            "../deps/parallel-hashmap/repo/parallel_hashmap",
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "dependencies": [
            "lse-lib-napi-ext",
            "lse-lib-logger",
            "lse-lib-util",
            "lse-lib-habitat",
            "lse-lib-platform",
            "lse-lib-font",
          ],
          "sources": [
            "test/FontSpec.cc",
            "test/FontManagerSpec.cc",
            "test/LightSourceFontTestSuite.cc",
          ]
        }]
      }
    ]
  ]
}
