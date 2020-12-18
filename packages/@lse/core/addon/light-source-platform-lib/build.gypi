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
        "logger-lib",
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
      "enable_native_tests==1", {
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
            "napi-ext-lib",
            "logger-lib",
            "light-source-util-lib",
            "light-source-platform-lib"
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