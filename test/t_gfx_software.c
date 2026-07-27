#include "alloc.h"
#include "gfx_driver.h"

#include "test.h"

static void *t_gfx_software_alloc_fail(alloc_t *alloc, size_t size)
{
	(void)alloc;
	(void)size;
	return NULL;
}

static u8 t_gfx_software_surface_pixels[4];
static int t_gfx_software_surface_target_calls;
static int t_gfx_software_surface_target_ret;
static int t_gfx_software_surface_present_calls;
static int t_gfx_software_surface_present_ret;

static int t_gfx_software_surface_target(gfx_surface_t *surface, gfx_target_t *target)
{
	t_gfx_software_surface_target_calls++;
	if (surface == NULL || target == NULL) {
		return 1;
	}
	if (t_gfx_software_surface_target_ret) {
		return t_gfx_software_surface_target_ret;
	}

	target->type   = GFX_TARGET_SURFACE;
	target->format = GFX_FORMAT_RGBA8;
	target->data   = t_gfx_software_surface_pixels;
	target->width  = 1;
	target->height = 1;
	target->stride = 4;
	return 0;
}

static int t_gfx_software_surface_present(gfx_surface_t *surface)
{
	t_gfx_software_surface_present_calls++;
	return surface == NULL ? 1 : t_gfx_software_surface_present_ret;
}

static const gfx_surface_ops_t t_gfx_software_surface_ops = {
	.present = t_gfx_software_surface_present,
	.target	 = t_gfx_software_surface_target,
};

static gfx_surface_t t_gfx_software_surface(void)
{
	return (gfx_surface_t){
		.api  = GFX_API_SOFTWARE,
		.data = t_gfx_software_surface_pixels,
		.ops  = &t_gfx_software_surface_ops,
	};
}

static void t_gfx_software_surface_reset(void)
{
	t_gfx_software_surface_pixels[0]     = 0;
	t_gfx_software_surface_pixels[1]     = 0;
	t_gfx_software_surface_pixels[2]     = 0;
	t_gfx_software_surface_pixels[3]     = 0;
	t_gfx_software_surface_target_calls  = 0;
	t_gfx_software_surface_target_ret    = 0;
	t_gfx_software_surface_present_calls = 0;
	t_gfx_software_surface_present_ret   = 0;
}

static gfx_driver_t *t_gfx_software_driver(void)
{
	return gfx_driver_find(STRV("software"));
}

static int t_gfx_software_init(gfx_t *gfx)
{
	gfx_driver_t *drv = t_gfx_software_driver();
	return drv == NULL || gfx_init(gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD) != gfx;
}

static int t_gfx_software_draw(gfx_t *gfx, const gfx_vertex_2d_t vertices[3])
{
	if (vertices == NULL) {
		return 1;
	}
	gfx_buffer_t buffer = {0};
	if (gfx_buffer_init(&buffer, gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}) != &buffer) {
		return 1;
	}
	if (gfx_buffer_set_data(&buffer, vertices, sizeof(gfx_vertex_2d_t) * 3)) {
		gfx_buffer_free(&buffer);
		return 1;
	}
	gfx_pipeline_t pipeline = {0};
	if (gfx_pipeline_init(&pipeline, gfx, &(gfx_pipeline_config_t){0}) != &pipeline) {
		gfx_buffer_free(&buffer);
		return 1;
	}
	int ret = gfx_draw_triangle_2d(&pipeline, &buffer);
	gfx_pipeline_free(&pipeline);
	gfx_buffer_free(&buffer);
	return ret;
}

static int t_gfx_software_set_target(gfx_t *gfx, u8 *pixels, u16 width, u16 height, size_t stride)
{
	gfx_target_t target = {
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= width,
		.height = height,
		.stride = stride,
	};

	return gfx_set_target(gfx, &target);
}

TEST(gfx_software_driver_is_registered)
{
	START;

	EXPECT_NOT_NULL(t_gfx_software_driver());

	END;
}

TEST(gfx_software_init_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_software_init_null_config)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(&gfx, NULL), 1);

	END;
}

TEST(gfx_software_init_null_alloc)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(&gfx, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_software_init_success)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_init_alloc_failure)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, (alloc_t){.alloc = t_gfx_software_alloc_fail}));

	END;
}

TEST(gfx_software_free_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->free(NULL), 1);

	END;
}

TEST(gfx_software_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->free(&gfx), 1);

	END;
}

TEST(gfx_software_set_target_null_pixels)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_target_t target = {0};

	EXPECT_EQ(gfx.drv->set_target(&gfx, &target), 1);

	END;
}

