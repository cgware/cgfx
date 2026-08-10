#include "gfx_driver.h"

gfx_image_t *gfx_image_init_memory(gfx_image_t *image, gfx_t *gfx, const gfx_image_memory_config_t *config)
{
	if (image == NULL || gfx == NULL || gfx->drv == NULL || config == NULL || config->format == GFX_FORMAT_NONE ||
	    config->data == NULL || config->width == 0 || config->height == 0 || config->stride < (size_t)config->width * 4 ||
	    gfx->frame != NULL) {
		return NULL;
	}

	*image = (gfx_image_t){
		.gfx	    = gfx,
		.origin	    = GFX_IMAGE_ORIGIN_MEMORY,
		.format	    = config->format,
		.data	    = config->data,
		.width	    = config->width,
		.height	    = config->height,
		.stride	    = config->stride,
		.usage	    = config->usage,
		.generation = 1,
	};
	if (gfx->drv->image_init != NULL && gfx->drv->image_init(image)) {
		if (gfx->drv->image_free != NULL) {
			gfx->drv->image_free(image);
		}
		*image = (gfx_image_t){0};
		return NULL;
	}
	return image;
}

void gfx_image_free(gfx_image_t *image)
{
	if (image == NULL || image->gfx == NULL || image->gfx->frame != NULL) {
		return;
	}

	if (image->gfx->drv != NULL && image->gfx->drv->image_free != NULL) {
		image->gfx->drv->image_free(image);
	}
	*image = (gfx_image_t){0};
}

int gfx_image_read(gfx_image_t *image, const gfx_memory_readback_config_t *config)
{
	if (image == NULL || image->gfx == NULL || image->gfx->drv == NULL || image->gfx->drv->image_read == NULL ||
	    image->gfx->frame != NULL || image->origin != GFX_IMAGE_ORIGIN_MEMORY || image->format == GFX_FORMAT_NONE ||
	    image->width == 0 || image->height == 0 || config == NULL || config->data == NULL ||
	    config->stride < (size_t)image->width * 4) {
		return 1;
	}

	return image->gfx->drv->image_read(image, config);
}
