#ifndef GFX_TARGET_H
#define GFX_TARGET_H

#include "gfx.h"

typedef enum gfx_target_type_e {
	GFX_TARGET_NONE,
	GFX_TARGET_MEMORY,
	GFX_TARGET_SURFACE,
} gfx_target_type_t;

typedef struct gfx_target_s {
	gfx_t *gfx;
	gfx_target_type_t type;
	gfx_format_t format;
	void *data;
	void *driver_data;
	gfx_surface_t *surface;
	u16 width;
	u16 height;
	size_t stride;
} gfx_target_t;

typedef struct gfx_memory_target_config_s {
	gfx_format_t format;
	void *data;
	u16 width;
	u16 height;
	size_t stride;
} gfx_memory_target_config_t;

typedef struct gfx_surface_target_config_s {
	gfx_format_t format;
	gfx_surface_t *surface;
	u16 width;
	u16 height;
} gfx_surface_target_config_t;

typedef struct gfx_memory_readback_config_s {
	void *data;
	size_t stride;
} gfx_memory_readback_config_t;

gfx_target_t *gfx_target_init_memory(gfx_target_t *target, gfx_t *gfx, const gfx_memory_target_config_t *config);
gfx_target_t *gfx_target_init_surface(gfx_target_t *target, gfx_t *gfx, const gfx_surface_target_config_t *config);
void gfx_target_free(gfx_target_t *target);

int gfx_target_resize(gfx_target_t *target, u16 width, u16 height);
int gfx_target_move(gfx_target_t *dst, gfx_target_t *src, gfx_t *gfx);
int gfx_target_read(gfx_target_t *target, const gfx_memory_readback_config_t *config);
int gfx_target_present(gfx_target_t *target);

#endif
