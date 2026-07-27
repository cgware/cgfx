#ifndef GFX_DRIVER_H
#define GFX_DRIVER_H

#include "driver.h"
#include "gfx_pipeline.h"

typedef struct gfx_driver_s {
	const char *name;
	gfx_api_t api;
	int (*init)(gfx_t *gfx, const gfx_config_t *config);
	int (*free)(gfx_t *gfx);
	int (*native)(gfx_t *gfx, gfx_native_t *native);
	int (*proc)(gfx_t *gfx, strv_t name, void **proc);
	int (*set_target)(gfx_t *gfx, const gfx_target_t *target);
	int (*viewport)(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height);
	int (*clear_color)(gfx_t *gfx, float r, float g, float b, float a);
	int (*clear)(gfx_t *gfx, u32 buffers);
	int (*buffer_init)(gfx_buffer_t *buffer, const gfx_buffer_config_t *config);
	void (*buffer_free)(gfx_buffer_t *buffer);
	int (*buffer_set_data)(gfx_buffer_t *buffer, const void *data, size_t size);
	int (*shader_init)(gfx_shader_t *shader, const gfx_shader_config_t *config);
	void (*shader_free)(gfx_shader_t *shader);
	int (*pipeline_init)(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config);
	void (*pipeline_free)(gfx_pipeline_t *pipeline);
	int (*draw_triangle_2d)(const gfx_pipeline_t *pipeline, const gfx_buffer_t *vertex_buffer);
	int (*present)(gfx_t *gfx);
} gfx_driver_t;

gfx_driver_t *gfx_driver_find(strv_t name);
gfx_driver_t *gfx_driver_next(const gfx_driver_t *driver);
u32 gfx_driver_list(gfx_driver_t **drivers, u32 capacity);

#define GFX_DRIVER_TYPE 0x474658

#define GFX_DRIVER(_name, _data) DRIVER(_name, GFX_DRIVER_TYPE, _data)

#endif
