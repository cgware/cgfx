#include "gfx_framebuffer.h"

#include "gfx_driver.h"
#include "test.h"

static int t_gfx_framebuffer_init_calls;
static int t_gfx_framebuffer_free_calls;
static int t_gfx_framebuffer_pass_begin_calls;
static int t_gfx_framebuffer_swapchain_resize_calls;
static int t_gfx_framebuffer_swapchain_refresh_calls;
static int t_gfx_framebuffer_image_init_calls;
static int t_gfx_framebuffer_image_free_calls;
static int t_gfx_framebuffer_init_ret;
static int t_gfx_framebuffer_pass_begin_ret;
static int t_gfx_framebuffer_swapchain_resize_ret;
static int t_gfx_framebuffer_swapchain_refresh_ret;
static int t_gfx_framebuffer_refresh_sets_frame;
static int t_gfx_framebuffer_pass_begin_stales_target;
static int t_gfx_framebuffer_swapchain_resize_changes_format;
static int t_gfx_framebuffer_image_init_ret;
static gfx_framebuffer_t *t_gfx_framebuffer;
static gfx_frame_t *t_gfx_framebuffer_frame;
static gfx_image_t *t_gfx_framebuffer_target;
static u16 t_gfx_framebuffer_resize_width;
static u16 t_gfx_framebuffer_resize_height;

static int t_gfx_framebuffer_init(gfx_framebuffer_t *framebuffer)
{
	t_gfx_framebuffer_init_calls++;
	t_gfx_framebuffer = framebuffer;
	framebuffer->data = (void *)0x1234;
	return t_gfx_framebuffer_init_ret;
}

static void t_gfx_framebuffer_free(gfx_framebuffer_t *framebuffer)
{
	t_gfx_framebuffer_free_calls++;
	t_gfx_framebuffer = framebuffer;
	framebuffer->data = NULL;
}

static int t_gfx_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame)
{
	t_gfx_framebuffer_pass_begin_calls++;
	t_gfx_framebuffer	= framebuffer;
	t_gfx_framebuffer_frame = frame;
	frame->data		= (void *)0x5678;
	if (t_gfx_framebuffer_pass_begin_stales_target) {
		framebuffer->image->width++;
	}
	return t_gfx_framebuffer_pass_begin_ret;
}

static int t_gfx_framebuffer_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height)
{
	t_gfx_framebuffer_swapchain_resize_calls++;
	t_gfx_framebuffer_resize_width	= width;
	t_gfx_framebuffer_resize_height = height;
	if (t_gfx_framebuffer_swapchain_resize_changes_format && swapchain != NULL && swapchain->images != NULL &&
	    swapchain->image_count > 0) {
		swapchain->format	    = GFX_FORMAT_BGRA8_UNORM;
		swapchain->images[0].format = GFX_FORMAT_BGRA8_UNORM;
	}
	return t_gfx_framebuffer_swapchain_resize_ret;
}

static int t_gfx_framebuffer_swapchain_refresh(gfx_swapchain_t *swapchain)
{
	t_gfx_framebuffer_swapchain_refresh_calls++;
	if (t_gfx_framebuffer_refresh_sets_frame) {
		static gfx_frame_t frame = {0};
		swapchain->gfx->frame	 = &frame;
	}
	return t_gfx_framebuffer_swapchain_refresh_ret;
}

static int t_gfx_framebuffer_image_init(gfx_image_t *target)
{
	t_gfx_framebuffer_image_init_calls++;
	t_gfx_framebuffer_target = target;
	return t_gfx_framebuffer_image_init_ret;
}

static void t_gfx_framebuffer_image_free(gfx_image_t *target)
{
	t_gfx_framebuffer_image_free_calls++;
	t_gfx_framebuffer_target = target;
}

static gfx_driver_t t_gfx_framebuffer_driver = {
	.name			= "test-framebuffer",
	.api			= GFX_API_SOFTWARE,
	.framebuffer_init	= t_gfx_framebuffer_init,
	.framebuffer_free	= t_gfx_framebuffer_free,
	.framebuffer_pass_begin = t_gfx_framebuffer_pass_begin,
	.swapchain_resize	= t_gfx_framebuffer_swapchain_resize,
	.swapchain_refresh	= t_gfx_framebuffer_swapchain_refresh,
	.image_init		= t_gfx_framebuffer_image_init,
	.image_free		= t_gfx_framebuffer_image_free,
};

