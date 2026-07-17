#include "gfx_driver.h"

#include "test.h"

static int t_gfx_init_calls;
static int t_gfx_free_calls;
static int t_gfx_native_calls;
static int t_gfx_proc_calls;
static int t_gfx_set_target_calls;
static int t_gfx_clear_color_calls;
static int t_gfx_clear_calls;
static int t_gfx_present_calls;
static int t_gfx_init_ret;
static int t_gfx_free_ret;
static int t_gfx_native_ret;
static int t_gfx_proc_ret;
static int t_gfx_set_target_ret;
static int t_gfx_clear_color_ret;
static int t_gfx_clear_ret;
static int t_gfx_present_ret;
static const gfx_config_t *t_gfx_config;
static strv_t t_gfx_proc_name;
static void *t_gfx_proc_sym;
static gfx_native_t t_gfx_native_value;
static const gfx_target_t *t_gfx_target;
static float t_gfx_r;
static float t_gfx_g;
static float t_gfx_b;
static float t_gfx_a;
static u32 t_gfx_buffers;

static int t_gfx_init(gfx_t *gfx, const gfx_config_t *config)
{
	t_gfx_init_calls++;
	t_gfx_config = config;
	gfx->data    = (void *)0x1234;
	return t_gfx_init_ret;
}

static int t_gfx_free(gfx_t *gfx)
{
	t_gfx_free_calls++;
	gfx->data = NULL;
	return t_gfx_free_ret;
}

static int t_gfx_proc(gfx_t *gfx, strv_t name, void **proc)
{
	(void)gfx;
	t_gfx_proc_calls++;
	t_gfx_proc_name = name;
	*proc		= t_gfx_proc_sym;
	return t_gfx_proc_ret;
}

static int t_gfx_native(gfx_t *gfx, gfx_native_t *native)
{
	(void)gfx;
	t_gfx_native_calls++;
	*native = t_gfx_native_value;
	return t_gfx_native_ret;
}

static int t_gfx_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	(void)gfx;
	t_gfx_set_target_calls++;
	t_gfx_target = target;
	return t_gfx_set_target_ret;
}

static int t_gfx_clear_color(gfx_t *gfx, float r, float g, float b, float a)
{
	(void)gfx;
	t_gfx_clear_color_calls++;
	t_gfx_r = r;
	t_gfx_g = g;
	t_gfx_b = b;
	t_gfx_a = a;
	return t_gfx_clear_color_ret;
}

static int t_gfx_clear(gfx_t *gfx, u32 buffers)
{
	(void)gfx;
	t_gfx_clear_calls++;
	t_gfx_buffers = buffers;
	return t_gfx_clear_ret;
}

static int t_gfx_present(gfx_t *gfx)
{
	(void)gfx;
	t_gfx_present_calls++;
	return t_gfx_present_ret;
}

static gfx_driver_t t_gfx_driver = {
	.name	     = "test",
	.api	     = GFX_API_OPENGL,
	.init	     = t_gfx_init,
	.free	     = t_gfx_free,
	.native	     = t_gfx_native,
	.proc	     = t_gfx_proc,
	.set_target  = t_gfx_set_target,
	.clear_color = t_gfx_clear_color,
	.clear	     = t_gfx_clear,
	.present     = t_gfx_present,
};

DRIVER(t_gfx_non_gfx_driver, 1, NULL);

static void t_gfx_reset(void)
{
	t_gfx_init_calls	= 0;
	t_gfx_free_calls	= 0;
	t_gfx_native_calls	= 0;
	t_gfx_proc_calls	= 0;
	t_gfx_set_target_calls	= 0;
	t_gfx_clear_color_calls = 0;
	t_gfx_clear_calls	= 0;
	t_gfx_present_calls	= 0;
	t_gfx_init_ret		= 0;
	t_gfx_free_ret		= 0;
	t_gfx_native_ret	= 0;
	t_gfx_proc_ret		= 0;
	t_gfx_set_target_ret	= 0;
	t_gfx_clear_color_ret	= 0;
	t_gfx_clear_ret		= 0;
	t_gfx_present_ret	= 0;
	t_gfx_config		= NULL;
	t_gfx_proc_name		= STRV_NULL;
	t_gfx_proc_sym		= NULL;
	t_gfx_native_value	= (gfx_native_t){0};
	t_gfx_target		= NULL;
	t_gfx_r			= 0.0f;
	t_gfx_g			= 0.0f;
	t_gfx_b			= 0.0f;
	t_gfx_a			= 0.0f;
	t_gfx_buffers		= 0;
}

