{
  "targets": [
    {
      "target_name": "yoga",
      "type": "static_library",
      "includes": [
        "../../addon/common.gypi",
      ],
      "include_dirs": [
        "lib"
      ],
      "sources": [
        "lib/event/event.cpp",
        "lib/internal/experiments.cpp",
        "lib/log.cpp",
        "lib/Utils.cpp",
        "lib/YGConfig.cpp",
        "lib/YGEnums.cpp",
        "lib/YGLayout.cpp",
        "lib/YGNode.cpp",
        "lib/YGNodePrint.cpp",
        "lib/YGStyle.cpp",
        "lib/YGValue.cpp",
        "lib/Yoga.cpp",
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "lib"
        ],
      },
    }
  ]
}
