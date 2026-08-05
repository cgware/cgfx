#include "gfx_driver.h"

gfx_swapchain_t *gfx_swapchain_init(gfx_swapchain_t *swapchain, gfx_t *gfx, const gfx_swapchain_config_t *config)
{
	if (swapchain == NULL || gfx == NULL || gfx->drv == NULL || config == NULL || config->format == GFX_FORMAT_NONE ||
	    config->surface == NULL || config->surface->api != gfx->drv->api || config->width == 0 || config->height == 0) {
		return NULL;
	}
	if (gfx->frame != NULL) {
		return NULL;
	}

	*swapchain = (gfx_swapchain_t){
		.gfx		     = gfx,
		.surface	     = config->surface,
		.format		     = config->format,
		.width		     = config->width,
		.height		     = config->height,
		.present_mode	     = config->present_mode,
		.actual_present_mode = config->present_mode,
	};
	if (gfx->drv->swapchain_init != NULL && gfx->drv->swapchain_init(swapchain, config)) {
		if (gfx->drv->swapchain_free != NULL) {
			gfx->drv->swapchain_free(swapchain);
		}
		*swapchain = (gfx_swapchain_t){0};
		return NULL;
	}
	swapchain->generation = 1;
	return swapchain;
}

void gfx_swapchain_free(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->frame != NULL) {
		return;
	}

	if (swapchain->gfx->drv != NULL && swapchain->gfx->drv->swapchain_free != NULL) {
		swapchain->gfx->drv->swapchain_free(swapchain);
	}
	*swapchain = (gfx_swapchain_t){0};
}

int gfx_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->frame != NULL || width == 0 || height == 0) {
		return 1;
	}

	u16 old_width		= swapchain->width;
	u16 old_height		= swapchain->height;
	gfx_format_t old_format = swapchain->format;
	swapchain->width	= width;
	swapchain->height	= height;
	if (swapchain->gfx->drv != NULL && swapchain->gfx->drv->swapchain_resize != NULL &&
	    swapchain->gfx->drv->swapchain_resize(swapchain, width, height)) {
		swapchain->width  = old_width;
		swapchain->height = old_height;
		swapchain->format = old_format;
		return 1;
	}
	swapchain->generation++;
	return 0;
}

int gfx_swapchain_present(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->drv == NULL || swapchain->gfx->drv->swapchain_present == NULL ||
	    swapchain->gfx->frame != NULL || swapchain->surface == NULL || swapchain->width == 0 || swapchain->height == 0) {
		return 1;
	}

	return swapchain->gfx->drv->swapchain_present(swapchain);
}