static void t_gfx_framebuffer_reset(void)
{
	t_gfx_framebuffer_init_calls			  = 0;
	t_gfx_framebuffer_free_calls			  = 0;
	t_gfx_framebuffer_pass_begin_calls		  = 0;
	t_gfx_framebuffer_swapchain_resize_calls	  = 0;
	t_gfx_framebuffer_swapchain_refresh_calls	  = 0;
	t_gfx_framebuffer_image_init_calls		  = 0;
	t_gfx_framebuffer_image_free_calls		  = 0;
	t_gfx_framebuffer_init_ret			  = 0;
	t_gfx_framebuffer_pass_begin_ret		  = 0;
	t_gfx_framebuffer_swapchain_resize_ret		  = 0;
	t_gfx_framebuffer_swapchain_refresh_ret		  = 0;
	t_gfx_framebuffer_refresh_sets_frame		  = 0;
	t_gfx_framebuffer_pass_begin_stales_target	  = 0;
	t_gfx_framebuffer_swapchain_resize_changes_format = 0;
	t_gfx_framebuffer_image_init_ret		  = 0;
	t_gfx_framebuffer				  = NULL;
	t_gfx_framebuffer_frame				  = NULL;
	t_gfx_framebuffer_target			  = NULL;
	t_gfx_framebuffer_resize_width			  = 0;
	t_gfx_framebuffer_resize_height			  = 0;
}

static gfx_image_t t_gfx_framebuffer_memory_target(gfx_t *gfx)
{
	return (gfx_image_t){
		.gfx	= gfx,
		.origin = GFX_IMAGE_ORIGIN_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= (void *)1,
		.width	= 2,
		.height = 3,
		.stride = 8,
	};
}

static gfx_swapchain_t t_gfx_framebuffer_swapchain(gfx_t *gfx, gfx_surface_t *surface)
{
	static gfx_image_t images[2];
	images[0] = (gfx_image_t){0};
	images[1] = (gfx_image_t){0};
	return (gfx_swapchain_t){
		.gfx		= gfx,
		.format		= GFX_FORMAT_RGBA8,
		.surface	= surface,
		.images		= images,
		.width		= 2,
		.height		= 3,
		.generation	= 1,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
	};
}

static gfx_image_t t_gfx_framebuffer_swapchain_target(gfx_t *gfx, gfx_swapchain_t *swapchain)
{
	return (gfx_image_t){
		.gfx	    = gfx,
		.origin	    = GFX_IMAGE_ORIGIN_SURFACE,
		.format	    = GFX_FORMAT_RGBA8,
		.swapchain  = swapchain,
		.width	    = 2,
		.height	    = 3,
		.generation = swapchain != NULL ? swapchain->generation : 0,
	};
}

static gfx_render_pass_t t_gfx_framebuffer_render_pass(gfx_t *gfx)
{
	return (gfx_render_pass_t){
		.gfx	      = gfx,
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
}

static gfx_framebuffer_t t_gfx_framebuffer_valid(gfx_t *gfx, gfx_image_t *target, const gfx_render_pass_t *render_pass)
{
	return (gfx_framebuffer_t){
		.gfx		  = gfx,
		.image		  = target,
		.render_pass	  = render_pass,
		.width		  = target->width,
		.height		  = target->height,
		.image_generation = target->generation,
		.data		  = (void *)0x1234,
	};
}

TEST(gfx_framebuffer_init_rejects_invalid_args)
{
	START;

	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_t other_gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_image_t target	      = t_gfx_framebuffer_memory_target(&gfx);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(NULL, &target, &render_pass));
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, NULL, &render_pass));
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &(gfx_image_t){0}, &render_pass));
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &(gfx_image_t){.gfx = &(gfx_t){0}}, &render_pass));
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, NULL));
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &(gfx_render_pass_t){.gfx = &other_gfx, .color_format = GFX_FORMAT_RGBA8}));
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &(gfx_render_pass_t){.gfx = &gfx, .color_format = GFX_FORMAT_BGRA8_UNORM}));
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &(gfx_image_t){.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .height = 1}, &render_pass));
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &(gfx_image_t){.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .width = 1}, &render_pass));

	END;
}

