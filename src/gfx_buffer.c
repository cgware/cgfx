#include "gfx_buffer.h"

#include "gfx_driver.h"

gfx_buffer_t *gfx_buffer_init(gfx_buffer_t *buf, gfx_t *gfx, const gfx_buffer_config_t *config)
{
	if (buf == NULL || gfx == NULL || gfx->drv == NULL || gfx->drv->buffer_init == NULL) {
		return NULL;
	}

	buf->gfx = gfx;
	if (gfx->drv->buffer_init(buf, config)) {
		return NULL;
	}

	return buf;
}

void gfx_buffer_free(gfx_buffer_t *buf)
{
	if (buf == NULL || buf->gfx == NULL || buf->gfx->drv == NULL || buf->gfx->drv->buffer_free == NULL) {
		return;
	}

	buf->gfx->drv->buffer_free(buf);
}

int gfx_buffer_set_data(gfx_buffer_t *buf, const void *data, size_t size)
{
	if (buf == NULL || buf->gfx == NULL || buf->gfx->drv == NULL || buf->gfx->drv->buffer_set_data == NULL || data == NULL ||
	    size == 0) {
		return 1;
	}

	return buf->gfx->drv->buffer_set_data(buf, data, size);
}
