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

static int gfx_none_target_init(gfx_target_t *target)
{
	(void)target;
	return 0;
}

static void gfx_none_target_free(gfx_target_t *target)
{
	(void)target;
}

static int gfx_none_swapchain_init(gfx_swapchain_t *swapchain, const gfx_swapchain_config_t *config)
{
	(void)config;
	if (swapchain != NULL) {
		swapchain->actual_present_mode = GFX_PRESENT_MODE_IMMEDIATE;
	}
	return 0;
}

static void gfx_none_swapchain_free(gfx_swapchain_t *swapchain)
{
	(void)swapchain;
}

static int gfx_none_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height)
{
	(void)swapchain;
	(void)width;
	(void)height;
	return 0;
}

static int gfx_none_swapchain_present(gfx_swapchain_t *swapchain)
{
	(void)swapchain;
	return 0;
}

static int gfx_none_target_read(gfx_target_t *target, const gfx_memory_readback_config_t *config)
{
	(void)target;
	(void)config;
	return 0;
}

static int gfx_none_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || framebuffer == NULL || framebuffer->target == NULL || framebuffer->render_pass == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_none_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	if (buffer == NULL || config == NULL ||
	    (config->type != GFX_BUFFER_VERTEX && config->type != GFX_BUFFER_INDEX && config->type != GFX_BUFFER_UNIFORM)) {
		return 1;
	}
	return 0;
}

static void gfx_none_buffer_free(gfx_buffer_t *buffer)
{
	(void)buffer;
}

static int gfx_none_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	if (buffer == NULL || data == NULL || size == 0 || buffer->usage == GFX_BUFFER_USAGE_STATIC) {
		return 1;
	}
	return 0;
}

static int gfx_none_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	(void)frame;
	(void)buffer;
	return 0;
}

static int gfx_none_bind_resources(gfx_frame_t *frame, const gfx_resource_binding_t *bindings, u32 binding_count)
{
	(void)bindings;
	(void)binding_count;
	if (frame == NULL) {
		return 1;
	}
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

static int gfx_none_draw_indexed(gfx_frame_t *frame, u32 index_count)
{
	(void)index_count;

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
	.name			= "none",
	.api			= GFX_API_NONE,
	.init			= gfx_none_init,
	.free			= gfx_none_free,
	.swapchain_init		= gfx_none_swapchain_init,
	.swapchain_free		= gfx_none_swapchain_free,
	.swapchain_resize	= gfx_none_swapchain_resize,
	.swapchain_present	= gfx_none_swapchain_present,
	.target_init		= gfx_none_target_init,
	.target_free		= gfx_none_target_free,
	.target_read		= gfx_none_target_read,
	.framebuffer_pass_begin = gfx_none_framebuffer_pass_begin,
	.buffer_init		= gfx_none_buffer_init,
	.buffer_free		= gfx_none_buffer_free,
	.buffer_set_data	= gfx_none_buffer_set_data,
	.buffer_bind		= gfx_none_buffer_bind,
	.bind_resources		= gfx_none_bind_resources,
	.shader_init		= gfx_none_shader_init,
	.shader_free		= gfx_none_shader_free,
	.pipeline_init		= gfx_none_pipeline_init,
	.pipeline_free		= gfx_none_pipeline_free,
	.pipeline_bind		= gfx_none_pipeline_bind,
	.draw			= gfx_none_draw,
	.draw_indexed		= gfx_none_draw_indexed,
	.end			= gfx_none_end,
};

GFX_DRIVER(gfx_none, &gfx_none);
