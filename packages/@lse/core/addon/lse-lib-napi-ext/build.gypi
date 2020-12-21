{
  "targets": [
    {
      "target_name": "lse-lib-napi-ext",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "sources": [
        "ClassBuilder.cc",
        "SafeObjectWrap.cc",
        "napi-ext.cc",
      ]
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
            ".",
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "sources": [
            "test/NapiExtSpec.cc",
            "test/ClassBuilderSpec.cc",
            "test/SafeObjectWrapSpec.cc",
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
