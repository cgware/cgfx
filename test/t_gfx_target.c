#include "gfx_target.h"

#include "gfx_driver.h"
#include "test.h"

static int t_gfx_target_init_calls;
static int t_gfx_target_free_calls;
static int t_gfx_target_read_calls;
static int t_gfx_target_init_ret;
static int t_gfx_target_read_ret;
static gfx_target_t *t_gfx_target;
static const gfx_memory_readback_config_t *t_gfx_target_read_config;

static int t_gfx_target_init(gfx_target_t *target)
{
	t_gfx_target_init_calls++;
	t_gfx_target = target;
	return t_gfx_target_init_ret;
}

static void t_gfx_target_free(gfx_target_t *target)
{
	t_gfx_target_free_calls++;
	t_gfx_target = target;
}

static int t_gfx_target_read(gfx_target_t *target, const gfx_memory_readback_config_t *config)
{
	t_gfx_target_read_calls++;
	t_gfx_target		 = target;
	t_gfx_target_read_config = config;
	return t_gfx_target_read_ret;
}

static gfx_driver_t t_gfx_target_driver = {
	.name	     = "test-target",
	.api	     = GFX_API_SOFTWARE,
	.target_init = t_gfx_target_init,
	.target_free = t_gfx_target_free,
	.target_read = t_gfx_target_read,
};

static void t_gfx_target_reset(void)
{
	t_gfx_target_init_calls	 = 0;
	t_gfx_target_free_calls	 = 0;
	t_gfx_target_read_calls	 = 0;
	t_gfx_target_init_ret	 = 0;
	t_gfx_target_read_ret	 = 0;
	t_gfx_target		 = NULL;
	t_gfx_target_read_config = NULL;
}

TEST(gfx_target_init_memory_rejects_invalid_args)
{
	START;

	u8 pixels[4]			  = {0};
	gfx_t gfx			  = {.drv = &t_gfx_target_driver};
	gfx_target_t target		  = {0};
	gfx_memory_target_config_t config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};

	EXPECT_NULL(gfx_target_init_memory(NULL, &gfx, &config));
	EXPECT_NULL(gfx_target_init_memory(&target, NULL, &config));
	EXPECT_NULL(gfx_target_init_memory(&target, &(gfx_t){0}, &config));
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, NULL));
	EXPECT_NULL(gfx_target_init_memory(
		&target,
		&gfx,
		&(gfx_memory_target_config_t){.format = GFX_FORMAT_NONE, .data = pixels, .width = 1, .height = 1, .stride = 4}));
	EXPECT_NULL(gfx_target_init_memory(
		&target, &gfx, &(gfx_memory_target_config_t){.format = GFX_FORMAT_RGBA8, .width = 1, .height = 1, .stride = 4}));
	EXPECT_NULL(gfx_target_init_memory(
		&target, &gfx, &(gfx_memory_target_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .height = 1, .stride = 4}));
	EXPECT_NULL(gfx_target_init_memory(
		&target, &gfx, &(gfx_memory_target_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 1, .stride = 4}));
	EXPECT_NULL(gfx_target_init_memory(
		&target,
		&gfx,
		&(gfx_memory_target_config_t){.format = GFX_FORMAT_RGBA8, .data = pixels, .width = 1, .height = 1, .stride = 3}));

	END;
}

TEST(gfx_target_init_memory_rejects_active_frame)
{
	START;

	u8 pixels[4]	    = {0};
	gfx_frame_t frame   = {0};
	gfx_t gfx	    = {.drv = &t_gfx_target_driver, .frame = &frame};
	gfx_target_t target = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));

	END;
}

TEST(gfx_target_init_memory_success)
{
	START;

	t_gfx_target_reset();
	u8 pixels[4]	    = {0};
	gfx_t gfx	    = {.drv = &t_gfx_target_driver};
	gfx_target_t target = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	EXPECT_EQ(t_gfx_target_init_calls, 1);
	EXPECT_PTR(t_gfx_target, &target);
	EXPECT_PTR(target.gfx, &gfx);
	EXPECT_EQ(target.type, GFX_TARGET_MEMORY);
	EXPECT_PTR(target.data, pixels);

	gfx_target_free(&target);
	END;
}

TEST(gfx_target_init_memory_driver_failure_clears_target)
{
	START;

	t_gfx_target_reset();
	t_gfx_target_init_ret = 1;
	u8 pixels[4]	      = {0};
	gfx_t gfx	      = {.drv = &t_gfx_target_driver};
	gfx_target_t target   = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));
	EXPECT_EQ(t_gfx_target_init_calls, 1);
	EXPECT_EQ(t_gfx_target_free_calls, 1);
	EXPECT_NULL(target.gfx);

	END;
}