TEST(gfx_init_null_gfx)
{
	START;

	gfx_config_t config = {0};

	EXPECT_EQ(gfx_init(NULL, &t_gfx_driver, &config), NULL);

	END;
}

TEST(gfx_init_null_driver)
{
	START;

	gfx_t gfx	    = {0};
	gfx_config_t config = {0};

	EXPECT_EQ(gfx_init(&gfx, NULL, &config), NULL);

	END;
}

TEST(gfx_init_null_config)
{
	START;

	gfx_t gfx = {0};

	EXPECT_EQ(gfx_init(&gfx, &t_gfx_driver, NULL), NULL);

	END;
}

TEST(gfx_init_null_driver_callback)
{
	START;

	gfx_t gfx	    = {0};
	gfx_driver_t drv    = t_gfx_driver;
	gfx_config_t config = {0};
	drv.init	    = NULL;

	EXPECT_EQ(gfx_init(&gfx, &drv, &config), NULL);

	END;
}

TEST(gfx_init_calls_driver)
{
	START;

	t_gfx_reset();
	gfx_t gfx	    = {0};
	gfx_config_t config = {0};

	EXPECT_EQ(gfx_init(&gfx, &t_gfx_driver, &config), &gfx);
	EXPECT_EQ(t_gfx_init_calls, 1);

	END;
}

TEST(gfx_init_passes_config)
{
	START;

	t_gfx_reset();
	gfx_t gfx	    = {0};
	gfx_config_t config = {0};

	gfx_init(&gfx, &t_gfx_driver, &config);

	EXPECT_EQ(t_gfx_config, &config);

	END;
}

TEST(gfx_init_sets_fields)
{
	START;

	t_gfx_reset();
	gfx_t gfx	    = {0};
	gfx_config_t config = {0};

	gfx_init(&gfx, &t_gfx_driver, &config);

	EXPECT_EQ(gfx.drv, &t_gfx_driver);
	EXPECT_EQ(gfx.data, (void *)0x1234);

	END;
}

TEST(gfx_init_failure_returns_null)
{
	START;

	t_gfx_reset();
	gfx_t gfx	    = {0};
	gfx_config_t config = {0};
	t_gfx_init_ret	    = 1;

	EXPECT_EQ(gfx_init(&gfx, &t_gfx_driver, &config), NULL);

	END;
}

TEST(gfx_init_failure_clears_fields)
{
	START;

	t_gfx_reset();
	gfx_t gfx	    = {0};
	gfx_config_t config = {0};
	t_gfx_init_ret	    = 1;

	gfx_init(&gfx, &t_gfx_driver, &config);

	EXPECT_EQ(gfx.drv, NULL);
	EXPECT_EQ(gfx.data, NULL);

	END;
}

TEST(gfx_free_null_gfx)
{
	START;

	gfx_free(NULL);

	END;
}

TEST(gfx_free_without_driver)
{
	START;

	gfx_t gfx = {0};

	gfx_free(&gfx);

	END;
}

TEST(gfx_free_calls_driver)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv  = &t_gfx_driver,
		.data = (void *)0x1234,
	};

	gfx_free(&gfx);

	EXPECT_EQ(t_gfx_free_calls, 1);

	END;
}

TEST(gfx_free_clears_fields)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv  = &t_gfx_driver,
		.data = (void *)0x1234,
	};

	gfx_free(&gfx);

	EXPECT_EQ(gfx.drv, NULL);
	EXPECT_EQ(gfx.data, NULL);

	END;
}

TEST(gfx_proc_null_gfx)
{
	START;

	void *proc = NULL;

	EXPECT_EQ(gfx_proc(NULL, STRV("test"), &proc), 1);

	END;
}

