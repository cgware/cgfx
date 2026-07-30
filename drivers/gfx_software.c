#include "gfx_driver.h"

#include "log.h"
#include "mem.h"

typedef struct gfx_software_buffer_s {
	buf_t buf;
} gfx_software_buffer_t;

typedef struct gfx_software_s {
	gfx_target_t target;
	gfx_swapchain_t *swapchain;
	u16 viewport_x;
	u16 viewport_y;
	u16 viewport_width;
	u16 viewport_height;
} gfx_software_t;

typedef struct gfx_software_surface_target_s {
	gfx_surface_memory_t memory;
} gfx_software_surface_target_t;

static u8 color_u8(float value)
{
	if (value <= 0.0f) {
		return 0;
	}
	if (value >= 1.0f) {
		return 255;
	}
	return (u8)(value * 255.0f + 0.5f);
}

static int memory_target_valid(const gfx_target_t *target)
{
	if (target == NULL || target->format != GFX_FORMAT_RGBA8 || target->data == NULL || target->width == 0 || target->height == 0) {
		return 0;
	}

	return target->stride >= (size_t)target->width * 4;
}

static int surface_target_valid(const gfx_target_t *target)
{
	return target != NULL && target->type == GFX_TARGET_SWAPCHAIN && target->format == GFX_FORMAT_RGBA8 && target->swapchain != NULL &&
	       target->swapchain->surface != NULL && target->swapchain->surface->api == GFX_API_SOFTWARE &&
	       target->swapchain->surface->ops != NULL && target->swapchain->surface->ops->memory != NULL && target->width != 0 &&
	       target->height != 0;
}

static int target_valid(const gfx_target_t *target)
{
	return target != NULL && (target->type == GFX_TARGET_MEMORY || target->type == GFX_TARGET_SWAPCHAIN) && memory_target_valid(target);
}

static int gfx_software_init(gfx_t *gfx, const gfx_config_t *config)
{
	if (gfx == NULL || config == NULL || gfx->alloc.alloc == NULL) {
		return 1;
	}

	gfx_software_t *render = alloc_alloc(&gfx->alloc, sizeof(gfx_software_t));
	if (render == NULL) {
		return 1;
	}
	*render	  = (gfx_software_t){0};
	gfx->data = render;
	return 0;
}

static int gfx_software_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_software_t *render = gfx->data;
	alloc_free(&gfx->alloc, render, sizeof(gfx_software_t));
	gfx->data = NULL;
	return 0;
}

static int gfx_software_target_set(gfx_t *gfx, const gfx_target_t *target)
{
	gfx_software_t *render = gfx->data;
	switch (target->type) {
	case GFX_TARGET_NONE: {
		render->target	  = (gfx_target_t){0};
		render->swapchain = NULL;
		return 0;
	}
	case GFX_TARGET_SWAPCHAIN: {
		if (!surface_target_valid(target) || target->driver_data == NULL) {
			return 1;
		}

		gfx_software_surface_target_t *surface_target = target->driver_data;

		render->target = (gfx_target_t){
			.gfx	= target->gfx,
			.type	= GFX_TARGET_MEMORY,
			.format = surface_target->memory.format,
			.data	= surface_target->memory.data,
			.width	= surface_target->memory.width,
			.height = surface_target->memory.height,
			.stride = surface_target->memory.stride,
		};
		if (!target_valid(&render->target)) {
			return 1;
		}
		render->swapchain = target->swapchain;
		break;
	}
	case GFX_TARGET_MEMORY: {
		if (!target_valid(target)) {
			return 1;
		}
		render->target	  = *target;
		render->swapchain = NULL;
		break;
	}
	default: {
		return 1;
	}
	}

	render->viewport_x	= 0;
	render->viewport_y	= 0;
	render->viewport_width	= render->target.width;
	render->viewport_height = render->target.height;
	return 0;
}

