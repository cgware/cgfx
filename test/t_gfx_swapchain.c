#include "gfx_swapchain.h"

#include "gfx_driver.h"
#include "test.h"

static int t_gfx_swapchain_init_calls;
static int t_gfx_swapchain_free_calls;
static int t_gfx_swapchain_resize_calls;
static int t_gfx_swapchain_present_calls;
static int t_gfx_swapchain_refresh_calls;
static int t_gfx_swapchain_acquire_calls;
static int t_gfx_swapchain_init_ret;
static int t_gfx_swapchain_resize_ret;
static int t_gfx_swapchain_present_ret;
static int t_gfx_swapchain_refresh_ret;
static int t_gfx_swapchain_acquire_ret;
static u32 t_gfx_swapchain_init_image_count;
static u32 t_gfx_swapchain_acquire_index;
static int t_gfx_swapchain_acquire_null_image;
static int t_gfx_swapchain_present_increments_generation;
static gfx_swapchain_t *t_gfx_swapchain;
static const gfx_swapchain_config_t *t_gfx_swapchain_config;
static u16 t_gfx_swapchain_width;
static u16 t_gfx_swapchain_height;

static int t_gfx_swapchain_init(gfx_swapchain_t *swapchain, const gfx_swapchain_config_t *config)
{
	t_gfx_swapchain_init_calls++;
	t_gfx_swapchain	       = swapchain;
	t_gfx_swapchain_config = config;
	swapchain->image_count = t_gfx_swapchain_init_image_count;
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

static int t_gfx_swapchain_refresh(gfx_swapchain_t *swapchain)
{
	t_gfx_swapchain_refresh_calls++;
	t_gfx_swapchain = swapchain;
	return t_gfx_swapchain_refresh_ret;
}

static int t_gfx_swapchain_acquire(gfx_swapchain_t *swapchain, gfx_swapchain_image_t *image)
{
	t_gfx_swapchain_acquire_calls++;
	t_gfx_swapchain = swapchain;
	if (t_gfx_swapchain_acquire_ret) {
		return t_gfx_swapchain_acquire_ret;
	}
	*image = (gfx_swapchain_image_t){
		.image	    = t_gfx_swapchain_acquire_null_image ? NULL : &swapchain->images[t_gfx_swapchain_acquire_index],
		.index	    = t_gfx_swapchain_acquire_index,
		.generation = swapchain->images[t_gfx_swapchain_acquire_index].generation,
	};
	return 0;
}

static int t_gfx_swapchain_present(gfx_swapchain_t *swapchain)
{
	t_gfx_swapchain_present_calls++;
	t_gfx_swapchain = swapchain;
	if (t_gfx_swapchain_present_increments_generation) {
		swapchain->generation++;
	}
	return t_gfx_swapchain_present_ret;
}

static gfx_driver_t t_gfx_swapchain_driver = {
	.name		   = "test-swapchain",
	.api		   = GFX_API_SOFTWARE,
	.swapchain_init	   = t_gfx_swapchain_init,
	.swapchain_free	   = t_gfx_swapchain_free,
	.swapchain_refresh = t_gfx_swapchain_refresh,
	.swapchain_acquire = t_gfx_swapchain_acquire,
	.swapchain_resize  = t_gfx_swapchain_resize,
	.swapchain_present = t_gfx_swapchain_present,
};

static void t_gfx_swapchain_reset(void)
{
	t_gfx_swapchain_init_calls		      = 0;
	t_gfx_swapchain_free_calls		      = 0;
	t_gfx_swapchain_resize_calls		      = 0;
	t_gfx_swapchain_present_calls		      = 0;
	t_gfx_swapchain_refresh_calls		      = 0;
	t_gfx_swapchain_acquire_calls		      = 0;
	t_gfx_swapchain_init_ret		      = 0;
	t_gfx_swapchain_resize_ret		      = 0;
	t_gfx_swapchain_present_ret		      = 0;
	t_gfx_swapchain_refresh_ret		      = 0;
	t_gfx_swapchain_acquire_ret		      = 0;
	t_gfx_swapchain_init_image_count	      = 0;
	t_gfx_swapchain_acquire_index		      = 0;
	t_gfx_swapchain_acquire_null_image	      = 0;
	t_gfx_swapchain_present_increments_generation = 0;
	t_gfx_swapchain				      = NULL;
	t_gfx_swapchain_config			      = NULL;
	t_gfx_swapchain_width			      = 0;
	t_gfx_swapchain_height			      = 0;
}

TEST(gfx_swapchain_init_rejects_invalid_args)
{
	START;

	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		      = {.drv = &t_gfx_swapchain_driver};
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t images[2]	      = {0};
	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &surface,
		.width		 = 1,
		.height		 = 1,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};

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
	EXPECT_NULL(gfx_swapchain_init(
		&swapchain, &gfx, &(gfx_swapchain_config_t){.format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 1, .height = 1}));
	EXPECT_NULL(gfx_swapchain_init(&swapchain,
				       &gfx,
				       &(gfx_swapchain_config_t){
					       .format		= GFX_FORMAT_RGBA8,
					       .surface		= &surface,
					       .width		= 1,
					       .height		= 1,
					       .images		= images,
					       .min_image_count = 1,
				       }));
	EXPECT_NULL(gfx_swapchain_init(&swapchain,
				       &gfx,
				       &(gfx_swapchain_config_t){
					       .format	       = GFX_FORMAT_RGBA8,
					       .surface	       = &surface,
					       .width	       = 1,
					       .height	       = 1,
					       .images	       = images,
					       .image_capacity = 1,
				       }));
	EXPECT_NULL(gfx_swapchain_init(&swapchain,
				       &gfx,
				       &(gfx_swapchain_config_t){
					       .format		= GFX_FORMAT_RGBA8,
					       .surface		= &surface,
					       .width		= 1,
					       .height		= 1,
					       .images		= images,
					       .min_image_count = 3,
					       .image_capacity	= 2,
				       }));
	EXPECT_NULL(gfx_swapchain_init(&swapchain,
				       &gfx,
				       &(gfx_swapchain_config_t){
					       .format		= GFX_FORMAT_RGBA8,
					       .surface		= &surface,
					       .width		= 1,
					       .height		= 1,
					       .images		= images,
					       .min_image_count = 2,
					       .max_image_count = 1,
					       .image_capacity	= 2,
				       }));
	EXPECT_NULL(gfx_swapchain_init(&swapchain,
				       &gfx,
				       &(gfx_swapchain_config_t){
					       .format		= GFX_FORMAT_RGBA8,
					       .surface		= &surface,
					       .width		= 1,
					       .height		= 1,
					       .images		= images,
					       .min_image_count = 1,
					       .max_image_count = 3,
					       .image_capacity	= 2,
				       }));

	END;
}