TEST(gfx_proc_null_driver)
{
	START;

	gfx_t gfx  = {0};
	void *proc = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("test"), &proc), 1);

	END;
}

TEST(gfx_proc_null_driver_callback)
{
	START;

	gfx_driver_t drv = t_gfx_driver;
	drv.proc	 = NULL;

	gfx_t gfx = {
		.drv = &drv,
	};
	void *proc = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("test"), &proc), 1);

	END;
}

TEST(gfx_native_null_gfx)
{
	START;

	gfx_native_t native = {0};

	EXPECT_EQ(gfx_native(NULL, &native), 1);

	END;
}

TEST(gfx_native_null_driver)
{
	START;

	gfx_t gfx	    = {0};
	gfx_native_t native = {0};

	EXPECT_EQ(gfx_native(&gfx, &native), 1);

	END;
}

TEST(gfx_native_null_native)
{
	START;

	gfx_t gfx = {.drv = &t_gfx_driver};

	EXPECT_EQ(gfx_native(&gfx, NULL), 1);

	END;
}

TEST(gfx_native_without_driver_callback_sets_api)
{
	START;

	gfx_driver_t drv    = t_gfx_driver;
	drv.native	    = NULL;
	drv.api		    = GFX_API_SOFTWARE;
	gfx_t gfx	    = {.drv = &drv};
	gfx_native_t native = {0};

	EXPECT_EQ(gfx_native(&gfx, &native), 0);
	EXPECT_EQ(native.api, GFX_API_SOFTWARE);

	END;
}

TEST(gfx_native_calls_driver)
{
	START;

	t_gfx_reset();
	gfx_t gfx	    = {.drv = &t_gfx_driver};
	gfx_native_t native = {0};

	gfx_native(&gfx, &native);

	EXPECT_EQ(t_gfx_native_calls, 1);

	END;
}

TEST(gfx_native_sets_native)
{
	START;

	t_gfx_reset();
	t_gfx_native_value  = (gfx_native_t){.api = GFX_API_VULKAN, .instance = 0x1234};
	gfx_t gfx	    = {.drv = &t_gfx_driver};
	gfx_native_t native = {0};

	gfx_native(&gfx, &native);

	EXPECT_EQ(native.instance, 0x1234);

	END;
}

TEST(gfx_native_returns_driver_result)
{
	START;

	t_gfx_reset();
	t_gfx_native_ret    = 1;
	gfx_t gfx	    = {.drv = &t_gfx_driver};
	gfx_native_t native = {0};

	EXPECT_EQ(gfx_native(&gfx, &native), 1);

	END;
}

TEST(gfx_proc_null_proc)
{
	START;

	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};

	EXPECT_EQ(gfx_proc(&gfx, STRV("test"), NULL), 1);

	END;
}

TEST(gfx_proc_calls_driver)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};
	void *proc = NULL;

	gfx_proc(&gfx, STRV("test"), &proc);

	EXPECT_EQ(t_gfx_proc_calls, 1);

	END;
}

TEST(gfx_proc_passes_name)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};
	void *proc = NULL;

	gfx_proc(&gfx, STRV("test"), &proc);

	EXPECT_EQ(strv_eq(t_gfx_proc_name, STRV("test")), 1);

	END;
}

TEST(gfx_proc_sets_proc)
{
	START;

	t_gfx_reset();
	t_gfx_proc_sym = (void *)0x1234;
	gfx_t gfx      = {
		     .drv = &t_gfx_driver,
	     };
	void *proc = NULL;

	gfx_proc(&gfx, STRV("test"), &proc);

	EXPECT_EQ(proc, (void *)0x1234);

	END;
}

TEST(gfx_proc_returns_driver_result)
{
	START;

	t_gfx_reset();
	t_gfx_proc_ret = 1;
	gfx_t gfx      = {
		     .drv = &t_gfx_driver,
	     };
	void *proc = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("test"), &proc), 1);

	END;
}

