#include "gfx_buffer.h"

#include "gfx_driver.h"
#include "test.h"

static int t_gfx_buffer_init_calls;
static int t_gfx_buffer_free_calls;
static int t_gfx_buffer_set_data_calls;
static int t_gfx_buffer_bind_calls;
static int t_gfx_buffer_init_ret;
static int t_gfx_buffer_set_data_ret;
static int t_gfx_buffer_bind_ret;
static const gfx_buffer_config_t *t_gfx_buffer_config;
static gfx_buffer_t *t_gfx_buffer;
static const void *t_gfx_buffer_data;
static size_t t_gfx_buffer_size;

static int t_gfx_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	t_gfx_buffer_init_calls++;
	t_gfx_buffer_config = config;
	buffer->data	    = (void *)0x5678;
	return t_gfx_buffer_init_ret;
}

static void t_gfx_buffer_free(gfx_buffer_t *buffer)
{
	t_gfx_buffer_free_calls++;
	t_gfx_buffer = buffer;
	buffer->data = NULL;
}

static int t_gfx_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	t_gfx_buffer_set_data_calls++;
	t_gfx_buffer	  = buffer;
	t_gfx_buffer_data = data;
	t_gfx_buffer_size = size;
	return t_gfx_buffer_set_data_ret;
}

static int t_gfx_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	(void)frame;
	t_gfx_buffer_bind_calls++;
	t_gfx_buffer = (gfx_buffer_t *)buffer;
	return t_gfx_buffer_bind_ret;
}

static gfx_driver_t t_gfx_buffer_driver = {
	.name		 = "test-buffer",
	.api		 = GFX_API_OPENGL,
	.buffer_init	 = t_gfx_buffer_init,
	.buffer_free	 = t_gfx_buffer_free,
	.buffer_set_data = t_gfx_buffer_set_data,
	.buffer_bind	 = t_gfx_buffer_bind,
};

static void t_gfx_buffer_reset(void)
{
	t_gfx_buffer_init_calls	    = 0;
	t_gfx_buffer_free_calls	    = 0;
	t_gfx_buffer_set_data_calls = 0;
	t_gfx_buffer_bind_calls	    = 0;
	t_gfx_buffer_init_ret	    = 0;
	t_gfx_buffer_set_data_ret   = 0;
	t_gfx_buffer_bind_ret	    = 0;
	t_gfx_buffer_config	    = NULL;
	t_gfx_buffer		    = NULL;
	t_gfx_buffer_data	    = NULL;
	t_gfx_buffer_size	    = 0;
}

TEST(gfx_buffer_init_null_buffer)
{
	START;

	gfx_t gfx = {
		.drv = &t_gfx_buffer_driver,
	};

	EXPECT_NULL(gfx_buffer_init(NULL, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));

	END;
}

TEST(gfx_buffer_init_null_gfx)
{
	START;

	gfx_buffer_t buffer = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, NULL, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));

	END;
}

TEST(gfx_buffer_init_null_driver)
{
	START;

	gfx_t gfx	    = {0};
	gfx_buffer_t buffer = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));

	END;
}

TEST(gfx_buffer_init_null_driver_callback)
{
	START;

	gfx_driver_t drv = t_gfx_buffer_driver;
	drv.buffer_init	 = NULL;
	gfx_t gfx	 = {
		       .drv = &drv,
	       };
	gfx_buffer_t buffer = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));

	END;
}

TEST(gfx_buffer_init_success)
{
	START;

	t_gfx_buffer_reset();
	gfx_t gfx = {
		.drv = &t_gfx_buffer_driver,
	};
	gfx_buffer_config_t config = {.type = GFX_BUFFER_VERTEX};
	gfx_buffer_t buffer	   = {0};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &config), &buffer);
	EXPECT_PTR(buffer.gfx, &gfx);
	EXPECT_PTR(buffer.data, (void *)0x5678);
	EXPECT_EQ(t_gfx_buffer_init_calls, 1);
	EXPECT_PTR(t_gfx_buffer_config, &config);

	END;
}

TEST(gfx_buffer_init_returns_driver_failure)
{
	START;

	t_gfx_buffer_reset();
	t_gfx_buffer_init_ret = 1;
	gfx_t gfx	      = {
			    .drv = &t_gfx_buffer_driver,
	    };
	gfx_buffer_t buffer = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));

	END;
}

TEST(gfx_buffer_free_null_buffer)
{
	START;

	gfx_buffer_free(NULL);

	END;
}

TEST(gfx_buffer_free_null_gfx)
{
	START;

	gfx_buffer_t buffer = {0};

	gfx_buffer_free(&buffer);

	END;
}

TEST(gfx_buffer_free_calls_driver)
{
	START;

	t_gfx_buffer_reset();
	gfx_t gfx = {
		.drv = &t_gfx_buffer_driver,
	};
	gfx_buffer_t buffer = {.gfx = &gfx, .data = (void *)0x5678};

	gfx_buffer_free(&buffer);

	EXPECT_EQ(t_gfx_buffer_free_calls, 1);
	EXPECT_PTR(t_gfx_buffer, &buffer);
	EXPECT_NULL(buffer.data);

	END;
}

TEST(gfx_buffer_set_data_null_buffer)
{
	START;

	int data = 1;

	EXPECT_EQ(gfx_buffer_set_data(NULL, &data, sizeof(data)), 1);

	END;
}

