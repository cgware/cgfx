#include "gfx.h"

#include "gfx_driver.h"

gfx_t *gfx_init(gfx_t *gfx, const struct gfx_driver_s *drv, const gfx_config_t *config, proc_t *proc, alloc_t alloc)
{
	if (gfx == NULL || drv == NULL || config == NULL || drv->init == NULL) {
		return NULL;
	}

	gfx->drv   = drv;
	gfx->proc  = proc;
	gfx->alloc = alloc;

	if (gfx->drv->init(gfx, config)) {
		if (gfx->drv->free != NULL) {
			gfx->drv->free(gfx);
		}
		*gfx = (gfx_t){0};
		return NULL;
	}

	return gfx;
}

void gfx_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->free == NULL) {
		return;
	}

	gfx->drv->free(gfx);
	*gfx = (gfx_t){0};
}

int gfx_proc(gfx_t *gfx, strv_t name, void **proc)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->proc == NULL || proc == NULL) {
		return 1;
	}

	return gfx->drv->proc(gfx, name, proc);
}

int gfx_native(gfx_t *gfx, gfx_native_t *native)
{
	if (gfx == NULL || gfx->drv == NULL || native == NULL) {
		return 1;
	}

	*native = (gfx_native_t){
		.api = gfx->drv->api,
	};
	if (gfx->drv->native != NULL) {
		return gfx->drv->native(gfx, native);
	}

	return 0;
}

static int gfx_set_target_apply(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->set_target == NULL || target == NULL) {
		return 1;
	}

	return gfx->drv->set_target(gfx, target);
}

int gfx_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || gfx->frame != NULL) {
		return 1;
	}

	return gfx_set_target_apply(gfx, target);
}

int gfx_viewport(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->viewport == NULL || width == 0 || height == 0) {
		return 1;
	}

	return gfx->drv->viewport(gfx, x, y, width, height);
}

int gfx_begin(gfx_t *gfx, gfx_frame_t *frame, const gfx_frame_config_t *config)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->begin == NULL || frame == NULL || frame->active || gfx->frame != NULL) {
		return 1;
	}

	*frame = (gfx_frame_t){
		.gfx	= gfx,
		.target = config != NULL ? config->target : NULL,
		.active = 1,
	};
	if (frame->target != NULL && gfx_set_target_apply(gfx, frame->target)) {
		*frame = (gfx_frame_t){0};
		return 1;
	}
	gfx->frame = frame;
	if (gfx->drv->begin(frame)) {
		gfx->frame = NULL;
		*frame	   = (gfx_frame_t){0};
		return 1;
	}

	return 0;
}

int gfx_clear_color(gfx_t *gfx, float r, float g, float b, float a)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->clear_color == NULL) {
		return 1;
	}

	return gfx->drv->clear_color(gfx, r, g, b, a);
}

int gfx_clear(gfx_t *gfx, u32 buffers)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->clear == NULL) {
		return 1;
	}
	if (gfx->frame != NULL) {
		return 1;
	}

	return gfx->drv->clear(gfx, buffers);
}

int gfx_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->frame != frame || frame->gfx->drv == NULL || frame->gfx->drv->draw == NULL ||
	    !frame->active || frame->pipeline == NULL || frame->vertex_buffer == NULL || vertex_count == 0) {
		return 1;
	}

	return frame->gfx->drv->draw(frame, vertex_count, first_vertex);
}

int gfx_end(gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->frame != frame || frame->gfx->drv == NULL || frame->gfx->drv->end == NULL ||
	    !frame->active) {
		return 1;
	}

	int ret		  = frame->gfx->drv->end(frame);
	frame->gfx->frame = NULL;
	*frame		  = (gfx_frame_t){0};
	return ret;
}

int gfx_present(gfx_t *gfx)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->present == NULL) {
		return 1;
	}
	if (gfx->frame != NULL) {
		return 1;
	}

	return gfx->drv->present(gfx);
}
