{
  "cflags_cc!": [
    "-fno-exceptions",
    "-std=gnu++1y",
  ],
  "cflags_cc+": [
    "-std=gnu++11",
  ],
  "xcode_settings": {
    "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
    "CLANG_CXX_LIBRARY": "libc++",
    "MACOSX_DEPLOYMENT_TARGET": "10.7",
    "CLANG_CXX_LANGUAGE_STANDARD": "gnu++11",
    "DEAD_CODE_STRIPPING": "YES",
  },
  "msvs_settings": {
    "VCCLCompilerTool": {
      "ExceptionHandling": 1
    }
  },
  "conditions": [
    [
      "OS==\"win\"", {
        "defines": [
          "_WIN32",
          "_HAS_EXCEPTIONS=1"
        ]
      }
    ],
    [
      "OS==\"linux\"", {
        "libraries": [
          "-Wl,--enable-new-dtags,-rpath,'$$ORIGIN/../lib'"
        ]
      }
    ]
  ],  
}