TEST(gfx_framebuffer_init_rejects_active_frame)
{
	START;

	gfx_frame_t frame	      = {0};
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver, .frame = &frame};
	gfx_image_t target	      = t_gfx_framebuffer_memory_target(&gfx);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));

	END;
}

TEST(gfx_framebuffer_init_success)
{
	START;

	t_gfx_framebuffer_reset();
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_image_t target	      = t_gfx_framebuffer_memory_target(&gfx);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(t_gfx_framebuffer_init_calls, 1);
	EXPECT_PTR(t_gfx_framebuffer, &framebuffer);
	EXPECT_PTR(framebuffer.gfx, &gfx);
	EXPECT_PTR(framebuffer.image, &target);
	EXPECT_PTR(framebuffer.render_pass, &render_pass);
	EXPECT_EQ(framebuffer.width, 2);
	EXPECT_EQ(framebuffer.height, 3);
	EXPECT_PTR(framebuffer.data, (void *)0x1234);

	gfx_framebuffer_free(&framebuffer);
	END;
}

TEST(gfx_framebuffer_init_driver_failure_clears_framebuffer)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_init_ret    = 1;
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_image_t target	      = t_gfx_framebuffer_memory_target(&gfx);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	EXPECT_EQ(t_gfx_framebuffer_init_calls, 1);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 1);
	EXPECT_NULL(framebuffer.gfx);

	END;
}

TEST(gfx_framebuffer_free_ignores_invalid_or_active)
{
	START;

	t_gfx_framebuffer_reset();
	gfx_frame_t frame = {0};
	gfx_t gfx	  = {.drv = &t_gfx_framebuffer_driver, .frame = &frame};

	gfx_framebuffer_free(NULL);
	gfx_framebuffer_free(&(gfx_framebuffer_t){0});
	gfx_framebuffer_free(&(gfx_framebuffer_t){.gfx = &gfx});

	EXPECT_EQ(t_gfx_framebuffer_free_calls, 0);

	END;
}

TEST(gfx_framebuffer_free_calls_driver_and_clears)
{
	START;

	t_gfx_framebuffer_reset();
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_image_t target	      = t_gfx_framebuffer_memory_target(&gfx);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);

	gfx_framebuffer_free(&framebuffer);

	EXPECT_EQ(t_gfx_framebuffer_free_calls, 1);
	EXPECT_PTR(t_gfx_framebuffer, &framebuffer);
	EXPECT_NULL(framebuffer.gfx);

	END;
}

TEST(gfx_framebuffer_resize_rejects_invalid_args)
{
	START;

	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_frame_t frame	      = {0};
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain     = t_gfx_framebuffer_swapchain(&gfx, &surface);
	swapchain.images[0]	      = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_image_t *target	      = &swapchain.images[0];
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, target, &render_pass);

	EXPECT_EQ(gfx_framebuffer_resize(NULL, 1, 1), 1);
	EXPECT_EQ(gfx_framebuffer_resize(&(gfx_framebuffer_t){0}, 1, 1), 1);
	EXPECT_EQ(gfx_framebuffer_resize(&(gfx_framebuffer_t){.gfx = &gfx}, 1, 1), 1);
	gfx.frame = &frame;
	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 1, 1), 1);
	gfx.frame      = NULL;
	target->origin = GFX_IMAGE_ORIGIN_MEMORY;
	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 1, 1), 1);
	target->origin	  = GFX_IMAGE_ORIGIN_SURFACE;
	target->swapchain = NULL;
	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 1, 1), 1);
	target->swapchain = &swapchain;
	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 0, 1), 1);
	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 1, 0), 1);

	END;
}