TEST(gfx_set_target_null_gfx)
{
	START;

	gfx_target_t target = {0};

	EXPECT_EQ(gfx_set_target(NULL, &target), 1);

	END;
}

TEST(gfx_set_target_null_driver)
{
	START;

	gfx_t gfx	    = {0};
	gfx_target_t target = {0};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	END;
}

TEST(gfx_set_target_null_driver_callback)
{
	START;

	gfx_driver_t drv = t_gfx_driver;
	drv.set_target	 = NULL;

	gfx_t gfx = {
		.drv = &drv,
	};
	gfx_target_t target = {0};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	END;
}

TEST(gfx_set_target_null_target)
{
	START;

	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};

	EXPECT_EQ(gfx_set_target(&gfx, NULL), 1);

	END;
}

TEST(gfx_set_target_calls_driver)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};
	gfx_target_t target = {0};

	gfx_set_target(&gfx, &target);

	EXPECT_EQ(t_gfx_set_target_calls, 1);

	END;
}

TEST(gfx_set_target_passes_target)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};
	gfx_target_t target = {0};

	gfx_set_target(&gfx, &target);

	EXPECT_EQ(t_gfx_target, &target);

	END;
}

TEST(gfx_set_target_returns_driver_result)
{
	START;

	t_gfx_reset();
	t_gfx_set_target_ret = 1;
	gfx_t gfx	     = {
			   .drv = &t_gfx_driver,
	   };
	gfx_target_t target = {0};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	END;
}

TEST(gfx_clear_color_null_gfx)
{
	START;

	EXPECT_EQ(gfx_clear_color(NULL, 0.0f, 0.0f, 0.0f, 0.0f), 1);

	END;
}

TEST(gfx_clear_color_null_driver)
{
	START;

	gfx_t gfx = {0};

	EXPECT_EQ(gfx_clear_color(&gfx, 0.0f, 0.0f, 0.0f, 0.0f), 1);

	END;
}

TEST(gfx_clear_color_null_driver_callback)
{
	START;

	gfx_driver_t drv = t_gfx_driver;
	drv.clear_color	 = NULL;

	gfx_t gfx = {
		.drv = &drv,
	};

	EXPECT_EQ(gfx_clear_color(&gfx, 0.0f, 0.0f, 0.0f, 0.0f), 1);

	END;
}

TEST(gfx_clear_color_calls_driver)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};

	EXPECT_EQ(gfx_clear_color(&gfx, 0.1f, 0.2f, 0.3f, 0.4f), 0);
	EXPECT_EQ(t_gfx_clear_color_calls, 1);
	EXPECT_EQ(t_gfx_r, 0.1f);
	EXPECT_EQ(t_gfx_g, 0.2f);
	EXPECT_EQ(t_gfx_b, 0.3f);
	EXPECT_EQ(t_gfx_a, 0.4f);

	END;
}

TEST(gfx_clear_color_returns_driver_result)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};
	t_gfx_clear_color_ret = 1;

	EXPECT_EQ(gfx_clear_color(&gfx, 0.0f, 0.0f, 0.0f, 0.0f), 1);

	END;
}

TEST(gfx_clear_null_gfx)
{
	START;

	EXPECT_EQ(gfx_clear(NULL, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_clear_null_driver)
{
	START;

	gfx_t gfx = {0};

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_clear_null_driver_callback)
{
	START;

	gfx_driver_t drv = t_gfx_driver;
	drv.clear	 = NULL;

	gfx_t gfx = {
		.drv = &drv,
	};

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_clear_calls_driver)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 0);
	EXPECT_EQ(t_gfx_clear_calls, 1);
	EXPECT_EQ(t_gfx_buffers, GFX_CLEAR_COLOR_BUFFER);

	END;
}

TEST(gfx_clear_returns_driver_result)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};
	t_gfx_clear_ret = 1;

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_present_null_gfx)
{
	START;

	EXPECT_EQ(gfx_present(NULL), 1);

	END;
}

TEST(gfx_present_null_driver)
{
	START;

	gfx_t gfx = {0};

	EXPECT_EQ(gfx_present(&gfx), 1);

	END;
}

