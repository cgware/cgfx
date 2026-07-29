#ifndef GFX_DRIVER_H
#define GFX_DRIVER_H

#include "driver.h"
#include "gfx_framebuffer.h"
#include "gfx_pipeline.h"
#include "gfx_target.h"

typedef struct gfx_driver_s {
	const char *name;
	gfx_api_t api;
	int (*init)(gfx_t *gfx, const gfx_config_t *config);
	int (*free)(gfx_t *gfx);
	int (*native)(gfx_t *gfx, gfx_native_t *native);
	int (*proc)(gfx_t *gfx, strv_t name, void **proc);
	int (*render_pass_init)(gfx_render_pass_t *render_pass, const gfx_render_pass_config_t *config);
	void (*render_pass_free)(gfx_render_pass_t *render_pass);
	int (*framebuffer_init)(gfx_framebuffer_t *framebuffer);
	void (*framebuffer_free)(gfx_framebuffer_t *framebuffer);
	int (*framebuffer_pass_begin)(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame);
	int (*target_init)(gfx_target_t *target);
	void (*target_free)(gfx_target_t *target);
	int (*target_resize)(gfx_target_t *target, u16 width, u16 height);
	int (*target_read)(gfx_target_t *target, const gfx_memory_readback_config_t *config);
	int (*target_present)(gfx_target_t *target);
	int (*buffer_init)(gfx_buffer_t *buffer, const gfx_buffer_config_t *config);
	void (*buffer_free)(gfx_buffer_t *buffer);
	int (*buffer_set_data)(gfx_buffer_t *buffer, const void *data, size_t size);
	int (*buffer_bind)(gfx_frame_t *frame, const gfx_buffer_t *buffer);
	int (*shader_init)(gfx_shader_t *shader, const gfx_shader_config_t *config);
	void (*shader_free)(gfx_shader_t *shader);
	int (*pipeline_init)(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config);
	void (*pipeline_free)(gfx_pipeline_t *pipeline);
	int (*pipeline_bind)(gfx_frame_t *frame, const gfx_pipeline_t *pipeline);
	int (*draw)(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex);
	int (*end)(gfx_frame_t *frame);
} gfx_driver_t;

gfx_driver_t *gfx_driver_find(strv_t name);
gfx_driver_t *gfx_driver_next(const gfx_driver_t *driver);
u32 gfx_driver_list(gfx_driver_t **drivers, u32 capacity);

#define GFX_DRIVER_TYPE 0x474658

#define GFX_DRIVER(_name, _data) DRIVER(_name, GFX_DRIVER_TYPE, _data)

#endif