TEST(gfx_framebuffer_resize_success)
{
	START;

	t_gfx_framebuffer_reset();
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain     = t_gfx_framebuffer_swapchain(&gfx, &surface);
	swapchain.images[0]	      = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_image_t *target	      = &swapchain.images[0];
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, target, &render_pass);

	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 4, 5), 0);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 1);
	EXPECT_EQ(t_gfx_framebuffer_swapchain_resize_calls, 1);
	EXPECT_EQ(t_gfx_framebuffer_resize_width, 4);
	EXPECT_EQ(t_gfx_framebuffer_resize_height, 5);
	EXPECT_EQ(t_gfx_framebuffer_init_calls, 1);
	EXPECT_PTR(framebuffer.gfx, &gfx);
	EXPECT_PTR(framebuffer.image, target);
	EXPECT_PTR(framebuffer.render_pass, &render_pass);
	EXPECT_EQ(framebuffer.width, 4);
	EXPECT_EQ(framebuffer.height, 5);

	gfx_framebuffer_free(&framebuffer);
	END;
}

TEST(gfx_framebuffer_resize_swapchain_failure_restores_framebuffer)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_swapchain_resize_ret = 1;
	gfx_t gfx			       = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface		       = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain	       = t_gfx_framebuffer_swapchain(&gfx, &surface);
	gfx_image_t target		       = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_render_pass_t render_pass	       = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer	       = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);

	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 4, 5), 1);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 1);
	EXPECT_EQ(t_gfx_framebuffer_swapchain_resize_calls, 1);
	EXPECT_EQ(t_gfx_framebuffer_init_calls, 1);
	EXPECT_PTR(framebuffer.gfx, &gfx);

	END;
}

TEST(gfx_framebuffer_resize_swapchain_failure_restore_init_failure_clears_framebuffer)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_swapchain_resize_ret = 1;
	t_gfx_framebuffer_init_ret	       = 1;
	gfx_t gfx			       = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface		       = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain	       = t_gfx_framebuffer_swapchain(&gfx, &surface);
	gfx_image_t target		       = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_render_pass_t render_pass	       = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer	       = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);

	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 4, 5), 1);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 2);
	EXPECT_EQ(t_gfx_framebuffer_init_calls, 1);
	EXPECT_NULL(framebuffer.gfx);

	END;
}

TEST(gfx_framebuffer_resize_image_init_failure_clears_framebuffer)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_init_ret    = 1;
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain     = t_gfx_framebuffer_swapchain(&gfx, &surface);
	swapchain.images[0]	      = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_image_t *target	      = &swapchain.images[0];
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, target, &render_pass);

	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 4, 5), 1);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 2);
	EXPECT_NULL(framebuffer.gfx);

	END;
}

TEST(gfx_framebuffer_resize_format_mismatch_clears_framebuffer)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_swapchain_resize_changes_format = 1;

	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain     = t_gfx_framebuffer_swapchain(&gfx, &surface);
	swapchain.images[0]	      = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_image_t *target	      = &swapchain.images[0];
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, target, &render_pass);

	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 4, 5), 1);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 1);
	EXPECT_EQ(t_gfx_framebuffer_swapchain_resize_calls, 1);
	EXPECT_EQ(t_gfx_framebuffer_init_calls, 0);
	EXPECT_NULL(framebuffer.gfx);

	END;
}

TEST(gfx_framebuffer_resize_init_failure_clears_framebuffer)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_init_ret    = 1;
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain     = t_gfx_framebuffer_swapchain(&gfx, &surface);
	swapchain.images[0]	      = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_image_t *target	      = &swapchain.images[0];
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, target, &render_pass);

	EXPECT_EQ(gfx_framebuffer_resize(&framebuffer, 4, 5), 1);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 2);
	EXPECT_EQ(t_gfx_framebuffer_init_calls, 1);
	EXPECT_NULL(framebuffer.gfx);

	END;
}

TEST(gfx_framebuffer_pass_begin_rejects_invalid_args)
{
	START;

	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_image_t target	      = t_gfx_framebuffer_memory_target(&gfx);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);
	gfx_frame_t frame	      = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(NULL, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&(gfx_framebuffer_t){0}, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	EXPECT_EQ(gfx_framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.gfx = &gfx}, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}),
		  1);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, NULL, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	frame.active = 1;
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	frame.active = 0;
	gfx.frame    = &frame;
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	gfx.frame = NULL;
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, NULL), 1);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {0, 0, 1, 1}}), 0);
	EXPECT_EQ(gfx_end(&frame), 1);
	gfx.frame = NULL;

	END;
}

