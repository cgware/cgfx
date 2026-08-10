#include "fs.h"
#include "gfx_driver.h"
#include "log.h"

static int write_bmp(fs_t *fs, strv_t path, const u8 *pixels, u16 width, u16 height, size_t stride)
{
	enum {
		FILE_HEADER_SIZE = 14,
		INFO_HEADER_SIZE = 40,
		PIXEL_SIZE	 = 4,
	};

	u32 image_size = (u32)width * height * PIXEL_SIZE;
	u32 file_size  = FILE_HEADER_SIZE + INFO_HEADER_SIZE + image_size;

	buf_t buf = {0};
	if (buf_init(&buf, file_size, ALLOC_STD) == NULL) {
		return 1;
	}

	int failed = buf_write_u8le(&buf, 'B') || buf_write_u8le(&buf, 'M') || buf_write_u32le(&buf, file_size) ||
		     buf_write_u16le(&buf, 0) || buf_write_u16le(&buf, 0) || buf_write_u32le(&buf, FILE_HEADER_SIZE + INFO_HEADER_SIZE) ||
		     buf_write_u32le(&buf, INFO_HEADER_SIZE) || buf_write_u32le(&buf, width) ||
		     buf_write_u32le(&buf, (u32)(-(int)height)) || buf_write_u16le(&buf, 1) || buf_write_u16le(&buf, 32) ||
		     buf_write_u32le(&buf, 0) || buf_write_u32le(&buf, image_size) || buf_write_u32le(&buf, 2835) ||
		     buf_write_u32le(&buf, 2835) || buf_write_u32le(&buf, 0) || buf_write_u32le(&buf, 0);
	for (u16 y = 0; !failed && y < height; y++) {
		const u8 *row = pixels + (size_t)y * stride;
		for (u16 x = 0; x < width; x++) {
			const u8 *pixel = row + (size_t)x * PIXEL_SIZE;
			u8 bgra[]	= {pixel[2], pixel[1], pixel[0], pixel[3]};
			failed		= buf_add(&buf, sizeof(bgra), bgra, NULL);
			if (failed) {
				break;
			}
		}
	}

	void *file = NULL;
	if (!failed && fs_open(fs, path, "wb", &file)) {
		failed = 1;
	}
	if (!failed && fs_writeb(fs, file, buf)) {
		failed = 1;
	}
	if (file != NULL && fs_close(fs, file)) {
		failed = 1;
	}
	buf_free(&buf);

	return failed;
}

