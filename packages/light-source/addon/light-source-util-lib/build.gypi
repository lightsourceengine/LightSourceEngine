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
      ],
      "sources": [
        "ls/Format.cc",
        "ls/Timer.cc",
      ]
    }
  ],
  "conditions": [
    [
      "ls_with_tests==\"true\"", {
        "targets": [{
          "target_name": "light-source-util-lib-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
            ".",
            "../napi-ext-lib",
            "../logger-lib",
            "../cpp-compat",
            "../deps/filesystem/include",
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "dependencies": [
            "light-source-util-lib"
          ],
          "sources": [
            "test/FormatSpec.cc",
            "test/LightSourceUtilTestSuite.cc",
          ]
        }]
      }
    ]
  ]
}
