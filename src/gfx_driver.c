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
