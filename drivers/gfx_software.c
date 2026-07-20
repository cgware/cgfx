#include "gfx_driver.h"

typedef struct gfx_software_s {
	gfx_target_t target;
	alloc_t alloc;
	u16 viewport_x;
	u16 viewport_y;
	u16 viewport_width;
	u16 viewport_height;
	u8 color[4];
} gfx_software_t;

static u8 color_u8(float value)
{
	if (value <= 0.0f) {
		return 0;
	}
	if (value >= 1.0f) {
		return 255;
	}
	return (u8)(value * 255.0f + 0.5f);
}

static int target_valid(const gfx_target_t *target)
{
	if (target == NULL || target->type != GFX_TARGET_MEMORY || target->format != GFX_FORMAT_RGBA8 || target->data == NULL ||
	    target->width == 0 || target->height == 0) {
		return 0;
	}

	return target->stride >= (size_t)target->width * 4;
}

static int gfx_software_init(gfx_t *gfx, const gfx_config_t *config)
{
	if (gfx == NULL || config == NULL || config->alloc.alloc == NULL) {
		return 1;
	}

	alloc_t alloc	       = config->alloc;
	gfx_software_t *render = alloc_alloc(&alloc, sizeof(gfx_software_t));
	if (render == NULL) {
		return 1;
	}
	*render = (gfx_software_t){
		.alloc = alloc,
		.color = {0, 0, 0, 255},
	};
	gfx->data = render;
	return 0;
}

static int gfx_software_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_software_t *render = gfx->data;
	alloc_free(&render->alloc, render, sizeof(*render));
	gfx->data = NULL;
	return 0;
}

static int gfx_software_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || gfx->data == NULL || target == NULL) {
		return 1;
	}

	gfx_software_t *render = gfx->data;
	if (target->type == GFX_TARGET_NONE) {
		render->target = (gfx_target_t){0};
		return 0;
	}
	if (!target_valid(target)) {
		return 1;
	}
	render->target = *target;
	return 0;
}

static int gfx_software_viewport(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_software_t *render	= gfx->data;
	render->viewport_x	= x;
	render->viewport_y	= y;
	render->viewport_width	= width;
	render->viewport_height = height;
	return 0;
}

static int gfx_software_clear_color(gfx_t *gfx, float r, float g, float b, float a)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_software_t *render = gfx->data;
	render->color[0]       = color_u8(r);
	render->color[1]       = color_u8(g);
	render->color[2]       = color_u8(b);
	render->color[3]       = color_u8(a);
	return 0;
}

static int gfx_software_clear(gfx_t *gfx, u32 buffers)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}
	if ((buffers & GFX_CLEAR_COLOR_BUFFER) == 0) {
		return 0;
	}

	gfx_software_t *render = gfx->data;
	if (!target_valid(&render->target)) {
		return 1;
	}

	for (u16 y = 0; y < render->target.height; y++) {
		u8 *row = (u8 *)render->target.data + (size_t)y * render->target.stride;
		for (u16 x = 0; x < render->target.width; x++) {
			u8 *pixel = row + (size_t)x * 4;
			pixel[0]  = render->color[0];
			pixel[1]  = render->color[1];
			pixel[2]  = render->color[2];
			pixel[3]  = render->color[3];
		}
	}

	return 0;
}

static gfx_driver_t gfx_software = {
	.name	     = "software",
	.api	     = GFX_API_SOFTWARE,
	.init	     = gfx_software_init,
	.free	     = gfx_software_free,
	.set_target  = gfx_software_set_target,
	.viewport    = gfx_software_viewport,
	.clear_color = gfx_software_clear_color,
	.clear	     = gfx_software_clear,
};

GFX_DRIVER(gfx_software, &gfx_software);
