#pragma once

#include <sf2d.h>

extern void batch_init(int size);
extern void batch_free();

// only call once per frame start (to reset indices)
extern void batch_reset();

extern void batch_start();

extern void batch_texture(sf2d_texture *texture, int x, int y);
extern void batch_texture_scale(sf2d_texture *texture, int x, int y, float x_scale, float y_scale);
extern void batch_texture_part(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h);
extern void batch_texture_part_scale(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale);
extern void batch_texture_blend(sf2d_texture *texture, int x, int y, u32 color);
extern void batch_texture_part_blend(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, u32 color);
extern void batch_texture_part_scale_blend(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale, u32 color);

extern void _batch_texture_part_scale_blend(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale, int blend, u32 color);

extern void batch_end();
extern void batch_flush();
