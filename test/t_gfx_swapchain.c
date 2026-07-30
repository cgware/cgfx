#include "gfx_swapchain.h"

#include "gfx_driver.h"
#include "test.h"

static int t_gfx_swapchain_init_calls;
static int t_gfx_swapchain_free_calls;
static int t_gfx_swapchain_resize_calls;
static int t_gfx_swapchain_present_calls;
static int t_gfx_swapchain_init_ret;
static int t_gfx_swapchain_resize_ret;
static int t_gfx_swapchain_present_ret;
static gfx_swapchain_t *t_gfx_swapchain;
static const gfx_swapchain_config_t *t_gfx_swapchain_config;
static u16 t_gfx_swapchain_width;
static u16 t_gfx_swapchain_height;

static int t_gfx_swapchain_init(gfx_swapchain_t *swapchain, const gfx_swapchain_config_t *config)
{
	t_gfx_swapchain_init_calls++;
	t_gfx_swapchain	       = swapchain;
	t_gfx_swapchain_config = config;
	return t_gfx_swapchain_init_ret;
}

static void t_gfx_swapchain_free(gfx_swapchain_t *swapchain)
{
	t_gfx_swapchain_free_calls++;
	t_gfx_swapchain = swapchain;
}

static int t_gfx_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height)
{
	t_gfx_swapchain_resize_calls++;
	t_gfx_swapchain	       = swapchain;
	t_gfx_swapchain_width  = width;
	t_gfx_swapchain_height = height;
	return t_gfx_swapchain_resize_ret;
}

static int t_gfx_swapchain_present(gfx_swapchain_t *swapchain)
{
	t_gfx_swapchain_present_calls++;
	t_gfx_swapchain = swapchain;
	return t_gfx_swapchain_present_ret;
}

static gfx_driver_t t_gfx_swapchain_driver = {
	.name		   = "test-swapchain",
	.api		   = GFX_API_SOFTWARE,
	.swapchain_init	   = t_gfx_swapchain_init,
	.swapchain_free	   = t_gfx_swapchain_free,
	.swapchain_resize  = t_gfx_swapchain_resize,
	.swapchain_present = t_gfx_swapchain_present,
};

static void t_gfx_swapchain_reset(void)
{
	t_gfx_swapchain_init_calls    = 0;
	t_gfx_swapchain_free_calls    = 0;
	t_gfx_swapchain_resize_calls  = 0;
	t_gfx_swapchain_present_calls = 0;
	t_gfx_swapchain_init_ret      = 0;
	t_gfx_swapchain_resize_ret    = 0;
	t_gfx_swapchain_present_ret   = 0;
	t_gfx_swapchain		      = NULL;
	t_gfx_swapchain_config	      = NULL;
	t_gfx_swapchain_width	      = 0;
	t_gfx_swapchain_height	      = 0;
}

TEST(gfx_swapchain_init_rejects_invalid_args)
{
	START;

	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		      = {.drv = &t_gfx_swapchain_driver};
	gfx_swapchain_t swapchain     = {0};
	gfx_swapchain_config_t config = {.format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 1, .height = 1};

	EXPECT_NULL(gfx_swapchain_init(NULL, &gfx, &config));
	EXPECT_NULL(gfx_swapchain_init(&swapchain, NULL, &config));
	EXPECT_NULL(gfx_swapchain_init(&swapchain, &(gfx_t){0}, &config));
	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, NULL));
	EXPECT_NULL(gfx_swapchain_init(
		&swapchain, &gfx, &(gfx_swapchain_config_t){.format = GFX_FORMAT_NONE, .surface = &surface, .width = 1, .height = 1}));
	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &(gfx_swapchain_config_t){.format = GFX_FORMAT_RGBA8, .width = 1, .height = 1}));
	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &(gfx_surface_t){.api = GFX_API_OPENGL},
		.width	 = 1,
		.height	 = 1,
	};
	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &swapchain_config));
	EXPECT_NULL(gfx_swapchain_init(
		&swapchain, &gfx, &(gfx_swapchain_config_t){.format = GFX_FORMAT_RGBA8, .surface = &surface, .height = 1}));
	EXPECT_NULL(gfx_swapchain_init(
		&swapchain, &gfx, &(gfx_swapchain_config_t){.format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 1}));

	END;
}

TEST(gfx_swapchain_init_rejects_active_frame)
{
	START;

	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_frame_t frame	  = {0};
	gfx_t gfx		  = {.drv = &t_gfx_swapchain_driver, .frame = &frame};
	gfx_swapchain_t swapchain = {0};

	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &surface,
		.width	 = 1,
		.height	 = 1,
	};
	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &swapchain_config));

	END;
}

TEST(gfx_swapchain_init_success_and_failure)
{
	START;

	t_gfx_swapchain_reset();
	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		  = {.drv = &t_gfx_swapchain_driver};
	gfx_swapchain_t swapchain = {0};

	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &surface,
		.width	 = 2,
		.height	 = 3,
	};
	EXPECT_PTR(gfx_swapchain_init(&swapchain, &gfx, &swapchain_config), &swapchain);
	EXPECT_EQ(t_gfx_swapchain_init_calls, 1);
	EXPECT_PTR(t_gfx_swapchain, &swapchain);
	EXPECT_NOT_NULL(t_gfx_swapchain_config);
	EXPECT_PTR(swapchain.gfx, &gfx);
	EXPECT_PTR(swapchain.surface, &surface);
	EXPECT_EQ(swapchain.width, 2);
	EXPECT_EQ(swapchain.height, 3);

	gfx_swapchain_free(&swapchain);

	t_gfx_swapchain_reset();
	t_gfx_swapchain_init_ret = 1;
	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &swapchain_config));
	EXPECT_EQ(t_gfx_swapchain_free_calls, 1);
	EXPECT_NULL(swapchain.gfx);

	END;
}

