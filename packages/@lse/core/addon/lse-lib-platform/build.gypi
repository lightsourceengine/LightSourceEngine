{
  "targets": [
    {
      "target_name": "lse-lib-platform",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        "."
      ],
      "dependencies": [
        "lse-lib-logger",
        "lse-lib-util"
      ],
      "sources": [
        "lse/GraphicsContext.cc",
        "lse/PixelConversion.cc",
        "lse/Texture.cc",
        "lse/Rect.cc"
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
          "target_name": "lse-lib-platform-test",
          "includes": [
            "../common.gypi",
          ],
          "include_dirs": [
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