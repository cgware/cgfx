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

static int gfx_framebuffer_valid(const gfx_framebuffer_t *framebuffer)
{
	return framebuffer != NULL && framebuffer->gfx != NULL && framebuffer->target != NULL &&
	       framebuffer->target->gfx == framebuffer->gfx && framebuffer->render_pass != NULL &&
	       framebuffer->render_pass->gfx == framebuffer->gfx && framebuffer->render_pass->color_format == framebuffer->target->format &&
	       framebuffer->target->width == framebuffer->width && framebuffer->target->height == framebuffer->height &&
	       framebuffer->target_generation == framebuffer->target->swapchain_generation && framebuffer->width != 0 &&
	       framebuffer->height != 0;
}

static int gfx_framebuffer_base_valid(const gfx_framebuffer_t *framebuffer)
{
	return framebuffer != NULL && framebuffer->gfx != NULL && framebuffer->target != NULL &&
	       framebuffer->target->gfx == framebuffer->gfx && framebuffer->render_pass != NULL &&
	       framebuffer->render_pass->gfx == framebuffer->gfx && framebuffer->width != 0 && framebuffer->height != 0;
}

static int gfx_target_swapchain_stale(const gfx_target_t *target)
{
	return target != NULL && target->type == GFX_TARGET_SWAPCHAIN && target->swapchain != NULL &&
	       (target->format != target->swapchain->format || target->width != target->swapchain->width ||
		target->height != target->swapchain->height || target->swapchain_generation != target->swapchain->generation);
}

static int gfx_framebuffer_stale(const gfx_framebuffer_t *framebuffer)
{
	return framebuffer != NULL && framebuffer->target != NULL &&
	       (gfx_target_swapchain_stale(framebuffer->target) || framebuffer->width != framebuffer->target->width ||
		framebuffer->height != framebuffer->target->height ||
		framebuffer->target_generation != framebuffer->target->swapchain_generation);
}

static int gfx_pass_valid(const gfx_pass_config_t *pass, const gfx_framebuffer_t *framebuffer)
{
	if (pass == NULL || !gfx_framebuffer_valid(framebuffer)) {
		return 0;
	}
	return 1;
}

gfx_framebuffer_t *gfx_framebuffer_init(gfx_framebuffer_t *framebuffer, gfx_target_t *target, const gfx_render_pass_t *render_pass)
{
	if (framebuffer == NULL || target == NULL || target->gfx == NULL || target->gfx->drv == NULL || render_pass == NULL ||
	    render_pass->gfx != target->gfx || render_pass->color_format != target->format || target->width == 0 || target->height == 0) {
		return NULL;
	}
	if (target->gfx->frame != NULL) {
		return NULL;
	}

	*framebuffer = (gfx_framebuffer_t){
		.gfx		   = target->gfx,
		.target		   = target,
		.render_pass	   = render_pass,
		.width		   = target->width,
		.height		   = target->height,
		.target_generation = target->swapchain_generation,
	};
	if (target->gfx->drv->framebuffer_init != NULL && target->gfx->drv->framebuffer_init(framebuffer)) {
		if (target->gfx->drv->framebuffer_free != NULL) {
			target->gfx->drv->framebuffer_free(framebuffer);
		}
		*framebuffer = (gfx_framebuffer_t){0};
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
	    framebuffer->target->type != GFX_TARGET_SWAPCHAIN || framebuffer->target->swapchain == NULL || width == 0 || height == 0) {
		return 1;
	}

	gfx_t *gfx			     = framebuffer->gfx;
	gfx_target_t *target		     = framebuffer->target;
	const gfx_render_pass_t *render_pass = framebuffer->render_pass;
	if (gfx->drv->framebuffer_free != NULL) {
		gfx->drv->framebuffer_free(framebuffer);
	}
	if (gfx_swapchain_resize(target->swapchain, width, height)) {
		*framebuffer = (gfx_framebuffer_t){
			.gfx		   = gfx,
			.target		   = target,
			.render_pass	   = render_pass,
			.width		   = target->width,
			.height		   = target->height,
			.target_generation = target->swapchain_generation,
		};
		if (gfx->drv->framebuffer_init != NULL && gfx->drv->framebuffer_init(framebuffer)) {
			if (gfx->drv->framebuffer_free != NULL) {
				gfx->drv->framebuffer_free(framebuffer);
			}
			*framebuffer = (gfx_framebuffer_t){0};
		}
		return 1;
	}
	if (gfx->drv->target_free != NULL) {
		gfx->drv->target_free(target);
	}
	target->format		     = target->swapchain->format;
	target->width		     = target->swapchain->width;
	target->height		     = target->swapchain->height;
	target->swapchain_generation = target->swapchain->generation;
	if (gfx->drv->target_init != NULL && gfx->drv->target_init(target)) {
		*framebuffer = (gfx_framebuffer_t){0};
		return 1;
	}

	*framebuffer = (gfx_framebuffer_t){
		.gfx		   = gfx,
		.target		   = target,
		.render_pass	   = render_pass,
		.width		   = target->width,
		.height		   = target->height,
		.target_generation = target->swapchain_generation,
	};
	if (gfx->drv->framebuffer_init != NULL && gfx->drv->framebuffer_init(framebuffer)) {
		if (gfx->drv->framebuffer_free != NULL) {
			gfx->drv->framebuffer_free(framebuffer);
		}
		*framebuffer = (gfx_framebuffer_t){0};
		return 1;
	}
	return 0;
}

static int gfx_framebuffer_refresh(gfx_framebuffer_t *framebuffer)
{
	if (!gfx_framebuffer_base_valid(framebuffer) || framebuffer->gfx->frame != NULL) {
		return 1;
	}
	if (framebuffer->target->type != GFX_TARGET_SWAPCHAIN) {
		return 0;
	}
	if (framebuffer->target->swapchain == NULL) {
		return 1;
	}
	if (!gfx_framebuffer_stale(framebuffer)) {
		return 0;
	}

	gfx_t *gfx			     = framebuffer->gfx;
	gfx_target_t *target		     = framebuffer->target;
	const gfx_render_pass_t *render_pass = framebuffer->render_pass;
	if (gfx->drv->framebuffer_free != NULL) {
		gfx->drv->framebuffer_free(framebuffer);
	}
	if (gfx->drv->target_free != NULL) {
		gfx->drv->target_free(target);
	}
	target->format		     = target->swapchain->format;
	target->width		     = target->swapchain->width;
	target->height		     = target->swapchain->height;
	target->swapchain_generation = target->swapchain->generation;
	if (render_pass->color_format != target->format || (gfx->drv->target_init != NULL && gfx->drv->target_init(target))) {
		*framebuffer = (gfx_framebuffer_t){0};
		return 1;
	}

	*framebuffer = (gfx_framebuffer_t){
		.gfx		   = gfx,
		.target		   = target,
		.render_pass	   = render_pass,
		.width		   = target->width,
		.height		   = target->height,
		.target_generation = target->swapchain_generation,
	};
	if (gfx->drv->framebuffer_init != NULL && gfx->drv->framebuffer_init(framebuffer)) {
		if (gfx->drv->framebuffer_free != NULL) {
			gfx->drv->framebuffer_free(framebuffer);
		}
		*framebuffer = (gfx_framebuffer_t){0};
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
	if (framebuffer->target->type == GFX_TARGET_SWAPCHAIN && framebuffer->target->swapchain != NULL &&
	    framebuffer->gfx->drv->swapchain_refresh != NULL && framebuffer->gfx->drv->swapchain_refresh(framebuffer->target->swapchain)) {
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
