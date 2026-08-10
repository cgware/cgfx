#ifndef GFX_H
#define GFX_H

#include "proc.h"

typedef enum gfx_format_e {
	GFX_FORMAT_NONE,
	GFX_FORMAT_RGBA8_UNORM,
	GFX_FORMAT_BGRA8_UNORM,
	GFX_FORMAT_RGBA8_SRGB,
	GFX_FORMAT_BGRA8_SRGB,
	GFX_FORMAT_RGBA8 = GFX_FORMAT_RGBA8_UNORM,
} gfx_format_t;

typedef enum gfx_load_op_e {
	GFX_LOAD_DONT_CARE,
	GFX_LOAD_CLEAR,
	GFX_LOAD_LOAD,
} gfx_load_op_t;

typedef enum gfx_store_op_e {
	GFX_STORE_DONT_CARE,
	GFX_STORE_STORE,
} gfx_store_op_t;

typedef enum gfx_api_e {
	GFX_API_NONE,
	GFX_API_SOFTWARE,
	GFX_API_OPENGL,
	GFX_API_VULKAN,
	GFX_API_D3D11,
} gfx_api_t;

typedef enum gfx_present_mode_e {
	GFX_PRESENT_MODE_DEFAULT,
	GFX_PRESENT_MODE_VSYNC,
	GFX_PRESENT_MODE_IMMEDIATE,
	GFX_PRESENT_MODE_MAILBOX,
} gfx_present_mode_t;

typedef struct gfx_frame_s gfx_frame_t;
typedef struct gfx_s gfx_t;
typedef struct gfx_native_s {
	gfx_api_t api;
	u64 instance;
	u64 physical_device;
	u64 device;
} gfx_native_t;

typedef struct gfx_surface_s gfx_surface_t;

typedef struct gfx_surface_memory_s {
	gfx_format_t format;
	void *data;
	u16 width;
	u16 height;
	size_t stride;
} gfx_surface_memory_t;

typedef struct gfx_surface_config_s {
	gfx_format_t format;
	u16 width;
	u16 height;
	u32 image_count;
	gfx_present_mode_t present_mode;
} gfx_surface_config_t;

typedef struct gfx_surface_ops_s {
	int (*proc)(gfx_surface_t *surface, strv_t name, void **proc);
	int (*make_current)(gfx_surface_t *surface);
	int (*clear_current)(gfx_surface_t *surface);
	int (*present_mode)(gfx_surface_t *surface, gfx_present_mode_t requested, gfx_present_mode_t *actual);
	int (*configure)(gfx_surface_t *surface, const gfx_surface_config_t *config);
	int (*present)(gfx_surface_t *surface, gfx_present_mode_t present_mode);
	int (*memory)(gfx_surface_t *surface, gfx_surface_memory_t *memory);
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

typedef struct gfx_vertex_2d_s {
	float x;
	float y;
	float r;
	float g;
	float b;
	float a;
} gfx_vertex_2d_t;

typedef struct gfx_color_s {
	float r;
	float g;
	float b;
	float a;
} gfx_color_t;

typedef struct gfx_rect_s {
	u16 x;
	u16 y;
	u16 width;
	u16 height;
} gfx_rect_t;

typedef struct gfx_config_s {
	const gfx_plan_t *plan;
	gfx_surface_t *surface;
} gfx_config_t;

typedef struct gfx_pass_config_s {
	gfx_color_t clear;
	gfx_rect_t viewport;
} gfx_pass_config_t;

struct gfx_s {
	const struct gfx_driver_s *drv;
	proc_t *proc;
	alloc_t alloc;
	void *data;
	gfx_frame_t *frame;
};

struct gfx_frame_s {
	gfx_t *gfx;
	const struct gfx_render_pass_s *render_pass;
	gfx_pass_config_t pass;
	const struct gfx_pipeline_s *pipeline;
	const struct gfx_buffer_s *vertex_buffer;
	const struct gfx_buffer_s *index_buffer;
	const struct gfx_resource_binding_s *resource_bindings;
	u32 resource_binding_count;
	int active;
	void *data;
};

gfx_t *gfx_init(gfx_t *gfx, const struct gfx_driver_s *drv, const gfx_config_t *config, proc_t *proc, alloc_t alloc);
void gfx_free(gfx_t *gfx);

int gfx_native(gfx_t *gfx, gfx_native_t *native);
int gfx_proc(gfx_t *gfx, strv_t name, void **proc);
int gfx_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex);
int gfx_draw_indexed(gfx_frame_t *frame, u32 index_count);
int gfx_end(gfx_frame_t *frame);

#endif