TEST(gfx_framebuffer_pass_begin_rejects_swapchain_without_swapchain)
{
	START;

	t_gfx_framebuffer_reset();
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_image_t target	      = t_gfx_framebuffer_swapchain_target(&gfx, NULL);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);
	gfx_frame_t frame	      = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	EXPECT_EQ(t_gfx_framebuffer_pass_begin_calls, 0);

	END;
}

TEST(gfx_framebuffer_pass_begin_rejects_refresh_failure)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_swapchain_refresh_ret = 1;
	gfx_t gfx				= {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface			= {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain		= t_gfx_framebuffer_swapchain(&gfx, &surface);
	gfx_image_t target			= t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_render_pass_t render_pass		= t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer		= t_gfx_framebuffer_valid(&gfx, &target, &render_pass);
	gfx_frame_t frame			= {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	EXPECT_EQ(t_gfx_framebuffer_swapchain_refresh_calls, 1);
	EXPECT_EQ(t_gfx_framebuffer_pass_begin_calls, 0);

	END;
}

TEST(gfx_framebuffer_pass_begin_rejects_invalidated_during_refresh)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_refresh_sets_frame = 1;
	gfx_t gfx			     = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface		     = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain	     = t_gfx_framebuffer_swapchain(&gfx, &surface);
	gfx_image_t target		     = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_render_pass_t render_pass	     = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer	     = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);
	gfx_frame_t frame		     = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	EXPECT_EQ(t_gfx_framebuffer_swapchain_refresh_calls, 1);
	EXPECT_EQ(t_gfx_framebuffer_pass_begin_calls, 0);
	gfx.frame = NULL;

	END;
}

TEST(gfx_framebuffer_pass_begin_rejects_stale_memory_framebuffer)
{
	START;

	t_gfx_framebuffer_reset();
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_image_t target	      = t_gfx_framebuffer_memory_target(&gfx);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);
	gfx_frame_t frame	      = {0};

	framebuffer.width = 1;
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	EXPECT_EQ(t_gfx_framebuffer_pass_begin_calls, 0);

	END;
}

TEST(gfx_framebuffer_pass_begin_rejects_stale_format)
{
	START;

	t_gfx_framebuffer_reset();
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain     = t_gfx_framebuffer_swapchain(&gfx, &surface);
	gfx_image_t target	      = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);
	gfx_frame_t frame	      = {0};

	swapchain.format = GFX_FORMAT_BGRA8_UNORM;
	swapchain.generation++;
	target.format	  = GFX_FORMAT_BGRA8_UNORM;
	target.generation = swapchain.generation;
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 1);
	EXPECT_NULL(framebuffer.gfx);

	END;
}

TEST(gfx_framebuffer_pass_begin_refresh_init_failure_clears_framebuffer)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_init_ret    = 1;
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain     = t_gfx_framebuffer_swapchain(&gfx, &surface);
	swapchain.images[0]	      = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_image_t *target	      = &swapchain.images[0];
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, target, &render_pass);
	gfx_frame_t frame	      = {0};

	swapchain.width = 4;
	swapchain.generation++;
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 2);
	EXPECT_EQ(t_gfx_framebuffer_init_calls, 1);
	EXPECT_NULL(framebuffer.gfx);

	END;
}

TEST(gfx_framebuffer_pass_begin_retries_after_stale_driver_failure)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_pass_begin_ret	   = 1;
	t_gfx_framebuffer_pass_begin_stales_target = 1;
	gfx_t gfx				   = {.drv = &t_gfx_framebuffer_driver};
	gfx_surface_t surface			   = {.api = GFX_API_SOFTWARE};
	gfx_swapchain_t swapchain		   = t_gfx_framebuffer_swapchain(&gfx, &surface);
	swapchain.images[0]			   = t_gfx_framebuffer_swapchain_target(&gfx, &swapchain);
	gfx_image_t *target			   = &swapchain.images[0];
	gfx_render_pass_t render_pass		   = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer		   = t_gfx_framebuffer_valid(&gfx, target, &render_pass);
	gfx_frame_t frame			   = {0};

	swapchain.width = 4;
	swapchain.generation++;
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 1);
	EXPECT_EQ(t_gfx_framebuffer_pass_begin_calls, 2);
	EXPECT_EQ(t_gfx_framebuffer_free_calls, 2);
	EXPECT_EQ(t_gfx_framebuffer_init_calls, 2);
	EXPECT_NULL(gfx.frame);
	EXPECT_NULL(frame.gfx);

	END;
}

