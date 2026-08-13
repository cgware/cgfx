#include "gfx_driver.h"

#include "cmath.h"
#include "log.h"
#include "mem.h"
#include "test.h"

static gfx_driver_t *t_gfx_software_driver(void)
{
	return gfx_driver_find(STRV("software"));
}

static int t_gfx_software_present_calls;
static int t_gfx_software_present_ret;
static int t_gfx_software_memory_calls;
static int t_gfx_software_memory_ret;
static gfx_image_t t_gfx_software_image;
static int t_gfx_software_alloc_count;
static gfx_surface_memory_t t_gfx_software_memory;

typedef struct t_gfx_software_surface_target_data_s {
	gfx_surface_memory_t memory;
} t_gfx_software_surface_target_data_t;

typedef struct t_gfx_software_data_s {
	gfx_image_t image;
	gfx_swapchain_t *swapchain;
	float *depth;
	size_t depth_size;
	u16 viewport_x;
	u16 viewport_y;
	u16 viewport_width;
	u16 viewport_height;
} t_gfx_software_data_t;

typedef struct t_gfx_software_buffer_data_s {
	buf_t buf;
} t_gfx_software_buffer_data_t;

typedef struct t_gfx_software_vertex_3d_s {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	float a;
} t_gfx_software_vertex_3d_t;

typedef struct t_gfx_software_transform_s {
	mat4f_t model;
	mat4f_t view;
	mat4f_t projection;
	vec4f_t tint;
} t_gfx_software_transform_t;

