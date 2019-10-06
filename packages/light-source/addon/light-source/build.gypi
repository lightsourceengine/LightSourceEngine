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
        "../logger",
        "../light-source-graphics-lib",
        "../light-source-util-lib",
        "../light-source-audio-lib",
        "../light-source-stage-lib",
        "../deps/yoga/lib",
        "../deps/fmt/repo/include",
        "../deps/stb/repo",
        "../deps/nanosvg/include",
        "../deps/utfcpp/repo/source",
        "../deps/concurrentqueue/repo",
        "../deps/filesystem/include",
      ],
      "dependencies": [
        "yoga",
        "fmt",
        "stb_image",
        "stb_truetype",
        "nanosvg",
        "napi-ext-lib",
        "light-source-util-lib",
        "light-source-graphics-lib",
      ],
      "sources": [
        "yoga-ext.cc",
        "AsyncTaskQueue.cc",
        "Style.cc",
        "StyleEnumMappings.cc",
        "StyleEnums.cc",
        "StyleValue.cc",
        "Surface.cc",
        "Font.cc",
        "FontStore.cc",
        "FontStoreView.cc",
        "Resource.cc",
        "FontResource.cc",
        "ImageResource.cc",
        "LayerResource.cc",
        "ResourceManager.cc",
        "TextBlock.cc",
        "SceneNode.cc",
        "RootSceneNode.cc",
        "BoxSceneNode.cc",
        "ImageSceneNode.cc",
        "TextSceneNode.cc",
        "Scene.cc",
        "Stage.cc",
        "Init.cc",
      ],
      "conditions": [
        [
          "with_native_tests==\"true\"", {
            "include_dirs": [
              "<!@(node -p \"require('napi-unit').include\")"
            ],
            "defines": [
              "LIGHT_SOURCE_NATIVE_TESTS"
            ],
            "sources": [
              "test/FileSystemSpec.cc",
              "test/SurfaceSpec.cc",
              "test/AsyncTaskQueueSpec.cc",
            ]
          }
        ]
      ],
    }
  ]
}