TEST(gfx_present_null_driver_callback)
{
	START;

	gfx_driver_t drv = t_gfx_driver;
	drv.present	 = NULL;

	gfx_t gfx = {
		.drv = &drv,
	};

	EXPECT_EQ(gfx_present(&gfx), 1);

	END;
}

TEST(gfx_present_calls_driver)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};

	EXPECT_EQ(gfx_present(&gfx), 0);
	EXPECT_EQ(t_gfx_present_calls, 1);

	END;
}

TEST(gfx_present_returns_driver_result)
{
	START;

	t_gfx_reset();
	gfx_t gfx = {
		.drv = &t_gfx_driver,
	};
	t_gfx_present_ret = 1;

	EXPECT_EQ(gfx_present(&gfx), 1);

	END;
}

TEST(gfx_driver_list_counts_all_without_plan)
{
	START;

	EXPECT_NE(gfx_driver_list(NULL, 0), 0u);

	END;
}

TEST(gfx_driver_find_returns_null_for_missing_driver)
{
	START;

	EXPECT_EQ(gfx_driver_find(STRV("missing")), NULL);

	END;
}

TEST(gfx_driver_list_limits_output)
{
	START;

	gfx_driver_t *drivers[1] = {0};
	u32 count		 = gfx_driver_list(drivers, sizeof(drivers) / sizeof(drivers[0]));

	EXPECT_NE(count, 0u);
	EXPECT_NE(drivers[0], NULL);

	END;
}

STEST(gfx)
{
	SSTART;

	RUN(gfx_init_null_gfx);
	RUN(gfx_init_null_driver);
	RUN(gfx_init_null_config);
	RUN(gfx_init_null_driver_callback);
	RUN(gfx_init_calls_driver);
	RUN(gfx_init_passes_config);
	RUN(gfx_init_sets_fields);
	RUN(gfx_init_failure_returns_null);
	RUN(gfx_init_failure_clears_fields);
	RUN(gfx_free_null_gfx);
	RUN(gfx_free_without_driver);
	RUN(gfx_free_calls_driver);
	RUN(gfx_free_clears_fields);
	RUN(gfx_native_null_gfx);
	RUN(gfx_native_null_driver);
	RUN(gfx_native_null_native);
	RUN(gfx_native_without_driver_callback_sets_api);
	RUN(gfx_native_calls_driver);
	RUN(gfx_native_sets_native);
	RUN(gfx_native_returns_driver_result);
	RUN(gfx_proc_null_gfx);
	RUN(gfx_proc_null_driver);
	RUN(gfx_proc_null_driver_callback);
	RUN(gfx_proc_null_proc);
	RUN(gfx_proc_calls_driver);
	RUN(gfx_proc_passes_name);
	RUN(gfx_proc_sets_proc);
	RUN(gfx_proc_returns_driver_result);
	RUN(gfx_set_target_null_gfx);
	RUN(gfx_set_target_null_driver);
	RUN(gfx_set_target_null_driver_callback);
	RUN(gfx_set_target_null_target);
	RUN(gfx_set_target_calls_driver);
	RUN(gfx_set_target_passes_target);
	RUN(gfx_set_target_returns_driver_result);
	RUN(gfx_clear_color_null_gfx);
	RUN(gfx_clear_color_null_driver);
	RUN(gfx_clear_color_null_driver_callback);
	RUN(gfx_clear_color_calls_driver);
	RUN(gfx_clear_color_returns_driver_result);
	RUN(gfx_clear_null_gfx);
	RUN(gfx_clear_null_driver);
	RUN(gfx_clear_null_driver_callback);
	RUN(gfx_clear_calls_driver);
	RUN(gfx_clear_returns_driver_result);
	RUN(gfx_present_null_gfx);
	RUN(gfx_present_null_driver);
	RUN(gfx_present_null_driver_callback);
	RUN(gfx_present_calls_driver);
	RUN(gfx_present_returns_driver_result);
	RUN(gfx_driver_list_counts_all_without_plan);
	RUN(gfx_driver_find_returns_null_for_missing_driver);
	RUN(gfx_driver_list_limits_output);

	SEND;
}
