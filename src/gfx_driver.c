#include "gfx_driver.h"

gfx_driver_t *gfx_driver_find(strv_t name)
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != GFX_DRIVER_TYPE) {
			continue;
		}

		gfx_driver_t *drv = i->data;
		if (drv != NULL && strv_eq(strv_cstr(drv->name), name)) {
			return drv;
		}
	}

	return NULL;
}

gfx_driver_t *gfx_driver_next(const gfx_driver_t *driver)
{
	gfx_api_t current = driver == NULL ? GFX_API_NONE : driver->api;
	if (current < GFX_API_NONE || current > GFX_API_D3D11) {
		current = GFX_API_NONE;
	}

	gfx_driver_t *first = NULL;
	gfx_driver_t *next  = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != GFX_DRIVER_TYPE) {
			continue;
		}

		gfx_driver_t *drv = i->data;
		if (drv == NULL || drv->api == GFX_API_NONE) {
			continue;
		}

		if (first == NULL || drv->api < first->api) {
			first = drv;
		}
		if (drv->api > current && (next == NULL || drv->api < next->api)) {
			next = drv;
		}
	}

	return next != NULL ? next : first;
}

u32 gfx_driver_list(gfx_driver_t **drivers, u32 capacity)
{
	u32 count = 0;
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != GFX_DRIVER_TYPE) {
			continue;
		}

		gfx_driver_t *drv = i->data;
		if (drivers != NULL && count < capacity) {
			drivers[count] = drv;
		}
		count++;
	}

	return count;
}
