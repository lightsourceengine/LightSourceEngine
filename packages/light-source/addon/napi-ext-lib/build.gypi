{
  "targets": [
    {
      "target_name": "napi-ext-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "sources": [
        "SafeObjectWrap.cc",
        "napi-ext.cc",
      ]
    }
  ],
  "conditions": [
    [
      "with_native_tests==\"true\"", {
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
