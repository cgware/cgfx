#include "gfx_driver.h"
#include "log.h"

typedef struct gfx_software_s {
	gfx_target_t target;
	alloc_t alloc;
	u16 viewport_x;
	u16 viewport_y;
	u16 viewport_width;
	u16 viewport_height;
	u8 color[4];
} gfx_software_t;

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

static int target_valid(const gfx_target_t *target)
{
	if (target == NULL || target->type != GFX_TARGET_MEMORY || target->format != GFX_FORMAT_RGBA8 || target->data == NULL ||
	    target->width == 0 || target->height == 0) {
		return 0;
	}

	return target->stride >= (size_t)target->width * 4;
}

static int gfx_software_init(gfx_t *gfx, const gfx_config_t *config)
{
	if (gfx == NULL || config == NULL || config->alloc.alloc == NULL) {
		return 1;
	}

	alloc_t alloc	       = config->alloc;
	gfx_software_t *render = alloc_alloc(&alloc, sizeof(gfx_software_t));
	if (render == NULL) {
		return 1;
	}
	*render = (gfx_software_t){
		.alloc = alloc,
		.color = {0, 0, 0, 255},
	};
	gfx->data = render;
	return 0;
}

static int gfx_software_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_software_t *render = gfx->data;
	alloc_free(&render->alloc, render, sizeof(*render));
	gfx->data = NULL;
	return 0;
}

static int gfx_software_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || gfx->data == NULL || target == NULL) {
		return 1;
	}

	gfx_software_t *render = gfx->data;
	if (target->type == GFX_TARGET_NONE) {
		render->target = (gfx_target_t){0};
		return 0;
	}
	if (!target_valid(target)) {
		return 1;
	}
	render->target		= *target;
	render->viewport_x	= 0;
	render->viewport_y	= 0;
	render->viewport_width	= target->width;
	render->viewport_height = target->height;
	return 0;
}

static int gfx_software_viewport(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_software_t *render	= gfx->data;
	render->viewport_x	= x;
	render->viewport_y	= y;
	render->viewport_width	= width;
	render->viewport_height = height;
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

static int gfx_software_draw_triangle_2d(const gfx_pipeline_t *pipeline, const gfx_vertex_2d_t vertices[3])
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL || vertices == NULL) {
		log_error("cgfx", "gfx_software", NULL, "data = NULL");
		return 1;
	}

	gfx_software_t *render = pipeline->gfx->data;
	if (!target_valid(&render->target) || render->viewport_width == 0 || render->viewport_height == 0) {
		log_error("cgfx", "gfx_software", NULL, "invalid target");
		return 1;
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

static int gfx_software_clear_color(gfx_t *gfx, float r, float g, float b, float a)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_software_t *render = gfx->data;
	render->color[0]       = color_u8(r);
	render->color[1]       = color_u8(g);
	render->color[2]       = color_u8(b);
	render->color[3]       = color_u8(a);
	return 0;
}

static int gfx_software_clear(gfx_t *gfx, u32 buffers)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}
	if ((buffers & GFX_CLEAR_COLOR_BUFFER) == 0) {
		return 0;
	}

	gfx_software_t *render = gfx->data;
	if (!target_valid(&render->target)) {
		return 1;
	}

	for (u16 y = 0; y < render->target.height; y++) {
		u8 *row = (u8 *)render->target.data + (size_t)y * render->target.stride;
		for (u16 x = 0; x < render->target.width; x++) {
			u8 *pixel = row + (size_t)x * 4;
			pixel[0]  = render->color[0];
			pixel[1]  = render->color[1];
			pixel[2]  = render->color[2];
			pixel[3]  = render->color[3];
		}
	}

	return 0;
}

static gfx_driver_t gfx_software = {
	.name		  = "software",
	.api		  = GFX_API_SOFTWARE,
	.init		  = gfx_software_init,
	.free		  = gfx_software_free,
	.set_target	  = gfx_software_set_target,
	.viewport	  = gfx_software_viewport,
	.clear_color	  = gfx_software_clear_color,
	.clear		  = gfx_software_clear,
	.shader_init	  = gfx_software_shader_init,
	.shader_free	  = gfx_software_shader_free,
	.pipeline_init	  = gfx_software_pipeline_init,
	.pipeline_free	  = gfx_software_pipeline_free,
	.draw_triangle_2d = gfx_software_draw_triangle_2d,
};

GFX_DRIVER(gfx_software, &gfx_software);
