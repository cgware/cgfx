#ifndef GFX_IMAGE_H
#define GFX_IMAGE_H

#include "gfx.h"

typedef enum gfx_image_usage_e {
	GFX_IMAGE_USAGE_COLOR_ATTACHMENT = 1u << 0,
	GFX_IMAGE_USAGE_TRANSFER_SRC	 = 1u << 1,
	GFX_IMAGE_USAGE_TRANSFER_DST	 = 1u << 2,
	GFX_IMAGE_USAGE_PRESENT		 = 1u << 3,
} gfx_image_usage_t;

typedef enum gfx_image_origin_e {
	GFX_IMAGE_ORIGIN_NONE,
	GFX_IMAGE_ORIGIN_MEMORY,
	GFX_IMAGE_ORIGIN_SURFACE,
} gfx_image_origin_t;

typedef struct gfx_image_memory_config_s {
	gfx_format_t format;
	void *data;
	u16 width;
	u16 height;
	size_t stride;
	u32 usage;
} gfx_image_memory_config_t;

typedef struct gfx_memory_readback_config_s {
	void *data;
	size_t stride;
} gfx_memory_readback_config_t;

typedef struct gfx_image_s {
	gfx_t *gfx;
	gfx_image_origin_t origin;
	gfx_format_t format;
	void *data;
	void *driver_data;
	struct gfx_swapchain_s *swapchain;
	u16 width;
	u16 height;
	size_t stride;
	u32 usage;
	u32 index;
	u32 generation;
} gfx_image_t;

gfx_image_t *gfx_image_init_memory(gfx_image_t *image, gfx_t *gfx, const gfx_image_memory_config_t *config);
void gfx_image_free(gfx_image_t *image);
int gfx_image_read(gfx_image_t *image, const gfx_memory_readback_config_t *config);

#endif
