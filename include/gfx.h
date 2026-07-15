#ifndef GFX_H
#define GFX_H

#include "proc.h"

#include <stddef.h>

enum {
	GFX_CLEAR_COLOR_BUFFER = 1u << 0,
};

typedef enum gfx_format_e {
	GFX_FORMAT_NONE,
	GFX_FORMAT_RGBA8,
} gfx_format_t;

typedef enum gfx_target_type_e {
	GFX_TARGET_NONE,
	GFX_TARGET_MEMORY,
} gfx_target_type_t;

typedef struct gfx_target_s {
	gfx_target_type_t type;
	gfx_format_t format;
	void *data;
	u16 width;
	u16 height;
	size_t stride;
} gfx_target_t;

typedef struct gfx_config_s {
	proc_t *proc;
	alloc_t alloc;
} gfx_config_t;

typedef struct gfx_s {
	const struct gfx_driver_s *drv;
	void *data;
} gfx_t;

gfx_t *gfx_init(gfx_t *gfx, const struct gfx_driver_s *drv, const gfx_config_t *config);
void gfx_free(gfx_t *gfx);
int gfx_proc(gfx_t *gfx, strv_t name, void **proc);
int gfx_set_target(gfx_t *gfx, const gfx_target_t *target);
int gfx_clear_color(gfx_t *gfx, float r, float g, float b, float a);
int gfx_clear(gfx_t *gfx, u32 buffers);

#endif