TEST(gfx_swapchain_init_rejects_active_frame)
{
	START;

	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_frame_t frame	  = {0};
	gfx_t gfx		  = {.drv = &t_gfx_swapchain_driver, .frame = &frame};
	gfx_swapchain_t swapchain = {0};
	gfx_image_t images[1]	  = {0};

	gfx_swapchain_config_t swapchain_config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &surface,
		.width		 = 1,
		.height		 = 1,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
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
	gfx_image_t images[2]	  = {0};

	gfx_swapchain_config_t swapchain_config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &surface,
		.width		 = 2,
		.height		 = 3,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
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

TEST(gfx_swapchain_init_driver_image_count_exceeds_capacity)
{
	START;

	t_gfx_swapchain_reset();
	t_gfx_swapchain_init_image_count = 3;
	gfx_surface_t surface		 = {.api = GFX_API_SOFTWARE};
	gfx_t gfx			 = {.drv = &t_gfx_swapchain_driver};
	gfx_swapchain_t swapchain	 = {0};
	gfx_image_t images[2]		 = {0};

	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &surface,
		.width		 = 2,
		.height		 = 3,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};

	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &config));
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
	gfx_image_t images[2]	  = {{.gfx = &gfx}, {.gfx = &gfx}};
	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.surface	= &surface,
		.format		= GFX_FORMAT_RGBA8,
		.width		= 1,
		.height		= 1,
		.images		= images,
		.image_capacity = sizeof(images) / sizeof(images[0]),
	};

	gfx_swapchain_free(&swapchain);

	EXPECT_EQ(t_gfx_swapchain_free_calls, 1);
	EXPECT_PTR(t_gfx_swapchain, &swapchain);
	EXPECT_NULL(swapchain.gfx);
	EXPECT_NULL(images[0].gfx);
	EXPECT_NULL(images[1].gfx);

	END;
}

