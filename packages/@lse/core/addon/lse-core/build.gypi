{
  "targets": [
    {
      "target_name": "lse-core",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../lse-lib-napi-ext",
        "../lse-lib-logger",
        "../lse-lib-habitat",
        "../lse-lib-util",
        "../lse-lib-audio",
        "../lse-lib-platform",
        "../lse-lib-font",
        "../lse-plugin-sdl",
        "../lse-plugin-ref",
        "../lse-plugin-sdl-audio",
        "../lse-plugin-sdl-mixer",
        "../deps/yoga/lib",
        "../deps/asmjit/repo/src",
        "../deps/blend2d/repo/src",
        "../deps/stb/repo",
        "../deps/nanosvg/include",
        "../deps/cpp17_headers/include",
        "../deps/utfcpp/repo/source",
        "../deps/concurrentqueue/repo",
        "../deps/SDL/repo/include",
        "../deps/SDL_mixer/repo",
        "../deps/filesystem/include",
        "../deps/parallel-hashmap/repo/parallel_hashmap",
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
        "asmjit",
        "blend2d",
        "yoga",
        "stb_image",
        "nanosvg",
      ],
      "sources": [
        "lse/yoga-ext.cc",
        "lse/Blend2DFontDriver.cc",
        "lse/BoxSceneNode.cc",
        "lse/CompositeContext.cc",
        "lse/DecodeFont.cc",
        "lse/DecodeImage.cc",
        "lse/Paintable.cc",
        "lse/RenderingContext2D.cc",
        "lse/Resources.cc",
        "lse/ResourceProgress.cc",
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
        "lse/LinkSceneNode.cc",
        "lse/bindings/Convert.cc",
        "lse/bindings/CoreEnums.cc",
        "lse/bindings/CoreExports.cc",
        "lse/bindings/CoreFunctions.cc",
        "lse/bindings/CRefGraphicsContext.cc",
        "lse/bindings/CScene.cc",
        "lse/bindings/CStage.cc",
        "lse/bindings/Init.cc",
        "lse/bindings/JSStyle.cc",
        "lse/bindings/JSStyleClass.cc",
        "lse/bindings/JSStyleTransformSpec.cc",
        "lse/bindings/JSStyleValue.cc",
        "lse/bindings/StyleOps.cc",
      ],
      "conditions": [
        [
          "enable_native_tests==1", {
            "include_dirs": [
              "<!@(node -p \"require('napi-unit').include\")"
            ],
            "sources": [
              "test/FontFaceSpec.cc",
              "test/ImageSpec.cc",
              "test/ResourcesSpec.cc",
              "test/ThreadPoolSpec.cc",
              "test/StyleSpec.cc",
              "test/StyleContextSpec.cc",
            ]
          }
        ]
      ],
    }
  ]
}