#include "gfx_driver.h"

static int gfx_swapchain_image_set(gfx_swapchain_t *swapchain, u32 index)
{
	if (swapchain == NULL || swapchain->images == NULL || index >= swapchain->image_capacity) {
		return 1;
	}

	void *driver_data	 = swapchain->images[index].driver_data;
	swapchain->images[index] = (gfx_image_t){
		.gfx	     = swapchain->gfx,
		.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
		.format	     = swapchain->format,
		.driver_data = driver_data,
		.swapchain   = swapchain,
		.width	     = swapchain->width,
		.height	     = swapchain->height,
		.usage	     = swapchain->usage | GFX_IMAGE_USAGE_PRESENT,
		.index	     = index,
		.generation  = swapchain->generation,
	};
	return 0;
}

static int gfx_swapchain_images_refresh(gfx_swapchain_t *swapchain, u32 count)
{
	if (swapchain == NULL || swapchain->images == NULL || swapchain->image_capacity == 0) {
		return 1;
	}
	if (count == 0) {
		count = 1;
	}
	if (count > swapchain->image_capacity) {
		return 1;
	}

	for (u32 i = 0; i < swapchain->image_capacity; i++) {
		if (i < count) {
			if (gfx_swapchain_image_set(swapchain, i)) {
				return 1; // LCOV_EXCL_LINE
			}
		} else {
			swapchain->images[i] = (gfx_image_t){0};
		}
	}
	swapchain->image_count	  = count;
	swapchain->acquired_index = 0;
	swapchain->acquired	  = 0;
	return 0;
}

static int gfx_present_mode_valid(gfx_present_mode_t present_mode)
{
	return present_mode == GFX_PRESENT_MODE_DEFAULT || present_mode == GFX_PRESENT_MODE_VSYNC ||
	       present_mode == GFX_PRESENT_MODE_IMMEDIATE || present_mode == GFX_PRESENT_MODE_MAILBOX;
}

static gfx_present_mode_t gfx_swapchain_default_actual_present_mode(const gfx_swapchain_t *swapchain, gfx_present_mode_t present_mode)
{
	if (swapchain != NULL && swapchain->surface != NULL &&
	    (swapchain->surface->api == GFX_API_NONE || swapchain->surface->api == GFX_API_SOFTWARE)) {
		return GFX_PRESENT_MODE_IMMEDIATE;
	}
	return present_mode;
}

gfx_swapchain_t *gfx_swapchain_init(gfx_swapchain_t *swapchain, gfx_t *gfx, const gfx_swapchain_config_t *config)
{
	if (swapchain == NULL || gfx == NULL || gfx->drv == NULL || config == NULL || config->format == GFX_FORMAT_NONE ||
	    config->surface == NULL || config->surface->api != gfx->drv->api || config->width == 0 || config->height == 0 ||
	    config->images == NULL || config->image_capacity == 0 || config->min_image_count == 0 ||
	    config->min_image_count > config->image_capacity || !gfx_present_mode_valid(config->present_mode) ||
	    (config->max_image_count != 0 &&
	     (config->max_image_count < config->min_image_count || config->max_image_count > config->image_capacity))) {
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
		.min_image_count     = config->min_image_count,
		.max_image_count     = config->max_image_count,
		.usage		     = config->usage != 0 ? config->usage : GFX_IMAGE_USAGE_COLOR_ATTACHMENT,
		.present_mode	     = config->present_mode,
		.actual_present_mode = config->present_mode,
		.images		     = config->images,
		.image_capacity	     = config->image_capacity,
	};
	if (gfx->drv->swapchain_init != NULL && gfx->drv->swapchain_init(swapchain, config)) {
		if (gfx->drv->swapchain_free != NULL) {
			gfx->drv->swapchain_free(swapchain);
		}
		*swapchain = (gfx_swapchain_t){0};
		return NULL;
	}
	swapchain->generation = 1;
	if (gfx_swapchain_images_refresh(swapchain, swapchain->image_count != 0 ? swapchain->image_count : config->min_image_count)) {
		if (gfx->drv->swapchain_free != NULL) {
			gfx->drv->swapchain_free(swapchain);
		}
		*swapchain = (gfx_swapchain_t){0};
		return NULL;
	}
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
	if (swapchain->images != NULL) {
		for (u32 i = 0; i < swapchain->image_capacity; i++) {
			swapchain->images[i] = (gfx_image_t){0};
		}
	}
	*swapchain = (gfx_swapchain_t){0};
}

int gfx_swapchain_info(const gfx_swapchain_t *swapchain, gfx_swapchain_info_t *info)
{
	if (swapchain == NULL || swapchain->gfx == NULL || info == NULL) {
		return 1;
	}

	*info = (gfx_swapchain_info_t){
		.format		     = swapchain->format,
		.width		     = swapchain->width,
		.height		     = swapchain->height,
		.image_count	     = swapchain->image_count,
		.present_mode	     = swapchain->present_mode,
		.actual_present_mode = swapchain->actual_present_mode,
		.generation	     = swapchain->generation,
	};
	return 0;
}