static int gfx_software_target_init(gfx_target_t *target)
{
	if (target == NULL || target->gfx == NULL) {
		return 1;
	}
	if (target->type == GFX_TARGET_MEMORY) {
		return !target_valid(target);
	}
	if (!surface_target_valid(target)) {
		return 1;
	}

	gfx_software_surface_target_t *surface_target = alloc_alloc(&target->gfx->alloc, sizeof(gfx_software_surface_target_t));
	if (surface_target == NULL) {
		return 1;
	}
	gfx_surface_memory_t memory = {
		.format = target->format,
		.data	= target->data,
		.width	= target->width,
		.height = target->height,
		.stride = target->stride,
	};
	if (target->swapchain->surface->ops->memory(target->swapchain->surface, &memory)) {
		alloc_free(&target->gfx->alloc, surface_target, sizeof(gfx_software_surface_target_t));
		return 1;
	}
	surface_target->memory = memory;
	if (!target_valid(&(gfx_target_t){
		    .type   = GFX_TARGET_MEMORY,
		    .format = memory.format,
		    .data   = memory.data,
		    .width  = memory.width,
		    .height = memory.height,
		    .stride = memory.stride,
	    })) {
		alloc_free(&target->gfx->alloc, surface_target, sizeof(gfx_software_surface_target_t));
		return 1;
	}
	target->driver_data = surface_target;
	return 0;
}

static void gfx_software_target_free(gfx_target_t *target)
{
	if (target == NULL || target->gfx == NULL || target->gfx->data == NULL) {
		return;
	}

	gfx_software_t *render = target->gfx->data;
	if (target->driver_data != NULL) {
		alloc_free(&target->gfx->alloc, target->driver_data, sizeof(gfx_software_surface_target_t));
		target->driver_data = NULL;
	}
	if (render->target.swapchain == target->swapchain || render->target.data == target->data) {
		render->target	  = (gfx_target_t){0};
		render->swapchain = NULL;
	}
}

static int gfx_software_swapchain_init(gfx_swapchain_t *swapchain, const gfx_swapchain_config_t *config)
{
	(void)config;

	if (swapchain == NULL || swapchain->surface == NULL || swapchain->surface->api != GFX_API_SOFTWARE ||
	    swapchain->surface->ops == NULL || swapchain->surface->ops->memory == NULL || swapchain->width == 0 || swapchain->height == 0) {
		return 1;
	}
	return 0;
}

static void gfx_software_swapchain_free(gfx_swapchain_t *swapchain)
{
	(void)swapchain;
}

static int gfx_software_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height)
{
	if (swapchain == NULL || swapchain->surface == NULL || swapchain->surface->ops == NULL || swapchain->surface->ops->memory == NULL ||
	    width == 0 || height == 0) {
		return 1;
	}

	swapchain->width  = width;
	swapchain->height = height;
	return 0;
}

static int gfx_software_swapchain_present(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->surface == NULL || swapchain->surface->ops == NULL ||
	    swapchain->surface->ops->present == NULL) {
		return 1;
	}

	return swapchain->surface->ops->present(swapchain->surface);
}

static void gfx_software_clear(gfx_software_t *render, gfx_color_t color)
{
	u8 clear[4] = {
		color_u8(color.r),
		color_u8(color.g),
		color_u8(color.b),
		color_u8(color.a),
	};
	for (u16 y = 0; y < render->target.height; y++) {
		u8 *row = (u8 *)render->target.data + (size_t)y * render->target.stride;
		for (u16 x = 0; x < render->target.width; x++) {
			u8 *pixel = row + (size_t)x * 4;
			pixel[0]  = clear[0];
			pixel[1]  = clear[1];
			pixel[2]  = clear[2];
			pixel[3]  = clear[3];
		}
	}
}