TEST(gfx_swapchain_info_and_images)
{
	START;

	gfx_t gfx		  = {.drv = &t_gfx_swapchain_driver};
	gfx_image_t images[2]	  = {0};
	gfx_swapchain_t swapchain = {
		.gfx		     = &gfx,
		.format		     = GFX_FORMAT_RGBA8,
		.width		     = 2,
		.height		     = 3,
		.image_count	     = 2,
		.present_mode	     = GFX_PRESENT_MODE_VSYNC,
		.actual_present_mode = GFX_PRESENT_MODE_IMMEDIATE,
		.generation	     = 4,
		.images		     = images,
		.image_capacity	     = sizeof(images) / sizeof(images[0]),
	};
	gfx_swapchain_info_t info  = {0};
	gfx_image_t *image_refs[3] = {0};
	u32 count		   = 0;

	EXPECT_EQ(gfx_swapchain_info(NULL, &info), 1);
	EXPECT_EQ(gfx_swapchain_info(&(gfx_swapchain_t){0}, &info), 1);
	EXPECT_EQ(gfx_swapchain_info(&swapchain, NULL), 1);
	EXPECT_EQ(gfx_swapchain_info(&swapchain, &info), 0);
	EXPECT_EQ(info.format, GFX_FORMAT_RGBA8);
	EXPECT_EQ(info.width, 2);
	EXPECT_EQ(info.height, 3);
	EXPECT_EQ(info.image_count, 2);
	EXPECT_EQ(info.present_mode, GFX_PRESENT_MODE_VSYNC);
	EXPECT_EQ(info.actual_present_mode, GFX_PRESENT_MODE_IMMEDIATE);
	EXPECT_EQ(info.generation, 4);

	EXPECT_EQ(gfx_swapchain_images(NULL, image_refs, 2, &count), 1);
	EXPECT_EQ(gfx_swapchain_images(&(gfx_swapchain_t){0}, image_refs, 2, &count), 1);
	EXPECT_EQ(gfx_swapchain_images(&swapchain, NULL, 1, &count), 1);
	EXPECT_EQ(gfx_swapchain_images(&swapchain, image_refs, 2, NULL), 1);
	EXPECT_EQ(gfx_swapchain_images(&swapchain, NULL, 0, &count), 0);
	EXPECT_EQ(count, 2);
	EXPECT_EQ(gfx_swapchain_images(&swapchain, image_refs, 3, &count), 0);
	EXPECT_EQ(count, 2);
	EXPECT_PTR(image_refs[0], &images[0]);
	EXPECT_PTR(image_refs[1], &images[1]);
	EXPECT_NULL(image_refs[2]);

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

TEST(gfx_swapchain_acquire_rejects_invalid_args)
{
	START;

	gfx_surface_t surface	    = {.api = GFX_API_SOFTWARE};
	gfx_frame_t frame	    = {0};
	gfx_t gfx		    = {.drv = &t_gfx_swapchain_driver};
	gfx_swapchain_image_t image = {0};

	EXPECT_EQ(gfx_swapchain_acquire(NULL, &image), 1);
	EXPECT_EQ(gfx_swapchain_acquire(&(gfx_swapchain_t){0}, &image), 1);
	EXPECT_EQ(gfx_swapchain_acquire(&(gfx_swapchain_t){.gfx = &(gfx_t){0}}, &image), 1);
	EXPECT_EQ(gfx_swapchain_acquire(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .width = 1, .height = 1}, NULL), 1);
	EXPECT_EQ(gfx_swapchain_acquire(&(gfx_swapchain_t){.gfx	    = &(gfx_t){.drv = &t_gfx_swapchain_driver, .frame = &frame},
							   .surface = &surface,
							   .width   = 1,
							   .height  = 1},
					&image),
		  1);
	EXPECT_EQ(gfx_swapchain_acquire(&(gfx_swapchain_t){.gfx = &gfx, .width = 1, .height = 1}, &image), 1);
	EXPECT_EQ(gfx_swapchain_acquire(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .height = 1}, &image), 1);
	EXPECT_EQ(gfx_swapchain_acquire(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .width = 1}, &image), 1);

	END;
}

TEST(gfx_swapchain_acquire_refresh_and_driver_failures)
{
	START;

	t_gfx_swapchain_reset();
	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		  = {.drv = &t_gfx_swapchain_driver};
	gfx_image_t images[1]	  = {0};
	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.surface	= &surface,
		.format		= GFX_FORMAT_RGBA8,
		.width		= 1,
		.height		= 1,
		.images		= images,
		.image_capacity = sizeof(images) / sizeof(images[0]),
	};
	gfx_swapchain_image_t image = {0};

	t_gfx_swapchain_refresh_ret = 1;
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 1);
	EXPECT_EQ(t_gfx_swapchain_refresh_calls, 1);

	t_gfx_swapchain_reset();
	t_gfx_swapchain_acquire_ret = 1;
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 1);
	EXPECT_EQ(t_gfx_swapchain_acquire_calls, 1);

	t_gfx_swapchain_reset();
	t_gfx_swapchain_acquire_index = 1;
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 1);

	t_gfx_swapchain_reset();
	t_gfx_swapchain_acquire_null_image = 1;
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 1);

	END;
}

