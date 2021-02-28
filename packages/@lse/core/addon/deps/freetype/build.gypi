{
  "targets": [
    {
      "target_name": "freetype",
      "type": "static_library",
      "includes": [
        "../../common.gypi",
      ],
      "include_dirs": [
        "include",
        "repo/include",
      ],
      "defines": [
        "FT2_BUILD_LIBRARY",
        "FT_CONFIG_OPTION_ERROR_STRINGS",
        # TODO: building with brotli is broken on windows
        # "FT_CONFIG_OPTION_USE_BROTLI",
      ],
      "sources": [
        # base components (required)
        "repo/src/base/ftsystem.c",
        "repo/src/base/ftinit.c",
        "repo/src/base/ftdebug.c",
        "repo/src/base/ftbase.c",
        "repo/src/base/ftbbox.c",
        "repo/src/base/ftglyph.c",
        # base components (optional)
        "repo/src/base/ftbdf.c",
        "repo/src/base/ftbitmap.c",
        "repo/src/base/ftcid.c",
        "repo/src/base/ftfstype.c",
        "repo/src/base/ftgasp.c",
        "repo/src/base/ftgxval.c",
        "repo/src/base/ftmm.c",
        "repo/src/base/ftotval.c",
        "repo/src/base/ftpatent.c",
        "repo/src/base/ftpfr.c",
        "repo/src/base/ftstroke.c",
        "repo/src/base/ftsynth.c",
        "repo/src/base/fttype1.c",
        "repo/src/base/ftwinfnt.c",
        # font loaders (optional)
        "repo/src/bdf/bdf.c",
        "repo/src/cff/cff.c",
        "repo/src/cid/type1cid.c",
        "repo/src/pcf/pcf.c",
        "repo/src/pfr/pfr.c",
        "repo/src/sfnt/sfnt.c",
        "repo/src/truetype/truetype.c",
        "repo/src/type1/type1.c",
        "repo/src/type42/type42.c",
        "repo/src/winfonts/winfnt.c",
        # rasterizers (optional)
        "repo/src/raster/raster.c",
        "repo/src/smooth/smooth.c",
        # auxiliary components (optional)
        "repo/src/autofit/autofit.c",
        "repo/src/cache/ftcache.c",
        "repo/src/gxvalid/gxvalid.c",
        "repo/src/otvalid/otvalid.c",
        "repo/src/psaux/psaux.c",
        "repo/src/pshinter/pshinter.c",
        "repo/src/psnames/psnames.c",
        "repo/src/gzip/ftgzip.c",
        'repo/src/lzw/ftlzw.c',
        'repo/src/bzip2/ftbzip2.c',
        "repo/src/sdf/sdf.c",
        # other
        "src/z_error.c",
      ],
      "conditions": [
        ["OS==\"mac\"", {
          "sources": [
            "repo/src/base/ftmac.c"
          ],
        }],
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
