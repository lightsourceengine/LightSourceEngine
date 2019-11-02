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
        "../light-source-stage-lib",
        "../deps/yoga/lib",
        "../deps/stb/repo",
        "../deps/nanosvg/include",
        "../deps/cpp17_headers/include",
        "../deps/utfcpp/repo/source",
        "../deps/concurrentqueue/repo",
        "../deps/ls-ctx/repo/include",
        "../deps/ls-ctx/repo/ctx",
        "../deps/filesystem/include",
      ],
      "dependencies": [
        "yoga",
        "stb_image",
        "stb_truetype",
        "nanosvg",
        "napi-ext-lib",
        "light-source-util-lib",
        "light-source-graphics-lib",
        "logger-lib",
        "logger-bindings-lib",
        "ls-ctx"
      ],
      "sources": [
        "yoga-ext.cc",
        "Executor.cc",
        "CompositeContext.cc",
        "Task.cc",
        "TaskQueue.cc",
        "Style.cc",
        "StyleEnumMappings.cc",
        "StyleEnums.cc",
        "StyleUtils.cc",
        "StyleValue.cc",
        "Font.cc",
        "FontId.cc",
        "FontStore.cc",
        "FontStoreView.cc",
        "ImageUri.cc",
        "ImageStore.cc",
        "ImageStoreView.cc",
        "Resource.cc",
        "ImageResource.cc",
        "FontResource.cc",
        "Layer.cc",
        "LayerCache.cc",
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
              "test/ExecutorSpec.cc",
              "test/TaskQueueSpec.cc",
              "test/StyleUtilsSpec.cc",
            ]
          }
        ]
      ],
    }
  ]
}