#include "gfx_target.h"

#include "gfx_driver.h"

gfx_target_t *gfx_target_init_memory(gfx_target_t *target, gfx_t *gfx, const gfx_memory_target_config_t *config)
{
	if (target == NULL || gfx == NULL || gfx->drv == NULL || config == NULL || config->format == GFX_FORMAT_NONE ||
	    config->data == NULL || config->width == 0 || config->height == 0 || config->stride < (size_t)config->width * 4) {
		return NULL;
	}
	if (gfx->frame != NULL) {
		return NULL;
	}

	*target = (gfx_target_t){
		.gfx	= gfx,
		.type	= GFX_TARGET_MEMORY,
		.format = config->format,
		.data	= config->data,
		.width	= config->width,
		.height = config->height,
		.stride = config->stride,
	};
	if (gfx->drv->target_init != NULL && gfx->drv->target_init(target)) {
		if (gfx->drv->target_free != NULL) {
			gfx->drv->target_free(target);
		}
		*target = (gfx_target_t){0};
		return NULL;
	}
	return target;
}

gfx_target_t *gfx_target_init_surface(gfx_target_t *target, gfx_t *gfx, const gfx_surface_target_config_t *config)
{
	if (target == NULL || gfx == NULL || gfx->drv == NULL || config == NULL || config->format == GFX_FORMAT_NONE ||
	    config->surface == NULL || config->surface->api != gfx->drv->api || config->width == 0 || config->height == 0) {
		return NULL;
	}
	if (gfx->frame != NULL) {
		return NULL;
	}

	*target = (gfx_target_t){
		.gfx	 = gfx,
		.type	 = GFX_TARGET_SURFACE,
		.format	 = config->format,
		.surface = config->surface,
		.width	 = config->width,
		.height	 = config->height,
	};
	if (gfx->drv->target_init != NULL && gfx->drv->target_init(target)) {
		if (gfx->drv->target_free != NULL) {
			gfx->drv->target_free(target);
		}
		*target = (gfx_target_t){0};
		return NULL;
	}
	return target;
}

void gfx_target_free(gfx_target_t *target)
{
	if (target == NULL || target->gfx == NULL || target->gfx->frame != NULL) {
		return;
	}

	if (target->gfx->drv != NULL && target->gfx->drv->target_free != NULL) {
		target->gfx->drv->target_free(target);
	}
	*target = (gfx_target_t){0};
}

int gfx_target_resize(gfx_target_t *target, u16 width, u16 height)
{
	if (target == NULL || target->gfx == NULL || target->gfx->frame != NULL || width == 0 || height == 0 ||
	    target->type != GFX_TARGET_SURFACE) {
		return 1;
	}

	u16 old_width  = target->width;
	u16 old_height = target->height;
	target->width  = width;
	target->height = height;
	if (target->gfx->drv != NULL && target->gfx->drv->target_resize != NULL && target->gfx->drv->target_resize(target, width, height)) {
		target->width  = old_width;
		target->height = old_height;
		return 1;
	}
	return 0;
}

int gfx_target_move(gfx_target_t *dst, gfx_target_t *src, gfx_t *gfx)
{
	if (dst == NULL || src == NULL || src->gfx == NULL || gfx == NULL || dst->gfx != NULL || src->gfx->frame != NULL ||
	    gfx->frame != NULL || src->gfx->drv != gfx->drv || src->gfx->data != gfx->data) {
		return 1;
	}

	*dst	 = *src;
	dst->gfx = gfx;
	*src	 = (gfx_target_t){0};
	return 0;
}

int gfx_target_read(gfx_target_t *target, const gfx_memory_readback_config_t *config)
{
	if (target == NULL || target->gfx == NULL || target->gfx->drv == NULL || target->gfx->drv->target_read == NULL ||
	    target->gfx->frame != NULL || target->type != GFX_TARGET_MEMORY || target->format == GFX_FORMAT_NONE || target->width == 0 ||
	    target->height == 0 || config == NULL || config->data == NULL || config->stride < (size_t)target->width * 4) {
		return 1;
	}

	return target->gfx->drv->target_read(target, config);
}

int gfx_target_present(gfx_target_t *target)
{
	if (target == NULL || target->gfx == NULL || target->gfx->drv == NULL || target->gfx->drv->target_present == NULL ||
	    target->gfx->frame != NULL || target->type != GFX_TARGET_SURFACE || target->surface == NULL || target->width == 0 ||
	    target->height == 0) {
		return 1;
	}

	return target->gfx->drv->target_present(target);
}
