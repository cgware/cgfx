#include "gfx_driver.h"

#include "test.h"

static gfx_driver_t *t_gfx_software_driver(void)
{
	return gfx_driver_find(STRV("software"));
}

static int t_gfx_software_present_calls;
static int t_gfx_software_present_ret;
static int t_gfx_software_memory_calls;
static int t_gfx_software_memory_ret;
static gfx_surface_memory_t t_gfx_software_memory;

static int t_gfx_software_surface_present(gfx_surface_t *surface)
{
	(void)surface;
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
	t_gfx_software_memory	     = (gfx_surface_memory_t){
		       .format = GFX_FORMAT_RGBA8,
		       .data   = pixels,
		       .width  = width,
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

static int t_gfx_software_scene(gfx_t *gfx, gfx_target_t *target, gfx_render_pass_t *render_pass, gfx_framebuffer_t *framebuffer,
				u8 *pixels, u16 width, u16 height, size_t stride)
{
	gfx_driver_t *drv = t_gfx_software_driver();
	if (drv == NULL || gfx_init(gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD) != gfx) {
		return 1;
	}
	if (gfx_target_init_memory(target,
				   gfx,
				   &(gfx_memory_target_config_t){
					   .format = GFX_FORMAT_RGBA8,
					   .data   = pixels,
					   .width  = width,
					   .height = height,
					   .stride = stride,
				   }) != target) {
		return 1;
	}
	if (gfx_render_pass_init(render_pass,
				 gfx,
				 &(gfx_render_pass_config_t){
					 .color_format = GFX_FORMAT_RGBA8,
					 .load	       = GFX_LOAD_CLEAR,
					 .store	       = GFX_STORE_STORE,
				 }) != render_pass) {
		return 1;
	}
	return gfx_framebuffer_init(framebuffer, target, render_pass) != framebuffer;
}

static int t_gfx_software_surface_scene(gfx_t *gfx, gfx_target_t *target, gfx_render_pass_t *render_pass, gfx_framebuffer_t *framebuffer,
					gfx_surface_t *surface)
{
	gfx_driver_t *drv = t_gfx_software_driver();
	if (drv == NULL || gfx_init(gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD) != gfx) {
		return 1;
	}
	if (gfx_target_init_surface(target,
				    gfx,
				    &(gfx_surface_target_config_t){
					    .format  = GFX_FORMAT_RGBA8,
					    .surface = surface,
					    .width   = 2,
					    .height  = 2,
				    }) != target) {
		return 1;
	}
	if (gfx_render_pass_init(render_pass,
				 gfx,
				 &(gfx_render_pass_config_t){
					 .color_format = GFX_FORMAT_RGBA8,
					 .load	       = GFX_LOAD_CLEAR,
					 .store	       = GFX_STORE_STORE,
				 }) != render_pass) {
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

	gfx_target_t target = {0};
	EXPECT_PTR(gfx_target_init_memory(&target,
					  &gfx,
					  &(gfx_memory_target_config_t){
						  .format = GFX_FORMAT_RGBA8,
						  .data	  = pixels,
						  .width  = 2,
						  .height = 2,
						  .stride = 8,
					  }),
		   &target);
	gfx_render_pass_t render_pass = {0};
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = GFX_FORMAT_RGBA8,
						.load	      = GFX_LOAD_CLEAR,
						.store	      = GFX_STORE_STORE,
					}),
		   &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer,
					     &frame,
					     &(gfx_pass_config_t){
						     .clear = {1.0f, 0.0f, 0.0f, 1.0f},
					     }),
		  0);
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
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = GFX_FORMAT_RGBA8,
						.load	      = GFX_LOAD_LOAD,
						.store	      = GFX_STORE_STORE,
					}),
		   &render_pass);
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

	gfx_target_t target = {0};
	EXPECT_PTR(gfx_target_init_surface(&target,
					   &gfx,
					   &(gfx_surface_target_config_t){
						   .format  = GFX_FORMAT_RGBA8,
						   .surface = &surface,
						   .width   = 2,
						   .height  = 2,
					   }),
		   &target);
	EXPECT_EQ(t_gfx_software_memory_calls, 1);
	EXPECT_NOT_NULL(target.driver_data);
	EXPECT_EQ(gfx_target_resize(&target, 3, 3), 0);
	EXPECT_EQ(t_gfx_software_memory_calls, 2);
	EXPECT_EQ(gfx_target_present(&target), 0);
	EXPECT_EQ(t_gfx_software_present_calls, 1);

	gfx_target_free(&target);
	EXPECT_NULL(target.driver_data);
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

	EXPECT_NULL(gfx_target_init_surface(&(gfx_target_t){0},
					    &gfx,
					    &(gfx_surface_target_config_t){
						    .format  = GFX_FORMAT_RGBA8,
						    .surface = &(gfx_surface_t){.api = GFX_API_SOFTWARE},
						    .width   = 1,
						    .height  = 1,
					    }));
	t_gfx_software_memory_ret = 1;
	EXPECT_NULL(gfx_target_init_surface(&(gfx_target_t){0},
					    &gfx,
					    &(gfx_surface_target_config_t){
						    .format  = GFX_FORMAT_RGBA8,
						    .surface = &surface,
						    .width   = 1,
						    .height  = 1,
					    }));
	t_gfx_software_memory_ret    = 0;
	t_gfx_software_memory.stride = 1;
	EXPECT_NULL(gfx_target_init_surface(&(gfx_target_t){0},
					    &gfx,
					    &(gfx_surface_target_config_t){
						    .format  = GFX_FORMAT_RGBA8,
						    .surface = &surface,
						    .width   = 1,
						    .height  = 1,
					    }));
	EXPECT_EQ(drv->target_resize(
			  &(gfx_target_t){
				  .type = GFX_TARGET_SURFACE, .format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 1, .height = 1},
			  2,
			  2),
		  1);
	t_gfx_software_reset_surface(pixels, 1, 1, 4);
	gfx_target_t target = {0};
	EXPECT_PTR(gfx_target_init_surface(&target,
					   &gfx,
					   &(gfx_surface_target_config_t){
						   .format  = GFX_FORMAT_RGBA8,
						   .surface = &surface,
						   .width   = 1,
						   .height  = 1,
					   }),
		   &target);
	t_gfx_software_memory_ret = 1;
	EXPECT_EQ(gfx_target_resize(&target, 2, 2), 1);
	EXPECT_EQ(drv->target_present(&(gfx_target_t){0}), 1);

	gfx_target_free(&target);
	gfx_free(&gfx);
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
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
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

