{
  "targets": [
    {
      "target_name": "napi-ext-lib",
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
      "ls_enable_native_tests==1", {
        "targets": [{
          "target_name": "napi-ext-lib-test",
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
            "napi-ext-lib",
          ],
        }]
      }
    ]
  ]
}
