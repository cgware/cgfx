#include "gfx.h"

#include "gfx_driver.h"
#include "test.h"

static int t_init_calls;
static int t_free_calls;
static int t_target_init_calls;
static int t_render_pass_init_calls;
static int t_framebuffer_init_calls;
static int t_framebuffer_pass_begin_calls;
static int t_pipeline_bind_calls;
static int t_buffer_bind_calls;
static int t_draw_calls;
static int t_end_calls;
static int t_proc_calls;
static int t_native_calls;
static int t_init_ret;
static int t_proc_ret;
static int t_native_ret;
static strv_t t_proc_name;
static void *t_proc_sym;
static gfx_native_t t_native_value;

static int t_gfx_drv_init(gfx_t *gfx, const gfx_config_t *config)
{
	(void)config;
	t_init_calls++;
	gfx->data = (void *)1;
	return t_init_ret;
}

static int t_free(gfx_t *gfx)
{
	t_free_calls++;
	gfx->data = NULL;
	return 0;
}

static int t_target_init(gfx_target_t *target)
{
	(void)target;
	t_target_init_calls++;
	return 0;
}

static int t_render_pass_init(gfx_render_pass_t *render_pass, const gfx_render_pass_config_t *config)
{
	(void)render_pass;
	(void)config;
	t_render_pass_init_calls++;
	return 0;
}

static int t_framebuffer_init(gfx_framebuffer_t *framebuffer)
{
	(void)framebuffer;
	t_framebuffer_init_calls++;
	return 0;
}

static int t_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame)
{
	(void)framebuffer;
	(void)frame;
	t_framebuffer_pass_begin_calls++;
	return 0;
}

static int t_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	(void)config;
	buffer->data = (void *)1;
	return 0;
}

static void t_buffer_free(gfx_buffer_t *buffer)
{
	buffer->data = NULL;
}

static int t_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	(void)buffer;
	(void)data;
	(void)size;
	return 0;
}

static int t_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	(void)frame;
	(void)buffer;
	t_buffer_bind_calls++;
	return 0;
}

static int t_shader_init(gfx_shader_t *shader, const gfx_shader_config_t *config)
{
	(void)config;
	shader->data = (void *)1;
	return 0;
}

static void t_shader_free(gfx_shader_t *shader)
{
	shader->data = NULL;
}

static int t_pipeline_init(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config)
{
	(void)config;
	pipeline->data = (void *)1;
	return 0;
}

static void t_pipeline_free(gfx_pipeline_t *pipeline)
{
	pipeline->data = NULL;
}

static int t_pipeline_bind(gfx_frame_t *frame, const gfx_pipeline_t *pipeline)
{
	(void)frame;
	(void)pipeline;
	t_pipeline_bind_calls++;
	return 0;
}

static int t_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex)
{
	(void)frame;
	(void)vertex_count;
	(void)first_vertex;
	t_draw_calls++;
	return 0;
}

static int t_gfx_drv_end(gfx_frame_t *frame)
{
	(void)frame;
	t_end_calls++;
	return 0;
}

static int t_proc(gfx_t *gfx, strv_t name, void **proc)
{
	(void)gfx;
	t_proc_calls++;
	t_proc_name = name;
	*proc	    = t_proc_sym;
	return t_proc_ret;
}

static int t_native(gfx_t *gfx, gfx_native_t *native)
{
	(void)gfx;
	t_native_calls++;
	*native = t_native_value;
	return t_native_ret;
}

static gfx_driver_t t_driver = {
	.name			= "test",
	.api			= GFX_API_SOFTWARE,
	.init			= t_gfx_drv_init,
	.free			= t_free,
	.native			= t_native,
	.proc			= t_proc,
	.target_init		= t_target_init,
	.render_pass_init	= t_render_pass_init,
	.framebuffer_init	= t_framebuffer_init,
	.framebuffer_pass_begin = t_framebuffer_pass_begin,
	.buffer_init		= t_buffer_init,
	.buffer_free		= t_buffer_free,
	.buffer_set_data	= t_buffer_set_data,
	.buffer_bind		= t_buffer_bind,
	.shader_init		= t_shader_init,
	.shader_free		= t_shader_free,
	.pipeline_init		= t_pipeline_init,
	.pipeline_free		= t_pipeline_free,
	.pipeline_bind		= t_pipeline_bind,
	.draw			= t_draw,
	.end			= t_gfx_drv_end,
};

