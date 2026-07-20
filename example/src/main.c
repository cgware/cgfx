#include "buf.h"
#include "fs.h"
#include "gfx.h"
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
	fs_t fs		   = {0};
	gfx_t gfx	   = {0};
	int ret		   = 0;
	strv_t driver_name = STRV("software");

	fs_init(&fs, 0, 0, ALLOC_STD);
	gfx_driver_t *driver = gfx_driver_find(driver_name);
	if (driver == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to find gfx driver: %.*s", driver_name.len, driver_name.data);
		ret = 1;
	}
	if (ret == 0 && gfx_init(&gfx, driver, &(gfx_config_t){.alloc = ALLOC_STD}) == NULL) {
		log_error("cgfx_example", "main", NULL, "failed to initialize gfx driver: %s", driver->name);
		ret = 1;
	}

	if (ret == 0 && gfx_set_target(&gfx,
				       &(gfx_target_t){
					       .type   = GFX_TARGET_MEMORY,
					       .format = GFX_FORMAT_RGBA8,
					       .data   = pixels,
					       .width  = WIDTH,
					       .height = HEIGHT,
					       .stride = WIDTH * 4,
				       })) {
		log_error("cgfx_example", "main", NULL, "failed to set memory render target");
		ret = 1;
	}
	if (ret == 0 && gfx_viewport(&gfx, 0, 0, WIDTH, HEIGHT)) {
		log_error("cgfx_example", "main", NULL, "failed to set viewport");
		ret = 1;
	}
	if (ret == 0 && gfx_clear_color(&gfx, R / 255.0f, G / 255.0f, B / 255.0f, A / 255.0f)) {
		log_error("cgfx_example", "main", NULL, "failed to set clear color");
		ret = 1;
	}
	if (ret == 0 && gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER)) {
		log_error("cgfx_example", "main", NULL, "failed to clear memory render target");
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

	gfx_free(&gfx);
	fs_free(&fs);
	return ret;
}
