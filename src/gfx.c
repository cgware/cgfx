#include "gfx_driver.h"

gfx_t *gfx_init(gfx_t *gfx, const struct gfx_driver_s *drv, const gfx_config_t *config, proc_t *proc, alloc_t alloc)
{
	if (gfx == NULL || drv == NULL || config == NULL || drv->init == NULL) {
		return NULL;
	}

	*gfx = (gfx_t){
		.drv   = drv,
		.proc  = proc,
		.alloc = alloc,
	};

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

int gfx_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->frame != frame || frame->gfx->drv == NULL || frame->gfx->drv->draw == NULL ||
	    !frame->active || frame->pipeline == NULL || frame->vertex_buffer == NULL || vertex_count == 0) {
		return 1;
	}

	return frame->gfx->drv->draw(frame, vertex_count, first_vertex);
}

int gfx_draw_indexed(gfx_frame_t *frame, u32 index_count)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->frame != frame || frame->gfx->drv == NULL ||
	    frame->gfx->drv->draw_indexed == NULL || !frame->active || frame->pipeline == NULL || frame->vertex_buffer == NULL ||
	    frame->index_buffer == NULL || index_count == 0) {
		return 1;
	}

	return frame->gfx->drv->draw_indexed(frame, index_count);
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
