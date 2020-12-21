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
        "<(ls_sdl_include)",
        "../deps/cpp17_headers/include",
        "../lse-lib-platform",
        "../lse-lib-util"
      ],
      "dependencies": [
        "lse-lib-napi-ext",
        "lse-lib-logger",
        "lse-lib-platform",
        "lse-lib-util"
      ],
      "sources": [
        "lse/SDLRenderer.cc",
        "lse/SDLGraphicsContextImpl.cc",
        "lse/SDLPlatformPluginImpl.cc",
        "lse/InputDevice.cc",
        "lse/SDLKeyboard.cc",
        "lse/SDLGamepad.cc",
        "lse/SDLUtil.cc"
      ]
    }
  ]
}
