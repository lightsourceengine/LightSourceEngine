/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

typedef void* nctx;

// Expose a Canvas Context2D like API.

nctx nctx_new(int32_t width, int32_t height);
void nctx_delete(nctx ctx);
void nctx_render(nctx ctx, uint8_t* data, int32_t width, int32_t height, int32_t stride);
void nctx_move_to(nctx ctx, float x, float y);
void nctx_line_to(nctx ctx, float x, float y);
void nctx_cubic_bezier_to(nctx ctx, float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);
void nctx_begin_path(nctx ctx);
void nctx_close_path(nctx ctx);
void nctx_fill(nctx ctx);
void nctx_stroke(nctx ctx);
void nctx_set_fill_color(nctx ctx, uint32_t color);
void nctx_set_fill_opacity(nctx ctx, float opacity);
void nctx_set_stroke_opacity(nctx ctx, float opacity);
void nctx_set_stroke_color(nctx ctx, uint32_t color);
void nctx_set_stroke_width(nctx ctx, uint32_t color);
