#include "test.h"

#include "gfx_driver.h"

static void *t_gfx_software_alloc_fail(alloc_t *alloc, size_t size)
{
	(void)alloc;
	(void)size;
	return NULL;
}

static gfx_driver_t *t_gfx_software_driver(void)
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GFX_DRIVER_TYPE) {
			gfx_driver_t *drv = i->data;
			if (strv_eq(strv_cstr(drv->name), STRV("software"))) {
				return drv;
			}
		}
	}

	return NULL;
}

TEST(gfx_software_driver_is_registered)
{
	START;

	EXPECT_NE(t_gfx_software_driver(), NULL);

	END;
}

TEST(gfx_software_init_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){.alloc = ALLOC_STD}), 1);

	END;
}

TEST(gfx_software_init_null_config)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(drv->init(&gfx, NULL), 1);

	END;
}

TEST(gfx_software_init_success)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD}), &gfx);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_init_alloc_failure)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(gfx_init(&gfx, drv, &(gfx_config_t){.alloc = {.alloc = t_gfx_software_alloc_fail}}), NULL);

	END;
}

TEST(gfx_software_free_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(drv->free(NULL), 1);

	END;
}

TEST(gfx_software_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NE(gfx.drv, NULL);

	EXPECT_EQ(gfx.drv->free(&gfx), 1);

	END;
}

TEST(gfx_software_set_target_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NE(gfx.drv, NULL);

	EXPECT_EQ(gfx.drv->set_target(&gfx, &(gfx_target_t){0}), 1);

	END;
}

TEST(gfx_software_set_target_invalid_type)
{
	START;

	u8 pixels[4]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD});

	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	 = GFX_TARGET_NONE,
					 .format = GFX_FORMAT_RGBA8,
					 .data	 = pixels,
					 .width	 = 1,
					 .height = 1,
					 .stride = 4,
				 }),
		  1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_invalid_format)
{
	START;

	u8 pixels[4]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD});

	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	 = GFX_TARGET_MEMORY,
					 .format = GFX_FORMAT_NONE,
					 .data	 = pixels,
					 .width	 = 1,
					 .height = 1,
					 .stride = 4,
				 }),
		  1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_invalid_stride)
{
	START;

	u8 pixels[4]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD});

	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	 = GFX_TARGET_MEMORY,
					 .format = GFX_FORMAT_RGBA8,
					 .data	 = pixels,
					 .width	 = 1,
					 .height = 1,
					 .stride = 3,
				 }),
		  1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_set_target_success)
{
	START;

	u8 pixels[4]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD});

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

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_color_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NE(gfx.drv, NULL);

	EXPECT_EQ(gfx.drv->clear_color(&gfx, 0.0f, 0.0f, 0.0f, 0.0f), 1);

	END;
}

TEST(gfx_software_clear_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_software_driver(),
	};
	EXPECT_NE(gfx.drv, NULL);

	EXPECT_EQ(gfx.drv->clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_software_clear_without_target)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD});

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_zero_buffers)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD});

	EXPECT_EQ(gfx_clear(&gfx, 0), 0);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_writes_first_pixel)
{
	START;

	u8 pixels[8]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 2,
			       .height = 1,
			       .stride = 8,
		       });
	gfx_clear_color(&gfx, 0.1f, 0.2f, 0.3f, 1.0f);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(pixels[0], 26);
	EXPECT_EQ(pixels[1], 51);
	EXPECT_EQ(pixels[2], 77);
	EXPECT_EQ(pixels[3], 255);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_clear_writes_second_pixel)
{
	START;

	u8 pixels[8]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 2,
			       .height = 1,
			       .stride = 8,
		       });
	gfx_clear_color(&gfx, 0.1f, 0.2f, 0.3f, 1.0f);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

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

	u8 pixels[16]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 1,
			       .height = 2,
			       .stride = 8,
		       });
	gfx_clear_color(&gfx, 1.0f, 0.0f, 0.0f, 1.0f);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(pixels[8], 255);
	EXPECT_EQ(pixels[9], 0);
	EXPECT_EQ(pixels[10], 0);
	EXPECT_EQ(pixels[11], 255);

	gfx_free(&gfx);
	END;
}

STEST(gfx_software)
{
	SSTART;

	RUN(gfx_software_driver_is_registered);
	RUN(gfx_software_init_null_gfx);
	RUN(gfx_software_init_null_config);
	RUN(gfx_software_init_success);
	RUN(gfx_software_init_alloc_failure);
	RUN(gfx_software_free_null_gfx);
	RUN(gfx_software_free_null_data);
	RUN(gfx_software_set_target_null_data);
	RUN(gfx_software_set_target_invalid_type);
	RUN(gfx_software_set_target_invalid_format);
	RUN(gfx_software_set_target_invalid_stride);
	RUN(gfx_software_set_target_success);
	RUN(gfx_software_clear_color_null_data);
	RUN(gfx_software_clear_null_data);
	RUN(gfx_software_clear_without_target);
	RUN(gfx_software_clear_zero_buffers);
	RUN(gfx_software_clear_writes_first_pixel);
	RUN(gfx_software_clear_writes_second_pixel);
	RUN(gfx_software_clear_uses_stride);

	SEND;
}