DRIVER(t_gfx_non_gfx_driver, 1, NULL);

static void t_reset(void)
{
	t_init_calls		       = 0;
	t_free_calls		       = 0;
	t_target_init_calls	       = 0;
	t_render_pass_init_calls       = 0;
	t_framebuffer_init_calls       = 0;
	t_framebuffer_pass_begin_calls = 0;
	t_pipeline_bind_calls	       = 0;
	t_buffer_bind_calls	       = 0;
	t_draw_calls		       = 0;
	t_end_calls		       = 0;
	t_proc_calls		       = 0;
	t_native_calls		       = 0;
	t_init_ret		       = 0;
	t_proc_ret		       = 0;
	t_native_ret		       = 0;
	t_proc_name		       = STRV_NULL;
	t_proc_sym		       = NULL;
	t_native_value		       = (gfx_native_t){0};
}

TEST(gfx_init_null_gfx)
{
	START;

	EXPECT_NULL(gfx_init(NULL, &t_driver, &(gfx_config_t){0}, NULL, ALLOC_STD));

	END;
}

TEST(gfx_init_success)
{
	START;

	t_reset();
	gfx_t gfx = {0};

	EXPECT_PTR(gfx_init(&gfx, &t_driver, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	EXPECT_EQ(t_init_calls, 1);

	gfx_free(&gfx);
	EXPECT_EQ(t_free_calls, 1);
	END;
}

TEST(gfx_init_driver_failure_calls_free_and_clears)
{
	START;

	t_reset();
	t_init_ret = 1;
	gfx_t gfx  = {0};

	EXPECT_NULL(gfx_init(&gfx, &t_driver, &(gfx_config_t){0}, NULL, ALLOC_STD));
	EXPECT_EQ(t_init_calls, 1);
	EXPECT_EQ(t_free_calls, 1);
	EXPECT_NULL(gfx.drv);
	EXPECT_NULL(gfx.data);

	END;
}

TEST(gfx_free_ignores_invalid_args)
{
	START;

	t_reset();

	gfx_free(NULL);
	gfx_free(&(gfx_t){0});
	gfx_free(&(gfx_t){.drv = &(gfx_driver_t){.name = "no-free"}});

	EXPECT_EQ(t_free_calls, 0);

	END;
}

TEST(gfx_proc_rejects_invalid_args)
{
	START;

	void *proc = NULL;
	gfx_t gfx  = {.drv = &t_driver};

	EXPECT_EQ(gfx_proc(NULL, STRV("symbol"), &proc), 1);
	EXPECT_EQ(gfx_proc(&(gfx_t){0}, STRV("symbol"), &proc), 1);
	EXPECT_EQ(gfx_proc(&(gfx_t){.drv = &(gfx_driver_t){.name = "no-proc"}}, STRV("symbol"), &proc), 1);
	EXPECT_EQ(gfx_proc(&gfx, STRV("symbol"), NULL), 1);

	END;
}

TEST(gfx_proc_calls_driver)
{
	START;

	t_reset();
	t_proc_sym = (void *)0x1234;
	gfx_t gfx  = {.drv = &t_driver};
	void *proc = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("symbol"), &proc), 0);
	EXPECT_EQ(t_proc_calls, 1);
	EXPECT_EQ(strv_eq(t_proc_name, STRV("symbol")), 1);
	EXPECT_PTR(proc, (void *)0x1234);

	t_proc_ret = 1;
	EXPECT_EQ(gfx_proc(&gfx, STRV("symbol"), &proc), 1);

	END;
}