TEST(gfx_software_set_target_none_clears_target)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	u8 pixels[4] = {0};
	t_gfx_software_set_target(&gfx, pixels, 1, 1, 4);
	gfx_target_t target = {
		.type = GFX_TARGET_NONE,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);
	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_invalid_format)
{
	START;

	u8 pixels[4]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD);
	gfx_target_t target = {
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_NONE,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_null_target)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);

	EXPECT_EQ(gfx_set_target(&gfx, NULL), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_invalid_type)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_target_t target = {
		.type	= GFX_TARGET_SURFACE,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_unknown_type)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_target_t target = {
		.type	= (gfx_target_type_t)0xFF,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_surface_target_callback_failure)
{
	START;

	t_gfx_software_surface_reset();
	t_gfx_software_surface_target_ret = 1;
	gfx_surface_t surface		  = t_gfx_software_surface();
	gfx_t gfx			  = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_target_t target = {
		.type	 = GFX_TARGET_SURFACE,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &surface,
		.width	 = 1,
		.height	 = 1,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);
	EXPECT_EQ(t_gfx_software_surface_target_calls, 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_surface_target_success)
{
	START;

	t_gfx_software_surface_reset();
	gfx_surface_t surface = t_gfx_software_surface();
	gfx_t gfx	      = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_target_t target = {
		.type	 = GFX_TARGET_SURFACE,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &surface,
		.width	 = 1,
		.height	 = 1,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);
	EXPECT_EQ(t_gfx_software_surface_target_calls, 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_memory_target_clears_surface)
{
	START;

	t_gfx_software_surface_reset();
	gfx_surface_t surface = t_gfx_software_surface();
	u8 pixels[4]	      = {0};
	gfx_t gfx	      = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	  = GFX_TARGET_SURFACE,
					 .format  = GFX_FORMAT_RGBA8,
					 .surface = &surface,
					 .width	  = 1,
					 .height  = 1,
				 }),
		  0);

	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	 = GFX_TARGET_MEMORY,
					 .format = GFX_FORMAT_RGBA8,
					 .data	 = pixels,
					 .width	 = 1,
					 .height = 1,
					 .stride = 4,
				 }),
		  0);
	EXPECT_EQ(gfx_present(&gfx), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_viewport_success)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);

	EXPECT_EQ(gfx_viewport(&gfx, 1, 2, 3, 4), 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_viewport_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->viewport(&gfx, 1, 2, 3, 4), 1);

	END;
}

TEST(gfx_software_draw_triangle_2d_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_pipeline_t pipeline = {.gfx = &gfx, .data = (void *)1};

	EXPECT_EQ(gfx.drv->draw_triangle_2d(&pipeline, NULL), 1);

	END;
}

