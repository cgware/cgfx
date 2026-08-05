#ifndef GFX_SWAPCHAIN_H
#define GFX_SWAPCHAIN_H

#include "gfx.h"

typedef struct gfx_swapchain_config_s {
	gfx_format_t format;
	gfx_surface_t *surface;
	u16 width;
	u16 height;
	gfx_present_mode_t present_mode;
} gfx_swapchain_config_t;

typedef struct gfx_swapchain_s {
	gfx_t *gfx;
	gfx_surface_t *surface;
	gfx_format_t format;
	u16 width;
	u16 height;
	gfx_present_mode_t present_mode;
	gfx_present_mode_t actual_present_mode;
	u32 generation;
	void *data;
} gfx_swapchain_t;

gfx_swapchain_t *gfx_swapchain_init(gfx_swapchain_t *swapchain, gfx_t *gfx, const gfx_swapchain_config_t *config);
void gfx_swapchain_free(gfx_swapchain_t *swapchain);
int gfx_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height);
int gfx_swapchain_present(gfx_swapchain_t *swapchain);

#endif
