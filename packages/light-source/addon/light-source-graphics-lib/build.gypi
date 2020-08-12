{
  "targets": [
    {
      "target_name": "light-source-graphics-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../light-source-util-lib",
        "../logger-lib",
        "../deps/cpp17_headers/include",
      ],
      "dependencies": [
        "napi-ext-lib"
      ],
      "sources": [
        "ls/PixelConversion.cc",
        "ls/Surface.cc",
        "ls/Texture.cc",
        "ls/Rect.cc",
      ]
    }
  ],
  "conditions": [
    [
      "ls_with_tests==\"true\"", {
        "targets": [{
          "target_name": "light-source-graphics-lib-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
            ".",
            "../napi-ext-lib",
            "../light-source-util-lib",
            "../logger-lib",
            "../deps/cpp17_headers/include",
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "dependencies": [
            "light-source-graphics-lib",
            "light-source-util-lib"
          ],
          "sources": [
            "test/SurfaceSpec.cc",
            "test/MatrixSpec.cc",
            "test/RectSpec.cc",
            "test/LightSourceGraphicsTestSuite.cc",
          ]
        }]
      }
    ]
  ]
}