TEST(gfx_target_init_swapchain_rejects_invalid_args)
{
	START;

	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		  = {.drv = &t_gfx_target_driver};
	gfx_target_t target	  = {0};
	gfx_swapchain_t swapchain = {.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 1, .height = 1};

	EXPECT_NULL(gfx_target_init_swapchain(NULL, &swapchain));
	EXPECT_NULL(gfx_target_init_swapchain(&target, NULL));
	EXPECT_NULL(gfx_target_init_swapchain(&target, &(gfx_swapchain_t){0}));
	EXPECT_NULL(gfx_target_init_swapchain(&target, &(gfx_swapchain_t){.gfx = &(gfx_t){0}}));
	EXPECT_NULL(gfx_target_init_swapchain(&target, &(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .width = 1, .height = 1}));
	EXPECT_NULL(
		gfx_target_init_swapchain(&target, &(gfx_swapchain_t){.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .width = 1, .height = 1}));
	EXPECT_NULL(gfx_target_init_swapchain(
		&target, &(gfx_swapchain_t){.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .surface = &surface, .height = 1}));
	EXPECT_NULL(gfx_target_init_swapchain(
		&target, &(gfx_swapchain_t){.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 1}));

	END;
}

TEST(gfx_target_init_swapchain_rejects_active_frame)
{
	START;

	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_frame_t frame	  = {0};
	gfx_t gfx		  = {.drv = &t_gfx_target_driver, .frame = &frame};
	gfx_swapchain_t swapchain = {.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 1, .height = 1};
	gfx_target_t target	  = {0};

	EXPECT_NULL(gfx_target_init_swapchain(&target, &swapchain));

	END;
}

TEST(gfx_target_init_swapchain_success_and_failure)
{
	START;

	t_gfx_target_reset();
	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		  = {.drv = &t_gfx_target_driver};
	gfx_swapchain_t swapchain = {.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 2, .height = 3};
	gfx_target_t target	  = {0};

	EXPECT_PTR(gfx_target_init_swapchain(&target, &swapchain), &target);
	EXPECT_EQ(t_gfx_target_init_calls, 1);
	EXPECT_EQ(target.type, GFX_TARGET_SWAPCHAIN);
	EXPECT_PTR(target.swapchain, &swapchain);
	EXPECT_EQ(target.width, 2);
	EXPECT_EQ(target.height, 3);

	gfx_target_free(&target);

	t_gfx_target_reset();
	t_gfx_target_init_ret = 1;
	EXPECT_NULL(gfx_target_init_swapchain(&target, &swapchain));
	EXPECT_EQ(t_gfx_target_free_calls, 1);
	EXPECT_NULL(target.gfx);

	END;
}

TEST(gfx_target_free_ignores_invalid_or_active)
{
	START;

	t_gfx_target_reset();
	gfx_frame_t frame = {0};
	gfx_t gfx	  = {.drv = &t_gfx_target_driver, .frame = &frame};

	gfx_target_free(NULL);
	gfx_target_free(&(gfx_target_t){0});
	gfx_target_free(&(gfx_target_t){.gfx = &gfx});

	EXPECT_EQ(t_gfx_target_free_calls, 0);

	END;
}

TEST(gfx_target_free_calls_driver_and_clears)
{
	START;

	t_gfx_target_reset();
	gfx_t gfx	    = {.drv = &t_gfx_target_driver};
	gfx_target_t target = {.gfx = &gfx, .type = GFX_TARGET_MEMORY};

	gfx_target_free(&target);

	EXPECT_EQ(t_gfx_target_free_calls, 1);
	EXPECT_PTR(t_gfx_target, &target);
	EXPECT_NULL(target.gfx);

	END;
}

TEST(gfx_target_move_rejects_invalid_args)
{
	START;

	gfx_driver_t drv  = t_gfx_target_driver;
	gfx_t gfx	  = {.drv = &drv, .data = (void *)1};
	gfx_t other	  = {.drv = &drv, .data = (void *)2};
	gfx_frame_t frame = {0};
	gfx_target_t dst  = {0};
	gfx_target_t src  = {.gfx = &gfx};

	EXPECT_EQ(gfx_target_move(NULL, &src, &gfx), 1);
	EXPECT_EQ(gfx_target_move(&dst, NULL, &gfx), 1);
	EXPECT_EQ(gfx_target_move(&dst, &(gfx_target_t){0}, &gfx), 1);
	EXPECT_EQ(gfx_target_move(&dst, &src, NULL), 1);
	EXPECT_EQ(gfx_target_move(&(gfx_target_t){.gfx = &gfx}, &src, &gfx), 1);
	gfx.frame = &frame;
	EXPECT_EQ(gfx_target_move(&dst, &src, &gfx), 1);
	gfx.frame = NULL;
	src.gfx	  = &(gfx_t){.drv = &drv, .data = (void *)1, .frame = &frame};
	EXPECT_EQ(gfx_target_move(&dst, &src, &gfx), 1);
	src.gfx = &other;
	EXPECT_EQ(gfx_target_move(&dst, &src, &gfx), 1);

	END;
}

