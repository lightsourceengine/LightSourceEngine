{
  "targets": [
    {
      "target_name": "lse-lib-platform",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../lse-lib-util",
        "../lse-lib-napi-ext",
        "../lse-lib-logger",
        "../deps/cpp17_headers/include"
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
        "lse-lib-util"
      ],
      "sources": [
        "lse/GraphicsContext.cc",
        "lse/PixelConversion.cc",
        "lse/Texture.cc",
        "lse/Rect.cc",
        "lse/bindings/JSGraphicsContext.cc"
      ]
    }
  ],
  "conditions": [
    [
      "enable_native_tests==1", {
        "targets": [{
          "target_name": "lse-lib-platform-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
            ".",
            "../lse-lib-napi-ext",
            "../lse-lib-util",
            "../lse-lib-logger",
            "../deps/cpp17_headers/include",
            "<!@(node -p \"require('napi-unit').include\")"
          ],
          "dependencies": [
            "lse-lib-napi-ext",
            "lse-lib-logger",
            "lse-lib-util",
            "lse-lib-platform"
          ],
          "sources": [
            "test/MatrixSpec.cc",
            "test/RectSpec.cc",
            "test/LightSourcePlatformTestSuite.cc",
          ]
        }]
      }
    ]
  ]
}