#ifndef GFX_FRAMEBUFFER_H
#define GFX_FRAMEBUFFER_H

#include "gfx_render_pass.h"
#include "gfx_target.h"

typedef struct gfx_framebuffer_s {
	gfx_t *gfx;
	gfx_target_t *target;
	const gfx_render_pass_t *render_pass;
	void *data;
	u16 width;
	u16 height;
} gfx_framebuffer_t;

gfx_framebuffer_t *gfx_framebuffer_init(gfx_framebuffer_t *framebuffer, gfx_target_t *target, const gfx_render_pass_t *render_pass);
void gfx_framebuffer_free(gfx_framebuffer_t *framebuffer);
int gfx_framebuffer_resize(gfx_framebuffer_t *framebuffer, u16 width, u16 height);
int gfx_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame, const gfx_pass_config_t *config);

#endif