TEST(gfx_software_surface_pass_begin_and_draw_clips)
{
	START;

	u8 pixels[64] = {0};
	t_gfx_software_reset_surface(pixels, 4, 4, 16);
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_t gfx		      = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_surface_scene(&gfx, &target, &render_pass, &framebuffer, &surface), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	gfx_vertex_2d_t vertices[] = {
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = 1.0f, .y = -1.0f, .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = 1.0f, .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer,
					     &frame,
					     &(gfx_pass_config_t){
						     .clear    = {0.0f, 0.0f, 0.0f, 1.0f},
						     .viewport = {1, 1, 4, 4},
					     }),
		  0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
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

	EXPECT_EQ(drv->target_init(&(gfx_target_t){.gfx	   = &gfx,
						   .type   = GFX_TARGET_MEMORY,
						   .format = GFX_FORMAT_NONE,
						   .data   = pixels,
						   .width  = 1,
						   .height = 1,
						   .stride = 4}),
		  1);
	t_gfx_software_reset_surface(pixels, 1, 1, 4);
	EXPECT_EQ(
		drv->target_init(&(gfx_target_t){.gfx  = &(gfx_t){.data = gfx.data, .alloc = (alloc_t){.alloc = t_gfx_software_alloc_fail}},
						 .type = GFX_TARGET_SURFACE,
						 .format  = GFX_FORMAT_RGBA8,
						 .surface = &(gfx_surface_t){.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops},
						 .width	  = 1,
						 .height  = 1}),
		1);
	drv->target_free(NULL);
	EXPECT_EQ(drv->target_read(&(gfx_target_t){.gfx = &gfx}, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);
	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.target = &(gfx_target_t){.type = GFX_TARGET_NONE}, .render_pass = &(gfx_render_pass_t){0}},
			  &(gfx_frame_t){.gfx = &gfx}),
		  0);
	EXPECT_EQ(drv->framebuffer_pass_begin(&(gfx_framebuffer_t){.target	= &(gfx_target_t){.type = GFX_TARGET_SURFACE},
								   .render_pass = &(gfx_render_pass_t){0}},
					      &(gfx_frame_t){.gfx = &gfx}),
		  1);
	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.target	    = &(gfx_target_t){.type	   = GFX_TARGET_SURFACE,
									      .format	   = GFX_FORMAT_RGBA8,
									      .surface	   = &(gfx_surface_t){.api = GFX_API_SOFTWARE,
													      .ops = &t_gfx_software_surface_ops},
									      .width	   = 1,
									      .height	   = 1,
									      .driver_data = &(gfx_target_t){.type = GFX_TARGET_MEMORY}},
					       .render_pass = &(gfx_render_pass_t){0}},
			  &(gfx_frame_t){.gfx = &gfx}),
		  1);
	EXPECT_EQ(drv->framebuffer_pass_begin(&(gfx_framebuffer_t){.target	= &(gfx_target_t){.type = GFX_TARGET_MEMORY},
								   .render_pass = &(gfx_render_pass_t){0}},
					      &(gfx_frame_t){.gfx = &gfx}),
		  1);
	EXPECT_EQ(drv->framebuffer_pass_begin(&(gfx_framebuffer_t){.target	= &(gfx_target_t){.type = (gfx_target_type_t)99},
								   .render_pass = &(gfx_render_pass_t){0}},
					      &(gfx_frame_t){.gfx = &gfx}),
		  1);
	drv->buffer_free(NULL);
	gfx_buffer_t buffer = {.gfx = &gfx};
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_software_alloc_fail};
	EXPECT_EQ(drv->buffer_init(&buffer, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), 1);
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
	EXPECT_EQ(drv->buffer_init(NULL, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), 1);
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
	RUN(gfx_software_draw_triangle);
	RUN(gfx_software_surface_pass_begin_and_draw_clips);
	RUN(gfx_software_driver_direct_branches);
	RUN(gfx_software_driver_callback_failures);
	SEND;
}
