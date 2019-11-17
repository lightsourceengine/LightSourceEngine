{
  "cflags_cc!": [
    "-fno-exceptions",
    "-std=gnu++1y",
    "-fno-rtti"
  ],
  "cflags_cc+": [
    "-std=gnu++14",
    "-frtti"
  ],
  "defines": [
    "STX_NAMESPACE_NAME=std17",
  ],
  "xcode_settings": {
    "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
    "GCC_ENABLE_CPP_RTTI": "YES",
    "CLANG_CXX_LIBRARY": "libc++",
    "MACOSX_DEPLOYMENT_TARGET": "10.10",
    "CLANG_CXX_LANGUAGE_STANDARD": "gnu++14",
    "DEAD_CODE_STRIPPING": "YES",
  },
  "msvs_settings": {
    "VCCLCompilerTool": {
      "RuntimeTypeInfo": "true",
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
          "_USE_MATH_DEFINES"
        ]
      }
    ]
  ],  
}
