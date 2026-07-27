#include "alloc.h"
#include "gfx_driver.h"

#include "test.h"

static gfx_driver_t *t_gfx_none_driver(void)
{
	return gfx_driver_find(STRV("none"));
}

static int t_gfx_none_init(gfx_t *gfx)
{
	gfx_driver_t *drv = t_gfx_none_driver();
	return drv == NULL || gfx_init(gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD) != gfx;
}

TEST(gfx_none_driver_is_registered)
{
	START;

	EXPECT_NOT_NULL(t_gfx_none_driver());

	END;
}

TEST(gfx_none_init_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);

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

TEST(gfx_none_free_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->free(NULL), 1);

	END;
}

TEST(gfx_none_clear_color_success)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_none_init(&gfx), 0);

	EXPECT_EQ(gfx_clear_color(&gfx, 0.0f, 0.0f, 0.0f, 1.0f), 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_none_clear_color_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->clear_color(NULL, 0.0f, 0.0f, 0.0f, 0.0f), 1);

	END;
}

TEST(gfx_none_viewport_success)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_none_init(&gfx), 0);

	EXPECT_EQ(gfx_viewport(&gfx, 0, 0, 1, 1), 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_none_viewport_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->viewport(NULL, 0, 0, 1, 1), 1);

	END;
}

TEST(gfx_none_set_target_success)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	EXPECT_EQ(t_gfx_none_init(&gfx), 0);
	gfx_target_t target = {
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_none_set_target_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);
	gfx_target_t target = {0};

	EXPECT_EQ(drv->set_target(NULL, &target), 1);

	END;
}

TEST(gfx_none_clear_success)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_none_init(&gfx), 0);

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_none_clear_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->clear(NULL, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_none_buffer_success)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_none_init(&gfx), 0);
	gfx_buffer_t buffer = {0};
	int data	    = 1;

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, &data, sizeof(data)), 0);
	gfx_buffer_free(&buffer);

	gfx_free(&gfx);
	END;
}

TEST(gfx_none_draw_success)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_none_init(&gfx), 0);
	gfx_shader_t shader = {0};
	EXPECT_NOT_NULL(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("none")}));
	gfx_pipeline_t pipeline = {.gfx = &gfx, .data = (void *)1};
	gfx_buffer_t buffer	= {.gfx = &gfx};
	gfx_target_t target	= {0};
	gfx_frame_t frame	= {0};
	EXPECT_EQ(gfx_begin(&gfx, &frame, &(gfx_frame_config_t){.target = &target}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_shader_free(&shader);
	gfx_free(&gfx);
	END;
}

TEST(gfx_none_pipeline_success)
{
	START;

	gfx_t gfx = {0};
	EXPECT_EQ(t_gfx_none_init(&gfx), 0);
	gfx_shader_t shader = {0};
	EXPECT_NOT_NULL(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("none")}));
	gfx_pipeline_t pipeline = {0};

	EXPECT_NOT_NULL(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.vs = shader, .fs = shader}));

	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	gfx_free(&gfx);
	END;
}

TEST(gfx_none_draw_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->draw(NULL, 3, 0), 1);

	END;
}

TEST(gfx_none_begin_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->begin(NULL), 1);

	END;
}

TEST(gfx_none_end_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->end(NULL), 1);

	END;
}

TEST(gfx_none_draw_null_buffer)
{
	START;

	EXPECT_EQ(gfx_buffer_bind(NULL, NULL), 1);

	END;
}

STEST(gfx_none)
{
	SSTART;

	RUN(gfx_none_driver_is_registered);
	RUN(gfx_none_init_null_gfx);
	RUN(gfx_none_init_success);
	RUN(gfx_none_free_null_gfx);
	RUN(gfx_none_clear_color_success);
	RUN(gfx_none_clear_color_null_gfx);
	RUN(gfx_none_viewport_success);
	RUN(gfx_none_viewport_null_gfx);
	RUN(gfx_none_set_target_success);
	RUN(gfx_none_set_target_null_gfx);
	RUN(gfx_none_clear_success);
	RUN(gfx_none_clear_null_gfx);
	RUN(gfx_none_buffer_success);
	RUN(gfx_none_pipeline_success);
	RUN(gfx_none_draw_success);
	RUN(gfx_none_draw_null_gfx);
	RUN(gfx_none_begin_null_frame);
	RUN(gfx_none_end_null_frame);
	RUN(gfx_none_draw_null_buffer);

	SEND;
}