static const char *t_gfx_software_shader_source =
	"vs_in 0 VertexIn {\n"
	"\tvec3f position : POSITION;\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"vs_out VertexOut {\n"
	"\tvec4f position : POSITION;\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"fs_in FragmentIn {\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"fs_out FragmentOut {\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"buffer 0 Transform {\n"
	"\tmat4f model;\n"
	"\tmat4f view;\n"
	"\tmat4f projection;\n"
	"\tvec4f tint;\n"
	"}\n"
	"VertexOut vertex(VertexIn input) {\n"
	"\tVertexOut output;\n"
	"\toutput.position = projection * view * model * vec4f(input.position.x, input.position.y, input.position.z, 1.0f);\n"
	"\toutput.color = tint;\n"
	"\treturn output;\n"
	"}\n"
	"FragmentOut fragment(FragmentIn input) {\n"
	"\tFragmentOut output;\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n";

static const char *t_gfx_software_color_shader_source =
	"vs_in 0 VertexIn {\n"
	"\tvec2f position : POSITION;\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"vs_out VertexOut {\n"
	"\tvec4f position : POSITION;\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"fs_in FragmentIn {\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"fs_out FragmentOut {\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"VertexOut vertex(VertexIn input) {\n"
	"\tVertexOut output;\n"
	"\toutput.position = vec4f(input.position.x, input.position.y, input.color.z, input.color.w);\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n"
	"FragmentOut fragment(FragmentIn input) {\n"
	"\tFragmentOut output;\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n";

static const gfx_layout_t t_gfx_software_layout_3d[] = {
	{.index = 0, .semantic = "POSITION", .count = 3, .type = GFX_VALUE_FLOAT32},
	{.index = 1, .semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
};

static const gfx_layout_t t_gfx_software_layout_2d[] = {
	{.index = 0, .semantic = "POSITION", .count = 2, .type = GFX_VALUE_FLOAT32},
	{.index = 1, .semantic = "COLOR0", .count = 4, .type = GFX_VALUE_FLOAT32},
};

static int t_gfx_software_surface_present(gfx_surface_t *surface, gfx_present_mode_t present_mode)
{
	(void)surface;
	(void)present_mode;
	t_gfx_software_present_calls++;
	return t_gfx_software_present_ret;
}

static int t_gfx_software_surface_memory(gfx_surface_t *surface, gfx_surface_memory_t *memory)
{
	(void)surface;
	t_gfx_software_memory_calls++;
	if (t_gfx_software_memory_ret) {
		return t_gfx_software_memory_ret;
	}
	*memory = t_gfx_software_memory;
	return 0;
}

static const gfx_surface_ops_t t_gfx_software_surface_ops = {
	.present = t_gfx_software_surface_present,
	.memory	 = t_gfx_software_surface_memory,
};

static void t_gfx_software_reset_surface(u8 *pixels, u16 width, u16 height, size_t stride)
{
	t_gfx_software_present_calls = 0;
	t_gfx_software_present_ret   = 0;
	t_gfx_software_memory_calls  = 0;
	t_gfx_software_memory_ret    = 0;
	t_gfx_software_image	     = (gfx_image_t){0};

	t_gfx_software_memory = (gfx_surface_memory_t){
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= width,
		.height = height,
		.stride = stride,
	};
}

static void *t_gfx_software_alloc_fail(alloc_t *alloc, size_t size)
{
	(void)alloc;
	(void)size;
	return NULL;
}

static void *t_gfx_software_alloc_fail_after_first(alloc_t *alloc, size_t size)
{
	t_gfx_software_alloc_count++;
	if (t_gfx_software_alloc_count > 1) {
		return NULL;
	}
	return alloc_alloc_std(alloc, size);
}

static int t_gfx_software_realloc_fail(alloc_t *alloc, void **ptr, size_t *capacity, size_t size)
{
	(void)alloc;
	(void)ptr;
	(void)capacity;
	(void)size;
	return 1;
}

static int t_gfx_software_scene(gfx_t *gfx, gfx_image_t *target, gfx_render_pass_t *render_pass, gfx_framebuffer_t *framebuffer, u8 *pixels,
				u16 width, u16 height, size_t stride)
{
	gfx_driver_t *drv			       = t_gfx_software_driver();
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= width,
		.height = height,
		.stride = stride,
	};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	if (gfx_init(gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD) != gfx ||
	    gfx_image_init_memory(target, gfx, &memory_target_config) != target) {
		return 1;
	}
	if (gfx_render_pass_init(render_pass, gfx, &render_pass_config) != render_pass) {
		return 1;
	}
	return gfx_framebuffer_init(framebuffer, target, render_pass) != framebuffer;
}

static int t_gfx_software_surface_scene(gfx_t *gfx, gfx_swapchain_t *swapchain, gfx_image_t *target, gfx_render_pass_t *render_pass,
					gfx_framebuffer_t *framebuffer, gfx_surface_t *surface)
{
	gfx_driver_t *drv = t_gfx_software_driver();
	static gfx_image_t images[8];
	mem_set(images, 0, sizeof(images));
	gfx_swapchain_config_t swapchain_config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = surface,
		.width		 = 2,
		.height		 = 2,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	if (gfx_init(gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD) != gfx ||
	    gfx_swapchain_init(swapchain, gfx, &swapchain_config) != swapchain) {
		return 1;
	}
	*target = swapchain->images[0];
	if (gfx_render_pass_init(render_pass, gfx, &render_pass_config) != render_pass) {
		return 1;
	}
	return gfx_framebuffer_init(framebuffer, target, render_pass) != framebuffer;
}

static void t_gfx_software_scene_free(gfx_t *gfx, gfx_image_t *target, gfx_render_pass_t *render_pass, gfx_framebuffer_t *framebuffer)
{
	gfx_framebuffer_free(framebuffer);
	gfx_render_pass_free(render_pass);
	gfx_image_free(target);
	gfx_free(gfx);
}

static int t_gfx_software_draw_source(const char *source, int indexed, int use_uniform)
{
	u8 pixels[256]		      = {0};
	gfx_t gfx		      = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	int ret			      = 1;
	if (t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 8, 8, 32)) {
		return 1;
	}

	gfx_shader_compiler_t compiler	   = {0};
	gfx_shader_t shader		   = {0};
	gfx_pipeline_t pipeline		   = {0};
	gfx_buffer_t vertex_buffer	   = {0};
	gfx_buffer_t index_buffer	   = {0};
	gfx_buffer_t uniform_buffer	   = {0};
	gfx_resource_binding_t bindings[1] = {0};

	if (gfx_shader_compiler_init(&compiler, ALLOC_STD) == NULL ||
	    gfx_shader_init(&shader,
			    &gfx,
			    &(gfx_shader_config_t){
				    .compiler = &compiler,
				    .source   = strv_cstr(source),
				    .stage    = GFX_SHADER_STAGE_VERTEX,
			    }) == NULL ||
	    gfx_pipeline_init(&pipeline,
			      &gfx,
			      &(gfx_pipeline_config_t){
				      .render_pass	 = &render_pass,
				      .vs		 = shader,
				      .fs		 = shader,
				      .input_layout	 = t_gfx_software_layout_2d,
				      .input_layout_size = sizeof(t_gfx_software_layout_2d),
			      }) == NULL) {
		goto cleanup;
	}

	gfx_vertex_2d_t vertices[] = {
		{.x = -0.8f, .y = -0.8f, .r = 1.0f, .a = 1.0f},
		{.x = 0.8f, .y = -0.8f, .g = 1.0f, .a = 1.0f},
		{.x = -0.8f, .y = 0.8f, .b = 1.0f, .a = 1.0f},
	};
	if (gfx_buffer_init(&vertex_buffer,
			    &gfx,
			    &(gfx_buffer_config_t){
				    .type  = GFX_BUFFER_VERTEX,
				    .usage = GFX_BUFFER_USAGE_STATIC,
				    .size  = sizeof(vertices),
				    .data  = vertices,
			    }) == NULL) {
		goto cleanup;
	}
	u32 indices[] = {0, 1, 2};
	if (indexed && gfx_buffer_init(&index_buffer,
				       &gfx,
				       &(gfx_buffer_config_t){
					       .type  = GFX_BUFFER_INDEX,
					       .usage = GFX_BUFFER_USAGE_STATIC,
					       .size  = sizeof(indices),
					       .data  = indices,
				       }) == NULL) {
		goto cleanup;
	}
	if (use_uniform) {
		t_gfx_software_transform_t transform = {
			.model	    = mat4f_identity(),
			.view	    = mat4f_identity(),
			.projection = mat4f_identity(),
			.tint	    = vec4f(1.0f, 0.5f, 0.25f, 1.0f),
		};
		if (gfx_buffer_init(&uniform_buffer,
				    &gfx,
				    &(gfx_buffer_config_t){
					    .type  = GFX_BUFFER_UNIFORM,
					    .usage = GFX_BUFFER_USAGE_STATIC,
					    .size  = sizeof(transform),
					    .data  = &transform,
				    }) == NULL) {
			goto cleanup;
		}
		bindings[0] = (gfx_resource_binding_t){.binding = 0, .type = GFX_RESOURCE_UNIFORM_BUFFER, .buffer = &uniform_buffer};
	}

	gfx_frame_t frame = {0};
	if (gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}) || gfx_pipeline_bind(&frame, &pipeline) ||
	    gfx_buffer_bind(&frame, &vertex_buffer) || (use_uniform && gfx_bind_resources(&frame, bindings, 1)) ||
	    (indexed && gfx_buffer_bind(&frame, &index_buffer))) {
		goto cleanup;
	}
	ret = indexed ? gfx_draw_indexed(&frame, 3) : gfx_draw(&frame, 3, 0);
	gfx_end(&frame);

cleanup:
	gfx_buffer_free(&uniform_buffer);
	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	gfx_shader_compiler_free(&compiler);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	return ret;
}

static u32 t_gfx_software_count_lit_pixels(const u8 *pixels, size_t size)
{
	u32 count = 0;
	for (size_t i = 0; i + 3 < size; i += 4) {
		if (pixels[i] != 0 || pixels[i + 1] != 0 || pixels[i + 2] != 0) {
			count++;
		}
	}
	return count;
}

static int t_gfx_software_draw_clip_vertices(const t_gfx_software_vertex_3d_t vertices[3], u32 *lit_pixels)
{
	static const char *source = "vs_in 0 VertexIn {\n"
				    "\tvec3f position : POSITION;\n"
				    "\tvec4f color : COLOR0;\n"
				    "}\n"
				    "vs_out VertexOut {\n"
				    "\tvec4f position : POSITION;\n"
				    "\tvec4f color : COLOR0;\n"
				    "}\n"
				    "fs_in FragmentIn {\n"
				    "\tvec4f color : COLOR0;\n"
				    "}\n"
				    "fs_out FragmentOut {\n"
				    "\tvec4f color : COLOR0;\n"
				    "}\n"
				    "VertexOut vertex(VertexIn input) {\n"
				    "\tVertexOut output;\n"
				    "\toutput.position = vec4f(input.position.x, input.position.y, input.position.z, 1.0f);\n"
				    "\toutput.color = input.color;\n"
				    "\treturn output;\n"
				    "}\n"
				    "FragmentOut fragment(FragmentIn input) {\n"
				    "\tFragmentOut output;\n"
				    "\toutput.color = input.color;\n"
				    "\treturn output;\n"
				    "}\n";

	u8 pixels[256]		      = {0};
	gfx_t gfx		      = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	int ret			      = 1;
	if (lit_pixels == NULL || t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 8, 8, 32)) {
		return 1;
	}

	gfx_shader_compiler_t compiler = {0};
	gfx_shader_t shader	       = {0};
	gfx_pipeline_t pipeline	       = {0};
	gfx_buffer_t vertex_buffer     = {0};
	if (gfx_shader_compiler_init(&compiler, ALLOC_STD) == NULL ||
	    gfx_shader_init(&shader,
			    &gfx,
			    &(gfx_shader_config_t){
				    .compiler = &compiler,
				    .source   = strv_cstr(source),
				    .stage    = GFX_SHADER_STAGE_VERTEX,
			    }) == NULL ||
	    gfx_pipeline_init(&pipeline,
			      &gfx,
			      &(gfx_pipeline_config_t){
				      .render_pass	 = &render_pass,
				      .vs		 = shader,
				      .fs		 = shader,
				      .input_layout	 = t_gfx_software_layout_3d,
				      .input_layout_size = sizeof(t_gfx_software_layout_3d),
			      }) == NULL ||
	    gfx_buffer_init(&vertex_buffer,
			    &gfx,
			    &(gfx_buffer_config_t){
				    .type  = GFX_BUFFER_VERTEX,
				    .usage = GFX_BUFFER_USAGE_STATIC,
				    .size  = sizeof(t_gfx_software_vertex_3d_t) * 3,
				    .data  = vertices,
			    }) == NULL) {
		goto cleanup;
	}

	gfx_frame_t frame = {0};
	if (gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}) || gfx_pipeline_bind(&frame, &pipeline) ||
	    gfx_buffer_bind(&frame, &vertex_buffer) || gfx_draw(&frame, 3, 0) || gfx_end(&frame)) {
		goto cleanup;
	}
	*lit_pixels = t_gfx_software_count_lit_pixels(pixels, sizeof(pixels));
	ret	    = 0;

cleanup:
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	gfx_shader_compiler_free(&compiler);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	return ret;
}

TEST(gfx_software_driver_is_registered)
{
	START;

	EXPECT_NOT_NULL(t_gfx_software_driver());

	END;
}

TEST(gfx_software_init_failures)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {.alloc = ALLOC_STD};

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);
	EXPECT_EQ(drv->init(&gfx, NULL), 1);
	EXPECT_EQ(drv->init(&(gfx_t){0}, &(gfx_config_t){0}), 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, (alloc_t){.alloc = t_gfx_software_alloc_fail}));
	EXPECT_EQ(drv->free(NULL), 1);
	EXPECT_EQ(drv->free(&(gfx_t){0}), 1);

	END;
}

