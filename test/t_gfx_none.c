#include "gfx_driver.h"

#include "test.h"

static gfx_driver_t *t_gfx_none_driver(void)
{
	return gfx_driver_find(STRV("none"));
}

TEST(gfx_none_driver_is_registered)
{
	START;

	EXPECT_NOT_NULL(t_gfx_none_driver());

	END;
}

TEST(gfx_none_init_success)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_free(&gfx);
	END;
}

TEST(gfx_none_driver_callbacks_handle_invalid_args)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);
	EXPECT_EQ(drv->free(NULL), 1);
	EXPECT_EQ(drv->framebuffer_pass_begin(NULL, NULL), 1);
	EXPECT_EQ(drv->draw(NULL, 3, 0), 1);
	EXPECT_EQ(drv->end(NULL), 1);

	END;
}

TEST(gfx_none_explicit_pass_success)
{
	START;

	u8 pixels[4]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_target_t target = {0};
	EXPECT_PTR(gfx_target_init_memory(&target,
					  &gfx,
					  &(gfx_memory_target_config_t){
						  .format = GFX_FORMAT_RGBA8,
						  .data	  = pixels,
						  .width  = 1,
						  .height = 1,
						  .stride = 4,
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
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 0);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	END;
}

TEST(gfx_none_draw_pipeline_success)
{
	START;

	u8 pixels[4]	  = {0};
	int data	  = 1;
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_target_t target = {0};
	EXPECT_PTR(gfx_target_init_memory(&target,
					  &gfx,
					  &(gfx_memory_target_config_t){
						  .format = GFX_FORMAT_RGBA8,
						  .data	  = pixels,
						  .width  = 1,
						  .height = 1,
						  .stride = 4,
					  }),
		   &target);
	gfx_render_pass_t render_pass = {0};
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = GFX_FORMAT_RGBA8,
						.load	      = GFX_LOAD_DONT_CARE,
						.store	      = GFX_STORE_STORE,
					}),
		   &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("none")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, &data, sizeof(data)), 0);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {0, 0, 1, 1}}), 0);
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
	gfx_free(&gfx);
	END;
}

TEST(gfx_none_surface_target_success)
{
	START;

	gfx_surface_t surface = {.api = GFX_API_NONE};
	gfx_t gfx	      = {0};
	gfx_driver_t *drv     = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

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
	EXPECT_EQ(gfx_target_resize(&target, 2, 3), 0);
	EXPECT_EQ(gfx_target_present(&target), 0);

	gfx_target_free(&target);
	gfx_free(&gfx);
	END;
}

STEST(gfx_none)
{
	SSTART;
	RUN(gfx_none_driver_is_registered);
	RUN(gfx_none_init_success);
	RUN(gfx_none_driver_callbacks_handle_invalid_args);
	RUN(gfx_none_explicit_pass_success);
	RUN(gfx_none_draw_pipeline_success);
	RUN(gfx_none_surface_target_success);
	SEND;
}
