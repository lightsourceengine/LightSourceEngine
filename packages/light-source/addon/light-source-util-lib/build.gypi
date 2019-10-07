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
        "../logger",
        "../deps/filesystem/include",
        "../deps/fmt/repo/include",
      ],
      "sources": [
        "ls/FileSystem.cc",
        "ls/Timer.cc",
      ]
    }
  ],
  "conditions": [
    [
      "with_native_tests==\"true\"", {
        "targets": [{
          "target_name": "light-source-util-lib-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
            ".",
            "../napi-ext-lib",
            "../logger",
            "../deps/filesystem/include",
            "../deps/fmt/repo/include",
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "dependencies": [
            "light-source-util-lib"
          ],
          "sources": [
            "test/FileSystemSpec.cc",
            "test/LightSourceUtilTestSuite.cc",
          ]
        }]
      }
    ]
  ]
}
