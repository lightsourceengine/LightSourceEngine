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
        "../light-source-graphics-lib",
        "../light-source-stage-lib",
        "../light-source-util-lib",
        "../deps/cpp17_headers/include",
      ],
      "dependencies": [
        "napi-ext-lib",
        "light-source-stage-lib",
      ],
      "sources": [
        "ls/RefRenderer.cc",
        "ls/RefSceneAdapter.cc",
        "ls/RefStageAdapter.cc",
        "ls/Init.cc",
      ]
    }
  ]
}