TEST(gfx_framebuffer_pass_begin_success_defaults_viewport)
{
	START;

	t_gfx_framebuffer_reset();
	gfx_t gfx		      = {.drv = &t_gfx_framebuffer_driver};
	gfx_image_t target	      = t_gfx_framebuffer_memory_target(&gfx);
	gfx_render_pass_t render_pass = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);
	gfx_frame_t frame	      = {0};

	gfx_pass_config_t pass_config = {
		.clear = {0.25f, 0.5f, 0.75f, 1.0f},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(t_gfx_framebuffer_pass_begin_calls, 1);
	EXPECT_PTR(t_gfx_framebuffer, &framebuffer);
	EXPECT_PTR(t_gfx_framebuffer_frame, &frame);
	EXPECT_PTR(gfx.frame, &frame);
	EXPECT_PTR(frame.gfx, &gfx);
	EXPECT_PTR(frame.render_pass, &render_pass);
	EXPECT_EQ(frame.pass.viewport.width, 2);
	EXPECT_EQ(frame.pass.viewport.height, 3);
	EXPECT_PTR(frame.data, (void *)0x5678);

	gfx.frame = NULL;
	END;
}

TEST(gfx_framebuffer_pass_begin_driver_failure_clears_frame)
{
	START;

	t_gfx_framebuffer_reset();
	t_gfx_framebuffer_pass_begin_ret = 1;
	gfx_t gfx			 = {.drv = &t_gfx_framebuffer_driver};
	gfx_image_t target		 = t_gfx_framebuffer_memory_target(&gfx);
	gfx_render_pass_t render_pass	 = t_gfx_framebuffer_render_pass(&gfx);
	gfx_framebuffer_t framebuffer	 = t_gfx_framebuffer_valid(&gfx, &target, &render_pass);
	gfx_frame_t frame		 = {0};

	gfx_pass_config_t pass_config = {
		.clear = {0.0f, 0.0f, 0.0f, 1.0f},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 1);
	EXPECT_EQ(t_gfx_framebuffer_pass_begin_calls, 1);
	EXPECT_NULL(gfx.frame);
	EXPECT_NULL(frame.gfx);

	END;
}

STEST(gfx_framebuffer)
{
	SSTART;
	RUN(gfx_framebuffer_init_rejects_invalid_args);
	RUN(gfx_framebuffer_init_rejects_active_frame);
	RUN(gfx_framebuffer_init_success);
	RUN(gfx_framebuffer_init_driver_failure_clears_framebuffer);
	RUN(gfx_framebuffer_free_ignores_invalid_or_active);
	RUN(gfx_framebuffer_free_calls_driver_and_clears);
	RUN(gfx_framebuffer_resize_rejects_invalid_args);
	RUN(gfx_framebuffer_resize_success);
	RUN(gfx_framebuffer_resize_swapchain_failure_restores_framebuffer);
	RUN(gfx_framebuffer_resize_swapchain_failure_restore_init_failure_clears_framebuffer);
	RUN(gfx_framebuffer_resize_image_init_failure_clears_framebuffer);
	RUN(gfx_framebuffer_resize_format_mismatch_clears_framebuffer);
	RUN(gfx_framebuffer_resize_init_failure_clears_framebuffer);
	RUN(gfx_framebuffer_pass_begin_rejects_invalid_args);
	RUN(gfx_framebuffer_pass_begin_rejects_swapchain_without_swapchain);
	RUN(gfx_framebuffer_pass_begin_rejects_refresh_failure);
	RUN(gfx_framebuffer_pass_begin_rejects_invalidated_during_refresh);
	RUN(gfx_framebuffer_pass_begin_rejects_stale_memory_framebuffer);
	RUN(gfx_framebuffer_pass_begin_rejects_stale_format);
	RUN(gfx_framebuffer_pass_begin_refresh_init_failure_clears_framebuffer);
	RUN(gfx_framebuffer_pass_begin_retries_after_stale_driver_failure);
	RUN(gfx_framebuffer_pass_begin_success_defaults_viewport);
	RUN(gfx_framebuffer_pass_begin_driver_failure_clears_frame);
	SEND;
}
