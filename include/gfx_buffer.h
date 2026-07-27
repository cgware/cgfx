#ifndef GFX_BUFFER_H
#define GFX_BUFFER_H

#include "gfx.h"

typedef enum gfx_buffer_type_s {
	GFX_BUFFER_UNKNOWN,
	GFX_BUFFER_VERTEX,
} gfx_buffer_type_t;

typedef struct gfx_buffer_config_s {
	gfx_buffer_type_t type;
} gfx_buffer_config_t;

typedef struct gfx_buffer_s {
	gfx_t *gfx;
	void *data;
} gfx_buffer_t;

gfx_buffer_t *gfx_buffer_init(gfx_buffer_t *buf, gfx_t *gfx, const gfx_buffer_config_t *config);
void gfx_buffer_free(gfx_buffer_t *buf);

int gfx_buffer_set_data(gfx_buffer_t *buf, const void *data, size_t size);

#endif
