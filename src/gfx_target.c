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

gfx_target_t *gfx_target_init_swapchain(gfx_target_t *target, gfx_swapchain_t *swapchain)
{
	if (target == NULL || swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->drv == NULL ||
	    swapchain->format == GFX_FORMAT_NONE || swapchain->surface == NULL || swapchain->width == 0 || swapchain->height == 0) {
		return NULL;
	}
	if (swapchain->gfx->frame != NULL) {
		return NULL;
	}

	*target = (gfx_target_t){
		.gfx		      = swapchain->gfx,
		.type		      = GFX_TARGET_SWAPCHAIN,
		.format		      = swapchain->format,
		.swapchain	      = swapchain,
		.width		      = swapchain->width,
		.height		      = swapchain->height,
		.swapchain_generation = swapchain->generation,
	};
	if (swapchain->gfx->drv->target_init != NULL && swapchain->gfx->drv->target_init(target)) {
		if (swapchain->gfx->drv->target_free != NULL) {
			swapchain->gfx->drv->target_free(target);
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
