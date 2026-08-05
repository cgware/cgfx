#include "gfx_driver.h"

static int gfx_buffer_config_valid(const gfx_buffer_config_t *config)
{
	if (config == NULL || (config->type != GFX_BUFFER_VERTEX && config->type != GFX_BUFFER_INDEX) ||
	    (config->usage != GFX_BUFFER_USAGE_DYNAMIC && config->usage != GFX_BUFFER_USAGE_STATIC) ||
	    (config->data != NULL && config->size == 0)) {
		return 0;
	}
	if (config->usage == GFX_BUFFER_USAGE_STATIC && (config->data == NULL || config->size == 0)) {
		return 0;
	}
	return 1;
}

gfx_buffer_t *gfx_buffer_init(gfx_buffer_t *buf, gfx_t *gfx, const gfx_buffer_config_t *config)
{
	if (buf == NULL || gfx == NULL || gfx->drv == NULL || gfx->drv->buffer_init == NULL || !gfx_buffer_config_valid(config)) {
		return NULL;
	}

	buf->gfx   = gfx;
	buf->type  = config->type;
	buf->usage = config->usage;
	buf->size  = config->size;
	if (gfx->drv->buffer_init(buf, config)) {
		buf->gfx   = NULL;
		buf->type  = GFX_BUFFER_UNKNOWN;
		buf->usage = GFX_BUFFER_USAGE_DYNAMIC;
		buf->size  = 0;
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
	    size == 0 || buf->usage == GFX_BUFFER_USAGE_STATIC) {
		return 1;
	}

	return buf->gfx->drv->buffer_set_data(buf, data, size);
}

int gfx_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buf)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->frame != frame || !frame->active || frame->pipeline == NULL || buf == NULL ||
	    buf->gfx != frame->gfx || buf->gfx->drv == NULL || buf->gfx->drv->buffer_bind == NULL) {
		return 1;
	}

	if (buf->gfx->drv->buffer_bind(frame, buf)) {
		return 1;
	}

	switch (buf->type) {
	case GFX_BUFFER_VERTEX:
		frame->vertex_buffer = buf;
		break;
	case GFX_BUFFER_INDEX:
		frame->index_buffer = buf;
		break;
	default:
		return 1;
	}
	return 0;
}