TEST(gfx_software_memory_pass_clears_target)
{
	START;

	u8 pixels[16]	  = {0};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 2,
		.stride = 8,
	};
	EXPECT_PTR(gfx_image_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_frame_t frame	      = {0};
	gfx_pass_config_t pass_config = {
		.clear = {1.0f, 0.0f, 0.0f, 1.0f},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	u8 readback[16] = {0};
	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = readback, .stride = 8}), 0);
	EXPECT_EQ(readback[0], 255);
	EXPECT_EQ(readback[1], 0);
	EXPECT_EQ(readback[2], 0);
	EXPECT_EQ(readback[3], 255);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_memory_pass_load_does_not_clear)
{
	START;

	u8 pixels[4]		      = {1, 2, 3, 4};
	gfx_t gfx		      = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 1, 1, 4), 0);
	gfx_render_pass_free(&render_pass);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_free(&framebuffer);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {1.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(pixels[0], 1);
	EXPECT_EQ(pixels[1], 2);
	EXPECT_EQ(pixels[2], 3);
	EXPECT_EQ(pixels[3], 4);

	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_surface_target_lifecycle)
{
	START;

	u8 pixels[64] = {0};
	t_gfx_software_reset_surface(pixels, 2, 2, 8);
	gfx_surface_t surface = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_t gfx	      = {0};
	gfx_driver_t *drv     = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_swapchain_t swapchain		= {0};
	gfx_image_t target			= {0};
	gfx_image_t images[2]			= {0};
	gfx_swapchain_config_t swapchain_config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &surface,
		.width		 = 2,
		.height		 = 2,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};
	EXPECT_PTR(gfx_swapchain_init(&swapchain, &gfx, &swapchain_config), &swapchain);
	target = swapchain.images[0];
	EXPECT_EQ(drv->image_init(&target), 0);
	EXPECT_EQ(t_gfx_software_memory_calls, 1);
	EXPECT_NOT_NULL(target.driver_data);
	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 3, 3), 0);
	EXPECT_EQ(t_gfx_software_memory_calls, 1);
	gfx_swapchain_image_t image = {0};
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 0);
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 0);
	EXPECT_EQ(t_gfx_software_present_calls, 1);

	gfx_image_free(&target);
	EXPECT_NULL(target.driver_data);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_surface_target_failures)
{
	START;

	u8 pixels[4] = {0};
	t_gfx_software_reset_surface(pixels, 1, 1, 4);
	gfx_surface_t surface = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_t gfx	      = {0};
	gfx_driver_t *drv     = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_swapchain_config_t swapchain_config_null = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &(gfx_surface_t){.api = GFX_API_SOFTWARE},
		.width	 = 1,
		.height	 = 1,
	};
	EXPECT_NULL(gfx_swapchain_init(&(gfx_swapchain_t){0}, &gfx, &swapchain_config_null));
	EXPECT_EQ(drv->swapchain_init(&(gfx_swapchain_t){.gfx	  = &gfx,
							 .format  = GFX_FORMAT_RGBA8,
							 .surface = &(gfx_surface_t){.api = GFX_API_SOFTWARE},
							 .width	  = 1,
							 .height  = 1},
				      &swapchain_config_null),
		  1);
	t_gfx_software_memory_ret		= 1;
	gfx_swapchain_t invalid_swapchain	= {0};
	gfx_image_t images[2]			= {0};
	gfx_swapchain_config_t swapchain_config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &surface,
		.width		 = 1,
		.height		 = 1,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};
	EXPECT_PTR(gfx_swapchain_init(&invalid_swapchain, &gfx, &swapchain_config), &invalid_swapchain);
	EXPECT_EQ(drv->image_init(&invalid_swapchain.images[0]), 1);
	gfx_swapchain_free(&invalid_swapchain);
	t_gfx_software_memory_ret    = 0;
	t_gfx_software_memory.stride = 1;
	EXPECT_PTR(gfx_swapchain_init(&invalid_swapchain, &gfx, &swapchain_config), &invalid_swapchain);
	EXPECT_EQ(drv->image_init(&invalid_swapchain.images[0]), 1);
	gfx_swapchain_free(&invalid_swapchain);
	EXPECT_EQ(drv->swapchain_resize(
			  &(gfx_swapchain_t){.gfx = &gfx, .format = GFX_FORMAT_RGBA8, .surface = &surface, .width = 1, .height = 1}, 2, 2),
		  0);
	t_gfx_software_reset_surface(pixels, 1, 1, 4);
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};
	EXPECT_PTR(gfx_swapchain_init(&swapchain, &gfx, &swapchain_config), &swapchain);
	target = swapchain.images[0];
	EXPECT_EQ(drv->image_init(&target), 0);
	t_gfx_software_memory_ret = 1;
	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 2, 2), 0);
	EXPECT_EQ(drv->swapchain_present(&(gfx_swapchain_t){0}), 1);

	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_swapchain_resize_rejects_invalid_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->swapchain_resize(NULL, 1, 1), 1);
	EXPECT_EQ(drv->swapchain_resize(&(gfx_swapchain_t){0}, 1, 1), 1);
	EXPECT_EQ(drv->swapchain_resize(&(gfx_swapchain_t){.surface = &(gfx_surface_t){0}}, 1, 1), 1);
	EXPECT_EQ(drv->swapchain_resize(
			  &(gfx_swapchain_t){.surface = &(gfx_surface_t){.ops = &(gfx_surface_ops_t){0}}, .width = 1, .height = 1}, 1, 1),
		  1);

	END;
}

TEST(gfx_software_swapchain_free_rejects_missing_image_storage_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {.drv = drv};

	drv->swapchain_free(&(gfx_swapchain_t){.gfx = &gfx});

	END;
}

