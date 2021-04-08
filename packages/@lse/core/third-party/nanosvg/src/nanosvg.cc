#include <cstdio>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>

#include "../include/nanoctx.h"

nctx nctx_new(int32_t width, int32_t height) {
  auto parser{nsvg__createParser()};

  if (!parser) {
    return {};
  }

  parser->image->width = width;
  parser->image->height = height;
  parser->dpi = 96;

  nsvg__pushAttr(parser);

  return parser;
}

void nctx_delete(nctx ctx) {
  if (ctx) {
    nsvg__deleteParser(static_cast<NSVGparser*>(ctx));
  }
}

void nctx_render(nctx ctx, uint8_t* data, int32_t width, int32_t height, int32_t stride) {
  if (ctx) {
    auto parser{static_cast<NSVGparser*>(ctx)};

//    nsvg__scaleToViewbox(parser, "px");

    if (static_cast<int32_t>(parser->image->width) != width
        || static_cast<int32_t>(parser->image->height) != height) {
      return;
    }

    auto rasterizer{nsvgCreateRasterizer()};

    nsvgRasterizeFull(rasterizer, parser->image, 0, 0, 1, 1, data, width, height, stride);

    nsvgDeleteRasterizer(rasterizer);
  }
}

void nctx_move_to(nctx ctx, float x, float y) {
  if (ctx) {
    nsvg__moveTo(static_cast<NSVGparser*>(ctx), x, y);
  }
}

void nctx_line_to(nctx ctx, float x, float y) {
  if (ctx) {
    nsvg__lineTo(static_cast<NSVGparser*>(ctx), x, y);
  }
}

void nctx_cubic_bezier_to(nctx ctx, float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) {
  if (ctx) {
    nsvg__cubicBezTo(static_cast<NSVGparser*>(ctx), cp1x, cp1y, cp2x, cp2y, x, y);
  }
}

void nctx_begin_path(nctx ctx) {
  if (ctx) {
    nsvg__resetPath(static_cast<NSVGparser*>(ctx));
  }
}

void nctx_close_path(nctx ctx) {
  if (ctx) {
    nsvg__addPath(static_cast<NSVGparser*>(ctx), 1);
  }
}

void nctx_fill(nctx ctx) {
  if (ctx) {
    auto parser{static_cast<NSVGparser*>(ctx)};

    if (nsvg__getAttr(parser)->hasFill == 0) {
      nsvg__getAttr(parser)->hasFill = 1;
    }

    nsvg__getAttr(parser)->hasStroke = 0;
    nsvg__addShape(parser);
  }
}

void nctx_stroke(nctx ctx) {
  if (ctx) {
    auto parser{static_cast<NSVGparser*>(ctx)};

    if (nsvg__getAttr(parser)->hasStroke == 0) {
      nsvg__getAttr(parser)->hasStroke = 1;
    }

    nsvg__getAttr(parser)->hasFill = 0;
    nsvg__addShape(parser);
  }
}

void nctx_set_fill_color(nctx ctx, uint32_t color) {
  if (ctx) {
    nsvg__getAttr(static_cast<NSVGparser*>(ctx))->fillColor = color;
  }
}

void nctx_set_fill_opacity(nctx ctx, float opacity) {
  if (ctx) {
    nsvg__getAttr(static_cast<NSVGparser*>(ctx))->fillOpacity = opacity;
  }
}

void nctx_set_stroke_opacity(nctx ctx, float opacity) {
  if (ctx) {
    nsvg__getAttr(static_cast<NSVGparser*>(ctx))->fillOpacity = opacity;
  }
}

void nctx_set_stroke_color(nctx ctx, uint32_t color) {
  if (ctx) {
    nsvg__getAttr(static_cast<NSVGparser*>(ctx))->strokeColor = color;
  }
}

void nctx_set_stroke_width(nctx ctx, uint32_t color) {
  if (ctx) {
    nsvg__getAttr(static_cast<NSVGparser*>(ctx))->strokeWidth = color;
  }
}
