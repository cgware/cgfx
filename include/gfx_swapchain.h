#ifndef GFX_SWAPCHAIN_H
#define GFX_SWAPCHAIN_H

#include "gfx_image.h"

typedef struct gfx_swapchain_config_s {
	gfx_format_t format;
	gfx_surface_t *surface;
	u16 width;
	u16 height;
	u32 min_image_count;
	u32 max_image_count;
	u32 usage;
	gfx_present_mode_t present_mode;
	gfx_image_t *images;
	u32 image_capacity;
} gfx_swapchain_config_t;

typedef struct gfx_swapchain_info_s {
	gfx_format_t format;
	u16 width;
	u16 height;
	u32 image_count;
	gfx_present_mode_t present_mode;
	gfx_present_mode_t actual_present_mode;
	u32 generation;
} gfx_swapchain_info_t;

typedef struct gfx_swapchain_image_s {
	gfx_image_t *image;
	u32 index;
	u32 generation;
} gfx_swapchain_image_t;

typedef struct gfx_swapchain_s {
	gfx_t *gfx;
	gfx_surface_t *surface;
	gfx_format_t format;
	u16 width;
	u16 height;
	u32 min_image_count;
	u32 max_image_count;
	u32 usage;
	gfx_present_mode_t present_mode;
	gfx_present_mode_t actual_present_mode;
	u32 generation;
	gfx_image_t *images;
	u32 image_count;
	u32 image_capacity;
	u32 acquired_index;
	int acquired;
	void *data;
} gfx_swapchain_t;

gfx_swapchain_t *gfx_swapchain_init(gfx_swapchain_t *swapchain, gfx_t *gfx, const gfx_swapchain_config_t *config);
void gfx_swapchain_free(gfx_swapchain_t *swapchain);
int gfx_swapchain_info(const gfx_swapchain_t *swapchain, gfx_swapchain_info_t *info);
int gfx_swapchain_images(gfx_swapchain_t *swapchain, gfx_image_t **images, u32 capacity, u32 *count);
int gfx_swapchain_acquire(gfx_swapchain_t *swapchain, gfx_swapchain_image_t *image);
int gfx_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height);
int gfx_swapchain_present(gfx_swapchain_t *swapchain, const gfx_swapchain_image_t *image);

#endif