int main(void)
{
	c_print_init();

	log_t log = {0};
	log_set(&log);
	log_add_callback(log_std_cb, DST_STD(), LOG_ERROR, 1, 1);

	enum {
		WIDTH  = 640,
		HEIGHT = 480,
		R      = 26,
		G      = 51,
		B      = 77,
		A      = 255,
	};

	static u8 pixels[WIDTH * HEIGHT * 4];
	fs_t fs			      = {0};
	proc_t proc		      = {0};
	gfx_t gfx		      = {0};
	gfx_buffer_t vb		      = {0};
	gfx_shader_t vertex	      = {0};
	gfx_shader_t fragment	      = {0};
	gfx_image_t image	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	gfx_pipeline_t pipeline	      = {0};
	int ret			      = 0;
	strv_t driver_name	      = STRV("software");

	fs_init(&fs, 0, 0, ALLOC_STD);
	proc_init(&proc, 0, 0, ALLOC_STD);
	gfx_driver_t *driver = gfx_driver_find(driver_name);
	if (driver == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to find gfx driver: %.*s", driver_name.len, driver_name.data);
		ret = 1;
	}
	if (ret == 0 && gfx_init(&gfx, driver, &(gfx_config_t){0}, &proc, ALLOC_STD) == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to initialize gfx driver: %s", driver->name);
		ret = 1;
	}

	gfx_vertex_2d_t vertices[3] = {
		{.x = 0.0f, .y = 0.7f, .r = 1.0f, .a = 1.0f},
		{.x = 0.7f, .y = -0.7f, .g = 1.0f, .a = 1.0f},
		{.x = -0.7f, .y = -0.7f, .b = 1.0f, .a = 1.0f},
	};
	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_VERTEX,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(vertices),
		.data  = vertices,
	};
	if (ret == 0 && gfx_buffer_init(&vb, &gfx, &buffer_config) == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to initialize triangle vertex buffer");
		ret = 1;
	}
	const char *triangle_src = "vs_in 0 VertexIn {\n"
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
				   "\toutput.position = vec4f(input.position.x, input.position.y, 0.0f, 1.0f);\n"
				   "\toutput.color = input.color;\n"
				   "\treturn output;\n"
				   "}\n"
				   "FragmentOut fragment(FragmentIn input) {\n"
				   "\tFragmentOut output;\n"
				   "\toutput.color = input.color;\n"
				   "\treturn output;\n"
				   "}\n";

	if (ret == 0 && gfx_shader_init(&vertex, &gfx, &(gfx_shader_config_t){.source = strv_cstr(triangle_src)}) == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to initialize triangle vertex shader");
		ret = 1;
	}
	if (ret == 0 && gfx_shader_init(&fragment, &gfx, &(gfx_shader_config_t){.source = strv_cstr(triangle_src)}) == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to initialize triangle fragment shader");
		ret = 1;
	}
	static const gfx_layout_t input_layout[] = {
		{.index = 0, .semantic = "POSITION", .count = 2, .type = GFX_VALUE_FLOAT32},
		{.index = 1, .semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
	};
	gfx_image_memory_config_t image_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= WIDTH,
		.height = HEIGHT,
		.stride = WIDTH * 4,
	};
	if (ret == 0 && gfx_image_init_memory(&image, &gfx, &image_config) == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to initialize memory image");
		ret = 1;
	}
	gfx_render_pass_config_t render_pass_config = {
		.color_format = image.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	if (ret == 0 && gfx_render_pass_init(&render_pass, &gfx, &render_pass_config) == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to initialize render pass");
		ret = 1;
	}
	if (ret == 0 && gfx_framebuffer_init(&framebuffer, &image, &render_pass) == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to initialize framebuffer");
		ret = 1;
	}
	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vertex,
		.fs		   = fragment,
		.input_layout	   = input_layout,
		.input_layout_size = sizeof(input_layout),
	};
	if (ret == 0 && gfx_pipeline_init(&pipeline, &gfx, &pipeline_config) == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to initialize pipeline");
		ret = 1;
	}
	gfx_pass_config_t pass_config = {
		.clear	  = {R / 255.0f, G / 255.0f, B / 255.0f, A / 255.0f},
		.viewport = {0, 0, WIDTH, HEIGHT},
	};
	gfx_frame_t frame = {0};
	if (ret == 0 && gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config)) {
		log_error("cgfx_example", "main", NULL, "failed to begin render pass");
		ret = 1;
	}
	if (ret == 0 && gfx_pipeline_bind(&frame, &pipeline)) {
		log_error("cgfx_example", "main", NULL, "failed to bind pipeline");
		ret = 1;
	}
	if (ret == 0 && gfx_buffer_bind(&frame, &vb)) {
		log_error("cgfx_example", "main", NULL, "failed to bind vertex buffer");
		ret = 1;
	}
	if (ret == 0 && gfx_draw(&frame, 3, 0)) {
		log_error("cgfx_example", "main", NULL, "failed to draw triangle");
		ret = 1;
	}
	if (frame.active && gfx_end(&frame)) {
		log_error("cgfx_example", "main", NULL, "failed to end frame");
		ret = 1;
	}
	if (ret == 0 && gfx_image_read(&image, &(gfx_memory_readback_config_t){.data = pixels, .stride = WIDTH * 4})) {
		log_error("cgfx_example", "main", NULL, "failed to read back rendered image");
		ret = 1;
	}

	if (ret == 0 && (pixels[0] != R || pixels[1] != G || pixels[2] != B || pixels[3] != A)) {
		log_error("cgfx_example", "main", NULL, "unexpected first pixel: %u %u %u %u", pixels[0], pixels[1], pixels[2], pixels[3]);
		ret = 1;
	}
	if (ret == 0 && write_bmp(&fs, STRV("cgfx_example.bmp"), pixels, WIDTH, HEIGHT, WIDTH * 4)) {
		log_error("cgfx_example", "main", NULL, "failed to write rendered image");
		ret = 1;
	}

	gfx_shader_free(&vertex);
	gfx_shader_free(&fragment);
	gfx_pipeline_free(&pipeline);
	gfx_framebuffer_free(&framebuffer);
	gfx_image_free(&image);
	gfx_render_pass_free(&render_pass);
	gfx_free(&gfx);
	proc_free(&proc);
	fs_free(&fs);
	return ret;
}
