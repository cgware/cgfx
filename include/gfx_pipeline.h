#ifndef GFX_PIPELINE_H
#define GFX_PIPELINE_H

#include "gfx.h"
#include "gfx_buffer.h"
#include "gfx_shader.h"

typedef struct gfx_pipeline_config_s {
	gfx_shader_t vs;
	gfx_shader_t fs;
} gfx_pipeline_config_t;

typedef struct gfx_pipeline_s {
	gfx_t *gfx;
	void *data;
} gfx_pipeline_t;

gfx_pipeline_t *gfx_pipeline_init(gfx_pipeline_t *pipeline, gfx_t *gfx, const gfx_pipeline_config_t *config);
void gfx_pipeline_free(gfx_pipeline_t *pipeline);

int gfx_draw_triangle_2d(const gfx_pipeline_t *pipeline, const gfx_buffer_t *vertex_buffer); // TODO: this should not exist

#endif
