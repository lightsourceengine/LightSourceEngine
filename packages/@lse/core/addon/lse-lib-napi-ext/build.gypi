{
  "targets": [
    {
      "target_name": "lse-lib-napi-ext",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
      ],
      "sources": [
        "napi-ext.cc",
        "napix.cc",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
        ],
      }
    }
  ],
  "conditions": [
    [
      "enable_native_tests==1", {
        "targets": [{
          "target_name": "lse-lib-napi-ext-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "sources": [
            "test/NapiExtSpec.cc",
            "test/napixSpec.cc",
            "test/NapiExtTestSuite.cc",
          ],
          "dependencies": [
            "lse-lib-napi-ext",
          ],
        }]
      }
    ]
  ]
}
