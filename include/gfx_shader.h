#ifndef GFX_SHADER_H
#define GFX_SHADER_H

#include "gfx.h"
#include "gfx_shader_compiler.h"

typedef struct gfx_shader_config_s {
	gfx_shader_compiler_t *compiler;
	strv_t source;
} gfx_shader_config_t;

typedef struct gfx_shader_s {
	gfx_t *gfx;
	void *data;
} gfx_shader_t;

gfx_shader_t *gfx_shader_init(gfx_shader_t *shader, gfx_t *gfx, const gfx_shader_config_t *config);
void gfx_shader_free(gfx_shader_t *shader);

int gfx_draw_triangle_2d(const gfx_shader_t *shader, const gfx_vertex_2d_t vertices[3]); // TODO: this should not exist

#endif