static int gfx_software_target_read(gfx_target_t *target, const gfx_memory_readback_config_t *config)
{
	if (target == NULL || target->gfx == NULL || target->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_software_t *render = target->gfx->data;
	if (!target_valid(&render->target)) {
		return 1;
	}

	for (u16 y = 0; y < render->target.height; y++) {
		u8 *dst	      = (u8 *)config->data + (size_t)y * config->stride;
		const u8 *src = (const u8 *)render->target.data + (size_t)y * render->target.stride;
		mem_copy(dst, config->stride, src, (size_t)render->target.width * 4);
	}
	return 0;
}

static int gfx_software_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || framebuffer == NULL || framebuffer->target == NULL ||
	    framebuffer->render_pass == NULL) {
		return 1;
	}

	if (gfx_software_target_set(frame->gfx, framebuffer->target)) {
		return 1;
	}

	gfx_software_t *render	= frame->gfx->data;
	render->viewport_x	= frame->pass.viewport.x;
	render->viewport_y	= frame->pass.viewport.y;
	render->viewport_width	= frame->pass.viewport.width;
	render->viewport_height = frame->pass.viewport.height;
	if (framebuffer->render_pass->load == GFX_LOAD_CLEAR) {
		gfx_software_clear(render, frame->pass.clear);
	}
	return 0;
}

static float edge(const gfx_vertex_2d_t *a, const gfx_vertex_2d_t *b, float x, float y)
{
	return (x - a->x) * (b->y - a->y) - (y - a->y) * (b->x - a->x);
}

static int point_inside(float w0, float w1, float w2, float area)
{
	if (area > 0.0f) {
		return w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f;
	}
	return w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f;
}

static void draw_pixel(gfx_software_t *render, u16 x, u16 y, const u8 color[4])
{
	u8 *row	  = (u8 *)render->target.data + (size_t)y * render->target.stride;
	u8 *pixel = row + (size_t)x * 4;
	pixel[0]  = color[0];
	pixel[1]  = color[1];
	pixel[2]  = color[2];
	pixel[3]  = color[3];
}

static void vertex_to_screen(gfx_vertex_2d_t *out, const gfx_vertex_2d_t *vertex, const gfx_software_t *render)
{
	*out = (gfx_vertex_2d_t){
		.x = (float)render->viewport_x + (vertex->x + 1.0f) * 0.5f * (float)render->viewport_width,
		.y = (float)render->viewport_y + (1.0f - vertex->y) * 0.5f * (float)render->viewport_height,
		.r = vertex->r,
		.g = vertex->g,
		.b = vertex->b,
		.a = vertex->a,
	};
}
static void gfx_software_buffer_free(gfx_buffer_t *buffer)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->data == NULL) {
		return;
	}

	gfx_software_buffer_t *sw_buffer = buffer->data;

	if (sw_buffer->buf.data != NULL) {
		buf_free(&sw_buffer->buf);
		sw_buffer->buf.data = NULL;
	}
	alloc_free(&buffer->gfx->alloc, sw_buffer, sizeof(gfx_software_buffer_t));
	buffer->data = NULL;
}

static int gfx_software_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	if (buffer == NULL) {
		return 1;
	}
	(void)config;

	gfx_software_buffer_t *sw_buffer = alloc_alloc(&buffer->gfx->alloc, sizeof(gfx_software_buffer_t));
	if (sw_buffer == NULL) {
		return 1;
	}
	*sw_buffer   = (gfx_software_buffer_t){0};
	buffer->data = sw_buffer;

	return 0;
}

static int gfx_software_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	if (buffer == NULL) {
		return 1;
	}

	gfx_software_buffer_t *sw_buffer = buffer->data;
	if (sw_buffer->buf.data == NULL) {
		buf_init(&sw_buffer->buf, size, buffer->gfx->alloc);
	} else {
		buf_resize(&sw_buffer->buf, size);
	}

	buf_set(&sw_buffer->buf, 0, size, data);

	return 0;
}

static int gfx_software_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || buffer == NULL || buffer->data == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_software_shader_init(gfx_shader_t *shader, const gfx_shader_config_t *config)
{
	(void)shader;
	(void)config;
	return 0;
}

static void gfx_software_shader_free(gfx_shader_t *shader)
{
	(void)shader;
}

static int gfx_software_pipeline_init(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config)
{
	(void)pipeline;
	(void)config;
	return 0;
}

