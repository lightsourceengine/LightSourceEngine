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
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
        "lse-lib-platform",
        "lse-lib-util",
        "lse-lib-habitat",
      ],
      "sources": [
        "lse/SDLRenderer.cc",
        "lse/SDLGraphicsContext.cc",
        "lse/SDLPlatformPlugin.cc",
        "lse/SDLUtil.cc",
        "lse/bindings/CSDLGraphicsContext.cc",
        "lse/bindings/SDLPlatformPluginExports.cc",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          ".",
        ]
      }
    }
  ]
}
