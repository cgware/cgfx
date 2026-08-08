#include "gfx_driver.h"

#include "log.h"
#include "test.h"

static gfx_driver_t *t_gfx_software_driver(void)
{
	return gfx_driver_find(STRV("software"));
}

static int t_gfx_software_present_calls;
static int t_gfx_software_present_ret;
static int t_gfx_software_memory_calls;
static int t_gfx_software_memory_ret;
static int t_gfx_software_alloc_count;
static gfx_surface_memory_t t_gfx_software_memory;

typedef struct t_gfx_software_surface_target_data_s {
	gfx_surface_memory_t memory;
} t_gfx_software_surface_target_data_t;

typedef struct t_gfx_software_buffer_data_s {
	buf_t buf;
} t_gfx_software_buffer_data_t;

static int t_gfx_software_surface_present(gfx_surface_t *surface, gfx_present_mode_t present_mode)
{
	(void)surface;
	(void)present_mode;
	t_gfx_software_present_calls++;
	return t_gfx_software_present_ret;
}

static int t_gfx_software_surface_memory(gfx_surface_t *surface, gfx_surface_memory_t *memory)
{
	(void)surface;
	t_gfx_software_memory_calls++;
	if (t_gfx_software_memory_ret) {
		return t_gfx_software_memory_ret;
	}
	*memory = t_gfx_software_memory;
	return 0;
}

static const gfx_surface_ops_t t_gfx_software_surface_ops = {
	.present = t_gfx_software_surface_present,
	.memory	 = t_gfx_software_surface_memory,
};

static void t_gfx_software_reset_surface(u8 *pixels, u16 width, u16 height, size_t stride)
{
	t_gfx_software_present_calls = 0;
	t_gfx_software_present_ret   = 0;
	t_gfx_software_memory_calls  = 0;
	t_gfx_software_memory_ret    = 0;

	t_gfx_software_memory = (gfx_surface_memory_t){
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= width,
		.height = height,
		.stride = stride,
	};
}

static void *t_gfx_software_alloc_fail(alloc_t *alloc, size_t size)
{
	(void)alloc;
	(void)size;
	return NULL;
}

static void *t_gfx_software_alloc_fail_after_first(alloc_t *alloc, size_t size)
{
	t_gfx_software_alloc_count++;
	if (t_gfx_software_alloc_count > 1) {
		return NULL;
	}
	return alloc_alloc_std(alloc, size);
}

static int t_gfx_software_realloc_fail(alloc_t *alloc, void **ptr, size_t *capacity, size_t size)
{
	(void)alloc;
	(void)ptr;
	(void)capacity;
	(void)size;
	return 1;
}

static int t_gfx_software_scene(gfx_t *gfx, gfx_target_t *target, gfx_render_pass_t *render_pass, gfx_framebuffer_t *framebuffer,
				u8 *pixels, u16 width, u16 height, size_t stride)
{
	gfx_driver_t *drv				= t_gfx_software_driver();
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= width,
		.height = height,
		.stride = stride,
	};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	if (gfx_init(gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD) != gfx ||
	    gfx_target_init_memory(target, gfx, &memory_target_config) != target) {
		return 1;
	}
	if (gfx_render_pass_init(render_pass, gfx, &render_pass_config) != render_pass) {
		return 1;
	}
	return gfx_framebuffer_init(framebuffer, target, render_pass) != framebuffer;
}

static int t_gfx_software_surface_scene(gfx_t *gfx, gfx_swapchain_t *swapchain, gfx_target_t *target, gfx_render_pass_t *render_pass,
					gfx_framebuffer_t *framebuffer, gfx_surface_t *surface)
{
	gfx_driver_t *drv			= t_gfx_software_driver();
	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = surface,
		.width	 = 2,
		.height	 = 2,
	};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	if (gfx_init(gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD) != gfx ||
	    gfx_swapchain_init(swapchain, gfx, &swapchain_config) != swapchain || gfx_target_init_swapchain(target, swapchain) != target) {
		return 1;
	}
	if (gfx_render_pass_init(render_pass, gfx, &render_pass_config) != render_pass) {
		return 1;
	}
	return gfx_framebuffer_init(framebuffer, target, render_pass) != framebuffer;
}

