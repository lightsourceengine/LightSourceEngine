{
  "targets": [
    {
      "target_name": "lse-plugin-sdl",
      "type": "static_library",
      "includes": [
        "../common.gypi"
      ],
      "include_dirs": [
        ".",
        "../lse-lib-napi-ext",
        "../lse-lib-logger",
        "<(lse_sdl_include)",
        "../deps/cpp17_headers/include",
        "../lse-lib-platform",
        "../lse-lib-util",
        "../deps/parallel-hashmap/repo/parallel_hashmap",
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
        "lse-lib-platform",
        "lse-lib-util"
      ],
      "sources": [
        "lse/SDLRenderer.cc",
        "lse/SDLGraphicsContext.cc",
        "lse/SDLPlatformPlugin.cc",
        "lse/SDLUtil.cc",
        "lse/bindings/SDLPlatformPluginExports.cc"
      ]
    }
  ]
}
