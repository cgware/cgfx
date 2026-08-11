#include "gfx_framebuffer.h"

#include "gfx_driver.h"

static int gfx_rect_valid(gfx_rect_t rect)
{
	return rect.width != 0 && rect.height != 0;
}

static gfx_rect_t gfx_rect_default(gfx_rect_t rect, const gfx_framebuffer_t *framebuffer)
{
	if (!gfx_rect_valid(rect) && framebuffer != NULL) {
		rect.width  = framebuffer->width;
		rect.height = framebuffer->height;
	}

	return rect;
}

static int gfx_image_swapchain_stale(const gfx_image_t *image)
{
	return image != NULL && image->origin == GFX_IMAGE_ORIGIN_SURFACE && image->swapchain != NULL &&
	       (image->format != image->swapchain->format || image->width != image->swapchain->width ||
		image->height != image->swapchain->height || image->generation != image->swapchain->generation ||
		image->index >= image->swapchain->image_count);
}

static int gfx_framebuffer_valid(const gfx_framebuffer_t *framebuffer)
{
	return framebuffer != NULL && framebuffer->gfx != NULL && framebuffer->image != NULL &&
	       framebuffer->image->gfx == framebuffer->gfx && framebuffer->render_pass != NULL &&
	       framebuffer->render_pass->gfx == framebuffer->gfx && framebuffer->render_pass->color_format == framebuffer->image->format &&
	       framebuffer->image->width == framebuffer->width && framebuffer->image->height == framebuffer->height &&
	       framebuffer->image_generation == framebuffer->image->generation && framebuffer->width != 0 && framebuffer->height != 0;
}

static int gfx_framebuffer_base_valid(const gfx_framebuffer_t *framebuffer)
{
	return framebuffer != NULL && framebuffer->gfx != NULL && framebuffer->image != NULL &&
	       framebuffer->image->gfx == framebuffer->gfx && framebuffer->render_pass != NULL &&
	       framebuffer->render_pass->gfx == framebuffer->gfx && framebuffer->width != 0 && framebuffer->height != 0;
}

static int gfx_framebuffer_stale(const gfx_framebuffer_t *framebuffer)
{
	return framebuffer != NULL && framebuffer->image != NULL &&
	       (gfx_image_swapchain_stale(framebuffer->image) || framebuffer->width != framebuffer->image->width ||
		framebuffer->height != framebuffer->image->height || framebuffer->image_generation != framebuffer->image->generation);
}

static int gfx_pass_valid(const gfx_pass_config_t *pass, const gfx_framebuffer_t *framebuffer)
{
	if (pass == NULL || !gfx_framebuffer_valid(framebuffer)) {
		return 0;
	}
	if (framebuffer->render_pass->depth_format != GFX_FORMAT_NONE && (pass->clear_depth < 0.0f || pass->clear_depth > 1.0f)) {
		return 0;
	}
	return 1;
}

static gfx_framebuffer_t gfx_framebuffer_desc(gfx_image_t *image, const gfx_render_pass_t *render_pass)
{
	return (gfx_framebuffer_t){
		.gfx		  = image->gfx,
		.image		  = image,
		.render_pass	  = render_pass,
		.width		  = image->width,
		.height		  = image->height,
		.image_generation = image->generation,
	};
}

static int gfx_framebuffer_driver_init(gfx_framebuffer_t *framebuffer)
{
	if (framebuffer->gfx->drv->framebuffer_init != NULL && framebuffer->gfx->drv->framebuffer_init(framebuffer)) {
		if (framebuffer->gfx->drv->framebuffer_free != NULL) {
			framebuffer->gfx->drv->framebuffer_free(framebuffer);
		}
		*framebuffer = (gfx_framebuffer_t){0};
		return 1;
	}
	return 0;
}

gfx_framebuffer_t *gfx_framebuffer_init(gfx_framebuffer_t *framebuffer, gfx_image_t *image, const gfx_render_pass_t *render_pass)
{
	if (framebuffer == NULL || image == NULL || image->gfx == NULL || image->gfx->drv == NULL || render_pass == NULL ||
	    render_pass->gfx != image->gfx || render_pass->color_format != image->format || image->width == 0 || image->height == 0) {
		return NULL;
	}
	if (image->gfx->frame != NULL) {
		return NULL;
	}

	*framebuffer = gfx_framebuffer_desc(image, render_pass);
	if (gfx_framebuffer_driver_init(framebuffer)) {
		return NULL;
	}
	return framebuffer;
}

