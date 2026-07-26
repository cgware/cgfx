#include "gfx_driver.h"

#include "test.h"

static int t_gfx_pipeline_init_calls;
static int t_gfx_pipeline_free_calls;
static int t_gfx_pipeline_init_ret;
static const gfx_pipeline_config_t *t_gfx_pipeline_config;

static int t_gfx_pipeline_init(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config)
{
	t_gfx_pipeline_init_calls++;
	t_gfx_pipeline_config = config;
	pipeline->data	      = (void *)0x9ABC;
	return t_gfx_pipeline_init_ret;
}

static void t_gfx_pipeline_free(gfx_pipeline_t *pipeline)
{
	t_gfx_pipeline_free_calls++;
	pipeline->data = NULL;
}

static gfx_driver_t t_gfx_pipeline_driver = {
	.name	       = "test",
	.api	       = GFX_API_OPENGL,
	.pipeline_init = t_gfx_pipeline_init,
	.pipeline_free = t_gfx_pipeline_free,
};

static void t_gfx_pipeline_reset(void)
{
	t_gfx_pipeline_init_calls = 0;
	t_gfx_pipeline_free_calls = 0;
	t_gfx_pipeline_init_ret	  = 0;
	t_gfx_pipeline_config	  = NULL;
}

TEST(gfx_pipeline_init_null_pipeline)
{
	START;

	gfx_t gfx = {
		.drv = &t_gfx_pipeline_driver,
	};
	gfx_pipeline_config_t config = {0};

	EXPECT_NULL(gfx_pipeline_init(NULL, &gfx, &config));

	END;
}

TEST(gfx_pipeline_init_null_gfx)
{
	START;

	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, NULL, &config));

	END;
}

TEST(gfx_pipeline_init_null_driver)
{
	START;

	gfx_t gfx		     = {0};
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));

	END;
}

TEST(gfx_pipeline_init_null_driver_callback)
{
	START;

	gfx_driver_t drv	     = t_gfx_pipeline_driver;
	drv.pipeline_init	     = NULL;
	gfx_t gfx		     = {.drv = &drv};
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));

	END;
}

TEST(gfx_pipeline_init_success)
{
	START;

	t_gfx_pipeline_reset();
	gfx_t gfx		     = {.drv = &t_gfx_pipeline_driver};
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = {0};

	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &config), &pipeline);
	EXPECT_EQ(t_gfx_pipeline_init_calls, 1);
	EXPECT_PTR(t_gfx_pipeline_config, &config);
	EXPECT_PTR(pipeline.gfx, &gfx);
	EXPECT_PTR(pipeline.data, (void *)0x9ABC);

	END;
}

TEST(gfx_pipeline_init_returns_null_on_driver_failure)
{
	START;

	t_gfx_pipeline_reset();
	t_gfx_pipeline_init_ret	     = 1;
	gfx_t gfx		     = {.drv = &t_gfx_pipeline_driver};
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));

	END;
}

TEST(gfx_pipeline_free_calls_driver_and_clears_data)
{
	START;

	t_gfx_pipeline_reset();
	gfx_t gfx		= {.drv = &t_gfx_pipeline_driver};
	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = (void *)0x9ABC,
	};

	gfx_pipeline_free(&pipeline);

	EXPECT_EQ(t_gfx_pipeline_free_calls, 1);
	EXPECT_NULL(pipeline.data);

	END;
}

TEST(gfx_pipeline_free_null_pipeline)
{
	START;

	gfx_pipeline_free(NULL);

	END;
}

STEST(gfx_pipeline)
{
	SSTART;

	RUN(gfx_pipeline_init_null_pipeline);
	RUN(gfx_pipeline_init_null_gfx);
	RUN(gfx_pipeline_init_null_driver);
	RUN(gfx_pipeline_init_null_driver_callback);
	RUN(gfx_pipeline_init_success);
	RUN(gfx_pipeline_init_returns_null_on_driver_failure);
	RUN(gfx_pipeline_free_calls_driver_and_clears_data);
	RUN(gfx_pipeline_free_null_pipeline);

	SEND;
}
