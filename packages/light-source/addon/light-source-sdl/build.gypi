{
  "targets": [
    {
      "target_name": "light-source-sdl",
      "includes": [
        "../common.gypi",
        "../sdl.gypi"
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
        "../light-source-graphics-lib",
        "../light-source-platform-lib",
        "../light-source-util-lib",
        "../deps/cpp17_headers/include",
        "../deps/SDL2/include"
      ],
      "dependencies": [
        "napi-ext-lib",
        "light-source-graphics-lib",
        "light-source-platform-lib",
        "light-source-util-lib",
        "logger-lib",
      ],
      "sources": [
        "ls/SDLRenderer.cc",
        "ls/SDLGraphicsContextImpl.cc",
        "ls/SDLPlatformPluginImpl.cc",
        "ls/InputDevice.cc",
        "ls/SDLKeyboard.cc",
        "ls/SDLGamepad.cc",
        "ls/SDLUtil.cc",
        "ls/Init.cc",
      ]
    }
  ]
}
