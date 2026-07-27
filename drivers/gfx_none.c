#include "gfx_driver.h"

static int gfx_none_init(gfx_t *gfx, const gfx_config_t *config)
{
	(void)config;

	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_free(gfx_t *gfx)
{
	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || target == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_begin(gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_clear_color(gfx_t *gfx, float r, float g, float b, float a)
{
	(void)r;
	(void)g;
	(void)b;
	(void)a;

	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_viewport(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height)
{
	(void)x;
	(void)y;
	(void)width;
	(void)height;

	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_clear(gfx_t *gfx, u32 buffers)
{
	(void)buffers;

	if (gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	(void)buffer;
	(void)config;
	return 0;
}

static void gfx_none_buffer_free(gfx_buffer_t *buffer)
{
	(void)buffer;
}

static int gfx_none_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	(void)buffer;
	(void)data;
	(void)size;
	return 0;
}

static int gfx_none_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	(void)frame;
	(void)buffer;
	return 0;
}

static int gfx_none_shader_init(gfx_shader_t *shader, const gfx_shader_config_t *config)
{
	(void)shader;
	(void)config;
	return 0;
}

static void gfx_none_shader_free(gfx_shader_t *shader)
{
	(void)shader;
}

static int gfx_none_pipeline_init(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config)
{
	(void)pipeline;
	(void)config;
	return 0;
}

static void gfx_none_pipeline_free(gfx_pipeline_t *pipeline)
{
	(void)pipeline;
}

static int gfx_none_pipeline_bind(gfx_frame_t *frame, const gfx_pipeline_t *pipeline)
{
	(void)frame;
	(void)pipeline;
	return 0;
}

static int gfx_none_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex)
{
	(void)vertex_count;
	(void)first_vertex;

	if (frame == NULL || frame->gfx == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_end(gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL) {
		return 1;
	}

	return 0;
}

static gfx_driver_t gfx_none = {
	.name		 = "none",
	.api		 = GFX_API_NONE,
	.init		 = gfx_none_init,
	.free		 = gfx_none_free,
	.set_target	 = gfx_none_set_target,
	.viewport	 = gfx_none_viewport,
	.begin		 = gfx_none_begin,
	.clear_color	 = gfx_none_clear_color,
	.clear		 = gfx_none_clear,
	.buffer_init	 = gfx_none_buffer_init,
	.buffer_free	 = gfx_none_buffer_free,
	.buffer_set_data = gfx_none_buffer_set_data,
	.buffer_bind	 = gfx_none_buffer_bind,
	.shader_init	 = gfx_none_shader_init,
	.shader_free	 = gfx_none_shader_free,
	.pipeline_init	 = gfx_none_pipeline_init,
	.pipeline_free	 = gfx_none_pipeline_free,
	.pipeline_bind	 = gfx_none_pipeline_bind,
	.draw		 = gfx_none_draw,
	.end		 = gfx_none_end,
};

GFX_DRIVER(gfx_none, &gfx_none);
