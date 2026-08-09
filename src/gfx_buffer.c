#include "gfx_driver.h"

static int gfx_buffer_config_valid(const gfx_buffer_config_t *config)
{
	if (config == NULL ||
	    (config->type != GFX_BUFFER_VERTEX && config->type != GFX_BUFFER_INDEX && config->type != GFX_BUFFER_UNIFORM) ||
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
	case GFX_BUFFER_UNIFORM:
		break;
	default:
		return 1;
	}
	return 0;
}

static int gfx_resource_binding_valid(gfx_t *gfx, const gfx_resource_binding_t *binding)
{
	if (gfx == NULL || binding == NULL) {
		return 0; // LCOV_EXCL_LINE
	}
	switch (binding->type) {
	case GFX_RESOURCE_UNIFORM_BUFFER:
		return binding->buffer != NULL && binding->buffer->gfx == gfx && binding->buffer->type == GFX_BUFFER_UNIFORM;
	default:
		return 0;
	}
}

static int gfx_resource_bindings_valid(gfx_t *gfx, const gfx_resource_binding_t *bindings, u32 binding_count)
{
	if (binding_count == 0) {
		return 1;
	}
	if (bindings == NULL) {
		return 0;
	}
	for (u32 i = 0; i < binding_count; i++) {
		if (!gfx_resource_binding_valid(gfx, &bindings[i])) {
			return 0;
		}
		for (u32 j = i + 1; j < binding_count; j++) {
			if (bindings[i].type == bindings[j].type && bindings[i].binding == bindings[j].binding) {
				return 0;
			}
		}
	}
	return 1;
}

int gfx_bind_resources(gfx_frame_t *frame, const gfx_resource_binding_t *bindings, u32 binding_count)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->frame != frame || !frame->active || frame->pipeline == NULL ||
	    frame->gfx->drv == NULL || frame->gfx->drv->bind_resources == NULL ||
	    !gfx_resource_bindings_valid(frame->gfx, bindings, binding_count)) {
		return 1;
	}

	if (frame->gfx->drv->bind_resources(frame, bindings, binding_count)) {
		return 1;
	}

	frame->resource_bindings      = bindings;
	frame->resource_binding_count = binding_count;
	return 0;
}