static void t_gfx_software_scene_free(gfx_t *gfx, gfx_target_t *target, gfx_render_pass_t *render_pass, gfx_framebuffer_t *framebuffer)
{
	gfx_framebuffer_free(framebuffer);
	gfx_render_pass_free(render_pass);
	gfx_target_free(target);
	gfx_free(gfx);
}

TEST(gfx_software_driver_is_registered)
{
	START;

	EXPECT_NOT_NULL(t_gfx_software_driver());

	END;
}

TEST(gfx_software_init_failures)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {.alloc = ALLOC_STD};

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);
	EXPECT_EQ(drv->init(&gfx, NULL), 1);
	EXPECT_EQ(drv->init(&(gfx_t){0}, &(gfx_config_t){0}), 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, (alloc_t){.alloc = t_gfx_software_alloc_fail}));
	EXPECT_EQ(drv->free(NULL), 1);
	EXPECT_EQ(drv->free(&(gfx_t){0}), 1);

	END;
}

TEST(gfx_software_memory_pass_clears_target)
{
	START;

	u8 pixels[16]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 2,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_frame_t frame	      = {0};
	gfx_pass_config_t pass_config = {
		.clear = {1.0f, 0.0f, 0.0f, 1.0f},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	u8 readback[16] = {0};
	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = readback, .stride = 8}), 0);
	EXPECT_EQ(readback[0], 255);
	EXPECT_EQ(readback[1], 0);
	EXPECT_EQ(readback[2], 0);
	EXPECT_EQ(readback[3], 255);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_memory_pass_load_does_not_clear)
{
	START;

	u8 pixels[4]		      = {1, 2, 3, 4};
	gfx_t gfx		      = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 1, 1, 4), 0);
	gfx_render_pass_free(&render_pass);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_free(&framebuffer);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {1.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(pixels[0], 1);
	EXPECT_EQ(pixels[1], 2);
	EXPECT_EQ(pixels[2], 3);
	EXPECT_EQ(pixels[3], 4);

	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_surface_target_lifecycle)
{
	START;

	u8 pixels[64] = {0};
	t_gfx_software_reset_surface(pixels, 2, 2, 8);
	gfx_surface_t surface = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_t gfx	      = {0};
	gfx_driver_t *drv     = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_swapchain_t swapchain		= {0};
	gfx_target_t target			= {0};
	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &surface,
		.width	 = 2,
		.height	 = 2,
	};
	EXPECT_PTR(gfx_swapchain_init(&swapchain, &gfx, &swapchain_config), &swapchain);
	EXPECT_PTR(gfx_target_init_swapchain(&target, &swapchain), &target);
	EXPECT_EQ(t_gfx_software_memory_calls, 1);
	EXPECT_NOT_NULL(target.driver_data);
	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 3, 3), 0);
	EXPECT_EQ(t_gfx_software_memory_calls, 1);
	EXPECT_EQ(gfx_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_gfx_software_present_calls, 1);

	gfx_target_free(&target);
	EXPECT_NULL(target.driver_data);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_surface_target_failures)
{
	START;

	u8 pixels[4] = {0};
	t_gfx_software_reset_surface(pixels, 1, 1, 4);
	gfx_surface_t surface = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_t gfx	      = {0};
	gfx_driver_t *drv     = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_swapchain_config_t swapchain_config_null = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &(gfx_surface_t){.api = GFX_API_SOFTWARE},
		.width	 = 1,
		.height	 = 1,
	};
	EXPECT_NULL(gfx_swapchain_init(&(gfx_swapchain_t){0}, &gfx, &swapchain_config_null));
	t_gfx_software_memory_ret		= 1;
	gfx_swapchain_t invalid_swapchain	= {0};
	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &surface,
		.width	 = 1,
		.height	 = 1,
	};
	EXPECT_PTR(gfx_swapchain_init(&invalid_swapchain, &gfx, &swapchain_config), &invalid_swapchain);
	EXPECT_NULL(gfx_target_init_swapchain(&(gfx_target_t){0}, &invalid_swapchain));
	gfx_swapchain_free(&invalid_swapchain);
	t_gfx_software_memory_ret    = 0;
	t_gfx_software_memory.stride = 1;
	EXPECT_PTR(gfx_swapchain_init(&invalid_swapchain, &gfx, &swapchain_config), &invalid_swapchain);
	EXPECT_NULL(gfx_target_init_swapchain(&(gfx_target_t){0}, &invalid_swapchain));
	gfx_swapchain_free(&invalid_swapchain);
	EXPECT_EQ(drv->swapchain_resize(
			  &(gfx_swapchain_t){.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 1, .height = 1}, 2, 2),
		  0);
	t_gfx_software_reset_surface(pixels, 1, 1, 4);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(gfx_swapchain_init(&swapchain, &gfx, &swapchain_config), &swapchain);
	EXPECT_PTR(gfx_target_init_swapchain(&target, &swapchain), &target);
	t_gfx_software_memory_ret = 1;
	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 2, 2), 0);
	EXPECT_EQ(drv->swapchain_present(&(gfx_swapchain_t){0}), 1);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_swapchain_resize_rejects_invalid_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->swapchain_resize(NULL, 1, 1), 1);
	EXPECT_EQ(drv->swapchain_resize(&(gfx_swapchain_t){0}, 1, 1), 1);
	EXPECT_EQ(drv->swapchain_resize(&(gfx_swapchain_t){.surface = &(gfx_surface_t){0}}, 1, 1), 1);
	EXPECT_EQ(drv->swapchain_resize(
			  &(gfx_swapchain_t){.surface = &(gfx_surface_t){.ops = &(gfx_surface_ops_t){0}}, .width = 1, .height = 1}, 1, 1),
		  1);

	END;
}

