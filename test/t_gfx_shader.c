#include "gfx_driver.h"

#include "log.h"
#include "test.h"

static int t_gfx_shader_init_calls;
static int t_gfx_shader_free_calls;
static int t_gfx_shader_init_ret;
static const gfx_shader_config_t *t_gfx_shader_config;

static void t_gfx_shader_reset(void)
{
	t_gfx_shader_init_calls = 0;
	t_gfx_shader_free_calls = 0;
	t_gfx_shader_init_ret	= 0;
	t_gfx_shader_config	= NULL;
}

static int t_gfx_shader_init(gfx_shader_t *shader, const gfx_shader_config_t *config)
{
	t_gfx_shader_init_calls++;
	t_gfx_shader_config = config;
	shader->data	    = (void *)0x5678;
	return t_gfx_shader_init_ret;
}

static void t_gfx_shader_free(gfx_shader_t *shader)
{
	t_gfx_shader_free_calls++;
	shader->data = NULL;
}

static gfx_driver_t t_gfx_shader_driver = {
	.name	     = "test",
	.api	     = GFX_API_OPENGL,
	.shader_init = t_gfx_shader_init,
	.shader_free = t_gfx_shader_free,
};

TEST(gfx_shader_init_null_shader)
{
	START;

	gfx_t gfx = {
		.drv = &t_gfx_shader_driver,
	};
	gfx_shader_config_t config = {0};

	EXPECT_NULL(gfx_shader_init(NULL, &gfx, &config));

	END;
}

TEST(gfx_shader_init_null_gfx)
{
	START;

	gfx_shader_t shader	   = {0};
	gfx_shader_config_t config = {0};

	EXPECT_NULL(gfx_shader_init(&shader, NULL, &config));

	END;
}

TEST(gfx_shader_init_null_driver)
{
	START;

	gfx_t gfx		   = {0};
	gfx_shader_t shader	   = {0};
	gfx_shader_config_t config = {0};

	EXPECT_NULL(gfx_shader_init(&shader, &gfx, &config));

	END;
}

TEST(gfx_shader_init_null_driver_callback)
{
	START;

	gfx_driver_t drv	   = t_gfx_shader_driver;
	drv.shader_init		   = NULL;
	gfx_t gfx		   = {.drv = &drv};
	gfx_shader_t shader	   = {0};
	gfx_shader_config_t config = {0};

	EXPECT_NULL(gfx_shader_init(&shader, &gfx, &config));

	END;
}

TEST(gfx_shader_init_success)
{
	START;

	t_gfx_shader_reset();
	gfx_t gfx		   = {.drv = &t_gfx_shader_driver};
	gfx_shader_t shader	   = {0};
	gfx_shader_config_t config = {0};

	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &config), &shader);
	EXPECT_EQ(t_gfx_shader_init_calls, 1);
	EXPECT_PTR(t_gfx_shader_config, &config);
	EXPECT_PTR(shader.gfx, &gfx);
	EXPECT_PTR(shader.data, (void *)0x5678);

	END;
}

TEST(gfx_shader_init_returns_null_on_driver_failure)
{
	START;

	t_gfx_shader_reset();
	t_gfx_shader_init_ret	   = 1;
	gfx_t gfx		   = {.drv = &t_gfx_shader_driver};
	gfx_shader_t shader	   = {0};
	gfx_shader_config_t config = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, &config));
	log_set_quiet(0, 0);

	END;
}

TEST(gfx_shader_free_calls_driver_and_clears_data)
{
	START;

	t_gfx_shader_reset();
	gfx_t gfx	    = {.drv = &t_gfx_shader_driver};
	gfx_shader_t shader = {
		.gfx  = &gfx,
		.data = (void *)0x5678,
	};

	gfx_shader_free(&shader);

	EXPECT_EQ(t_gfx_shader_free_calls, 1);
	EXPECT_NULL(shader.data);

	END;
}

TEST(gfx_shader_free_null_shader)
{
	START;

	gfx_shader_free(NULL);

	END;
}

STEST(gfx_shader)
{
	SSTART;

	RUN(gfx_shader_init_null_shader);
	RUN(gfx_shader_init_null_gfx);
	RUN(gfx_shader_init_null_driver);
	RUN(gfx_shader_init_null_driver_callback);
	RUN(gfx_shader_init_success);
	RUN(gfx_shader_init_returns_null_on_driver_failure);
	RUN(gfx_shader_free_calls_driver_and_clears_data);
	RUN(gfx_shader_free_null_shader);

	SEND;
}