TEST(gfx_swapchain_acquire_without_driver_returns_first_image)
{
	START;

	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_driver_t driver	  = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		  = {.drv = &driver};
	gfx_image_t images[2]	  = {0};
	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.surface	= &surface,
		.format		= GFX_FORMAT_RGBA8,
		.width		= 2,
		.height		= 3,
		.usage		= GFX_IMAGE_USAGE_TRANSFER_SRC,
		.images		= images,
		.image_capacity = sizeof(images) / sizeof(images[0]),
	};
	gfx_swapchain_image_t image = {0};

	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 0);
	EXPECT_EQ(swapchain.image_count, 1);
	EXPECT_EQ(swapchain.acquired, 1);
	EXPECT_EQ(swapchain.acquired_index, 0);
	EXPECT_PTR(image.image, &images[0]);
	EXPECT_EQ(image.index, 0);
	EXPECT_EQ(images[0].origin, GFX_IMAGE_ORIGIN_SURFACE);
	EXPECT_EQ(images[0].usage, GFX_IMAGE_USAGE_TRANSFER_SRC | GFX_IMAGE_USAGE_PRESENT);

	EXPECT_EQ(gfx_swapchain_acquire(
			  &(gfx_swapchain_t){
				  .gfx = &gfx, .surface = &surface, .format = GFX_FORMAT_RGBA8, .width = 1, .height = 1, .image_count = 1},
			  &image),
		  1);
	EXPECT_EQ(
		gfx_swapchain_acquire(
			&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .format = GFX_FORMAT_RGBA8, .width = 1, .height = 1}, &image),
		1);

	END;
}

TEST(gfx_swapchain_resize_success_and_failure)
{
	START;

	t_gfx_swapchain_reset();
	gfx_t gfx		  = {.drv = &t_gfx_swapchain_driver};
	gfx_surface_t surface	  = {.api = GFX_API_SOFTWARE};
	gfx_image_t images[2]	  = {0};
	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.surface	= &surface,
		.format		= GFX_FORMAT_RGBA8,
		.images		= images,
		.width		= 2,
		.height		= 3,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
	};

	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 4, 5), 0);
	EXPECT_EQ(t_gfx_swapchain_resize_calls, 1);
	EXPECT_EQ(t_gfx_swapchain_width, 4);
	EXPECT_EQ(t_gfx_swapchain_height, 5);
	EXPECT_EQ(swapchain.width, 4);
	EXPECT_EQ(swapchain.height, 5);

	swapchain.image_count = 0;
	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 5, 6), 0);
	EXPECT_EQ(swapchain.image_count, 1);
	EXPECT_EQ(swapchain.width, 5);
	EXPECT_EQ(swapchain.height, 6);

	t_gfx_swapchain_resize_ret = 1;
	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 6, 7), 1);
	EXPECT_EQ(swapchain.width, 5);
	EXPECT_EQ(swapchain.height, 6);

	t_gfx_swapchain_resize_ret = 0;
	swapchain.image_count	   = 3;
	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 7, 8), 1);

	END;
}

TEST(gfx_swapchain_present_rejects_invalid_args)
{
	START;

	gfx_surface_t surface = {.api = GFX_API_SOFTWARE};
	gfx_frame_t frame     = {0};
	gfx_t gfx	      = {.drv = &t_gfx_swapchain_driver};

	gfx_swapchain_image_t image = {0};
	EXPECT_EQ(gfx_swapchain_present(NULL, &image), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){0}, &image), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &(gfx_t){0}}, &image), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &(gfx_t){.drv = &(gfx_driver_t){0}}}, &image), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &(gfx_t){.drv = &t_gfx_swapchain_driver, .frame = &frame}}, &image), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &gfx}, &image), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface}, &image), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .width = 1}, &image), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .width = 1, .height = 1}, NULL), 1);
	EXPECT_EQ(gfx_swapchain_present(&(gfx_swapchain_t){.gfx = &gfx, .surface = &surface, .width = 1, .height = 1}, &image), 1);

	END;
}

TEST(gfx_swapchain_present_calls_driver)
{
	START;

	t_gfx_swapchain_reset();
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		      = {.drv = &t_gfx_swapchain_driver};
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t images[2]	      = {0};
	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &surface,
		.width		 = 1,
		.height		 = 1,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};
	EXPECT_PTR(gfx_swapchain_init(&swapchain, &gfx, &config), &swapchain);

	gfx_swapchain_image_t image = {0};
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 0);
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 0);
	EXPECT_EQ(t_gfx_swapchain_present_calls, 1);
	EXPECT_PTR(t_gfx_swapchain, &swapchain);

	t_gfx_swapchain_present_ret = 1;
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 0);
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 1);

	END;
}