TEST(gfx_software_draw_triangle)
{
	START;

	u8 pixels[64]		      = {0};
	gfx_t gfx		      = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 4, 4, 16), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx_vertex_2d_t vertices[] = {
		{.x = -1.0f, .y = -1.0f, .r = -1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = 1.0f, .y = -1.0f, .r = 0.0f, .g = 2.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = 1.0f, .r = 0.0f, .g = 0.0f, .b = 0.5f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = 1.0f, .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f},
		{.x = 1.0f, .y = -1.0f, .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 3), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 6), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	u8 readback[64] = {0};
	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = readback, .stride = 16}), 0);
	EXPECT_NE(readback[3], 0);

	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_draw_indexed_triangle)
{
	START;

	u8 pixels[64]		      = {0};
	gfx_t gfx		      = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 4, 4, 16), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t vertex_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&vertex_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&vertex_buffer);
	gfx_vertex_2d_t vertices[] = {
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = 1.0f, .y = -1.0f, .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = 1.0f, .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(vertices)), 0);
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&index_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&index_buffer);
	u32 indices[3] = {0, 1, 2};
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(indices)), 0);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &vertex_buffer), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &index_buffer), 0);
	EXPECT_EQ(gfx_draw_indexed(&frame, 3), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	u8 readback[64] = {0};
	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = readback, .stride = 16}), 0);
	EXPECT_NE(readback[3], 0);

	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_draw_buffer_failures)
{
	START;

	u8 pixels[64]		      = {0};
	gfx_t gfx		      = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 4, 4, 16), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t vertex_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&vertex_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&vertex_buffer);
	gfx_vertex_2d_t vertices[] = {
		{.x = -1.0f, .y = -1.0f},
		{.x = 1.0f, .y = -1.0f},
		{.x = -1.0f, .y = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(gfx_vertex_2d_t) * 2), 0);
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&index_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&index_buffer);
	u32 indices[3] = {0, 1, 2};
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(u32) * 2), 0);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &vertex_buffer), 0);
	EXPECT_EQ(gfx.drv->draw(&frame, 3, 0), 1);
	EXPECT_EQ(gfx.drv->draw_indexed(NULL, 3), 1);
	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 2), 1);
	EXPECT_EQ(gfx_buffer_bind(&frame, &index_buffer), 0);
	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 3), 1);
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(vertices)), 0);
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(indices)), 0);
	indices[2] = 3;
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(indices)), 0);
	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 3), 1);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_buffer_set_data_alloc_failure)
{
	START;

	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx.alloc		    = (alloc_t){.alloc = t_gfx_software_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_vertex_2d_t vertices[3] = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);
	log_set_quiet(0, 0);

	gfx.alloc = ALLOC_STD;
	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_buffer_init_static_uploads_data)
{
	START;

	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 1.0f},
		{.x = 2.0f},
		{.x = 3.0f},
	};
	gfx_buffer_t buffer = {0};

	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_VERTEX,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(vertices),
		.data  = vertices,
	};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &buffer_config), &buffer);
	EXPECT_EQ(buffer.size, sizeof(vertices));
	t_gfx_software_buffer_data_t *data = buffer.data;
	EXPECT_EQ(data->buf.used, sizeof(vertices));
	EXPECT_EQ(((gfx_vertex_2d_t *)data->buf.data)[0].x, 1.0f);
	EXPECT_EQ(((gfx_vertex_2d_t *)data->buf.data)[2].x, 3.0f);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_buffer_init_static_alloc_failure)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	t_gfx_software_alloc_count = 0;

	gfx_t gfx = {
		.drv   = drv,
		.alloc = {.alloc = t_gfx_software_alloc_fail_after_first, .realloc = alloc_realloc_std, .free = alloc_free_std},
	};
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_buffer_t buffer	    = {.gfx = &gfx};

	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_VERTEX,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(vertices),
		.data  = vertices,
	};

	log_set_quiet(0, 1);
	EXPECT_EQ(drv->buffer_init(&buffer, &buffer_config), 1);
	log_set_quiet(0, 0);
	EXPECT_NULL(buffer.data);

	END;
}

