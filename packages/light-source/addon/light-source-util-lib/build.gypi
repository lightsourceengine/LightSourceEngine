{
  "targets": [
    {
      "target_name": "light-source-util-lib",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../napi-ext-lib",
        "../logger",
        "../deps/filesystem/include",
        "../deps/fmt/repo/include",
      ],
      "sources": [
        "ls/FileSystem.cc",
        "ls/Timer.cc",
      ]
    }
  ]
}