TEST(gfx_native_rejects_invalid_args)
{
	START;

	gfx_native_t native = {0};

	EXPECT_EQ(gfx_native(NULL, &native), 1);
	EXPECT_EQ(gfx_native(&(gfx_t){0}, &native), 1);
	EXPECT_EQ(gfx_native(&(gfx_t){.drv = &t_driver}, NULL), 1);

	END;
}

TEST(gfx_native_sets_api_without_driver_callback)
{
	START;

	gfx_driver_t drv    = t_driver;
	drv.native	    = NULL;
	gfx_t gfx	    = {.drv = &drv};
	gfx_native_t native = {0};

	EXPECT_EQ(gfx_native(&gfx, &native), 0);
	EXPECT_EQ(native.api, GFX_API_SOFTWARE);

	END;
}

TEST(gfx_native_calls_driver)
{
	START;

	t_reset();
	t_native_value = (gfx_native_t){
		.api	= GFX_API_D3D11,
		.device = 42,
	};
	gfx_t gfx	    = {.drv = &t_driver};
	gfx_native_t native = {0};

	EXPECT_EQ(gfx_native(&gfx, &native), 0);
	EXPECT_EQ(t_native_calls, 1);
	EXPECT_EQ(native.api, GFX_API_D3D11);
	EXPECT_EQ(native.device, 42);

	t_native_ret = 1;
	EXPECT_EQ(gfx_native(&gfx, &native), 1);

	END;
}

TEST(gfx_driver_find_returns_null_for_unknown_driver)
{
	START;

	EXPECT_NULL(gfx_driver_find(STRV("missing-driver")));

	END;
}

TEST(gfx_driver_next_and_list_enumerate_registered_drivers)
{
	START;

	gfx_driver_t *first = gfx_driver_next(NULL);
	EXPECT_NOT_NULL(first);
	EXPECT_NE(first->api, GFX_API_NONE);

	gfx_driver_t invalid = {.api = (gfx_api_t)99};
	EXPECT_PTR(gfx_driver_next(&invalid), first);

	gfx_driver_t *drivers[8] = {0};
	u32 count		 = gfx_driver_list(drivers, 2);
	EXPECT_GE(count, 2);
	EXPECT_NOT_NULL(drivers[0]);
	EXPECT_NOT_NULL(drivers[1]);
	EXPECT_EQ(gfx_driver_list(NULL, 0), count);

	END;
}

TEST(gfx_frame_requires_bound_pipeline_and_buffer)
{
	START;

	t_reset();
	u8 pixels[16] = {0};
	gfx_t gfx     = {0};
	EXPECT_PTR(gfx_init(&gfx, &t_driver, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 2,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	EXPECT_EQ(t_target_init_calls, 1);

	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	EXPECT_EQ(t_render_pass_init_calls, 1);

	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(t_framebuffer_init_calls, 1);

	gfx_frame_t frame	      = {0};
	gfx_pass_config_t pass_config = {
		.clear = {0.0f, 0.0f, 0.0f, 1.0f},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(t_framebuffer_pass_begin_calls, 1);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 1);

	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("shader")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(t_pipeline_bind_calls, 1);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 1);

	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(t_buffer_bind_calls, 1);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(t_draw_calls, 1);

	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(t_end_calls, 1);

	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	END;
}

STEST(gfx)
{
	SSTART;
	RUN(gfx_init_null_gfx);
	RUN(gfx_init_success);
	RUN(gfx_init_driver_failure_calls_free_and_clears);
	RUN(gfx_free_ignores_invalid_args);
	RUN(gfx_proc_rejects_invalid_args);
	RUN(gfx_proc_calls_driver);
	RUN(gfx_native_rejects_invalid_args);
	RUN(gfx_native_sets_api_without_driver_callback);
	RUN(gfx_native_calls_driver);
	RUN(gfx_driver_find_returns_null_for_unknown_driver);
	RUN(gfx_driver_next_and_list_enumerate_registered_drivers);
	RUN(gfx_frame_requires_bound_pipeline_and_buffer);
	SEND;
}