TEST(gfx_software_draw_triangle_2d_null_buffer)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);

	EXPECT_EQ(t_gfx_software_draw(&gfx, NULL), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_triangle_2d_without_target)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_EQ(t_gfx_software_draw(&gfx, vertices), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_shader_success)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_shader_t shader = {0};

	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){0}), &shader);

	gfx_shader_free(&shader);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_buffer_init_alloc_failure)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_buffer_t buffer = {0};
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_software_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_buffer_free_null_data)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_buffer_t buffer = {.gfx = &gfx};

	gfx_buffer_free(&buffer);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_buffer_set_data_resizes_existing_buffer)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_buffer_t buffer	    = {0};
	gfx_vertex_2d_t vertices[3] = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_triangle_2d_writes_inside_pixel)
{
	START;

	u8 pixels[4 * 4 * 4] = {0};
	gfx_t gfx	     = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(t_gfx_software_set_target(&gfx, pixels, 4, 4, 16), 0);
	gfx_vertex_2d_t vertices[3] = {
		{.x = -1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
		{.x = 1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .a = 1.0f},
	};

	EXPECT_EQ(t_gfx_software_draw(&gfx, vertices), 0);
	EXPECT_EQ(pixels[0], 255);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_triangle_2d_accepts_positive_winding)
{
	START;

	u8 pixels[4 * 4 * 4] = {0};
	gfx_t gfx	     = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(t_gfx_software_set_target(&gfx, pixels, 4, 4, 16), 0);
	gfx_vertex_2d_t vertices[3] = {
		{.x = -1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .a = 1.0f},
		{.x = 1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
	};

	EXPECT_EQ(t_gfx_software_draw(&gfx, vertices), 0);
	EXPECT_EQ(pixels[0], 255);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_triangle_2d_clamps_viewport_to_target)
{
	START;

	u8 pixels[2 * 2 * 4] = {0};
	gfx_t gfx	     = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(t_gfx_software_set_target(&gfx, pixels, 2, 2, 8), 0);
	EXPECT_EQ(gfx_viewport(&gfx, 1, 1, 4, 4), 0);
	gfx_vertex_2d_t vertices[3] = {
		{.x = -1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .a = 1.0f},
		{.x = 1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
	};

	EXPECT_EQ(t_gfx_software_draw(&gfx, vertices), 0);
	EXPECT_EQ(pixels[12], 255);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_triangle_2d_leaves_outside_pixel)
{
	START;

	u8 pixels[4 * 4 * 4] = {0};
	gfx_t gfx	     = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(t_gfx_software_set_target(&gfx, pixels, 4, 4, 16), 0);
	gfx_vertex_2d_t vertices[3] = {
		{.x = -1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
		{.x = -0.5f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
		{.x = -1.0f, .y = 0.5f, .r = 1.0f, .a = 1.0f},
	};

	EXPECT_EQ(t_gfx_software_draw(&gfx, vertices), 0);
	EXPECT_EQ(pixels[3 * 16 + 3 * 4], 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_triangle_2d_respects_viewport)
{
	START;

	u8 pixels[4 * 4 * 4] = {0};
	gfx_t gfx	     = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(t_gfx_software_set_target(&gfx, pixels, 4, 4, 16), 0);
	EXPECT_EQ(gfx_viewport(&gfx, 1, 1, 3, 3), 0);
	gfx_vertex_2d_t vertices[3] = {
		{.x = -1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
		{.x = 1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .a = 1.0f},
	};

	EXPECT_EQ(t_gfx_software_draw(&gfx, vertices), 0);
	EXPECT_EQ(pixels[0], 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_triangle_2d_interpolates_first_vertex_color)
{
	START;

	u8 pixels[4 * 4 * 4] = {0};
	gfx_t gfx	     = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(t_gfx_software_set_target(&gfx, pixels, 4, 4, 16), 0);
	gfx_vertex_2d_t vertices[3] = {
		{.x = -1.0f, .y = 1.0f, .r = 1.0f, .a = 1.0f},
		{.x = 1.0f, .y = 1.0f, .g = 1.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .b = 1.0f, .a = 1.0f},
	};

	EXPECT_EQ(t_gfx_software_draw(&gfx, vertices), 0);
	EXPECT_EQ(pixels[0], 191);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_triangle_2d_degenerate_succeeds)
{
	START;

	u8 pixels[4 * 4 * 4] = {0};
	gfx_t gfx	     = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(t_gfx_software_set_target(&gfx, pixels, 4, 4, 16), 0);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 0.0f, .y = 0.0f},
		{.x = 0.0f, .y = 0.0f},
		{.x = 0.0f, .y = 0.0f},
	};

	EXPECT_EQ(t_gfx_software_draw(&gfx, vertices), 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_null_data)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_target_t target = {
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_zero_width)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_target_t target = {
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.height = 1,
		.stride = 4,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_zero_height)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	gfx_target_t target = {
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.stride = 4,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_invalid_stride)
{
	START;

	u8 pixels[4]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD);
	gfx_target_t target = {
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 3,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_success)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);

	EXPECT_EQ(t_gfx_software_set_target(&gfx, pixels, 1, 1, 4), 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_color_clamps_low)
{
	START;

	u8 pixels[4] = {255, 255, 255, 255};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	t_gfx_software_set_target(&gfx, pixels, 1, 1, 4);

	EXPECT_EQ(gfx_clear_color(&gfx, -1.0f, -0.1f, 0.0f, 1.0f), 0);
	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 0);
	EXPECT_EQ(pixels[0], 0);
	EXPECT_EQ(pixels[1], 0);
	EXPECT_EQ(pixels[2], 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_color_clamps_high)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	t_gfx_software_set_target(&gfx, pixels, 1, 1, 4);

	EXPECT_EQ(gfx_clear_color(&gfx, 1.0f, 1.1f, 2.0f, 1.0f), 0);
	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 0);
	EXPECT_EQ(pixels[0], 255);
	EXPECT_EQ(pixels[1], 255);
	EXPECT_EQ(pixels[2], 255);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_color_rounds)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	t_gfx_software_set_target(&gfx, pixels, 1, 1, 4);

	EXPECT_EQ(gfx_clear_color(&gfx, 0.5f, 0.0f, 0.0f, 1.0f), 0);
	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 0);
	EXPECT_EQ(pixels[0], 128);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_color_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->clear_color(&gfx, 0.0f, 0.0f, 0.0f, 0.0f), 1);

	END;
}

TEST(gfx_software_clear_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_software_clear_without_target)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_zero_buffers)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);

	EXPECT_EQ(gfx_clear(&gfx, 0), 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_writes_pixels)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	t_gfx_software_set_target(&gfx, pixels, 2, 1, 8);
	gfx_clear_color(&gfx, 0.1f, 0.2f, 0.3f, 1.0f);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(pixels[0], 26);
	EXPECT_EQ(pixels[1], 51);
	EXPECT_EQ(pixels[2], 77);
	EXPECT_EQ(pixels[3], 255);
	EXPECT_EQ(pixels[4], 26);
	EXPECT_EQ(pixels[5], 51);
	EXPECT_EQ(pixels[6], 77);
	EXPECT_EQ(pixels[7], 255);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_uses_stride)
{
	START;

	u8 pixels[16] = {0};
	gfx_t gfx     = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	t_gfx_software_set_target(&gfx, pixels, 1, 2, 8);
	gfx_clear_color(&gfx, 1.0f, 0.0f, 0.0f, 1.0f);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(pixels[8], 255);
	EXPECT_EQ(pixels[9], 0);
	EXPECT_EQ(pixels[10], 0);
	EXPECT_EQ(pixels[11], 255);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_present_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->present(&gfx), 1);

	END;
}