TEST(gfx_software_draw_triangle)
{
	START;

	u8 pixels[64]		      = {0};
	gfx_t gfx		      = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 4, 4, 16), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx_vertex_2d_t vertices[] = {
		{.x = -1.0f, .y = -1.0f, .r = -1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = 1.0f, .y = -1.0f, .r = 0.0f, .g = 2.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = 1.0f, .r = 0.0f, .g = 0.0f, .b = 0.5f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = 1.0f, .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f},
		{.x = 1.0f, .y = -1.0f, .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 3), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 6), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	u8 readback[64] = {0};
	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = readback, .stride = 16}), 0);
	EXPECT_NE(readback[3], 0);

	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_draw_indexed_triangle)
{
	START;

	u8 pixels[64]		      = {0};
	gfx_t gfx		      = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 4, 4, 16), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t vertex_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&vertex_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&vertex_buffer);
	gfx_vertex_2d_t vertices[] = {
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = 1.0f, .y = -1.0f, .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = 1.0f, .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(vertices)), 0);
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&index_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&index_buffer);
	u32 indices[3] = {0, 1, 2};
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(indices)), 0);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &vertex_buffer), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &index_buffer), 0);
	EXPECT_EQ(gfx_draw_indexed(&frame, 3), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	u8 readback[64] = {0};
	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = readback, .stride = 16}), 0);
	EXPECT_NE(readback[3], 0);

	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_draw_shader_pipeline_vec3_uniform)
{
	START;

	u8 pixels[256]		      = {0};
	gfx_t gfx		      = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 8, 8, 32), 0);

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);
	gfx_shader_t vertex = {0};
	EXPECT_PTR(gfx_shader_init(&vertex,
				   &gfx,
				   &(gfx_shader_config_t){
					   .compiler = &compiler,
					   .source   = strv_cstr(t_gfx_software_shader_source),
					   .stage    = GFX_SHADER_STAGE_VERTEX,
				   }),
		   &vertex);
	gfx_shader_t fragment = {0};
	EXPECT_PTR(gfx_shader_init(&fragment,
				   &gfx,
				   &(gfx_shader_config_t){
					   .compiler = &compiler,
					   .source   = strv_cstr(t_gfx_software_shader_source),
					   .stage    = GFX_SHADER_STAGE_FRAGMENT,
				   }),
		   &fragment);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline,
				     &gfx,
				     &(gfx_pipeline_config_t){
					     .render_pass	= &render_pass,
					     .vs		= vertex,
					     .fs		= fragment,
					     .input_layout	= t_gfx_software_layout_3d,
					     .input_layout_size = sizeof(t_gfx_software_layout_3d),
				     }),
		   &pipeline);

	t_gfx_software_vertex_3d_t vertices[] = {
		{.x = -0.8f, .y = -0.8f, .z = 0.0f, .r = 1.0f, .a = 1.0f},
		{.x = 0.8f, .y = -0.8f, .z = 0.0f, .g = 1.0f, .a = 1.0f},
		{.x = -0.8f, .y = 0.8f, .z = 0.0f, .b = 1.0f, .a = 1.0f},
	};
	gfx_buffer_t vertex_buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&vertex_buffer,
				   &gfx,
				   &(gfx_buffer_config_t){
					   .type  = GFX_BUFFER_VERTEX,
					   .usage = GFX_BUFFER_USAGE_STATIC,
					   .size  = sizeof(vertices),
					   .data  = vertices,
				   }),
		   &vertex_buffer);
	u32 indices[]		  = {0, 1, 2};
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&index_buffer,
				   &gfx,
				   &(gfx_buffer_config_t){
					   .type  = GFX_BUFFER_INDEX,
					   .usage = GFX_BUFFER_USAGE_STATIC,
					   .size  = sizeof(indices),
					   .data  = indices,
				   }),
		   &index_buffer);
	t_gfx_software_transform_t transform = {
		.model	    = mat4f_identity(),
		.view	    = mat4f_identity(),
		.projection = mat4f_identity(),
		.tint	    = vec4f(1.0f, 0.5f, 0.25f, 1.0f),
	};
	gfx_buffer_t uniform_buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&uniform_buffer,
				   &gfx,
				   &(gfx_buffer_config_t){
					   .type  = GFX_BUFFER_UNIFORM,
					   .usage = GFX_BUFFER_USAGE_STATIC,
					   .size  = sizeof(transform),
					   .data  = &transform,
				   }),
		   &uniform_buffer);
	gfx_resource_binding_t bindings[] = {
		{.binding = 0, .type = GFX_RESOURCE_UNIFORM_BUFFER, .buffer = &uniform_buffer},
	};

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &vertex_buffer), 0);
	EXPECT_EQ(gfx_bind_resources(&frame, bindings, 1), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &index_buffer), 0);
	EXPECT_EQ(gfx_draw_indexed(&frame, 3), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	u8 readback[256] = {0};
	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = readback, .stride = 32}), 0);
	EXPECT_NE(readback[3], 0);

	gfx_buffer_free(&uniform_buffer);
	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&fragment);
	gfx_shader_free(&vertex);
	gfx_shader_compiler_free(&compiler);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_shader_pipeline_uniform_failures)
{
	START;

	u8 pixels[256]		      = {0};
	gfx_t gfx		      = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 8, 8, 32), 0);

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader,
				   &gfx,
				   &(gfx_shader_config_t){
					   .compiler = &compiler,
					   .source   = strv_cstr(t_gfx_software_shader_source),
					   .stage    = GFX_SHADER_STAGE_VERTEX,
				   }),
		   &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline,
				     &gfx,
				     &(gfx_pipeline_config_t){
					     .render_pass	= &render_pass,
					     .vs		= shader,
					     .fs		= shader,
					     .input_layout	= t_gfx_software_layout_3d,
					     .input_layout_size = sizeof(t_gfx_software_layout_3d),
				     }),
		   &pipeline);
	t_gfx_software_vertex_3d_t vertices[] = {
		{.x = -0.8f, .y = -0.8f, .z = 0.0f, .a = 1.0f},
		{.x = 0.8f, .y = -0.8f, .z = 0.0f, .a = 1.0f},
		{.x = -0.8f, .y = 0.8f, .z = 0.0f, .a = 1.0f},
	};
	gfx_buffer_t vertex_buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&vertex_buffer,
				   &gfx,
				   &(gfx_buffer_config_t){
					   .type  = GFX_BUFFER_VERTEX,
					   .usage = GFX_BUFFER_USAGE_STATIC,
					   .size  = sizeof(vertices),
					   .data  = vertices,
				   }),
		   &vertex_buffer);
	mat4f_t short_transform[2] = {mat4f_identity(), mat4f_identity()};
	gfx_buffer_t short_uniform = {0};
	EXPECT_PTR(gfx_buffer_init(&short_uniform,
				   &gfx,
				   &(gfx_buffer_config_t){
					   .type  = GFX_BUFFER_UNIFORM,
					   .usage = GFX_BUFFER_USAGE_STATIC,
					   .size  = sizeof(short_transform),
					   .data  = short_transform,
				   }),
		   &short_uniform);
	gfx_resource_binding_t bindings[] = {
		{.binding = 0, .type = GFX_RESOURCE_UNIFORM_BUFFER, .buffer = &short_uniform},
	};

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &vertex_buffer), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 1);
	EXPECT_EQ(gfx_bind_resources(&frame, bindings, 1), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 1);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_buffer_free(&short_uniform);
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	gfx_shader_compiler_free(&compiler);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_shader_pipeline_color_components)
{
	START;

	const char *vec2_uniform_padding = "vs_in 0 VertexIn {\n"
					   "\tvec2f position : POSITION;\n"
					   "\tvec4f color : COLOR0;\n"
					   "}\n"
					   "vs_out VertexOut {\n"
					   "\tvec4f position : POSITION;\n"
					   "\tvec4f color : COLOR0;\n"
					   "}\n"
					   "fs_in FragmentIn {\n"
					   "\tvec4f color : COLOR0;\n"
					   "}\n"
					   "fs_out FragmentOut {\n"
					   "\tvec4f color : COLOR0;\n"
					   "}\n"
					   "buffer 0 Transform {\n"
					   "\tvec2f offset;\n"
					   "\tvec4f tint;\n"
					   "}\n"
					   "VertexOut vertex(VertexIn input) {\n"
					   "\tVertexOut output;\n"
					   "\toutput.position = vec4f(input.position.x, input.position.y, 0.0f, 1.0f);\n"
					   "\toutput.color = tint;\n"
					   "\treturn output;\n"
					   "}\n"
					   "FragmentOut fragment(FragmentIn input) {\n"
					   "\tFragmentOut output;\n"
					   "\toutput.color = input.color;\n"
					   "\treturn output;\n"
					   "}\n";

	EXPECT_EQ(t_gfx_software_draw_source(t_gfx_software_color_shader_source, 0, 0), 0);
	EXPECT_EQ(t_gfx_software_draw_source(vec2_uniform_padding, 0, 1), 0);

	END;
}

