#include "gfx_shader.h"

#include "gfx_driver.h"
#include "log.h"

gfx_shader_t *gfx_shader_init(gfx_shader_t *shader, gfx_t *gfx, const gfx_shader_config_t *config)
{
	if (shader == NULL || gfx == NULL || gfx->drv == NULL || gfx->drv->shader_init == NULL) {
		return NULL;
	}

	shader->gfx = gfx;
	if (gfx->drv->shader_init(shader, config)) {
		log_error("cgfx", "gfx", NULL, "failed to initialize shader with driver: %s", gfx->drv->name);
		return NULL;
	}

	return shader;
}

void gfx_shader_free(gfx_shader_t *shader)
{
	if (shader == NULL || shader->gfx == NULL || shader->gfx->drv == NULL || shader->gfx->drv->shader_free) {
		return;
	}

	shader->gfx->drv->shader_free(shader);
}

int gfx_draw_triangle_2d(const gfx_shader_t *shader, const gfx_vertex_2d_t vertices[3])
{
	if (shader == NULL || shader->gfx == NULL || shader->gfx->drv == NULL || shader->gfx->drv->draw_triangle_2d == NULL) {
		return 1;
	}

	return shader->gfx->drv->draw_triangle_2d(shader, vertices);
}