TEST(gfx_swapchain_free_ignores_invalid_or_active)
{
	START;

	t_gfx_swapchain_reset();
	gfx_frame_t frame = {0};
	gfx_t gfx	  = {.drv = &t_gfx_swapchain_driver, .frame = &frame};

	gfx_swapchain_free(NULL);
	gfx_swapchain_free(&(gfx_swapchain_t){0});
	gfx_swapchain_free(&(gfx_swapchain_t){.gfx = &gfx});

	EXPECT_EQ(t_gfx_swapchain_free_calls, 0);

	END;
}

TEST(gfx_swapchain_free_calls_driver_and_clears)
{
	START;

	t_gfx_swapchain_reset();
	gfx_t gfx		  = {.drv = &t_gfx_swapchain_driver};
	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain = {.gfx = &gfx, .surface = &surface, .format = GFX_FORMAT_RGBA8, .width = 1, .height = 1};

	gfx_swapchain_free(&swapchain);

	EXPECT_EQ(t_gfx_swapchain_free_calls, 1);
	EXPECT_PTR(t_gfx_swapchain, &swapchain);
	EXPECT_NULL(swapchain.gfx);

	END;
}

TEST(gfx_swapchain_resize_rejects_invalid_args)
{
	START;

	gfx_t gfx	      = {.drv = &t_gfx_swapchain_driver};
	gfx_frame_t frame     = {0};
	gfx_surface_t surface = {.api = GFX_API_SOFTWARE};

	EXPECT_EQ(gfx_swapchain_resize(NULL, 1, 1), 1);
	EXPECT_EQ(gfx_swapchain_resize(&(gfx_swapchain_t){0}, 1, 1), 1);
	EXPECT_EQ(gfx_swapchain_resize(&(gfx_swapchain_t){.gfx = &(gfx_t){.frame = &frame}}, 1, 1), 1);
	EXPECT_EQ(gfx_swapchain_resize(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .format = GFX_FORMAT_RGBA8}, 0, 1), 1);
	EXPECT_EQ(gfx_swapchain_resize(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .format = GFX_FORMAT_RGBA8}, 1, 0), 1);

	END;
}

TEST(gfx_swapchain_resize_success_and_failure)
{
	START;

	t_gfx_swapchain_reset();
	gfx_t gfx		  = {.drv = &t_gfx_swapchain_driver};
	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain = {.gfx = &gfx, .surface = &surface, .format = GFX_FORMAT_RGBA8, .width = 2, .height = 3};

	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 4, 5), 0);
	EXPECT_EQ(t_gfx_swapchain_resize_calls, 1);
	EXPECT_EQ(t_gfx_swapchain_width, 4);
	EXPECT_EQ(t_gfx_swapchain_height, 5);
	EXPECT_EQ(swapchain.width, 4);
	EXPECT_EQ(swapchain.height, 5);

	t_gfx_swapchain_resize_ret = 1;
	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 6, 7), 1);
	EXPECT_EQ(swapchain.width, 4);
	EXPECT_EQ(swapchain.height, 5);

	END;
}

TEST(gfx_swapchain_present_rejects_invalid_args)
{
	START;

	gfx_surface_t surface = {.api = GFX_API_SOFTWARE};
	gfx_frame_t frame     = {0};
	gfx_t gfx	      = {.drv = &t_gfx_swapchain_driver};

	EXPECT_EQ(gfx_swapchain_present(NULL), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){0}), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &(gfx_t){0}}), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &(gfx_t){.drv = &(gfx_driver_t){0}}}), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &(gfx_t){.drv = &t_gfx_swapchain_driver, .frame = &frame}}), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &gfx}), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface}), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .width = 1}), 1);

	END;
}

TEST(gfx_swapchain_present_calls_driver)
{
	START;

	t_gfx_swapchain_reset();
	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		  = {.drv = &t_gfx_swapchain_driver};
	gfx_swapchain_t swapchain = {.gfx = &gfx, .surface = &surface, .format = GFX_FORMAT_RGBA8, .width = 1, .height = 1};

	EXPECT_EQ(gfx_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_gfx_swapchain_present_calls, 1);
	EXPECT_PTR(t_gfx_swapchain, &swapchain);

	t_gfx_swapchain_present_ret = 1;
	EXPECT_EQ(gfx_swapchain_present(&swapchain), 1);

	END;
}

STEST(gfx_swapchain)
{
	SSTART;
	RUN(gfx_swapchain_init_rejects_invalid_args);
	RUN(gfx_swapchain_init_rejects_active_frame);
	RUN(gfx_swapchain_init_success_and_failure);
	RUN(gfx_swapchain_free_ignores_invalid_or_active);
	RUN(gfx_swapchain_free_calls_driver_and_clears);
	RUN(gfx_swapchain_resize_rejects_invalid_args);
	RUN(gfx_swapchain_resize_success_and_failure);
	RUN(gfx_swapchain_present_rejects_invalid_args);
	RUN(gfx_swapchain_present_calls_driver);
	SEND;
}
