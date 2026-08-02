#include "gfx_pipeline.h"

#include "gfx_driver.h"

gfx_pipeline_t *gfx_pipeline_init(gfx_pipeline_t *pipeline, gfx_t *gfx, const gfx_pipeline_config_t *config)
{
	if (pipeline == NULL || gfx == NULL || gfx->drv == NULL || gfx->drv->pipeline_init == NULL || config == NULL ||
	    config->render_pass == NULL || config->render_pass->gfx != gfx) {
		return NULL;
	}

	pipeline->gfx	      = gfx;
	pipeline->render_pass = config->render_pass;
	if (gfx->drv->pipeline_init(pipeline, config)) {
		*pipeline = (gfx_pipeline_t){0};
		return NULL;
	}

	return pipeline;
}

void gfx_pipeline_free(gfx_pipeline_t *pipeline)
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->drv == NULL || pipeline->gfx->drv->pipeline_free == NULL) {
		return;
	}

	pipeline->gfx->drv->pipeline_free(pipeline);
}

int gfx_pipeline_bind(gfx_frame_t *frame, const gfx_pipeline_t *pipeline)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->frame != frame || !frame->active || pipeline == NULL ||
	    pipeline->gfx != frame->gfx || pipeline->render_pass != frame->render_pass || pipeline->gfx->drv == NULL ||
	    pipeline->gfx->drv->pipeline_bind == NULL) {
		return 1;
	}

	if (pipeline->gfx->drv->pipeline_bind(frame, pipeline)) {
		return 1;
	}

	frame->pipeline	     = pipeline;
	frame->vertex_buffer = NULL;
	frame->index_buffer  = NULL;
	return 0;
}