TEST(gfx_software_shader_pipeline_expression_failures)
{
	START;

	const char *missing_uniform = "vs_in 0 VertexIn {\n"
				      "\tvec2f position : POSITION;\n"
				      "\tvec4f color : COLOR0;\n"
				      "}\n"
				      "vs_out VertexOut {\n"
				      "\tvec4f position : POSITION;\n"
				      "\tvec4f color : COLOR0;\n"
				      "}\n"
				      "fs_in FragmentIn {\n"
				      "\tvec4f color : COLOR0;\n"
				      "}\n"
				      "fs_out FragmentOut {\n"
				      "\tvec4f color : COLOR0;\n"
				      "}\n"
				      "VertexOut vertex(VertexIn input) {\n"
				      "\tVertexOut output;\n"
				      "\toutput.position = missing.x;\n"
				      "\toutput.color = input.color;\n"
				      "\treturn output;\n"
				      "}\n"
				      "FragmentOut fragment(FragmentIn input) {\n"
				      "\tFragmentOut output;\n"
				      "\toutput.color = input.color;\n"
				      "\treturn output;\n"
				      "}\n";

	const char *bad_call = "vs_in 0 VertexIn {\n"
			       "\tvec2f position : POSITION;\n"
			       "\tvec4f color : COLOR0;\n"
			       "}\n"
			       "vs_out VertexOut {\n"
			       "\tvec4f position : POSITION;\n"
			       "\tvec4f color : COLOR0;\n"
			       "}\n"
			       "fs_in FragmentIn {\n"
			       "\tvec4f color : COLOR0;\n"
			       "}\n"
			       "fs_out FragmentOut {\n"
			       "\tvec4f color : COLOR0;\n"
			       "}\n"
			       "VertexOut vertex(VertexIn input) {\n"
			       "\tVertexOut output;\n"
			       "\toutput.position = vec2f(1.0f, 2.0f);\n"
			       "\toutput.color = input.color;\n"
			       "\treturn output;\n"
			       "}\n"
			       "FragmentOut fragment(FragmentIn input) {\n"
			       "\tFragmentOut output;\n"
			       "\toutput.color = input.color;\n"
			       "\treturn output;\n"
			       "}\n";

	const char *bad_arg = "vs_in 0 VertexIn {\n"
			      "\tvec2f position : POSITION;\n"
			      "\tvec4f color : COLOR0;\n"
			      "}\n"
			      "vs_out VertexOut {\n"
			      "\tvec4f position : POSITION;\n"
			      "\tvec4f color : COLOR0;\n"
			      "}\n"
			      "fs_in FragmentIn {\n"
			      "\tvec4f color : COLOR0;\n"
			      "}\n"
			      "fs_out FragmentOut {\n"
			      "\tvec4f color : COLOR0;\n"
			      "}\n"
			      "VertexOut vertex(VertexIn input) {\n"
			      "\tVertexOut output;\n"
			      "\toutput.position = vec4f(input.color, 0.0f, 0.0f, 1.0f);\n"
			      "\toutput.color = input.color;\n"
			      "\treturn output;\n"
			      "}\n"
			      "FragmentOut fragment(FragmentIn input) {\n"
			      "\tFragmentOut output;\n"
			      "\toutput.color = input.color;\n"
			      "\treturn output;\n"
			      "}\n";

	const char *bad_binary = "vs_in 0 VertexIn {\n"
				 "\tvec2f position : POSITION;\n"
				 "\tvec4f color : COLOR0;\n"
				 "}\n"
				 "vs_out VertexOut {\n"
				 "\tvec4f position : POSITION;\n"
				 "\tvec4f color : COLOR0;\n"
				 "}\n"
				 "fs_in FragmentIn {\n"
				 "\tvec4f color : COLOR0;\n"
				 "}\n"
				 "fs_out FragmentOut {\n"
				 "\tvec4f color : COLOR0;\n"
				 "}\n"
				 "VertexOut vertex(VertexIn input) {\n"
				 "\tVertexOut output;\n"
				 "\toutput.position = input.color * input.color;\n"
				 "\toutput.color = input.color;\n"
				 "\treturn output;\n"
				 "}\n"
				 "FragmentOut fragment(FragmentIn input) {\n"
				 "\tFragmentOut output;\n"
				 "\toutput.color = input.color;\n"
				 "\treturn output;\n"
				 "}\n";

	EXPECT_EQ(t_gfx_software_draw_source(missing_uniform, 0, 0), 1);
	EXPECT_EQ(t_gfx_software_draw_source(bad_call, 0, 0), 1);
	EXPECT_EQ(t_gfx_software_draw_source(bad_arg, 0, 0), 1);
	EXPECT_EQ(t_gfx_software_draw_source(bad_binary, 0, 0), 1);
	EXPECT_EQ(t_gfx_software_draw_source(t_gfx_software_shader_source, 1, 0), 1);

	END;
}

TEST(gfx_software_draw_buffer_failures)
{
	START;

	u8 pixels[64]		      = {0};
	gfx_t gfx		      = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 4, 4, 16), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t vertex_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&vertex_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&vertex_buffer);
	gfx_vertex_2d_t vertices[] = {
		{.x = -1.0f, .y = -1.0f},
		{.x = 1.0f, .y = -1.0f},
		{.x = -1.0f, .y = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(gfx_vertex_2d_t) * 2), 0);
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&index_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&index_buffer);
	u32 indices[3] = {0, 1, 2};
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(u32) * 2), 0);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.clear = {0.0f, 0.0f, 0.0f, 1.0f}}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &vertex_buffer), 0);
	EXPECT_EQ(gfx.drv->draw(&frame, 3, 0), 1);
	EXPECT_EQ(gfx.drv->draw_indexed(NULL, 3), 1);
	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 2), 1);
	EXPECT_EQ(gfx_buffer_bind(&frame, &index_buffer), 0);
	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 3), 1);
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(vertices)), 0);
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(indices)), 0);
	indices[2] = 3;
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(indices)), 0);
	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 3), 1);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_buffer_set_data_alloc_failure)
{
	START;

	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx.alloc		    = (alloc_t){.alloc = t_gfx_software_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_vertex_2d_t vertices[3] = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);
	log_set_quiet(0, 0);

	gfx.alloc = ALLOC_STD;
	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_buffer_init_static_uploads_data)
{
	START;

	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 1.0f},
		{.x = 2.0f},
		{.x = 3.0f},
	};
	gfx_buffer_t buffer = {0};

	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_VERTEX,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(vertices),
		.data  = vertices,
	};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &buffer_config), &buffer);
	EXPECT_EQ(buffer.size, sizeof(vertices));
	t_gfx_software_buffer_data_t *data = buffer.data;
	EXPECT_EQ(data->buf.used, sizeof(vertices));
	EXPECT_EQ(((gfx_vertex_2d_t *)data->buf.data)[0].x, 1.0f);
	EXPECT_EQ(((gfx_vertex_2d_t *)data->buf.data)[2].x, 3.0f);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_buffer_init_static_alloc_failure)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	t_gfx_software_alloc_count = 0;

	gfx_t gfx = {
		.drv   = drv,
		.alloc = {.alloc = t_gfx_software_alloc_fail_after_first, .realloc = alloc_realloc_std, .free = alloc_free_std},
	};
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_buffer_t buffer	    = {.gfx = &gfx};

	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_VERTEX,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(vertices),
		.data  = vertices,
	};

	log_set_quiet(0, 1);
	EXPECT_EQ(drv->buffer_init(&buffer, &buffer_config), 1);
	log_set_quiet(0, 0);
	EXPECT_NULL(buffer.data);

	END;
}

TEST(gfx_software_buffer_set_data_resize_failure)
{
	START;

	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx_vertex_2d_t triangle[3]  = {0};
	gfx_vertex_2d_t rectangle[4] = {0};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, triangle, sizeof(triangle)), 0);
	t_gfx_software_buffer_data_t *data = buffer.data;
	data->buf.alloc.realloc		   = t_gfx_software_realloc_fail;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, rectangle, sizeof(rectangle)), 1);
	log_set_quiet(0, 0);

	data->buf.alloc = ALLOC_STD;
	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_buffer_set_data_rejects_invalid_storage)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	char storage[1]			  = {0};
	t_gfx_software_buffer_data_t data = {.buf = {.data = storage, .size = sizeof(storage), .used = sizeof(storage)}};
	u32 values[2]			  = {1, 2};

	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.size = sizeof(values),
		.data = &data,
	};

	EXPECT_EQ(drv->buffer_set_data(&buffer, values, sizeof(values)), 1);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_bind_resources_uniform_buffer_success)
{
	START;

	u8 pixels[16]		      = {0};
	gfx_t gfx		      = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_scene(&gfx, &target, &render_pass, &framebuffer, pixels, 2, 2, 8), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	float data[16]	    = {0};
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer,
				   &gfx,
				   &(gfx_buffer_config_t){
					   .type  = GFX_BUFFER_UNIFORM,
					   .usage = GFX_BUFFER_USAGE_STATIC,
					   .size  = sizeof(data),
					   .data  = data,
				   }),
		   &buffer);
	gfx_resource_binding_t bindings[] = {
		{.binding = 0, .type = GFX_RESOURCE_UNIFORM_BUFFER, .buffer = &buffer},
	};

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_bind_resources(&frame, bindings, 1), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	t_gfx_software_scene_free(&gfx, &target, &render_pass, &framebuffer);
	END;
}

TEST(gfx_software_bind_resources_rejects_invalid_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	gfx_t gfx		       = {.drv = drv, .data = (void *)0x1234};
	gfx_frame_t frame	       = {.gfx = &gfx};
	gfx_buffer_t buffer	       = {.gfx = &gfx, .type = GFX_BUFFER_UNIFORM};
	gfx_resource_binding_t binding = {.binding = 0, .type = GFX_RESOURCE_UNIFORM_BUFFER, .buffer = &buffer};

	EXPECT_EQ(drv->bind_resources(NULL, NULL, 0), 1);
	EXPECT_EQ(drv->bind_resources(&frame, &binding, 1), 1);

	END;
}

