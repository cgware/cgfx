#ifndef GFX_BUFFER_H
#define GFX_BUFFER_H

#include "gfx.h"
#include "gfx_shader_compiler.h"

typedef enum gfx_buffer_type_s {
	GFX_BUFFER_UNKNOWN,
	GFX_BUFFER_VERTEX,
	GFX_BUFFER_INDEX,
	GFX_BUFFER_UNIFORM,
} gfx_buffer_type_t;

typedef enum gfx_buffer_usage_s {
	GFX_BUFFER_USAGE_UNKNOWN,
	GFX_BUFFER_USAGE_STATIC,
	GFX_BUFFER_USAGE_DYNAMIC,
} gfx_buffer_usage_t;

typedef struct gfx_buffer_config_s {
	gfx_buffer_type_t type;
	gfx_buffer_usage_t usage;
	size_t size;
	const void *data;
} gfx_buffer_config_t;

typedef struct gfx_buffer_s {
	gfx_t *gfx;
	gfx_buffer_type_t type;
	gfx_buffer_usage_t usage;
	size_t size;
	void *data;
} gfx_buffer_t;

typedef enum gfx_resource_type_s {
	GFX_RESOURCE_UNKNOWN,
	GFX_RESOURCE_UNIFORM_BUFFER,
} gfx_resource_type_t;

typedef struct gfx_resource_binding_s {
	u32 binding;
	gfx_resource_type_t type;
	const gfx_buffer_t *buffer;
} gfx_resource_binding_t;

gfx_buffer_t *gfx_buffer_init(gfx_buffer_t *buf, gfx_t *gfx, const gfx_buffer_config_t *config);
void gfx_buffer_free(gfx_buffer_t *buf);

int gfx_buffer_set_data(gfx_buffer_t *buf, const void *data, size_t size);

int gfx_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buf);
int gfx_bind_resources(gfx_frame_t *frame, const gfx_resource_binding_t *bindings, u32 binding_count);

#endif
