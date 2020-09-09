{
  "conditions": [
  [
    "ls_with_sdl_mixer==\"true\"", {
      "targets": [
        {
          "target_name": "light-source-sdl-mixer",
          "includes": [
            "../common.gypi",
            "../sdl-mixer.gypi",
            "../sdl.gypi"
          ],
          "include_dirs": [
            ".",
            "../napi-ext-lib",
            "../logger-lib",
            "../light-source-audio-lib",
            "../light-source-util-lib",
            "../deps/cpp17_headers/include",
            "../deps/SDL2/include",
          ],
          "dependencies": [
              "napi-ext-lib",
              "logger-lib",
              "light-source-audio-lib",
              "light-source-util-lib",
          ],
          "sources": [
            "ls/SDLMixerAudioPluginImpl.cc",
            "ls/Init.cc",
          ]
        }
      ]
    }
  ]
  ]
}