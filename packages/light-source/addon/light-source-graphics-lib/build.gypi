{
  "targets": [
    {
      "target_name": "light-source-graphics-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        "../napi-ext-lib",
        "../light-source-util-lib",
        "../logger",
        ".",
      ],
      "sources": [
        "ls/PixelConversion.cc",
        "ls/Surface.cc",
      ]
    }
  ],
  "conditions": [
    [
      "with_native_tests==\"true\"", {
        "targets": [{
          "target_name": "light-source-graphics-lib-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
            ".",
            "../napi-ext-lib",
            "../logger",
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "dependencies": [
            "light-source-graphics-lib",
            "light-source-util-lib"
          ],
          "sources": [
            "test/SurfaceSpec.cc",
            "test/LightSourceGraphicsTestSuite.cc",
          ]
        }]
      }
    ]
  ]
}
