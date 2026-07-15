#ifndef GFX_DRIVER_H
#define GFX_DRIVER_H

#include "driver.h"
#include "gfx.h"

typedef struct gfx_driver_s {
	const char *name;
	int (*init)(gfx_t *gfx, const gfx_config_t *config);
	int (*free)(gfx_t *gfx);
	int (*proc)(gfx_t *gfx, strv_t name, void **proc);
	int (*set_target)(gfx_t *gfx, const gfx_target_t *target);
	int (*clear_color)(gfx_t *gfx, float r, float g, float b, float a);
	int (*clear)(gfx_t *gfx, u32 buffers);
} gfx_driver_t;

#define GFX_DRIVER_TYPE 0x474658

#define GFX_DRIVER(_name, _data) DRIVER(_name, GFX_DRIVER_TYPE, _data)

#endif