TEST(gfx_buffer_set_data_null_data)
{
	START;

	gfx_t gfx = {
		.drv = &t_gfx_buffer_driver,
	};
	gfx_buffer_t buffer = {.gfx = &gfx, .data = (void *)0x5678};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, NULL, sizeof(int)), 1);

	END;
}

TEST(gfx_buffer_set_data_zero_size)
{
	START;

	gfx_t gfx = {
		.drv = &t_gfx_buffer_driver,
	};
	int data	    = 1;
	gfx_buffer_t buffer = {.gfx = &gfx, .data = (void *)0x5678};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, &data, 0), 1);

	END;
}

TEST(gfx_buffer_set_data_calls_driver)
{
	START;

	t_gfx_buffer_reset();
	gfx_t gfx = {
		.drv = &t_gfx_buffer_driver,
	};
	int data	    = 1;
	gfx_buffer_t buffer = {.gfx = &gfx, .data = (void *)0x5678};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, &data, sizeof(data)), 0);

	EXPECT_EQ(t_gfx_buffer_set_data_calls, 1);
	EXPECT_PTR(t_gfx_buffer, &buffer);
	EXPECT_PTR(t_gfx_buffer_data, &data);
	EXPECT_EQ(t_gfx_buffer_size, sizeof(data));

	END;
}

TEST(gfx_buffer_set_data_returns_driver_result)
{
	START;

	t_gfx_buffer_reset();
	t_gfx_buffer_set_data_ret = 1;
	gfx_t gfx		  = {
				.drv = &t_gfx_buffer_driver,
	};
	int data	    = 1;
	gfx_buffer_t buffer = {.gfx = &gfx, .data = (void *)0x5678};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, &data, sizeof(data)), 1);

	END;
}

TEST(gfx_buffer_bind_returns_driver_result)
{
	START;

	t_gfx_buffer_reset();
	t_gfx_buffer_bind_ret	= 1;
	gfx_t gfx		= {.drv = &t_gfx_buffer_driver};
	gfx_pipeline_t pipeline = {.gfx = &gfx};
	gfx_frame_t frame	= {.gfx = &gfx, .pipeline = &pipeline, .active = 1};
	gfx_buffer_t buffer	= {.gfx = &gfx, .data = (void *)0x5678};
	gfx.frame		= &frame;

	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 1);
	EXPECT_EQ(t_gfx_buffer_bind_calls, 1);
	EXPECT_NULL(frame.vertex_buffer);

	END;
}

TEST(gfx_buffer_bind_rejects_invalid_args)
{
	START;

	gfx_t gfx		    = {.drv = &t_gfx_buffer_driver};
	gfx_pipeline_t pipeline	    = {.gfx = &gfx};
	gfx_frame_t frame	    = {.gfx = &gfx, .pipeline = &pipeline, .active = 1};
	gfx_buffer_t buffer	    = {.gfx = &gfx, .data = (void *)0x5678};
	gfx_buffer_t foreign_buffer = {.gfx = &(gfx_t){.drv = &t_gfx_buffer_driver}, .data = (void *)0x5678};
	gfx.frame		    = &frame;

	EXPECT_EQ(gfx_buffer_bind(NULL, &buffer), 1);
	EXPECT_EQ(gfx_buffer_bind(&(gfx_frame_t){0}, &buffer), 1);
	EXPECT_EQ(gfx_buffer_bind(&(gfx_frame_t){.gfx = &gfx}, &buffer), 1);
	gfx.frame = NULL;
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 1);
	gfx.frame    = &frame;
	frame.active = 0;
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 1);
	frame.active   = 1;
	frame.pipeline = NULL;
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 1);
	frame.pipeline = &pipeline;
	EXPECT_EQ(gfx_buffer_bind(&frame, NULL), 1);
	EXPECT_EQ(gfx_buffer_bind(&frame, &foreign_buffer), 1);
	EXPECT_EQ(gfx_buffer_bind(&frame, &(gfx_buffer_t){.gfx = &(gfx_t){0}}), 1);
	EXPECT_EQ(gfx_buffer_bind(&frame, &(gfx_buffer_t){.gfx = &(gfx_t){.drv = &(gfx_driver_t){0}}}), 1);

	END;
}

STEST(gfx_buffer)
{
	SSTART;

	RUN(gfx_buffer_init_null_buffer);
	RUN(gfx_buffer_init_null_gfx);
	RUN(gfx_buffer_init_null_driver);
	RUN(gfx_buffer_init_null_driver_callback);
	RUN(gfx_buffer_init_success);
	RUN(gfx_buffer_init_returns_driver_failure);
	RUN(gfx_buffer_free_null_buffer);
	RUN(gfx_buffer_free_null_gfx);
	RUN(gfx_buffer_free_calls_driver);
	RUN(gfx_buffer_set_data_null_buffer);
	RUN(gfx_buffer_set_data_null_data);
	RUN(gfx_buffer_set_data_zero_size);
	RUN(gfx_buffer_set_data_calls_driver);
	RUN(gfx_buffer_set_data_returns_driver_result);
	RUN(gfx_buffer_bind_rejects_invalid_args);
	RUN(gfx_buffer_bind_returns_driver_result);

	SEND;
}