TEST(gfx_software_shader_init_failures)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);

	t_gfx_software_alloc_count     = 0;
	alloc_t alloc_fail_after_first = ALLOC_STD;
	alloc_fail_after_first.alloc   = t_gfx_software_alloc_fail_after_first;
	gfx_t gfx_fail_alloc	       = {0};
	EXPECT_PTR(gfx_init(&gfx_fail_alloc, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, alloc_fail_after_first), &gfx_fail_alloc);
	gfx_shader_t shader = {0};
	log_set_quiet(0, 1);
	EXPECT_PTR(gfx_shader_init(&shader,
				   &gfx_fail_alloc,
				   &(gfx_shader_config_t){
					   .compiler = &compiler,
					   .source   = strv_cstr(t_gfx_software_shader_source),
					   .stage    = GFX_SHADER_STAGE_VERTEX,
				   }),
		   NULL);
	log_set_quiet(0, 0);
	gfx_free(&gfx_fail_alloc);

	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	log_set_quiet(0, 1);
	EXPECT_PTR(gfx_shader_init(&shader,
				   &gfx,
				   &(gfx_shader_config_t){
					   .compiler = &compiler,
					   .source   = STRV("not shader"),
					   .stage    = GFX_SHADER_STAGE_VERTEX,
				   }),
		   NULL);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_software_pipeline_layout_failures)
{
	START;

	gfx_t gfx		      = {0};
	gfx_render_pass_t render_pass = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = GFX_FORMAT_RGBA8,
						.load	      = GFX_LOAD_CLEAR,
						.store	      = GFX_STORE_STORE,
					}),
		   &render_pass);

	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline,
				     &gfx,
				     &(gfx_pipeline_config_t){
					     .render_pass	= &render_pass,
					     .input_layout_size = sizeof(gfx_layout_t),
				     }),
		   NULL);

	gfx_layout_t bad_type[] = {
		{.semantic = "POSITION", .count = 2, .type = GFX_VALUE_UNKNOWN},
		{.semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
	};
	EXPECT_PTR(gfx_pipeline_init(&pipeline,
				     &gfx,
				     &(gfx_pipeline_config_t){
					     .render_pass	= &render_pass,
					     .input_layout	= bad_type,
					     .input_layout_size = sizeof(bad_type),
				     }),
		   NULL);

	gfx_layout_t null_semantic[] = {
		{.semantic = NULL, .count = 2, .type = GFX_VALUE_FLOAT32},
		{.semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
	};
	EXPECT_PTR(gfx_pipeline_init(&pipeline,
				     &gfx,
				     &(gfx_pipeline_config_t){
					     .render_pass	= &render_pass,
					     .input_layout	= null_semantic,
					     .input_layout_size = sizeof(null_semantic),
				     }),
		   NULL);

	gfx_layout_t bad_count[] = {
		{.semantic = "POSITION", .count = 5, .type = GFX_VALUE_FLOAT32},
		{.semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
	};
	EXPECT_PTR(gfx_pipeline_init(&pipeline,
				     &gfx,
				     &(gfx_pipeline_config_t){
					     .render_pass	= &render_pass,
					     .input_layout	= bad_count,
					     .input_layout_size = sizeof(bad_count),
				     }),
		   NULL);

	gfx_layout_t missing_color[] = {
		{.semantic = "POSITION", .count = 2, .type = GFX_VALUE_FLOAT32},
	};
	EXPECT_PTR(gfx_pipeline_init(&pipeline,
				     &gfx,
				     &(gfx_pipeline_config_t){
					     .render_pass	= &render_pass,
					     .input_layout	= missing_color,
					     .input_layout_size = sizeof(missing_color),
				     }),
		   NULL);

	EXPECT_EQ(gfx.drv->pipeline_bind(&(gfx_frame_t){.gfx = &gfx}, &(gfx_pipeline_t){.gfx = &gfx}), 1);

	gfx_render_pass_free(&render_pass);
	gfx_free(&gfx);

	t_gfx_software_alloc_count     = 0;
	alloc_t alloc_fail_after_first = ALLOC_STD;
	alloc_fail_after_first.alloc   = t_gfx_software_alloc_fail_after_first;
	gfx_t gfx_fail_alloc	       = {0};
	render_pass		       = (gfx_render_pass_t){0};
	EXPECT_PTR(gfx_init(&gfx_fail_alloc, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, alloc_fail_after_first), &gfx_fail_alloc);
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx_fail_alloc,
					&(gfx_render_pass_config_t){
						.color_format = GFX_FORMAT_RGBA8,
						.load	      = GFX_LOAD_CLEAR,
						.store	      = GFX_STORE_STORE,
					}),
		   &render_pass);
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx_fail_alloc, &(gfx_pipeline_config_t){.render_pass = &render_pass}), NULL);
	gfx_render_pass_free(&render_pass);
	gfx_free(&gfx_fail_alloc);

	END;
}

TEST(gfx_software_draw_indexed_rejects_invalid_target)
{
	START;

	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_software_driver(), &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	gfx_buffer_t vertex_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&vertex_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&vertex_buffer);
	gfx_vertex_2d_t vertices[3] = {0};
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(vertices)), 0);
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&index_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&index_buffer);
	u32 indices[3] = {0, 1, 2};
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(indices)), 0);
	gfx_frame_t frame = {
		.gfx	       = &gfx,
		.vertex_buffer = &vertex_buffer,
		.index_buffer  = &index_buffer,
		.active	       = 1,
	};

	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 3), 1);

	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_surface_pass_begin_and_draw_clips)
{
	START;

	u8 pixels[64] = {0};
	t_gfx_software_reset_surface(pixels, 4, 4, 16);
	gfx_surface_t surface	      = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_t gfx		      = {0};
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_EQ(t_gfx_software_surface_scene(&gfx, &swapchain, &target, &render_pass, &framebuffer, &surface), 0);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = STRV("software")}), &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass, .vs = shader, .fs = shader}),
		   &pipeline);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx_vertex_2d_t vertices[] = {
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f},
		{.x = 1.0f, .y = -1.0f, .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f},
		{.x = -1.0f, .y = 1.0f, .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);

	gfx_frame_t frame	      = {0};
	gfx_pass_config_t pass_config = {
		.clear	  = {0.0f, 0.0f, 0.0f, 1.0f},
		.viewport = {1, 1, 4, 4},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_draw_clips_triangle_behind_near_plane)
{
	START;

	const t_gfx_software_vertex_3d_t vertices[3] = {
		{.x = -0.8f, .y = -0.8f, .z = -2.0f, .r = 1.0f, .a = 1.0f},
		{.x = 0.8f, .y = -0.8f, .z = -2.0f, .r = 1.0f, .a = 1.0f},
		{.x = -0.8f, .y = 0.8f, .z = -2.0f, .r = 1.0f, .a = 1.0f},
	};
	u32 lit_pixels = 0;
	EXPECT_EQ(t_gfx_software_draw_clip_vertices(vertices, &lit_pixels), 0);
	EXPECT_EQ(lit_pixels, 0);

	END;
}

TEST(gfx_software_draw_clips_triangle_crossing_near_plane)
{
	START;

	const t_gfx_software_vertex_3d_t vertices[3] = {
		{.x = -0.8f, .y = -0.8f, .z = -2.0f, .r = 1.0f, .a = 1.0f},
		{.x = 0.8f, .y = -0.8f, .z = 0.0f, .r = 1.0f, .a = 1.0f},
		{.x = -0.8f, .y = 0.8f, .z = 0.0f, .r = 1.0f, .a = 1.0f},
	};
	u32 lit_pixels = 0;
	EXPECT_EQ(t_gfx_software_draw_clip_vertices(vertices, &lit_pixels), 0);
	EXPECT_NE(lit_pixels, 0);

	END;
}

