{
  "variables": {
    "ls_asmjit_build%": "host"
  },
  "cflags_cc!": [
    "-fno-exceptions",
    "-std=gnu++1y",
  ],
  "cflags_cc+": [
    "-std=gnu++14",
  ],
  "defines": [
    "STX_NAMESPACE_NAME=std17",
    "ASMJIT_STATIC=1",
    "BLEND2D_STATIC=TRUE",
    "YG_ENABLE_NODE_LAYOUT_EVENT=1",
  ],
  "xcode_settings": {
    "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
    "CLANG_CXX_LIBRARY": "libc++",
    "MACOSX_DEPLOYMENT_TARGET": "10.10",
    "CLANG_CXX_LANGUAGE_STANDARD": "gnu++14",
    "DEAD_CODE_STRIPPING": "YES",
    "LD_RUNPATH_SEARCH_PATHS": [
      # mac runpath for ls-node build
      "@executable_path/../lib/Frameworks",
    ],
    "FRAMEWORK_SEARCH_PATHS": [
      # this allows the mac build to find SDL2 and SDL2_mixer include files; however, the search path (-F) still
      # needs to be specified when linking frameworks
      "<(ls_framework_path)"
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
          "_USE_MATH_DEFINES"
        ]
      }
    ],
    [
      "ls_asmjit_build==\"arm\"", {
        "defines": [
          "ASMJIT_BUILD_ARM=1"
        ]
      },
      "ls_asmjit_build==\"x86\"", {
        "defines": [
          "ASMJIT_BUILD_X86=1"
        ]
      },
      {
        "defines": [
          "ASMJIT_BUILD_HOST=1"
        ]
      }
    ]
  ],  
}