TEST(gfx_software_buffer_set_data_resize_failure)
{
	START;

	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx_vertex_2d_t triangle[3]  = {0};
	gfx_vertex_2d_t rectangle[4] = {0};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, triangle, sizeof(triangle)), 0);
	t_gfx_software_buffer_data_t *data = buffer.data;
	data->buf.alloc.realloc		   = t_gfx_software_realloc_fail;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, rectangle, sizeof(rectangle)), 1);
	log_set_quiet(0, 0);

	data->buf.alloc = ALLOC_STD;
	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_buffer_set_data_rejects_invalid_storage)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	char storage[1]			  = {0};
	t_gfx_software_buffer_data_t data = {.buf = {.data = storage, .size = sizeof(storage), .used = sizeof(storage)}};
	u32 values[2]			  = {1, 2};

	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.size = sizeof(values),
		.data = &data,
	};

	EXPECT_EQ(drv->buffer_set_data(&buffer, values, sizeof(values)), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_indexed_rejects_invalid_target)
{
	START;

	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	gfx_buffer_t vertex_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&vertex_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&vertex_buffer);
	gfx_vertex_2d_t vertices[3] = {0};
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(vertices)), 0);
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&index_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&index_buffer);
	u32 indices[3] = {0, 1, 2};
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(indices)), 0);
	gfx_frame_t frame = {
		.gfx	       = &gfx,
		.vertex_buffer = &vertex_buffer,
		.index_buffer  = &index_buffer,
		.active	       = 1,
	};

	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 3), 1);

	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_surface_pass_begin_and_draw_clips)
{
	START;

	u8 pixels[64] = {0};
	t_gfx_software_reset_surface(pixels, 4, 4, 16);
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_t gfx		      = {0};
	gfx_swapchain_t swapchain     = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_surface_scene(&gfx, &swapchain, &target, &render_pass, &framebuffer, &surface), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx_vertex_2d_t vertices[] = {
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = 1.0f, .y = -1.0f, .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = 1.0f, .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);

	gfx_frame_t frame	      = {0};
	gfx_pass_config_t pass_config = {
		.clear	  = {0.0f, 0.0f, 0.0f, 1.0f},
		.viewport = {1, 1, 4, 4},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_driver_direct_branches)
{
	START;

	u8 pixels[16]	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_target_t target_1 = {
		.gfx	= &gfx,
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_NONE,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_EQ(drv->target_init(&target_1), 1);
	t_gfx_software_reset_surface(pixels, 1, 1, 4);
	gfx_swapchain_t swapchain_1 = {
		.surface = &(gfx_surface_t){.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops},
		.format	 = GFX_FORMAT_RGBA8,
		.width	 = 1,
		.height	 = 1,
	};
	gfx_target_t target = {
		.gfx	   = &(gfx_t){.data = gfx.data, .alloc = (alloc_t){.alloc = t_gfx_software_alloc_fail}},
		.type	   = GFX_TARGET_SWAPCHAIN,
		.format	   = GFX_FORMAT_RGBA8,
		.swapchain = &swapchain_1,
		.width	   = 1,
		.height	   = 1,
	};
	EXPECT_EQ(drv->target_init(&target), 1);
	drv->target_free(NULL);

	EXPECT_EQ(drv->target_read(&(gfx_target_t){.gfx = &gfx}, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);

	gfx_frame_t frame		   = {.gfx = &gfx};
	gfx_render_pass_t pass		   = {0};
	gfx_surface_t software_surface	   = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_swapchain_t software_swapchain = {
		.surface = &software_surface,
		.format	 = GFX_FORMAT_RGBA8,
		.width	 = 1,
		.height	 = 1,
	};
	t_gfx_software_surface_target_data_t surface_data = {.memory = {0}};

	gfx_target_t invalid_surface_target = {
		.type	     = GFX_TARGET_SWAPCHAIN,
		.format	     = GFX_FORMAT_RGBA8,
		.swapchain   = &software_swapchain,
		.width	     = 1,
		.height	     = 1,
		.driver_data = &surface_data,
	};
	gfx_framebuffer_t invalid_surface_framebuffer = {.target = &invalid_surface_target, .render_pass = &pass};

	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.target = &(gfx_target_t){.type = GFX_TARGET_NONE}, .render_pass = &pass}, &frame),
		  0);
	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.target = &(gfx_target_t){.type = GFX_TARGET_SWAPCHAIN}, .render_pass = &pass}, &frame),
		  1);
	EXPECT_EQ(drv->framebuffer_pass_begin(&invalid_surface_framebuffer, &frame), 1);
	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.target = &(gfx_target_t){.type = GFX_TARGET_MEMORY}, .render_pass = &pass}, &frame),
		  1);
	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.target = &(gfx_target_t){.type = (gfx_target_type_t)99}, .render_pass = &pass}, &frame),
		  1);
	drv->buffer_free(NULL);
	gfx_buffer_t buffer = {.gfx = &gfx};
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_software_alloc_fail};
	EXPECT_EQ(drv->buffer_init(&buffer, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}), 1);
	gfx.alloc = ALLOC_STD;
	EXPECT_EQ(drv->draw(&(gfx_frame_t){.gfx = &gfx, .vertex_buffer = &(gfx_buffer_t){.gfx = &gfx, .data = (void *)1}}, 3, 0), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_driver_callback_failures)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->target_init(NULL), 1);
	EXPECT_EQ(drv->target_init(&(gfx_target_t){.type = GFX_TARGET_NONE, .gfx = &(gfx_t){.data = (void *)1}}), 1);
	EXPECT_EQ(drv->target_read(NULL, &(gfx_memory_readback_config_t){0}), 1);
	EXPECT_EQ(drv->framebuffer_pass_begin(NULL, NULL), 1);
	EXPECT_EQ(drv->buffer_init(NULL, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}), 1);
	EXPECT_EQ(drv->buffer_set_data(NULL, NULL, 0), 1);
	EXPECT_EQ(drv->buffer_bind(NULL, NULL), 1);
	EXPECT_EQ(drv->draw(NULL, 3, 0), 1);
	EXPECT_EQ(drv->draw(&(gfx_frame_t){.gfx = &(gfx_t){.data = (void *)1}}, 2, 0), 1);
	EXPECT_EQ(drv->end(NULL), 1);

	END;
}

STEST(gfx_software)
{
	SSTART;
	RUN(gfx_software_driver_is_registered);
	RUN(gfx_software_init_failures);
	RUN(gfx_software_memory_pass_clears_target);
	RUN(gfx_software_memory_pass_load_does_not_clear);
	RUN(gfx_software_surface_target_lifecycle);
	RUN(gfx_software_surface_target_failures);
	RUN(gfx_software_swapchain_resize_rejects_invalid_direct);
	RUN(gfx_software_draw_triangle);
	RUN(gfx_software_draw_indexed_triangle);
	RUN(gfx_software_draw_buffer_failures);
	RUN(gfx_software_buffer_set_data_alloc_failure);
	RUN(gfx_software_buffer_init_static_uploads_data);
	RUN(gfx_software_buffer_init_static_alloc_failure);
	RUN(gfx_software_buffer_set_data_resize_failure);
	RUN(gfx_software_buffer_set_data_rejects_invalid_storage);
	RUN(gfx_software_draw_indexed_rejects_invalid_target);
	RUN(gfx_software_surface_pass_begin_and_draw_clips);
	RUN(gfx_software_driver_direct_branches);
	RUN(gfx_software_driver_callback_failures);
	SEND;
}