TEST(gfx_swapchain_present_rejects_stale_or_unacquired_image)
{
	START;

	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_t gfx		      = {.drv = &t_gfx_swapchain_driver};
	gfx_image_t images[1]	      = {0};
	gfx_swapchain_t swapchain     = {0};
	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &surface,
		.width		 = 1,
		.height		 = 1,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};
	EXPECT_PTR(gfx_swapchain_init(&swapchain, &gfx, &config), &swapchain);

	gfx_swapchain_image_t image = {.image = &images[0], .index = 0, .generation = images[0].generation};
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 1);
	swapchain.acquired	 = 1;
	swapchain.acquired_index = 0;
	image.generation++;
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 1);
	image.generation = images[0].generation;
	image.image	 = &(gfx_image_t){.swapchain = &(gfx_swapchain_t){0}};
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 1);
	image.image = &images[0];
	image.index = 1;
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 1);
	image.index		 = 0;
	swapchain.acquired_index = 1;
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 1);

	END;
}

TEST(gfx_swapchain_present_refreshes_images_after_driver_generation_change)
{
	START;

	t_gfx_swapchain_reset();
	t_gfx_swapchain_present_increments_generation = 1;
	gfx_surface_t surface			      = {.api = GFX_API_SOFTWARE};
	gfx_t gfx				      = {.drv = &t_gfx_swapchain_driver};
	gfx_image_t images[1]			      = {0};
	gfx_swapchain_t swapchain		      = {0};

	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &surface,
		.width		 = 1,
		.height		 = 1,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};
	EXPECT_PTR(gfx_swapchain_init(&swapchain, &gfx, &config), &swapchain);

	gfx_swapchain_image_t image = {0};
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 0);
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 0);
	EXPECT_EQ(images[0].generation, swapchain.generation);
	EXPECT_EQ(swapchain.acquired, 0);

	END;
}

TEST(gfx_swapchain_present_returns_error_when_generation_refresh_fails)
{
	START;

	t_gfx_swapchain_reset();
	t_gfx_swapchain_present_increments_generation = 1;
	gfx_surface_t surface			      = {.api = GFX_API_SOFTWARE};
	gfx_t gfx				      = {.drv = &t_gfx_swapchain_driver};

	gfx_image_t images[1] = {
		{
			.gfx	    = &gfx,
			.origin	    = GFX_IMAGE_ORIGIN_SURFACE,
			.format	    = GFX_FORMAT_RGBA8,
			.swapchain  = &(gfx_swapchain_t){0},
			.width	    = 1,
			.height	    = 1,
			.generation = 1,
		},
	};
	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.surface	= &surface,
		.format		= GFX_FORMAT_RGBA8,
		.width		= 1,
		.height		= 1,
		.generation	= 1,
		.images		= images,
		.image_count	= 2,
		.image_capacity = sizeof(images) / sizeof(images[0]),
		.acquired_index = 0,
		.acquired	= 1,
	};
	images[0].swapchain	    = &swapchain;
	gfx_swapchain_image_t image = {.image = &images[0], .index = 0, .generation = 1};

	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 1);

	END;
}

STEST(gfx_swapchain)
{
	SSTART;
	RUN(gfx_swapchain_init_rejects_invalid_args);
	RUN(gfx_swapchain_init_rejects_active_frame);
	RUN(gfx_swapchain_init_success_and_failure);
	RUN(gfx_swapchain_init_driver_image_count_exceeds_capacity);
	RUN(gfx_swapchain_free_ignores_invalid_or_active);
	RUN(gfx_swapchain_free_calls_driver_and_clears);
	RUN(gfx_swapchain_info_and_images);
	RUN(gfx_swapchain_resize_rejects_invalid_args);
	RUN(gfx_swapchain_resize_success_and_failure);
	RUN(gfx_swapchain_acquire_rejects_invalid_args);
	RUN(gfx_swapchain_acquire_refresh_and_driver_failures);
	RUN(gfx_swapchain_acquire_without_driver_returns_first_image);
	RUN(gfx_swapchain_present_rejects_invalid_args);
	RUN(gfx_swapchain_present_calls_driver);
	RUN(gfx_swapchain_present_rejects_stale_or_unacquired_image);
	RUN(gfx_swapchain_present_refreshes_images_after_driver_generation_change);
	RUN(gfx_swapchain_present_returns_error_when_generation_refresh_fails);
	SEND;
}