void gfx_framebuffer_free(gfx_framebuffer_t *framebuffer)
{
	if (framebuffer == NULL || framebuffer->gfx == NULL || framebuffer->gfx->frame != NULL) {
		return;
	}

	if (framebuffer->gfx->drv != NULL && framebuffer->gfx->drv->framebuffer_free != NULL) {
		framebuffer->gfx->drv->framebuffer_free(framebuffer);
	}
	*framebuffer = (gfx_framebuffer_t){0};
}

int gfx_framebuffer_resize(gfx_framebuffer_t *framebuffer, u16 width, u16 height)
{
	if (!gfx_framebuffer_base_valid(framebuffer) || framebuffer->gfx->frame != NULL ||
	    framebuffer->image->origin != GFX_IMAGE_ORIGIN_SURFACE || framebuffer->image->swapchain == NULL || width == 0 || height == 0) {
		return 1;
	}

	gfx_t *gfx			     = framebuffer->gfx;
	gfx_image_t *image		     = framebuffer->image;
	const gfx_render_pass_t *render_pass = framebuffer->render_pass;
	if (gfx->drv->framebuffer_free != NULL) {
		gfx->drv->framebuffer_free(framebuffer);
	}
	if (gfx_swapchain_resize(image->swapchain, width, height)) {
		*framebuffer = gfx_framebuffer_desc(image, render_pass);
		if (gfx_framebuffer_driver_init(framebuffer)) {
			*framebuffer = (gfx_framebuffer_t){0};
		}
		return 1;
	}
	if (render_pass->color_format != image->format) {
		*framebuffer = (gfx_framebuffer_t){0};
		return 1;
	}

	*framebuffer = gfx_framebuffer_desc(image, render_pass);
	if (gfx_framebuffer_driver_init(framebuffer)) {
		return 1;
	}
	return 0;
}

static int gfx_framebuffer_refresh(gfx_framebuffer_t *framebuffer)
{
	if (!gfx_framebuffer_base_valid(framebuffer) || framebuffer->gfx->frame != NULL) {
		return 1;
	}
	if (framebuffer->image->origin != GFX_IMAGE_ORIGIN_SURFACE) {
		return 0;
	}
	if (framebuffer->image->swapchain == NULL) {
		return 1;
	}
	if (!gfx_framebuffer_stale(framebuffer)) {
		return 0;
	}

	gfx_t *gfx			     = framebuffer->gfx;
	gfx_image_t *image		     = framebuffer->image;
	const gfx_render_pass_t *render_pass = framebuffer->render_pass;
	if (gfx->drv->framebuffer_free != NULL) {
		gfx->drv->framebuffer_free(framebuffer);
	}
	if (render_pass->color_format != image->format) {
		*framebuffer = (gfx_framebuffer_t){0};
		return 1;
	}

	*framebuffer = gfx_framebuffer_desc(image, render_pass);
	if (gfx_framebuffer_driver_init(framebuffer)) {
		return 1;
	}
	return 0;
}

int gfx_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame, const gfx_pass_config_t *config)
{
	if (!gfx_framebuffer_base_valid(framebuffer) || framebuffer->gfx->drv == NULL ||
	    framebuffer->gfx->drv->framebuffer_pass_begin == NULL || frame == NULL || frame->active || framebuffer->gfx->frame != NULL ||
	    config == NULL) {
		return 1;
	}
	if (framebuffer->image->origin == GFX_IMAGE_ORIGIN_SURFACE && framebuffer->image->swapchain != NULL &&
	    framebuffer->gfx->drv->swapchain_refresh != NULL && framebuffer->gfx->drv->swapchain_refresh(framebuffer->image->swapchain)) {
		return 1;
	}
	if (gfx_framebuffer_refresh(framebuffer) || !gfx_pass_valid(config, framebuffer)) {
		return 1;
	}

	for (u32 attempt = 0; attempt < 2; attempt++) {
		*frame = (gfx_frame_t){.gfx = framebuffer->gfx, .render_pass = framebuffer->render_pass, .pass = *config, .active = 1};
		frame->pass.viewport	= gfx_rect_default(frame->pass.viewport, framebuffer);
		framebuffer->gfx->frame = frame;
		if (!framebuffer->gfx->drv->framebuffer_pass_begin(framebuffer, frame)) {
			return 0;
		}
		int retry		= attempt == 0 && gfx_framebuffer_stale(framebuffer);
		framebuffer->gfx->frame = NULL;
		*frame			= (gfx_frame_t){0};
		if (!retry || gfx_framebuffer_refresh(framebuffer)) {
			return 1;
		}
	}
	return 1; // LCOV_EXCL_LINE
}