TEST(gfx_software_present_surface_target)
{
	START;

	t_gfx_software_surface_reset();
	gfx_surface_t surface = t_gfx_software_surface();
	gfx_t gfx	      = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	  = GFX_TARGET_SURFACE,
					 .format  = GFX_FORMAT_RGBA8,
					 .surface = &surface,
					 .width	  = 1,
					 .height  = 1,
				 }),
		  0);

	EXPECT_EQ(gfx_present(&gfx), 0);
	EXPECT_EQ(t_gfx_software_surface_present_calls, 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_present_surface_failure)
{
	START;

	t_gfx_software_surface_reset();
	t_gfx_software_surface_present_ret = 1;
	gfx_surface_t surface		   = t_gfx_software_surface();
	gfx_t gfx			   = {0};
	EXPECT_EQ(t_gfx_software_init(&gfx), 0);
	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	  = GFX_TARGET_SURFACE,
					 .format  = GFX_FORMAT_RGBA8,
					 .surface = &surface,
					 .width	  = 1,
					 .height  = 1,
				 }),
		  0);

	EXPECT_EQ(gfx_present(&gfx), 1);
	EXPECT_EQ(t_gfx_software_surface_present_calls, 1);

	gfx_free(&gfx);
	END;
}

STEST(gfx_software)
{
	SSTART;

	RUN(gfx_software_driver_is_registered);
	RUN(gfx_software_init_null_gfx);
	RUN(gfx_software_init_null_config);
	RUN(gfx_software_init_null_alloc);
	RUN(gfx_software_init_success);
	RUN(gfx_software_init_alloc_failure);
	RUN(gfx_software_free_null_gfx);
	RUN(gfx_software_free_null_data);
	RUN(gfx_software_set_target_null_pixels);
	RUN(gfx_software_set_target_none_clears_target);
	RUN(gfx_software_set_target_null_target);
	RUN(gfx_software_set_target_invalid_type);
	RUN(gfx_software_set_target_unknown_type);
	RUN(gfx_software_set_surface_target_callback_failure);
	RUN(gfx_software_set_surface_target_success);
	RUN(gfx_software_set_memory_target_clears_surface);
	RUN(gfx_software_viewport_success);
	RUN(gfx_software_viewport_null_data);
	RUN(gfx_software_draw_triangle_2d_null_data);
	RUN(gfx_software_draw_triangle_2d_null_buffer);
	RUN(gfx_software_draw_triangle_2d_without_target);
	RUN(gfx_software_buffer_init_alloc_failure);
	RUN(gfx_software_buffer_free_null_data);
	RUN(gfx_software_buffer_set_data_resizes_existing_buffer);
	RUN(gfx_software_shader_success);
	RUN(gfx_software_draw_triangle_2d_writes_inside_pixel);
	RUN(gfx_software_draw_triangle_2d_accepts_positive_winding);
	RUN(gfx_software_draw_triangle_2d_clamps_viewport_to_target);
	RUN(gfx_software_draw_triangle_2d_leaves_outside_pixel);
	RUN(gfx_software_draw_triangle_2d_respects_viewport);
	RUN(gfx_software_draw_triangle_2d_interpolates_first_vertex_color);
	RUN(gfx_software_draw_triangle_2d_degenerate_succeeds);
	RUN(gfx_software_set_target_invalid_format);
	RUN(gfx_software_set_target_null_data);
	RUN(gfx_software_set_target_zero_width);
	RUN(gfx_software_set_target_zero_height);
	RUN(gfx_software_set_target_invalid_stride);
	RUN(gfx_software_set_target_success);
	RUN(gfx_software_clear_color_clamps_low);
	RUN(gfx_software_clear_color_clamps_high);
	RUN(gfx_software_clear_color_rounds);
	RUN(gfx_software_clear_color_null_data);
	RUN(gfx_software_clear_null_data);
	RUN(gfx_software_clear_without_target);
	RUN(gfx_software_clear_zero_buffers);
	RUN(gfx_software_clear_writes_pixels);
	RUN(gfx_software_clear_uses_stride);
	RUN(gfx_software_present_null_data);
	RUN(gfx_software_present_surface_target);
	RUN(gfx_software_present_surface_failure);

	SEND;
}
