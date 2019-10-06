{
  "targets": [
    {
      "target_name": "light-source-stage-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../light-source-graphics-lib",
        "../light-source-stage-lib",
        "../napi-ext-lib",
        "../logger",
      ],
      "sources": [
        "ls/BaseSceneAdapter.cc",
        "ls/BaseStageAdapter.cc",
      ]
    }
  ]
}