static void gfx_software_pipeline_free(gfx_pipeline_t *pipeline)
{
	(void)pipeline;
}

static int gfx_software_pipeline_bind(gfx_frame_t *frame, const gfx_pipeline_t *pipeline)
{
	(void)frame;
	(void)pipeline;
	return 0;
}

static int gfx_software_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || frame->vertex_buffer == NULL ||
	    frame->vertex_buffer->data == NULL || vertex_count < 3) {
		return 1;
	}

	gfx_software_t *render = frame->gfx->data;
	if (!target_valid(&render->target) || render->viewport_width == 0 || render->viewport_height == 0) {
		return 1;
	}

	gfx_software_buffer_t *sw_buffer       = frame->vertex_buffer->data;
	const gfx_vertex_2d_t *buffer_vertices = sw_buffer->buf.data;
	gfx_vertex_2d_t vertices[3];
	for (u32 i = 0; i < 3; i++) {
		vertex_to_screen(&vertices[i], &buffer_vertices[first_vertex + i], render);
	}

	float area = edge(&vertices[0], &vertices[1], vertices[2].x, vertices[2].y);
	if (area == 0.0f) {
		return 0;
	}

	u16 x0 = render->viewport_x;
	u16 y0 = render->viewport_y;
	u32 x1 = (u32)x0 + render->viewport_width;
	u32 y1 = (u32)y0 + render->viewport_height;
	if (x1 > render->target.width) {
		x1 = render->target.width;
	}
	if (y1 > render->target.height) {
		y1 = render->target.height;
	}

	for (u16 y = y0; y < y1; y++) {
		for (u16 x = x0; x < x1; x++) {
			float px = (float)x + 0.5f;
			float py = (float)y + 0.5f;
			float w0 = edge(&vertices[1], &vertices[2], px, py);
			float w1 = edge(&vertices[2], &vertices[0], px, py);
			float w2 = edge(&vertices[0], &vertices[1], px, py);
			if (!point_inside(w0, w1, w2, area)) {
				continue;
			}

			float inv_area = 1.0f / area;
			w0 *= inv_area;
			w1 *= inv_area;
			w2 *= inv_area;
			u8 color[4] = {
				color_u8(vertices[0].r * w0 + vertices[1].r * w1 + vertices[2].r * w2),
				color_u8(vertices[0].g * w0 + vertices[1].g * w1 + vertices[2].g * w2),
				color_u8(vertices[0].b * w0 + vertices[1].b * w1 + vertices[2].b * w2),
				color_u8(vertices[0].a * w0 + vertices[1].a * w1 + vertices[2].a * w2),
			};
			draw_pixel(render, x, y, color);
		}
	}

	return 0;
}

static int gfx_software_end(gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	return 0;
}

static gfx_driver_t gfx_software = {
	.name			= "software",
	.api			= GFX_API_SOFTWARE,
	.init			= gfx_software_init,
	.free			= gfx_software_free,
	.swapchain_init		= gfx_software_swapchain_init,
	.swapchain_free		= gfx_software_swapchain_free,
	.swapchain_resize	= gfx_software_swapchain_resize,
	.swapchain_present	= gfx_software_swapchain_present,
	.target_init		= gfx_software_target_init,
	.target_free		= gfx_software_target_free,
	.target_read		= gfx_software_target_read,
	.framebuffer_pass_begin = gfx_software_framebuffer_pass_begin,
	.buffer_init		= gfx_software_buffer_init,
	.buffer_free		= gfx_software_buffer_free,
	.buffer_set_data	= gfx_software_buffer_set_data,
	.buffer_bind		= gfx_software_buffer_bind,
	.shader_init		= gfx_software_shader_init,
	.shader_free		= gfx_software_shader_free,
	.pipeline_init		= gfx_software_pipeline_init,
	.pipeline_free		= gfx_software_pipeline_free,
	.pipeline_bind		= gfx_software_pipeline_bind,
	.draw			= gfx_software_draw,
	.end			= gfx_software_end,
};

GFX_DRIVER(gfx_software, &gfx_software);
