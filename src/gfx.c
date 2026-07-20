#include "gfx.h"

#include "gfx_driver.h"

gfx_t *gfx_init(gfx_t *gfx, const struct gfx_driver_s *drv, const gfx_config_t *config)
{
	if (gfx == NULL || drv == NULL || config == NULL || drv->init == NULL) {
		return NULL;
	}

	gfx->drv = drv;
	if (gfx->drv->init(gfx, config)) {
		gfx->drv  = NULL;
		gfx->data = NULL;
		return NULL;
	}

	return gfx;
}

void gfx_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->drv == NULL) {
		return;
	}

	if (gfx->drv->free != NULL) {
		gfx->drv->free(gfx);
	}
	gfx->drv  = NULL;
	gfx->data = NULL;
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

int gfx_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->set_target == NULL || target == NULL) {
		return 1;
	}

	return gfx->drv->set_target(gfx, target);
}

int gfx_viewport(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->viewport == NULL || width == 0 || height == 0) {
		return 1;
	}

	return gfx->drv->viewport(gfx, x, y, width, height);
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

	return gfx->drv->clear(gfx, buffers);
}

int gfx_present(gfx_t *gfx)
{
	if (gfx == NULL || gfx->drv == NULL || gfx->drv->present == NULL) {
		return 1;
	}

	return gfx->drv->present(gfx);
}
