#include "gfx_driver.h"

static int gfx_none_init(gfx_t *gfx, const gfx_config_t *config)
{
	(void)config;

	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_free(gfx_t *gfx)
{
	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || target == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_clear_color(gfx_t *gfx, float r, float g, float b, float a)
{
	(void)r;
	(void)g;
	(void)b;
	(void)a;

	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_viewport(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height)
{
	(void)x;
	(void)y;
	(void)width;
	(void)height;

	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_clear(gfx_t *gfx, u32 buffers)
{
	(void)buffers;

	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static gfx_driver_t gfx_none = {
	.name	     = "none",
	.api	     = GFX_API_NONE,
	.init	     = gfx_none_init,
	.free	     = gfx_none_free,
	.set_target  = gfx_none_set_target,
	.viewport    = gfx_none_viewport,
	.clear_color = gfx_none_clear_color,
	.clear	     = gfx_none_clear,
};

GFX_DRIVER(gfx_none, &gfx_none);