TEST(gfx_software_surface_pass_begin_reports_lazy_image_init_failure)
{
	START;

	u8 pixels[4] = {0};
	t_gfx_software_reset_surface(pixels, 1, 1, 4);
	gfx_surface_t surface = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_driver_t *drv     = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_swapchain_t swapchain = {
		.gfx	 = &gfx,
		.surface = &surface,
		.format	 = GFX_FORMAT_RGBA8,
		.width	 = 1,
		.height	 = 1,
	};
	gfx_image_t target = {
		.gfx	   = &gfx,
		.origin	   = GFX_IMAGE_ORIGIN_SURFACE,
		.format	   = GFX_FORMAT_RGBA8,
		.swapchain = &swapchain,
		.width	   = 1,
		.height	   = 1,
	};
	gfx_render_pass_t render_pass = {
		.gfx	      = &gfx,
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.image	     = &target,
		.render_pass = &render_pass,
		.width	     = 1,
		.height	     = 1,
	};
	gfx_frame_t frame	  = {.gfx = &gfx};
	t_gfx_software_memory_ret = 1;

	EXPECT_EQ(drv->framebuffer_pass_begin(&framebuffer, &frame), 1);
	EXPECT_NULL(target.driver_data);

	t_gfx_software_memory_ret = 0;
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_depth_test_keeps_nearer_triangle)
{
	START;

	u8 pixels[16]	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_image_init_memory(&target, &gfx, &memory_target_config), &target);

	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.depth_format = GFX_FORMAT_D32_FLOAT,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
		.depth_load   = GFX_LOAD_CLEAR,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	gfx_shader_t shader = {0};
	EXPECT_PTR(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){.source = strv_cstr(t_gfx_software_color_shader_source)}),
		   &shader);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline,
				     &gfx,
				     &(gfx_pipeline_config_t){
					     .render_pass	= &render_pass,
					     .vs		= shader,
					     .fs		= shader,
					     .input_layout	= t_gfx_software_layout_2d,
					     .input_layout_size = sizeof(t_gfx_software_layout_2d),
					     .depth		= {.test = 1, .write = 1, .compare = GFX_COMPARE_LESS},
				     }),
		   &pipeline);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer,
					     &frame,
					     &(gfx_pass_config_t){
						     .clear	  = {0.0f, 0.0f, 0.0f, 1.0f},
						     .clear_depth = 1.0f,
					     }),
		  0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);

	gfx_vertex_2d_t near_vertices[3] = {
		{.x = -1.0f, .y = -1.0f, .r = 0.0f, .g = 1.0f, .b = 0.25f, .a = 1.0f},
		{.x = 3.0f, .y = -1.0f, .r = 0.0f, .g = 1.0f, .b = 0.25f, .a = 1.0f},
		{.x = -1.0f, .y = 3.0f, .r = 0.0f, .g = 1.0f, .b = 0.25f, .a = 1.0f},
	};
	gfx_vertex_2d_t far_vertices[3] = {
		{.x = -1.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.75f, .a = 1.0f},
		{.x = 3.0f, .y = -1.0f, .r = 1.0f, .g = 0.0f, .b = 0.75f, .a = 1.0f},
		{.x = -1.0f, .y = 3.0f, .r = 1.0f, .g = 0.0f, .b = 0.75f, .a = 1.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, near_vertices, sizeof(near_vertices)), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, far_vertices, sizeof(far_vertices)), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(pixels[0], 0);
	EXPECT_EQ(pixels[1], 255);

	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&shader);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_depth_buffer_resizes_and_clears_when_disabled)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	u8 pixels_a[4]	     = {0};
	u8 pixels_b[8]	     = {0};
	gfx_image_t target_a = {0};
	gfx_image_t target_b = {0};
	EXPECT_PTR(gfx_image_init_memory(&target_a,
					 &gfx,
					 &(gfx_image_memory_config_t){
						 .format = GFX_FORMAT_RGBA8,
						 .data	 = pixels_a,
						 .width	 = 1,
						 .height = 1,
						 .stride = 4,
					 }),
		   &target_a);
	EXPECT_PTR(gfx_image_init_memory(&target_b,
					 &gfx,
					 &(gfx_image_memory_config_t){
						 .format = GFX_FORMAT_RGBA8,
						 .data	 = pixels_b,
						 .width	 = 2,
						 .height = 1,
						 .stride = 8,
					 }),
		   &target_b);
	gfx_render_pass_t depth_pass = {0};
	EXPECT_PTR(gfx_render_pass_init(&depth_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = GFX_FORMAT_RGBA8,
						.depth_format = GFX_FORMAT_D32_FLOAT,
						.depth_load   = GFX_LOAD_CLEAR,
					}),
		   &depth_pass);
	gfx_render_pass_t color_pass = {0};
	EXPECT_PTR(gfx_render_pass_init(&color_pass, &gfx, &(gfx_render_pass_config_t){.color_format = GFX_FORMAT_RGBA8}), &color_pass);
	gfx_framebuffer_t framebuffer_a = {0};
	gfx_framebuffer_t framebuffer_b = {0};
	gfx_framebuffer_t framebuffer_c = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer_a, &target_a, &depth_pass), &framebuffer_a);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer_b, &target_b, &depth_pass), &framebuffer_b);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer_c, &target_b, &color_pass), &framebuffer_c);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer_a, &frame, &(gfx_pass_config_t){.clear_depth = 1.0f}), 0);
	t_gfx_software_data_t *render = gfx.data;
	EXPECT_NOT_NULL(render->depth);
	EXPECT_EQ(render->depth_size, 1);
	EXPECT_EQ(gfx_end(&frame), 0);

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer_b, &frame, &(gfx_pass_config_t){.clear_depth = 0.5f}), 0);
	EXPECT_NOT_NULL(render->depth);
	EXPECT_EQ(render->depth_size, 2);
	EXPECT_EQ(gfx_end(&frame), 0);

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer_c, &frame, &(gfx_pass_config_t){0}), 0);
	EXPECT_NULL(render->depth);
	EXPECT_EQ(render->depth_size, 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_framebuffer_free(&framebuffer_c);
	gfx_framebuffer_free(&framebuffer_b);
	gfx_framebuffer_free(&framebuffer_a);
	gfx_render_pass_free(&color_pass);
	gfx_render_pass_free(&depth_pass);
	gfx_image_free(&target_b);
	gfx_image_free(&target_a);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_depth_buffer_alloc_failure)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);
	u8 pixels[4]	   = {0};
	gfx_image_t target = {0};
	EXPECT_PTR(gfx_image_init_memory(&target,
					 &gfx,
					 &(gfx_image_memory_config_t){
						 .format = GFX_FORMAT_RGBA8,
						 .data	 = pixels,
						 .width	 = 1,
						 .height = 1,
						 .stride = 4,
					 }),
		   &target);
	gfx_render_pass_t render_pass = {0};
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = GFX_FORMAT_RGBA8,
						.depth_format = GFX_FORMAT_D32_FLOAT,
					}),
		   &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	alloc_t saved	  = gfx.alloc;
	gfx.alloc	  = ALLOC_STD;
	gfx.alloc.alloc	  = t_gfx_software_alloc_fail;
	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}), 1);
	gfx.alloc = saved;

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_free(&gfx);
	END;
}

