{
  "targets": [
    {
      "target_name": "yoga",
      "type": "static_library",
      "includes": [
        "../../common.gypi",
      ],
      "include_dirs": [
        "include"
      ],
      "sources": [
        "lib/log.cpp",
        "lib/Utils.cpp",
        "lib/YGConfig.cpp",
        "lib/YGEnums.cpp",
        "lib/YGLayout.cpp",
        "lib/YGMarker.cpp",
        "lib/YGNode.cpp",
        "lib/YGNodePrint.cpp",
        "lib/YGStyle.cpp",
        "lib/YGValue.cpp",
        "lib/Yoga.cpp",
      ]
    },
  ]
}
