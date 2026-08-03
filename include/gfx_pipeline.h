#ifndef GFX_PIPELINE_H
#define GFX_PIPELINE_H

#include "gfx_render_pass.h"
#include "gfx_shader.h"

typedef enum gfx_value_type_s {
	GFX_VALUE_UNKNOWN,
	GFX_VALUE_FLOAT32,
} gfx_value_type_t;

typedef struct gfx_layout_s {
	uint index;
	const char *semantic;
	uint semantic_index;
	uint count;
	gfx_value_type_t type;
} gfx_layout_t;

typedef struct gfx_pipeline_config_s {
	const gfx_render_pass_t *render_pass;
	gfx_shader_t vs;
	gfx_shader_t fs;
	const gfx_layout_t *input_layout;
	size_t input_layout_size;
} gfx_pipeline_config_t;

typedef struct gfx_pipeline_s {
	gfx_t *gfx;
	const gfx_render_pass_t *render_pass;
	void *data;
} gfx_pipeline_t;

gfx_pipeline_t *gfx_pipeline_init(gfx_pipeline_t *pipeline, gfx_t *gfx, const gfx_pipeline_config_t *config);
void gfx_pipeline_free(gfx_pipeline_t *pipeline);

int gfx_pipeline_bind(gfx_frame_t *frame, const gfx_pipeline_t *pipeline);

#endif
