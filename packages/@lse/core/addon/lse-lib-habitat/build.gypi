{
  "targets": [
    {
      "target_name": "lse-lib-habitat",
      "type": "static_library",
      "includes": [
        "../common.gypi",
      ],
      "include_dirs": [
        ".",
        "../lse-lib-logger",
        "../deps/parallel-hashmap/repo/parallel_hashmap",
      ],
      "dependencies": [
        "lse-lib-logger",
      ],
      "sources": [
        "lse/Habitat.cc",
      ]
    }
  ]
}