TEST(gfx_target_move_success)
{
	START;

	gfx_driver_t drv = t_gfx_target_driver;
	gfx_t src_gfx	 = {.drv = &drv, .data = (void *)1};
	gfx_t dst_gfx	 = {.drv = &drv, .data = (void *)1};
	gfx_target_t src = {.gfx = &src_gfx, .type = GFX_TARGET_MEMORY, .format = GFX_FORMAT_RGBA8, .width = 1, .height = 1};
	gfx_target_t dst = {0};

	EXPECT_EQ(gfx_target_move(&dst, &src, &dst_gfx), 0);
	EXPECT_PTR(dst.gfx, &dst_gfx);
	EXPECT_EQ(dst.type, GFX_TARGET_MEMORY);
	EXPECT_NULL(src.gfx);

	END;
}

TEST(gfx_target_read_rejects_invalid_args)
{
	START;

	u8 pixels[4]			    = {0};
	gfx_frame_t frame		    = {0};
	gfx_t gfx			    = {.drv = &t_gfx_target_driver};
	gfx_target_t target		    = {.gfx = &gfx, .type = GFX_TARGET_MEMORY, .format = GFX_FORMAT_RGBA8, .width = 1, .height = 1};
	gfx_memory_readback_config_t config = {.data = pixels, .stride = 4};

	EXPECT_EQ(gfx_target_read(NULL, &config), 1);
	EXPECT_EQ(gfx_target_read(&(gfx_target_t){0}, &config), 1);
	EXPECT_EQ(gfx_target_read(&(gfx_target_t){.gfx = &(gfx_t){0}}, &config), 1);
	EXPECT_EQ(gfx_target_read(&(gfx_target_t){.gfx = &(gfx_t){.drv = &(gfx_driver_t){0}}}, &config), 1);
	EXPECT_EQ(gfx_target_read(&(gfx_target_t){.gfx = &(gfx_t){.drv = &t_gfx_target_driver, .frame = &frame}}, &config), 1);
	EXPECT_EQ(gfx_target_read(&(gfx_target_t){.gfx = &gfx, .type = GFX_TARGET_SWAPCHAIN}, &config), 1);
	EXPECT_EQ(gfx_target_read(&(gfx_target_t){.gfx = &gfx, .type = GFX_TARGET_MEMORY}, &config), 1);
	EXPECT_EQ(gfx_target_read(&(gfx_target_t){.gfx = &gfx, .type = GFX_TARGET_MEMORY, .format = GFX_FORMAT_RGBA8}, &config), 1);
	EXPECT_EQ(gfx_target_read(&(gfx_target_t){.gfx = &gfx, .type = GFX_TARGET_MEMORY, .format = GFX_FORMAT_RGBA8, .width = 1}, &config),
		  1);
	EXPECT_EQ(gfx_target_read(&target, NULL), 1);
	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.stride = 4}), 1);
	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 3}), 1);

	END;
}

TEST(gfx_target_read_calls_driver)
{
	START;

	t_gfx_target_reset();
	u8 pixels[4]			    = {0};
	gfx_t gfx			    = {.drv = &t_gfx_target_driver};
	gfx_target_t target		    = {.gfx = &gfx, .type = GFX_TARGET_MEMORY, .format = GFX_FORMAT_RGBA8, .width = 1, .height = 1};
	gfx_memory_readback_config_t config = {.data = pixels, .stride = 4};

	EXPECT_EQ(gfx_target_read(&target, &config), 0);
	EXPECT_EQ(t_gfx_target_read_calls, 1);
	EXPECT_PTR(t_gfx_target, &target);
	EXPECT_PTR(t_gfx_target_read_config, &config);

	t_gfx_target_read_ret = 1;
	EXPECT_EQ(gfx_target_read(&target, &config), 1);

	END;
}

STEST(gfx_target)
{
	SSTART;
	RUN(gfx_target_init_memory_rejects_invalid_args);
	RUN(gfx_target_init_memory_rejects_active_frame);
	RUN(gfx_target_init_memory_success);
	RUN(gfx_target_init_memory_driver_failure_clears_target);
	RUN(gfx_target_init_swapchain_rejects_invalid_args);
	RUN(gfx_target_init_swapchain_rejects_active_frame);
	RUN(gfx_target_init_swapchain_success_and_failure);
	RUN(gfx_target_free_ignores_invalid_or_active);
	RUN(gfx_target_free_calls_driver_and_clears);
	RUN(gfx_target_move_rejects_invalid_args);
	RUN(gfx_target_move_success);
	RUN(gfx_target_read_rejects_invalid_args);
	RUN(gfx_target_read_calls_driver);
	SEND;
}
