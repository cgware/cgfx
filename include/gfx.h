#ifndef GFX_H
#define GFX_H

#include "proc.h"

enum {
	GFX_CLEAR_COLOR_BUFFER = 1u << 0,
};

typedef enum gfx_format_e {
	GFX_FORMAT_NONE,
	GFX_FORMAT_RGBA8_UNORM,
	GFX_FORMAT_BGRA8_UNORM,
	GFX_FORMAT_RGBA8_SRGB,
	GFX_FORMAT_BGRA8_SRGB,
	GFX_FORMAT_RGBA8 = GFX_FORMAT_RGBA8_UNORM,
} gfx_format_t;

typedef enum gfx_target_type_e {
	GFX_TARGET_NONE,
	GFX_TARGET_MEMORY,
	GFX_TARGET_SURFACE,
} gfx_target_type_t;

typedef enum gfx_api_e {
	GFX_API_NONE,
	GFX_API_SOFTWARE,
	GFX_API_OPENGL,
	GFX_API_VULKAN,
	GFX_API_D3D11,
} gfx_api_t;

typedef struct gfx_native_s {
	gfx_api_t api;
	u64 instance;
	u64 physical_device;
	u64 device;
} gfx_native_t;

typedef struct gfx_surface_s gfx_surface_t;

typedef struct gfx_surface_ops_s {
	int (*proc)(gfx_surface_t *surface, strv_t name, void **proc);
	int (*make_current)(gfx_surface_t *surface);
	int (*clear_current)(gfx_surface_t *surface);
	int (*present)(gfx_surface_t *surface);
} gfx_surface_ops_t;

struct gfx_surface_s {
	gfx_api_t api;
	u64 handle;
	void *data;
	const gfx_surface_ops_t *ops;
};

typedef struct gfx_plan_s {
	const char *const *instance_extensions;
	u32 instance_extension_count;
	const char *const *device_extensions;
	u32 device_extension_count;
} gfx_plan_t;

typedef struct gfx_target_s {
	gfx_target_type_t type;
	gfx_format_t format;
	void *data;
	gfx_surface_t *surface;
	u16 width;
	u16 height;
	size_t stride;
} gfx_target_t;

typedef struct gfx_config_s {
	proc_t *proc;
	alloc_t alloc;
	const gfx_plan_t *plan;
} gfx_config_t;

typedef struct gfx_s {
	const struct gfx_driver_s *drv;
	void *data;
} gfx_t;

gfx_t *gfx_init(gfx_t *gfx, const struct gfx_driver_s *drv, const gfx_config_t *config);
void gfx_free(gfx_t *gfx);

int gfx_native(gfx_t *gfx, gfx_native_t *native);
int gfx_proc(gfx_t *gfx, strv_t name, void **proc);
int gfx_set_target(gfx_t *gfx, const gfx_target_t *target);
int gfx_clear_color(gfx_t *gfx, float r, float g, float b, float a);
int gfx_clear(gfx_t *gfx, u32 buffers);
int gfx_present(gfx_t *gfx);

#endif
