#ifndef GFX_RENDER_PASS_H
#define GFX_RENDER_PASS_H

#include "gfx.h"

typedef struct gfx_render_pass_config_s {
	gfx_format_t color_format;
	gfx_format_t depth_format;
	gfx_load_op_t load;
	gfx_store_op_t store;
	gfx_load_op_t depth_load;
	gfx_store_op_t depth_store;
} gfx_render_pass_config_t;

typedef struct gfx_render_pass_s {
	gfx_t *gfx;
	gfx_format_t color_format;
	gfx_format_t depth_format;
	gfx_load_op_t load;
	gfx_store_op_t store;
	gfx_load_op_t depth_load;
	gfx_store_op_t depth_store;
	void *data;
} gfx_render_pass_t;

gfx_render_pass_t *gfx_render_pass_init(gfx_render_pass_t *render_pass, gfx_t *gfx, const gfx_render_pass_config_t *config);
void gfx_render_pass_free(gfx_render_pass_t *render_pass);

#endif