int gfx_swapchain_images(gfx_swapchain_t *swapchain, gfx_image_t **images, u32 capacity, u32 *count)
{
	if (swapchain == NULL || swapchain->gfx == NULL || count == NULL || (capacity != 0 && images == NULL)) {
		return 1;
	}

	*count = swapchain->image_count;
	if (capacity > swapchain->image_count) {
		capacity = swapchain->image_count;
	}
	for (u32 i = 0; i < capacity; i++) {
		images[i] = &swapchain->images[i];
	}
	return 0;
}

int gfx_swapchain_acquire(gfx_swapchain_t *swapchain, gfx_swapchain_image_t *image)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->drv == NULL || swapchain->gfx->frame != NULL ||
	    swapchain->surface == NULL || swapchain->width == 0 || swapchain->height == 0 || image == NULL) {
		return 1;
	}
	if (swapchain->gfx->drv->swapchain_refresh != NULL && swapchain->gfx->drv->swapchain_refresh(swapchain)) {
		return 1;
	}
	if (swapchain->image_count == 0) {
		if (gfx_swapchain_images_refresh(swapchain, 1)) {
			return 1;
		}
	}
	if (swapchain->gfx->drv->swapchain_acquire != NULL) {
		if (swapchain->gfx->drv->swapchain_acquire(swapchain, image)) {
			return 1;
		}
		if (image->index >= swapchain->image_count || image->image == NULL) {
			return 1;
		}
		swapchain->acquired_index = image->index;
		swapchain->acquired	  = 1;
		return 0;
	}

	swapchain->acquired_index = 0;
	swapchain->acquired	  = 1;
	if (gfx_swapchain_image_set(swapchain, 0)) {
		return 1;
	}
	*image = (gfx_swapchain_image_t){
		.image	    = &swapchain->images[0],
		.index	    = 0,
		.generation = swapchain->images[0].generation,
	};
	return 0;
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
	if (gfx_swapchain_images_refresh(swapchain, swapchain->image_count)) {
		return 1;
	}
	return 0;
}

int gfx_swapchain_set_present_mode(gfx_swapchain_t *swapchain, gfx_present_mode_t present_mode)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->frame != NULL || swapchain->surface == NULL ||
	    !gfx_present_mode_valid(present_mode)) {
		return 1;
	}
	if (swapchain->present_mode == present_mode) {
		return 0;
	}

	gfx_present_mode_t old_present_mode	   = swapchain->present_mode;
	gfx_present_mode_t old_actual_present_mode = swapchain->actual_present_mode;
	gfx_format_t old_format			   = swapchain->format;
	u16 old_width				   = swapchain->width;
	u16 old_height				   = swapchain->height;

	gfx_present_mode_t actual = gfx_swapchain_default_actual_present_mode(swapchain, present_mode);
	if (swapchain->surface->ops != NULL && swapchain->surface->ops->present_mode != NULL &&
	    swapchain->surface->ops->present_mode(swapchain->surface, present_mode, &actual)) {
		return 1;
	}

	swapchain->present_mode	       = present_mode;
	swapchain->actual_present_mode = actual;
	if ((swapchain->surface->ops == NULL || swapchain->surface->ops->present_mode == NULL) && swapchain->gfx->drv != NULL &&
	    swapchain->gfx->drv->swapchain_resize != NULL) {
		if (swapchain->gfx->drv->swapchain_resize(swapchain, swapchain->width, swapchain->height)) {
			swapchain->present_mode	       = old_present_mode;
			swapchain->actual_present_mode = old_actual_present_mode;
			swapchain->format	       = old_format;
			swapchain->width	       = old_width;
			swapchain->height	       = old_height;
			return 1;
		}
		swapchain->generation++;
		if (gfx_swapchain_images_refresh(swapchain, swapchain->image_count)) {
			return 1;
		}
	}
	swapchain->acquired = 0;
	return 0;
}

int gfx_swapchain_present(gfx_swapchain_t *swapchain, const gfx_swapchain_image_t *image)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->drv == NULL || swapchain->gfx->drv->swapchain_present == NULL ||
	    swapchain->gfx->frame != NULL || swapchain->surface == NULL || swapchain->width == 0 || swapchain->height == 0 ||
	    image == NULL || image->image == NULL) {
		return 1;
	}
	if (image->image->swapchain != swapchain || image->index >= swapchain->image_count ||
	    image->generation != swapchain->images[image->index].generation || !swapchain->acquired ||
	    swapchain->acquired_index != image->index) {
		return 1;
	}

	u32 generation = swapchain->generation;
	int ret	       = swapchain->gfx->drv->swapchain_present(swapchain);
	if (ret == 0) {
		if (swapchain->generation != generation) {
			if (gfx_swapchain_images_refresh(swapchain, swapchain->image_count)) {
				return 1;
			}
		}
		swapchain->acquired = 0;
	}
	return ret;
}
