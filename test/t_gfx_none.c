#include "gfx_driver.h"

#include "test.h"

static gfx_driver_t *t_gfx_none_driver(void)
{
	return gfx_driver_find(STRV("none"));
}

static int t_gfx_none_init(gfx_t *gfx)
{
	gfx_driver_t *drv = t_gfx_none_driver();
	return drv == NULL || gfx_init(gfx, drv, &(gfx_config_t){0}) != gfx;
}

TEST(gfx_none_driver_is_registered)
{
	START;

	EXPECT_NE(t_gfx_none_driver(), NULL);

	END;
}

TEST(gfx_none_init_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_none_init_success)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(gfx_init(&gfx, drv, &(gfx_config_t){0}), &gfx);

	gfx_free(&gfx);
	END;
}

TEST(gfx_none_free_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_none_driver();
	EXPECT_NE(drv, NULL);

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
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(drv->clear_color(NULL, 0.0f, 0.0f, 0.0f, 0.0f), 1);

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
	EXPECT_NE(drv, NULL);
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
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(drv->clear(NULL, GFX_CLEAR_COLOR_BUFFER), 1);

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
	RUN(gfx_none_set_target_success);
	RUN(gfx_none_set_target_null_gfx);
	RUN(gfx_none_clear_success);
	RUN(gfx_none_clear_null_gfx);

	SEND;
}
