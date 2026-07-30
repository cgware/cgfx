#include "gfx_render_pass.h"

#include "gfx_driver.h"
#include "test.h"

static int t_gfx_render_pass_init_calls;
static int t_gfx_render_pass_free_calls;
static int t_gfx_render_pass_init_ret;
static gfx_render_pass_t *t_gfx_render_pass;
static const gfx_render_pass_config_t *t_gfx_render_pass_config;

static int t_gfx_render_pass_init(gfx_render_pass_t *render_pass, const gfx_render_pass_config_t *config)
{
	t_gfx_render_pass_init_calls++;
	t_gfx_render_pass	 = render_pass;
	t_gfx_render_pass_config = config;
	render_pass->data	 = (void *)0x1234;
	return t_gfx_render_pass_init_ret;
}

static void t_gfx_render_pass_free(gfx_render_pass_t *render_pass)
{
	t_gfx_render_pass_free_calls++;
	t_gfx_render_pass = render_pass;
	render_pass->data = NULL;
}

static gfx_driver_t t_gfx_render_pass_driver = {
	.name		  = "test-render-pass",
	.api		  = GFX_API_SOFTWARE,
	.render_pass_init = t_gfx_render_pass_init,
	.render_pass_free = t_gfx_render_pass_free,
};

static void t_gfx_render_pass_reset(void)
{
	t_gfx_render_pass_init_calls = 0;
	t_gfx_render_pass_free_calls = 0;
	t_gfx_render_pass_init_ret   = 0;
	t_gfx_render_pass	     = NULL;
	t_gfx_render_pass_config     = NULL;
}

TEST(gfx_render_pass_init_rejects_invalid_args)
{
	START;

	gfx_t gfx			= {.drv = &t_gfx_render_pass_driver};
	gfx_render_pass_t render_pass	= {0};
	gfx_render_pass_config_t config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};

	EXPECT_NULL(gfx_render_pass_init(NULL, &gfx, &config));
	EXPECT_NULL(gfx_render_pass_init(&render_pass, NULL, &config));
	EXPECT_NULL(gfx_render_pass_init(&render_pass, &(gfx_t){0}, &config));
	EXPECT_NULL(gfx_render_pass_init(&render_pass, &gfx, NULL));
	EXPECT_NULL(gfx_render_pass_init(&render_pass, &gfx, &(gfx_render_pass_config_t){.color_format = GFX_FORMAT_NONE}));
	gfx_render_pass_config_t render_pass_config_load = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = (gfx_load_op_t)99,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_NULL(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config_load));
	gfx_render_pass_config_t render_pass_config_store = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = (gfx_store_op_t)99,
	};
	EXPECT_NULL(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config_store));

	END;
}

TEST(gfx_render_pass_init_rejects_active_frame)
{
	START;

	gfx_frame_t frame	      = {0};
	gfx_t gfx		      = {.drv = &t_gfx_render_pass_driver, .frame = &frame};
	gfx_render_pass_t render_pass = {0};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_NULL(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config));

	END;
}

TEST(gfx_render_pass_init_success)
{
	START;

	t_gfx_render_pass_reset();
	gfx_t gfx			= {.drv = &t_gfx_render_pass_driver};
	gfx_render_pass_t render_pass	= {0};
	gfx_render_pass_config_t config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};

	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &config), &render_pass);
	EXPECT_EQ(t_gfx_render_pass_init_calls, 1);
	EXPECT_PTR(t_gfx_render_pass, &render_pass);
	EXPECT_PTR(t_gfx_render_pass_config, &config);
	EXPECT_PTR(render_pass.gfx, &gfx);
	EXPECT_EQ(render_pass.color_format, GFX_FORMAT_RGBA8);
	EXPECT_EQ(render_pass.load, GFX_LOAD_CLEAR);
	EXPECT_EQ(render_pass.store, GFX_STORE_STORE);
	EXPECT_PTR(render_pass.data, (void *)0x1234);

	gfx_render_pass_free(&render_pass);
	END;
}

TEST(gfx_render_pass_init_driver_failure_clears_render_pass)
{
	START;

	t_gfx_render_pass_reset();
	t_gfx_render_pass_init_ret    = 1;
	gfx_t gfx		      = {.drv = &t_gfx_render_pass_driver};
	gfx_render_pass_t render_pass = {0};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_NULL(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config));
	EXPECT_EQ(t_gfx_render_pass_init_calls, 1);
	EXPECT_EQ(t_gfx_render_pass_free_calls, 1);
	EXPECT_NULL(render_pass.gfx);

	END;
}

TEST(gfx_render_pass_free_ignores_invalid_or_active)
{
	START;

	t_gfx_render_pass_reset();
	gfx_frame_t frame = {0};
	gfx_t gfx	  = {.drv = &t_gfx_render_pass_driver, .frame = &frame};

	gfx_render_pass_free(NULL);
	gfx_render_pass_free(&(gfx_render_pass_t){0});
	gfx_render_pass_free(&(gfx_render_pass_t){.gfx = &gfx});

	EXPECT_EQ(t_gfx_render_pass_free_calls, 0);

	END;
}

TEST(gfx_render_pass_free_calls_driver_and_clears)
{
	START;

	t_gfx_render_pass_reset();
	gfx_t gfx		      = {.drv = &t_gfx_render_pass_driver};
	gfx_render_pass_t render_pass = {.gfx = &gfx, .data = (void *)0x1234};

	gfx_render_pass_free(&render_pass);

	EXPECT_EQ(t_gfx_render_pass_free_calls, 1);
	EXPECT_PTR(t_gfx_render_pass, &render_pass);
	EXPECT_NULL(render_pass.gfx);
	EXPECT_NULL(render_pass.data);

	END;
}

STEST(gfx_render_pass)
{
	SSTART;
	RUN(gfx_render_pass_init_rejects_invalid_args);
	RUN(gfx_render_pass_init_rejects_active_frame);
	RUN(gfx_render_pass_init_success);
	RUN(gfx_render_pass_init_driver_failure_clears_render_pass);
	RUN(gfx_render_pass_free_ignores_invalid_or_active);
	RUN(gfx_render_pass_free_calls_driver_and_clears);
	SEND;
}
