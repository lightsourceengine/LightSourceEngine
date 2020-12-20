{
  "targets": [
    {
      "target_name": "light-source-sdl",
      "type": "static_library",
      "includes": [
        "../common.gypi"
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
        "<(ls_sdl_include)",
        "../deps/cpp17_headers/include",
        "../light-source-platform-lib",
        "../light-source-util-lib"
      ],
      "dependencies": [
        "napi-ext-lib",
        "logger-lib",
        "light-source-platform-lib",
        "light-source-util-lib"
      ],
      "sources": [
        "ls/SDLRenderer.cc",
        "ls/SDLGraphicsContextImpl.cc",
        "ls/SDLPlatformPluginImpl.cc",
        "ls/InputDevice.cc",
        "ls/SDLKeyboard.cc",
        "ls/SDLGamepad.cc",
        "ls/SDLUtil.cc"
      ]
    }
  ]
}
