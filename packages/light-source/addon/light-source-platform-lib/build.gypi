{
  "targets": [
    {
      "target_name": "light-source-platform-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../light-source-util-lib",
        "../napi-ext-lib",
        "../logger-lib",
        "../deps/cpp17_headers/include"
      ],
      "dependencies": [
        "napi-ext-lib",
        "light-source-util-lib"
      ],
      "sources": [
        "ls/GraphicsContext.cc",
        "ls/PlatformPlugin.cc",
        "ls/CapabilitiesView.cc",
        "ls/PixelConversion.cc",
        "ls/Texture.cc",
        "ls/Rect.cc"
      ]
    }
  ],
  "conditions": [
    [
      "ls_with_tests==\"true\"", {
        "targets": [{
          "target_name": "light-source-platform-lib-test",
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
            "light-source-util-lib"
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