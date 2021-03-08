{
  "cflags_cc!": [
    "-fno-exceptions",
    "-std=gnu++1y",
  ],
  "cflags_cc+": [
    "-std=gnu++14",
    "-fvisibility=hidden",
    "-fvisibility-inlines-hidden",
    "-ffunction-sections",
    "-fdata-sections",
    "-fno-semantic-interposition",
    "-fno-threadsafe-statics",
    "-fmerge-all-constants",
    "-fno-math-errno"
  ],
  "ldflags+": [
    "-fvisibility=hidden",
    "-fvisibility-inlines-hidden",
    "-ffunction-sections",
    "-fdata-sections",
    "-Wl,--gc-sections",
    "-flto"
  ],
  "defines": [
    "STX_NAMESPACE_NAME=std17",
    "YG_ENABLE_NODE_LAYOUT_EVENT=1",
  ],
  "xcode_settings": {
    "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
    "CLANG_CXX_LIBRARY": "libc++",
    "MACOSX_DEPLOYMENT_TARGET": "10.10",
    "CLANG_CXX_LANGUAGE_STANDARD": "gnu++14",
    "DEAD_CODE_STRIPPING": "YES",
    "GCC_THREADSAFE_STATICS": "NO",
    "OTHER_CFLAGS": [
      "-fvisibility=hidden",
      "-fvisibility-inlines-hidden",
      "-ffunction-sections",
      "-fdata-sections"
    ]
  },
  "msvs_settings": {
    "VCCLCompilerTool": {
      "ExceptionHandling": "1",
      "AdditionalOptions": [
        "/std:c++14"
      ]
    }
  },
  "conditions": [
    [
      "OS==\"win\"", {
        "defines": [
          "_WIN32",
          "_HAS_EXCEPTIONS=1",
          "NOMINMAX",
          "_USE_MATH_DEFINES",
          "GHC_WIN_DISABLE_WSTRING_STORAGE_TYPE"
        ]
      }
    ]
  ],  
}
