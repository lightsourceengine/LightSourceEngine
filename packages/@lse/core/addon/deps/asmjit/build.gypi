{
  "targets": [
    {
      "target_name": "asmjit",
      "type": "static_library",
      "includes": [
        "../../common.gypi",
      ],
      "include_dirs": [
        "repo/asmjit/src"
      ],
	  "defines": [
	  "ASMJIT_STATIC=1"
	  ],
      "sources": [
        "repo/src/asmjit/core/func.cpp",
        "repo/src/asmjit/core/cpuinfo.cpp",
        "repo/src/asmjit/core/rapass.cpp",
        "repo/src/asmjit/core/errorhandler.cpp",
        "repo/src/asmjit/core/zonestack.cpp",
        "repo/src/asmjit/core/zone.cpp",
        "repo/src/asmjit/core/rastack.cpp",
        "repo/src/asmjit/core/inst.cpp",
        "repo/src/asmjit/core/environment.cpp",
        "repo/src/asmjit/core/codeholder.cpp",
        "repo/src/asmjit/core/jitruntime.cpp",
        "repo/src/asmjit/core/compiler.cpp",
        "repo/src/asmjit/core/emitter.cpp",
        "repo/src/asmjit/core/ralocal.cpp",
        "repo/src/asmjit/core/codewriter.cpp",
        "repo/src/asmjit/core/funcargscontext.cpp",
        "repo/src/asmjit/core/zonevector.cpp",
        "repo/src/asmjit/core/zonetree.cpp",
        "repo/src/asmjit/core/string.cpp",
        "repo/src/asmjit/core/archtraits.cpp",
        "repo/src/asmjit/core/globals.cpp",
        "repo/src/asmjit/core/assembler.cpp",
        "repo/src/asmjit/core/builder.cpp",
        "repo/src/asmjit/core/emithelper.cpp",
        "repo/src/asmjit/core/virtmem.cpp",
        "repo/src/asmjit/core/emitterutils.cpp",
        "repo/src/asmjit/core/target.cpp",
        "repo/src/asmjit/core/zonelist.cpp",
        "repo/src/asmjit/core/formatter.cpp",
        "repo/src/asmjit/core/zonehash.cpp",
        "repo/src/asmjit/core/logger.cpp",
        "repo/src/asmjit/core/support.cpp",
        "repo/src/asmjit/core/operand.cpp",
        "repo/src/asmjit/core/constpool.cpp",
        "repo/src/asmjit/core/jitallocator.cpp",
        "repo/src/asmjit/core/type.cpp",
        "repo/src/asmjit/core/osutils.cpp",
        "repo/src/asmjit/x86/x86emithelper.cpp",
        "repo/src/asmjit/x86/x86instdb.cpp",
        "repo/src/asmjit/x86/x86func.cpp",
        "repo/src/asmjit/x86/x86features.cpp",
        "repo/src/asmjit/x86/x86operand.cpp",
        "repo/src/asmjit/x86/x86formatter.cpp",
        "repo/src/asmjit/x86/x86builder.cpp",
        "repo/src/asmjit/x86/x86assembler.cpp",
        "repo/src/asmjit/x86/x86instapi.cpp",
        "repo/src/asmjit/x86/x86compiler.cpp",
        "repo/src/asmjit/x86/x86rapass.cpp",
      ],
      "cflags_cc+": [
        "-w",
      ],
      "xcode_settings": {
        "WARNING_CFLAGS": [
            "-w"
        ],
      },
      "msvs_settings": {
        "VCCLCompilerTool": {
            "WarningLevel": "0",
        },
      }
    }
  ]
}
