{
  "targets": [
    {
      "target_name": "light-source",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
        "../light-source-graphics-lib",
        "../light-source-util-lib",
        "../light-source-audio-lib",
        "../light-source-platform-lib",
        "../deps/yoga/lib",
        "../deps/asmjit/repo/src",
        "../deps/blend2d/repo/src",
        "../deps/stb/repo",
        "../deps/nanosvg/include",
        "../deps/cpp17_headers/include",
        "../deps/utfcpp/repo/source",
        "../deps/concurrentqueue/repo",
        "../deps/ls-ctx/repo/include",
        "../deps/filesystem/include",
      ],
      "dependencies": [
        "asmjit",
        "blend2d",
        "yoga",
        "stb_image",
        "nanosvg",
        "napi-ext-lib",
        "light-source-util-lib",
        "light-source-graphics-lib",
        "logger-lib",
        "logger-bindings-lib",
        "ls-ctx"
      ],
      "sources": [
        "ls/yoga-ext.cc",
        "ls/ThreadPool.cc",
        "ls/CompositeContext.cc",
        "ls/Style.cc",
        "ls/StyleEnums.cc",
        "ls/StyleUtils.cc",
        "ls/StyleValue.cc",
        "ls/Font.cc",
        "ls/Resource.cc",
        "ls/Resources.cc",
        "ls/ResourceProgress.cc",
        "ls/DecodeImage.cc",
        "ls/DecodeFont.cc",
        "ls/TextLayout.cc",
        "ls/SceneNode.cc",
        "ls/RootSceneNode.cc",
        "ls/BoxSceneNode.cc",
        "ls/ImageSceneNode.cc",
        "ls/TextSceneNode.cc",
        "ls/LinkSceneNode.cc",
        "ls/Scene.cc",
        "ls/Stage.cc",
        "ls/bindings/GlobalFunctions.cc",
        "ls/bindings/Init.cc",
      ],
      "conditions": [
        [
          "ls_with_tests==\"true\"", {
            "include_dirs": [
              "<!@(node -p \"require('napi-unit').include\")"
            ],
            "defines": [
              "LIGHT_SOURCE_NATIVE_TESTS"
            ],
            "sources": [
              "test/ThreadPoolSpec.cc",
              "test/StyleUtilsSpec.cc",
            ]
          }
        ]
      ],
    }
  ]
}