TEST(gfx_software_driver_direct_branches)
{
	START;

	u8 pixels[16]	  = {0};
	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, NULL, ALLOC_STD), &gfx);

	gfx_image_t target_1 = {
		.gfx	= &gfx,
		.origin = GFX_IMAGE_ORIGIN_MEMORY,
		.format = GFX_FORMAT_NONE,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_EQ(drv->image_init(&target_1), 1);
	t_gfx_software_reset_surface(pixels, 1, 1, 4);
	gfx_swapchain_t swapchain_1 = {
		.surface = &(gfx_surface_t){.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops},
		.format	 = GFX_FORMAT_RGBA8,
		.width	 = 1,
		.height	 = 1,
	};
	gfx_image_t target = {
		.gfx	   = &(gfx_t){.data = gfx.data, .alloc = (alloc_t){.alloc = t_gfx_software_alloc_fail}},
		.origin	   = GFX_IMAGE_ORIGIN_SURFACE,
		.format	   = GFX_FORMAT_RGBA8,
		.swapchain = &swapchain_1,
		.width	   = 1,
		.height	   = 1,
	};
	EXPECT_EQ(drv->image_init(&target), 1);
	drv->image_free(NULL);

	EXPECT_EQ(drv->image_read(&(gfx_image_t){.gfx = &gfx}, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);

	gfx_frame_t frame		   = {.gfx = &gfx};
	gfx_render_pass_t pass		   = {0};
	gfx_surface_t software_surface	   = {.api = GFX_API_SOFTWARE, .ops = &t_gfx_software_surface_ops};
	gfx_swapchain_t software_swapchain = {
		.surface = &software_surface,
		.format	 = GFX_FORMAT_RGBA8,
		.width	 = 1,
		.height	 = 1,
	};
	t_gfx_software_surface_target_data_t surface_data = {.memory = {0}};

	gfx_image_t invalid_surface_target = {
		.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
		.format	     = GFX_FORMAT_RGBA8,
		.swapchain   = &software_swapchain,
		.width	     = 1,
		.height	     = 1,
		.driver_data = &surface_data,
	};
	gfx_framebuffer_t invalid_surface_framebuffer = {.image = &invalid_surface_target, .render_pass = &pass};

	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.image = &(gfx_image_t){.origin = GFX_IMAGE_ORIGIN_NONE}, .render_pass = &pass}, &frame),
		  0);
	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.image = &(gfx_image_t){.origin = GFX_IMAGE_ORIGIN_SURFACE}, .render_pass = &pass}, &frame),
		  1);
	EXPECT_EQ(drv->framebuffer_pass_begin(&invalid_surface_framebuffer, &frame), 1);
	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.image = &(gfx_image_t){.origin = GFX_IMAGE_ORIGIN_MEMORY}, .render_pass = &pass}, &frame),
		  1);
	EXPECT_EQ(drv->framebuffer_pass_begin(
			  &(gfx_framebuffer_t){.image = &(gfx_image_t){.origin = (gfx_image_origin_t)99}, .render_pass = &pass}, &frame),
		  1);
	drv->buffer_free(NULL);
	gfx_buffer_t buffer = {.gfx = &gfx};
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_software_alloc_fail};
	EXPECT_EQ(drv->buffer_init(&buffer, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}), 1);
	gfx.alloc = ALLOC_STD;
	EXPECT_EQ(drv->draw(&(gfx_frame_t){.gfx = &gfx, .vertex_buffer = &(gfx_buffer_t){.gfx = &gfx, .data = (void *)1}}, 3, 0), 1);

	gfx_render_pass_t render_pass = {0};
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = GFX_FORMAT_RGBA8,
						.load	      = GFX_LOAD_CLEAR,
						.store	      = GFX_STORE_STORE,
					}),
		   &render_pass);
	gfx_pipeline_t pipeline = {0};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){.render_pass = &render_pass}), &pipeline);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_buffer_t vertex_buffer  = {0};
	EXPECT_PTR(gfx_buffer_init(&vertex_buffer,
				   &gfx,
				   &(gfx_buffer_config_t){
					   .type  = GFX_BUFFER_VERTEX,
					   .usage = GFX_BUFFER_USAGE_STATIC,
					   .size  = sizeof(vertices),
					   .data  = vertices,
				   }),
		   &vertex_buffer);
	u32 indices[3]		  = {0, 1, 2};
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&index_buffer,
				   &gfx,
				   &(gfx_buffer_config_t){
					   .type  = GFX_BUFFER_INDEX,
					   .usage = GFX_BUFFER_USAGE_STATIC,
					   .size  = sizeof(indices),
					   .data  = indices,
				   }),
		   &index_buffer);
	EXPECT_EQ(drv->draw(&(gfx_frame_t){.gfx = &gfx, .pipeline = &pipeline, .vertex_buffer = &vertex_buffer}, 3, 0), 1);
	EXPECT_EQ(drv->draw_indexed(
			  &(gfx_frame_t){
				  .gfx		 = &gfx,
				  .pipeline	 = &pipeline,
				  .vertex_buffer = &vertex_buffer,
				  .index_buffer	 = &index_buffer,
			  },
			  3),
		  1);
	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_render_pass_free(&render_pass);

	gfx_free(&gfx);
	END;
}

TEST(gfx_software_driver_callback_failures)
{
	START;

	gfx_driver_t *drv = t_gfx_software_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->image_init(NULL), 1);
	EXPECT_EQ(drv->image_init(&(gfx_image_t){.origin = GFX_IMAGE_ORIGIN_NONE, .gfx = &(gfx_t){.data = (void *)1}}), 1);
	EXPECT_EQ(drv->image_read(NULL, &(gfx_memory_readback_config_t){0}), 1);
	EXPECT_EQ(drv->framebuffer_pass_begin(NULL, NULL), 1);
	EXPECT_EQ(drv->buffer_init(NULL, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}), 1);
	EXPECT_EQ(drv->buffer_set_data(NULL, NULL, 0), 1);
	EXPECT_EQ(drv->buffer_bind(NULL, NULL), 1);
	EXPECT_EQ(drv->shader_init(NULL, &(gfx_shader_config_t){0}), 1);
	EXPECT_EQ(drv->pipeline_init(NULL, &(gfx_pipeline_config_t){0}), 1);
	drv->pipeline_free(&(gfx_pipeline_t){0});
	EXPECT_EQ(drv->draw(NULL, 3, 0), 1);
	EXPECT_EQ(drv->draw(&(gfx_frame_t){.gfx = &(gfx_t){.data = (void *)1}}, 2, 0), 1);
	EXPECT_EQ(drv->end(NULL), 1);

	END;
}

STEST(gfx_software)
{
	SSTART;
	RUN(gfx_software_driver_is_registered);
	RUN(gfx_software_init_failures);
	RUN(gfx_software_memory_pass_clears_target);
	RUN(gfx_software_memory_pass_load_does_not_clear);
	RUN(gfx_software_surface_target_lifecycle);
	RUN(gfx_software_surface_target_failures);
	RUN(gfx_software_swapchain_resize_rejects_invalid_direct);
	RUN(gfx_software_swapchain_free_rejects_missing_image_storage_direct);
	RUN(gfx_software_draw_triangle);
	RUN(gfx_software_draw_indexed_triangle);
	RUN(gfx_software_draw_shader_pipeline_vec3_uniform);
	RUN(gfx_software_shader_pipeline_uniform_failures);
	RUN(gfx_software_shader_pipeline_color_components);
	RUN(gfx_software_shader_pipeline_expression_failures);
	RUN(gfx_software_draw_buffer_failures);
	RUN(gfx_software_buffer_set_data_alloc_failure);
	RUN(gfx_software_buffer_init_static_uploads_data);
	RUN(gfx_software_buffer_init_static_alloc_failure);
	RUN(gfx_software_buffer_set_data_resize_failure);
	RUN(gfx_software_buffer_set_data_rejects_invalid_storage);
	RUN(gfx_software_bind_resources_uniform_buffer_success);
	RUN(gfx_software_bind_resources_rejects_invalid_direct);
	RUN(gfx_software_shader_init_failures);
	RUN(gfx_software_pipeline_layout_failures);
	RUN(gfx_software_draw_indexed_rejects_invalid_target);
	RUN(gfx_software_surface_pass_begin_and_draw_clips);
	RUN(gfx_software_draw_clips_triangle_behind_near_plane);
	RUN(gfx_software_draw_clips_triangle_crossing_near_plane);
	RUN(gfx_software_surface_pass_begin_reports_lazy_image_init_failure);
	RUN(gfx_software_depth_test_keeps_nearer_triangle);
	RUN(gfx_software_depth_buffer_resizes_and_clears_when_disabled);
	RUN(gfx_software_depth_buffer_alloc_failure);
	RUN(gfx_software_driver_direct_branches);
	RUN(gfx_software_driver_callback_failures);
	SEND;
}
