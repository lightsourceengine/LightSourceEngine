/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <cstdint>

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
