{
  "targets": [
    {
      "target_name": "lse-core",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
        "lse-lib-habitat",
        "lse-lib-platform",
        "lse-lib-audio",
        "lse-lib-util",
        "lse-lib-font",
        "lse-lib-logger-bindings",
        "lse-plugin-sdl",
        "lse-plugin-ref",
        "lse-plugin-sdl-audio",
        "lse-plugin-sdl-mixer",
        "freetype",
        "yoga",
        "stb_image",
        "nanosvg",
      ],
      "sources": [
        "lse/yoga-ext.cc",
        "lse/BoxSceneNode.cc",
        "lse/CompositeContext.cc",
        "lse/DecodeImage.cc",
        "lse/FTFontDriver.cc",
        "lse/Image.cc",
        "lse/ImageManager.cc",
        "lse/Paintable.cc",
        "lse/Scene.cc",
        "lse/SceneNode.cc",
        "lse/Stage.cc",
        "lse/Style.cc",
        "lse/StyleEnums.cc",
        "lse/StyleContext.cc",
        "lse/StyleProperty.cc",
        "lse/StyleValidator.cc",
        "lse/TextBlock.cc",
        "lse/ThreadPool.cc",
        "lse/RootSceneNode.cc",
        "lse/ImageSceneNode.cc",
        "lse/TextSceneNode.cc",
        "lse/bindings/CoreEnums.cc",
        "lse/bindings/CoreExports.cc",
        "lse/bindings/CoreFunctions.cc",
        "lse/bindings/CBoxSceneNode.cc",
        "lse/bindings/CImage.cc",
        "lse/bindings/CImageManager.cc",
        "lse/bindings/CImageSceneNode.cc",
        "lse/bindings/CRefGraphicsContext.cc",
        "lse/bindings/CRootSceneNode.cc",
        "lse/bindings/CScene.cc",
        "lse/bindings/CSceneNode.cc",
        "lse/bindings/CTextSceneNode.cc",
        "lse/bindings/CStage.cc",
        "lse/bindings/CStyle.cc",
        "lse/bindings/CStyleUtil.cc",
        "lse/bindings/Init.cc",
      ],
      "conditions": [
        [
          "enable_native_tests==1", {
            "include_dirs": [
              "<!@(node -p \"require('napi-unit').include\")"
            ],
            "defines": [ "LSE_ENABLE_NATIVE_TESTS" ],
            "sources": [
              "test/DecodeImageSpec.cc",
              "test/FTFontDriverSpec.cc",
              "test/ImageManagerSpec.cc",
              "test/ImageSpec.cc",
              "test/StyleContextSpec.cc",
              "test/StyleSpec.cc",
              "test/ThreadPoolSpec.cc",
            ]
          }
        ]
      ],
    }
  ]
}
