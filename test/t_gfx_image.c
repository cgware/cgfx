#include "gfx_driver.h"
#include "test.h"

static int t_gfx_image_init_calls;
static int t_gfx_image_free_calls;
static int t_gfx_image_read_calls;
static int t_gfx_image_init_ret;
static int t_gfx_image_read_ret;
static gfx_image_t *t_gfx_image_image;
static const gfx_memory_readback_config_t *t_gfx_image_read_config;

static int t_gfx_image_init(gfx_image_t *image)
{
	t_gfx_image_init_calls++;
	t_gfx_image_image = image;
	return t_gfx_image_init_ret;
}

static void t_gfx_image_free(gfx_image_t *image)
{
	t_gfx_image_free_calls++;
	t_gfx_image_image = image;
}

static int t_gfx_image_read(gfx_image_t *image, const gfx_memory_readback_config_t *config)
{
	t_gfx_image_read_calls++;
	t_gfx_image_image	= image;
	t_gfx_image_read_config = config;
	return t_gfx_image_read_ret;
}

static gfx_driver_t t_gfx_image_driver = {
	.image_init = t_gfx_image_init,
	.image_free = t_gfx_image_free,
	.image_read = t_gfx_image_read,
};

static void t_gfx_image_reset(void)
{
	t_gfx_image_init_calls	= 0;
	t_gfx_image_free_calls	= 0;
	t_gfx_image_read_calls	= 0;
	t_gfx_image_init_ret	= 0;
	t_gfx_image_read_ret	= 0;
	t_gfx_image_image	= NULL;
	t_gfx_image_read_config = NULL;
}

TEST(gfx_image_memory_rejects_invalid_args)
{
	START;
	t_gfx_image_reset();
	u8 pixels[4] = {0};
	gfx_t gfx    = {.drv = &t_gfx_image_driver};
	gfx_image_t image;

	EXPECT_NULL(gfx_image_init_memory(
		NULL,
		&gfx,
		&(gfx_image_memory_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 1, .height = 1, .stride = 4}));
	EXPECT_NULL(gfx_image_init_memory(
		&image,
		NULL,
		&(gfx_image_memory_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 1, .height = 1, .stride = 4}));
	EXPECT_NULL(gfx_image_init_memory(&image, &gfx, NULL));
	EXPECT_NULL(gfx_image_init_memory(
		&image,
		&gfx,
		&(gfx_image_memory_config_t){.format = GFX_FORMAT_NONE, .data = pixels, .width = 1, .height = 1, .stride = 4}));
	EXPECT_NULL(gfx_image_init_memory(
		&image,
		&gfx,
		&(gfx_image_memory_config_t){.format = GFX_FORMAT_RGBA8, .data = NULL, .width = 1, .height = 1, .stride = 4}));
	EXPECT_NULL(gfx_image_init_memory(
		&image,
		&gfx,
		&(gfx_image_memory_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 0, .height = 1, .stride = 4}));
	EXPECT_NULL(gfx_image_init_memory(
		&image,
		&gfx,
		&(gfx_image_memory_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 1, .height = 0, .stride = 4}));
	EXPECT_NULL(gfx_image_init_memory(
		&image,
		&gfx,
		&(gfx_image_memory_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 1, .height = 1, .stride = 3}));
	EXPECT_EQ(t_gfx_image_init_calls, 0);
	END;
}

TEST(gfx_image_memory_success_and_free)
{
	START;
	t_gfx_image_reset();
	u8 pixels[8] = {0};
	gfx_t gfx    = {.drv = &t_gfx_image_driver};
	gfx_image_t image;

	EXPECT_PTR(gfx_image_init_memory(
			   &image,
			   &gfx,
			   &(gfx_image_memory_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 1, .height = 2, .stride = 4}),
		   &image);
	EXPECT_EQ(t_gfx_image_init_calls, 1);
	EXPECT_PTR(t_gfx_image_image, &image);
	EXPECT_EQ(image.origin, GFX_IMAGE_ORIGIN_MEMORY);
	EXPECT_EQ(image.height, 2);

	gfx_image_free(&image);
	EXPECT_EQ(t_gfx_image_free_calls, 1);
	EXPECT_EQ(image.origin, GFX_IMAGE_ORIGIN_NONE);
	END;
}

TEST(gfx_image_memory_driver_failure_clears_image)
{
	START;
	t_gfx_image_reset();
	t_gfx_image_init_ret = 1;
	u8 pixels[4]	     = {0};
	gfx_t gfx	     = {.drv = &t_gfx_image_driver};
	gfx_image_t image;

	EXPECT_NULL(gfx_image_init_memory(
		&image,
		&gfx,
		&(gfx_image_memory_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 1, .height = 1, .stride = 4}));
	EXPECT_EQ(t_gfx_image_init_calls, 1);
	EXPECT_EQ(t_gfx_image_free_calls, 1);
	EXPECT_EQ(image.origin, GFX_IMAGE_ORIGIN_NONE);
	END;
}

TEST(gfx_image_read_validates_and_dispatches)
{
	START;
	t_gfx_image_reset();
	u8 pixels[4]   = {0};
	u8 readback[4] = {0};
	gfx_t gfx      = {.drv = &t_gfx_image_driver};
	gfx_image_t image;

	EXPECT_PTR(gfx_image_init_memory(
			   &image,
			   &gfx,
			   &(gfx_image_memory_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 1, .height = 1, .stride = 4}),
		   &image);
	EXPECT_EQ(gfx_image_read(NULL, &(gfx_memory_readback_config_t){.data = readback, .stride = 4}), 1);
	EXPECT_EQ(gfx_image_read(&image, NULL), 1);
	EXPECT_EQ(gfx_image_read(&image, &(gfx_memory_readback_config_t){.data = NULL, .stride = 4}), 1);
	EXPECT_EQ(gfx_image_read(&image, &(gfx_memory_readback_config_t){.data = readback, .stride = 3}), 1);
	EXPECT_EQ(gfx_image_read(&image, &(gfx_memory_readback_config_t){.data = readback, .stride = 4}), 0);
	EXPECT_EQ(t_gfx_image_read_calls, 1);
	EXPECT_PTR(t_gfx_image_image, &image);
	EXPECT_PTR(t_gfx_image_read_config->data, readback);
	gfx_image_free(&image);
	END;
}

STEST(gfx_image)
{
	SSTART;
	RUN(gfx_image_memory_rejects_invalid_args);
	RUN(gfx_image_memory_success_and_free);
	RUN(gfx_image_memory_driver_failure_clears_image);
	RUN(gfx_image_read_validates_and_dispatches);
	SEND;
}
