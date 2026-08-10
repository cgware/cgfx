#ifndef GFX_FRAMEBUFFER_H
#define GFX_FRAMEBUFFER_H

#include "gfx_image.h"
#include "gfx_render_pass.h"

typedef struct gfx_framebuffer_s {
	gfx_t *gfx;
	gfx_image_t *image;
	const gfx_render_pass_t *render_pass;
	void *data;
	u16 width;
	u16 height;
	u32 image_generation;
} gfx_framebuffer_t;

gfx_framebuffer_t *gfx_framebuffer_init(gfx_framebuffer_t *framebuffer, gfx_image_t *image, const gfx_render_pass_t *render_pass);
void gfx_framebuffer_free(gfx_framebuffer_t *framebuffer);
int gfx_framebuffer_resize(gfx_framebuffer_t *framebuffer, u16 width, u16 height);
int gfx_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame, const gfx_pass_config_t *config);

#endif
