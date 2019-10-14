{
  "targets": [
    {
      "target_name": "light-source-ref",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger-lib",
        "../light-source-audio-lib",
        "../light-source-graphics-lib",
        "../light-source-stage-lib",
        "../deps/fmt/repo/include",
      ],
      "dependencies": [
        "fmt",
        "napi-ext-lib",
        "light-source-audio-lib",
        "light-source-stage-lib",
      ],
      "sources": [
        "RefRenderer.cc",
        "RefAudioAdapter.cc",
        "RefSceneAdapter.cc",
        "RefStageAdapter.cc",
        "Init.cc",
      ]
    }
  ]
}