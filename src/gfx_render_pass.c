#include "gfx_driver.h"

static int gfx_render_pass_config_valid(const gfx_render_pass_config_t *config)
{
	return config != NULL && config->color_format != GFX_FORMAT_NONE && config->load <= GFX_LOAD_LOAD &&
	       config->store <= GFX_STORE_STORE;
}

gfx_render_pass_t *gfx_render_pass_init(gfx_render_pass_t *render_pass, gfx_t *gfx, const gfx_render_pass_config_t *config)
{
	if (render_pass == NULL || gfx == NULL || gfx->drv == NULL || !gfx_render_pass_config_valid(config)) {
		return NULL;
	}
	if (gfx->frame != NULL) {
		return NULL;
	}

	*render_pass = (gfx_render_pass_t){
		.gfx	      = gfx,
		.color_format = config->color_format,
		.load	      = config->load,
		.store	      = config->store,
	};
	if (gfx->drv->render_pass_init != NULL && gfx->drv->render_pass_init(render_pass, config)) {
		if (gfx->drv->render_pass_free != NULL) {
			gfx->drv->render_pass_free(render_pass);
		}
		*render_pass = (gfx_render_pass_t){0};
		return NULL;
	}
	return render_pass;
}

void gfx_render_pass_free(gfx_render_pass_t *render_pass)
{
	if (render_pass == NULL || render_pass->gfx == NULL || render_pass->gfx->frame != NULL) {
		return;
	}

	if (render_pass->gfx->drv != NULL && render_pass->gfx->drv->render_pass_free != NULL) {
		render_pass->gfx->drv->render_pass_free(render_pass);
	}
	*render_pass = (gfx_render_pass_t){0};
}
