#include "gfx_driver.h"

#include "log.h"
#include "opengl.h"

typedef struct gfx_opengl_s {
	void *gl_lib;
	const gfx_image_t *image;
	const gfx_framebuffer_t *framebuffer;
	gfx_surface_t *surface;
	PFN_glClearColor ClearColor;
	PFN_glClear Clear;
	PFN_glGenFramebuffers GenFramebuffers;
	PFN_glDeleteFramebuffers DeleteFramebuffers;
	PFN_glBindFramebuffer BindFramebuffer;
	PFN_glCheckFramebufferStatus CheckFramebufferStatus;
	PFN_glFramebufferTexture2D FramebufferTexture2D;
	PFN_glGenTextures GenTextures;
	PFN_glDeleteTextures DeleteTextures;
	PFN_glBindTexture BindTexture;
	PFN_glTexParameteri TexParameteri;
	PFN_glTexImage2D TexImage2D;
	PFN_glViewport Viewport;
	PFN_glReadPixels ReadPixels;
	PFN_glGetError GetError;
	PFN_glGetString GetString;
	PFN_glCreateShader CreateShader;
	PFN_glShaderSource ShaderSource;
	PFN_glCompileShader CompileShader;
	PFN_glGetShaderiv GetShaderiv;
	PFN_glGetShaderInfoLog GetShaderInfoLog;
	PFN_glDeleteShader DeleteShader;
	PFN_glCreateProgram CreateProgram;
	PFN_glAttachShader AttachShader;
	PFN_glLinkProgram LinkProgram;
	PFN_glGetProgramiv GetProgramiv;
	PFN_glGetProgramInfoLog GetProgramInfoLog;
	PFN_glDeleteProgram DeleteProgram;
	PFN_glGetUniformBlockIndex GetUniformBlockIndex;
	PFN_glUniformBlockBinding UniformBlockBinding;
	PFN_glGenBuffers GenBuffers;
	PFN_glDeleteBuffers DeleteBuffers;
	PFN_glBindBuffer BindBuffer;
	PFN_glBindBufferBase BindBufferBase;
	PFN_glBufferData BufferData;
	PFN_glUseProgram UseProgram;
	PFN_glEnableVertexAttribArray EnableVertexAttribArray;
	PFN_glDisableVertexAttribArray DisableVertexAttribArray;
	PFN_glVertexAttribPointer VertexAttribPointer;
	PFN_glDrawArrays DrawArrays;
	PFN_glDrawElements DrawElements;
	PFN_glClearDepth ClearDepth;
	PFN_glEnable Enable;
	PFN_glDisable Disable;
	PFN_glDepthFunc DepthFunc;
	PFN_glFrontFace FrontFace;
	PFN_glCullFace CullFace;
} gfx_opengl_t;

typedef struct gfx_opengl_render_pass_s {
	gfx_format_t color_format;
	gfx_load_op_t load;
	gfx_store_op_t store;
} gfx_opengl_render_pass_t;

typedef struct gfx_opengl_target_s {
	unsigned int texture;
} gfx_opengl_target_t;

typedef struct gfx_opengl_framebuffer_s {
	unsigned int framebuffer;
	unsigned int depth_texture;
} gfx_opengl_framebuffer_t;

typedef struct gfx_opengl_buffer_s {
	unsigned int buffer;
	unsigned int target;
} gfx_opengl_buffer_t;

typedef struct gfx_opengl_uniform_block_s {
	u32 slot;
	char name[64];
} gfx_opengl_uniform_block_t;

typedef struct gfx_opengl_shader_s {
	unsigned int shader;
	gfx_opengl_uniform_block_t uniform_blocks[16];
	u32 uniform_block_count;
} gfx_opengl_shader_t;

typedef struct gfx_opengl_pipeline_s {
	unsigned int program;
	const gfx_layout_t *input_layout;
	size_t input_layout_size;
	int stride;
} gfx_opengl_pipeline_t;

static int gfx_opengl_make_current(gfx_opengl_t *opengl, const char *operation);
static int _gfx_opengl_begin(gfx_t *gfx, const char *operation, gfx_opengl_t **out);
static int gfx_opengl_bind_framebuffer(gfx_opengl_t *opengl);

static int find_gl_symbol(gfx_t *gfx, gfx_surface_t *surface, void **sym, strv_t name)
{
	gfx_opengl_t *opengl = gfx->data;
	if (opengl->gl_lib != NULL && proc_dlsym(gfx->proc, opengl->gl_lib, name, sym) == 0) {
		return 0;
	}
	if (surface != NULL && surface->ops != NULL && surface->ops->proc != NULL) {
		if (surface->ops->proc(surface, name, sym) == 0) {
			return 0;
		}
	}
	return 1;
}

static int load_symbol(gfx_t *gfx, gfx_surface_t *surface, void **sym, strv_t name)
{
	if (find_gl_symbol(gfx, surface, sym, name) == 0) {
		return 0;
	}

	log_error("cgfx", "gfx_opengl", NULL, "failed to load OpenGL symbol: %.*s", name.len, name.data);
	return 1;
}

#define LOAD_GL(_gfx, _opengl, _surface, _name)		 load_symbol((_gfx), (_surface), (void **)&(_opengl)->_name, STRV("gl" #_name))
#define LOAD_GL_OPTIONAL(_gfx, _opengl, _surface, _name) find_gl_symbol((_gfx), (_surface), (void **)&(_opengl)->_name, STRV("gl" #_name))

static int gfx_opengl_clear_surface(gfx_surface_t *surface)
{
	if (surface == NULL) {
		return 0;
	}
	if (surface->ops == NULL || surface->ops->clear_current == NULL) {
		return 1;
	}
	return surface->ops->clear_current(surface);
}

static int gfx_opengl_init_free(gfx_t *gfx, gfx_opengl_t *opengl, gfx_surface_t *surface)
{
	gfx_opengl_clear_surface(surface);
	if (opengl->gl_lib != NULL) {
		proc_dlclose(gfx->proc, opengl->gl_lib);
	}
	alloc_free(&gfx->alloc, opengl, sizeof(gfx_opengl_t));
	gfx->data = NULL;
	return 1;
}

static int gfx_opengl_load_gl(gfx_t *gfx)
{
	gfx_opengl_t *opengl = gfx->data;
	if (proc_dlopen(gfx->proc, STRV("opengl32.dll"), &opengl->gl_lib) == 0) {
		return 0;
	}
	if (proc_dlopen(gfx->proc, STRV("libOpenGL.so.0"), &opengl->gl_lib) == 0) {
		return 0;
	}
	if (proc_dlopen(gfx->proc, STRV("libGL.so.1"), &opengl->gl_lib) == 0) {
		return 0;
	}
	if (proc_dlopen(gfx->proc, STRV("libGL.so"), &opengl->gl_lib) == 0) {
		return 0;
	}

	log_error("cgfx", "gfx_opengl", NULL, "failed to load OpenGL library");
	return 1;
}

static int gfx_opengl_surface_valid(gfx_surface_t *surface)
{
	return surface != NULL && surface->api == GFX_API_OPENGL && surface->ops != NULL && surface->ops->proc != NULL &&
	       surface->ops->make_current != NULL;
}

static int gfx_opengl_load_symbols(gfx_t *gfx, gfx_surface_t *surface)
{
	if (gfx == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_opengl_t *opengl = gfx->data;

	if (LOAD_GL(gfx, opengl, surface, ClearColor) || LOAD_GL(gfx, opengl, surface, Clear) ||
	    LOAD_GL(gfx, opengl, surface, GenFramebuffers) || LOAD_GL(gfx, opengl, surface, DeleteFramebuffers) ||
	    LOAD_GL(gfx, opengl, surface, BindFramebuffer) || LOAD_GL(gfx, opengl, surface, CheckFramebufferStatus) ||
	    LOAD_GL(gfx, opengl, surface, FramebufferTexture2D) || LOAD_GL(gfx, opengl, surface, GenTextures) ||
	    LOAD_GL(gfx, opengl, surface, DeleteTextures) || LOAD_GL(gfx, opengl, surface, BindTexture) ||
	    LOAD_GL(gfx, opengl, surface, TexParameteri) || LOAD_GL(gfx, opengl, surface, TexImage2D) ||
	    LOAD_GL(gfx, opengl, surface, Viewport) || LOAD_GL(gfx, opengl, surface, ReadPixels) ||
	    LOAD_GL(gfx, opengl, surface, CreateShader) || LOAD_GL(gfx, opengl, surface, ShaderSource) ||
	    LOAD_GL(gfx, opengl, surface, CompileShader) || LOAD_GL(gfx, opengl, surface, GetShaderiv) ||
	    LOAD_GL(gfx, opengl, surface, DeleteShader) || LOAD_GL(gfx, opengl, surface, CreateProgram) ||
	    LOAD_GL(gfx, opengl, surface, AttachShader) || LOAD_GL(gfx, opengl, surface, LinkProgram) ||
	    LOAD_GL(gfx, opengl, surface, GetProgramiv) || LOAD_GL(gfx, opengl, surface, DeleteProgram) ||
	    LOAD_GL(gfx, opengl, surface, GetUniformBlockIndex) || LOAD_GL(gfx, opengl, surface, UniformBlockBinding) ||
	    LOAD_GL(gfx, opengl, surface, GenBuffers) || LOAD_GL(gfx, opengl, surface, DeleteBuffers) ||
	    LOAD_GL(gfx, opengl, surface, BindBuffer) || LOAD_GL(gfx, opengl, surface, BindBufferBase) ||
	    LOAD_GL(gfx, opengl, surface, BufferData) || LOAD_GL(gfx, opengl, surface, UseProgram) ||
	    LOAD_GL(gfx, opengl, surface, EnableVertexAttribArray) || LOAD_GL(gfx, opengl, surface, DisableVertexAttribArray) ||
	    LOAD_GL(gfx, opengl, surface, VertexAttribPointer) || LOAD_GL(gfx, opengl, surface, DrawArrays) ||
	    LOAD_GL(gfx, opengl, surface, DrawElements)) {
		return 1;
	}
	LOAD_GL_OPTIONAL(gfx, opengl, surface, GetShaderInfoLog);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, GetProgramInfoLog);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, GetError);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, GetString);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, ClearDepth);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, Enable);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, Disable);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, DepthFunc);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, FrontFace);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, CullFace);

	return 0;
}

static int gfx_opengl_init(gfx_t *gfx, const gfx_config_t *config)
{
	if (gfx == NULL || config == NULL || gfx->proc == NULL || gfx->alloc.alloc == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = alloc_alloc(&gfx->alloc, sizeof(gfx_opengl_t));
	if (opengl == NULL) {
		return 1;
	}
	*opengl	  = (gfx_opengl_t){0};
	gfx->data = opengl;

	if (gfx_opengl_load_gl(gfx)) {
		return gfx_opengl_init_free(gfx, opengl, NULL);
	}
	if (config->surface != NULL) {
		if (!gfx_opengl_surface_valid(config->surface)) {
			return gfx_opengl_init_free(gfx, opengl, NULL);
		}
		opengl->surface = config->surface;
		if (config->surface->ops->make_current(config->surface)) {
			log_error("cgfx", "gfx_opengl", NULL, "failed to make the OpenGL surface current");
			return gfx_opengl_init_free(gfx, opengl, NULL);
		}
	}
	if (gfx_opengl_load_symbols(gfx, config->surface)) {
		return gfx_opengl_init_free(gfx, opengl, config->surface);
	}
	if (gfx_opengl_clear_surface(config->surface)) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to clear the current OpenGL surface");
		return gfx_opengl_init_free(gfx, opengl, NULL);
	}

	return 0;
}

static int gfx_opengl_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = gfx->data;
	opengl->image	     = NULL;
	if (opengl->gl_lib != NULL) {
		proc_dlclose(gfx->proc, opengl->gl_lib);
	}
	alloc_free(&gfx->alloc, opengl, sizeof(gfx_opengl_t));
	gfx->data = NULL;
	return 0;
}

static int gfx_opengl_proc(gfx_t *gfx, strv_t name, void **proc)
{
	if (gfx == NULL || gfx->data == NULL || proc == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = gfx->data;
	if (find_gl_symbol(gfx, opengl->surface, proc, name) == 0) {
		return 0;
	}
	log_error("cgfx", "gfx_opengl", NULL, "failed to load OpenGL symbol: %.*s", name.len, name.data);
	return 1;
}

static void gfx_opengl_render_pass_free(gfx_render_pass_t *render_pass)
{
	if (render_pass == NULL || render_pass->gfx == NULL || render_pass->data == NULL) {
		return;
	}

	alloc_free(&render_pass->gfx->alloc, render_pass->data, sizeof(gfx_opengl_render_pass_t));
	render_pass->data = NULL;
}

static int gfx_opengl_render_pass_init(gfx_render_pass_t *render_pass, const gfx_render_pass_config_t *config)
{
	if (render_pass == NULL || render_pass->gfx == NULL || config == NULL || config->color_format != GFX_FORMAT_RGBA8) {
		return 1;
	}

	gfx_opengl_render_pass_t *gl_render_pass = alloc_alloc(&render_pass->gfx->alloc, sizeof(gfx_opengl_render_pass_t));
	if (gl_render_pass == NULL) {
		return 1;
	}
	*gl_render_pass = (gfx_opengl_render_pass_t){
		.color_format = config->color_format,
		.load	      = config->load,
		.store	      = config->store,
	};
	render_pass->data = gl_render_pass;
	return 0;
}

static int memory_image_valid(const gfx_image_t *image)
{
	return image != NULL && image->origin == GFX_IMAGE_ORIGIN_MEMORY && image->format == GFX_FORMAT_RGBA8 && image->data != NULL &&
	       image->width != 0 && image->height != 0 && image->stride >= (size_t)image->width * 4;
}

static int swapchain_valid(const gfx_swapchain_t *swapchain)
{
	return swapchain != NULL && swapchain->format == GFX_FORMAT_RGBA8 && swapchain->surface != NULL &&
	       swapchain->surface->api == GFX_API_OPENGL && swapchain->surface->ops != NULL &&
	       swapchain->surface->ops->make_current != NULL && swapchain->surface->ops->present != NULL && swapchain->width != 0 &&
	       swapchain->height != 0;
}

static int surface_image_valid(const gfx_image_t *image)
{
	return image != NULL && image->origin == GFX_IMAGE_ORIGIN_SURFACE && image->swapchain != NULL &&
	       swapchain_valid(image->swapchain) && image->format == image->swapchain->format && image->width == image->swapchain->width &&
	       image->height == image->swapchain->height;
}

static void gfx_opengl_image_free(gfx_image_t *image)
{
	if (image == NULL || image->gfx == NULL || image->gfx->data == NULL) {
		return;
	}

	gfx_opengl_t *opengl = image->gfx->data;
	if (image->driver_data != NULL) {
		gfx_opengl_target_t *gl_target = image->driver_data;
		if (gfx_opengl_make_current(opengl, "target destruction")) {
			return;
		}
		if (gl_target->texture != 0 && opengl->DeleteTextures != NULL) {
			opengl->DeleteTextures(1, &gl_target->texture);
		}
		alloc_free(&image->gfx->alloc, gl_target, sizeof(gfx_opengl_target_t));
		image->driver_data = NULL;
	}
	if (opengl->image == image) {
		opengl->image = NULL;
	}
}

static int gfx_opengl_memory_image_init(gfx_image_t *image)
{
	if (!memory_image_valid(image)) {
		return 1;
	}

	gfx_opengl_t *opengl	       = image->gfx->data;
	gfx_opengl_target_t *gl_target = alloc_alloc(&image->gfx->alloc, sizeof(gfx_opengl_target_t));
	if (gl_target == NULL) {
		return 1;
	}
	*gl_target	   = (gfx_opengl_target_t){0};
	image->driver_data = gl_target;

	if (gfx_opengl_make_current(opengl, "memory target initialization")) {
		return 1;
	}
	opengl->GenTextures(1, &gl_target->texture);
	opengl->BindTexture(GL_TEXTURE_2D, gl_target->texture);
	opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	opengl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	return 0;
}

static int gfx_opengl_swapchain_init(gfx_swapchain_t *swapchain, const gfx_swapchain_config_t *config)
{
	(void)config;

	if (!swapchain_valid(swapchain)) {
		return 1;
	}

	gfx_opengl_t *opengl = swapchain->gfx->data;
	opengl->surface	     = swapchain->surface;
	if (swapchain->surface->ops->make_current(swapchain->surface)) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to make the OpenGL surface current");
		return 1;
	}
	if (swapchain->surface->ops->present_mode != NULL) {
		if (swapchain->surface->ops->present_mode(swapchain->surface, swapchain->present_mode, &swapchain->actual_present_mode)) {
			return 1;
		}
	} else {
		swapchain->actual_present_mode = GFX_PRESENT_MODE_DEFAULT;
	}

	return 0;
}

static void gfx_opengl_swapchain_free(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->data == NULL) {
		return;
	}

	gfx_opengl_t *opengl = swapchain->gfx->data;
	if (opengl->surface == swapchain->surface) {
		opengl->surface = NULL;
	}
}

static int gfx_opengl_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height)
{
	(void)width;
	(void)height;

	return !swapchain_valid(swapchain);
}

static int gfx_opengl_swapchain_present(gfx_swapchain_t *swapchain)
{
	if (!swapchain_valid(swapchain)) {
		return 1;
	}

	return swapchain->surface->ops->present(swapchain->surface, swapchain->actual_present_mode);
}

static int gfx_opengl_image_init(gfx_image_t *image)
{
	if (image == NULL || image->gfx == NULL || image->gfx->data == NULL) {
		return 1;
	}

	switch (image->origin) {
	case GFX_IMAGE_ORIGIN_NONE:
		return 0;
	case GFX_IMAGE_ORIGIN_MEMORY:
		return gfx_opengl_memory_image_init(image);
	case GFX_IMAGE_ORIGIN_SURFACE:
		if (!surface_image_valid(image)) {
			return 1;
		}
		((gfx_opengl_t *)image->gfx->data)->surface = image->swapchain->surface;
		return 0;
	default:
		return 1;
	}
}

static int gfx_opengl_image_read(gfx_image_t *image, const gfx_memory_readback_config_t *config)
{
	if (image == NULL || image->gfx == NULL || image->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = image->gfx->data;
	if (opengl->image != image || image->origin != GFX_IMAGE_ORIGIN_MEMORY || image->driver_data == NULL ||
	    gfx_opengl_make_current(opengl, "memory readback") || gfx_opengl_bind_framebuffer(opengl)) {
		return 1;
	}

	const gfx_image_t *gl_image = opengl->image;
	for (u16 y = 0; y < gl_image->height; y++) {
		u8 *row = (u8 *)config->data + (size_t)y * config->stride;
		opengl->ReadPixels(0, gl_image->height - 1 - y, gl_image->width, 1, GL_RGBA, GL_UNSIGNED_BYTE, row);
	}

	return 0;
}

static void gfx_opengl_framebuffer_free(gfx_framebuffer_t *framebuffer)
{
	if (framebuffer == NULL || framebuffer->gfx == NULL || framebuffer->gfx->data == NULL || framebuffer->data == NULL) {
		return;
	}

	gfx_opengl_t *opengl			 = framebuffer->gfx->data;
	gfx_opengl_framebuffer_t *gl_framebuffer = framebuffer->data;
	if (gfx_opengl_make_current(opengl, "framebuffer destruction")) {
		return;
	}
	if (gl_framebuffer->framebuffer != 0 && opengl->DeleteFramebuffers != NULL) {
		opengl->DeleteFramebuffers(1, &gl_framebuffer->framebuffer);
		gl_framebuffer->framebuffer = 0;
	}
	if (gl_framebuffer->depth_texture != 0 && opengl->DeleteTextures != NULL) {
		opengl->DeleteTextures(1, &gl_framebuffer->depth_texture);
		gl_framebuffer->depth_texture = 0;
	}
	if (opengl->framebuffer == framebuffer) {
		opengl->framebuffer = NULL;
	}
	alloc_free(&framebuffer->gfx->alloc, gl_framebuffer, sizeof(gfx_opengl_framebuffer_t));
	framebuffer->data = NULL;
}

static int gfx_opengl_framebuffer_init(gfx_framebuffer_t *framebuffer)
{
	if (framebuffer == NULL || framebuffer->gfx == NULL || framebuffer->gfx->data == NULL || framebuffer->image == NULL ||
	    framebuffer->render_pass == NULL || framebuffer->render_pass->data == NULL) {
		return 1;
	}

	gfx_opengl_framebuffer_t *gl_framebuffer = alloc_alloc(&framebuffer->gfx->alloc, sizeof(gfx_opengl_framebuffer_t));
	if (gl_framebuffer == NULL) {
		return 1;
	}
	*gl_framebuffer	  = (gfx_opengl_framebuffer_t){0};
	framebuffer->data = gl_framebuffer;

	if (framebuffer->image->origin == GFX_IMAGE_ORIGIN_SURFACE) {
		return 0;
	}
	if (framebuffer->image->origin != GFX_IMAGE_ORIGIN_MEMORY || framebuffer->image->driver_data == NULL) {
		gfx_opengl_framebuffer_free(framebuffer);
		return 1;
	}

	gfx_opengl_t *opengl	       = framebuffer->gfx->data;
	gfx_opengl_target_t *gl_target = framebuffer->image->driver_data;
	if (gfx_opengl_make_current(opengl, "framebuffer initialization")) {
		gfx_opengl_framebuffer_free(framebuffer);
		return 1;
	}
	opengl->GenFramebuffers(1, &gl_framebuffer->framebuffer);
	opengl->BindFramebuffer(GL_FRAMEBUFFER, gl_framebuffer->framebuffer);
	opengl->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_target->texture, 0);
	if (framebuffer->render_pass->depth_format != GFX_FORMAT_NONE) {
		opengl->GenTextures(1, &gl_framebuffer->depth_texture);
		opengl->BindTexture(GL_TEXTURE_2D, gl_framebuffer->depth_texture);
		opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		opengl->TexImage2D(GL_TEXTURE_2D,
				   0,
				   GL_DEPTH_COMPONENT32F,
				   framebuffer->width,
				   framebuffer->height,
				   0,
				   GL_DEPTH_COMPONENT,
				   GL_FLOAT,
				   NULL);
		opengl->FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gl_framebuffer->depth_texture, 0);
	}
	if (opengl->CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to create complete framebuffer");
		gfx_opengl_framebuffer_free(framebuffer);
		return 1;
	}
	return 0;
}

static int gfx_opengl_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || framebuffer == NULL || framebuffer->image == NULL ||
	    framebuffer->render_pass == NULL || framebuffer->render_pass->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = frame->gfx->data;
	opengl->image	     = framebuffer->image;
	opengl->framebuffer  = framebuffer;
	opengl->surface	     = opengl->image->origin == GFX_IMAGE_ORIGIN_SURFACE ? opengl->image->swapchain->surface : opengl->surface;
	if (_gfx_opengl_begin(frame->gfx, "pass begin", &opengl)) {
		return 1;
	}
	if (gfx_opengl_bind_framebuffer(opengl)) {
		return 1;
	}
	opengl->Viewport(frame->pass.viewport.x, frame->pass.viewport.y, frame->pass.viewport.width, frame->pass.viewport.height);
	GLbitfield clear = 0;
	if (framebuffer->render_pass->load == GFX_LOAD_CLEAR) {
		opengl->ClearColor(frame->pass.clear.r, frame->pass.clear.g, frame->pass.clear.b, frame->pass.clear.a);
		clear |= GL_COLOR_BUFFER_BIT;
	}
	if (framebuffer->render_pass->depth_format != GFX_FORMAT_NONE && framebuffer->render_pass->depth_load == GFX_LOAD_CLEAR) {
		if (opengl->ClearDepth == NULL) {
			return 1;
		}
		opengl->ClearDepth(frame->pass.clear_depth);
		clear |= GL_DEPTH_BUFFER_BIT;
	}
	if (clear != 0) {
		opengl->Clear(clear);
	}
	return 0;
}

static int gfx_opengl_bind_framebuffer(gfx_opengl_t *opengl)
{
	if (opengl->image == NULL || opengl->framebuffer == NULL) {
		return 1;
	}
	if (opengl->image->origin == GFX_IMAGE_ORIGIN_MEMORY) {
		gfx_opengl_framebuffer_t *framebuffer = opengl->framebuffer->data;
		if (framebuffer == NULL || framebuffer->framebuffer == 0) {
			return 1;
		}
		opengl->BindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebuffer);
		return 0;
	}
	if (opengl->image->origin == GFX_IMAGE_ORIGIN_SURFACE) {
		opengl->BindFramebuffer(GL_FRAMEBUFFER, 0);
		return 0;
	}
	return 1;
}

static int gfx_opengl_make_current(gfx_opengl_t *opengl, const char *operation)
{
	if (opengl == NULL) {
		log_error("cgfx", // LCOV_EXCL_LINE
			  "gfx_opengl",
			  NULL,
			  "failed to make OpenGL context current for %s: driver data is null",
			  operation);
		return 1; // LCOV_EXCL_LINE
	}
	if (opengl->surface == NULL) {
		return 0;
	}
	if (opengl->surface->ops == NULL || opengl->surface->ops->make_current == NULL) {
		log_error("cgfx",
			  "gfx_opengl",
			  NULL,
			  "failed to make OpenGL context current for %s: surface has no make_current callback",
			  operation);
		return 1;
	}
	if (opengl->surface->ops->make_current(opengl->surface)) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to make OpenGL context current for %s", operation);
		return 1;
	}
	return 0;
}

static int _gfx_opengl_begin(gfx_t *gfx, const char *operation, gfx_opengl_t **out)
{
	if (gfx == NULL || gfx->data == NULL || out == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_opengl_t *opengl = gfx->data;
	if (gfx_opengl_make_current(opengl, operation)) {
		return 1;
	}

	*out = opengl;
	return 0;
}

static const char *gfx_opengl_get_string(gfx_opengl_t *opengl, unsigned int name)
{
	if (opengl == NULL || opengl->GetString == NULL) {
		return NULL; // LCOV_EXCL_LINE
	}
	const unsigned char *str = opengl->GetString(name);
	return str != NULL ? (const char *)str : NULL;
}

static void gfx_opengl_log_context(gfx_opengl_t *opengl, const char *reason)
{
	if (opengl == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "%s: OpenGL driver data is null", reason); // LCOV_EXCL_LINE
		return;										 // LCOV_EXCL_LINE
	}
	if (opengl->GetString == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "%s: glGetString is unavailable", reason);
		return;
	}
	const char *version  = gfx_opengl_get_string(opengl, GL_VERSION);
	const char *sl	     = gfx_opengl_get_string(opengl, GL_SHADING_LANGUAGE_VERSION);
	const char *vendor   = gfx_opengl_get_string(opengl, GL_VENDOR);
	const char *renderer = gfx_opengl_get_string(opengl, GL_RENDERER);
	log_error("cgfx",
		  "gfx_opengl",
		  NULL,
		  "%s: GL_VERSION=%s GLSL=%s VENDOR=%s RENDERER=%s",
		  reason,
		  version != NULL ? version : "(null)",
		  sl != NULL ? sl : "(null)",
		  vendor != NULL ? vendor : "(null)",
		  renderer != NULL ? renderer : "(null)");
}

static unsigned int gfx_opengl_get_error(gfx_opengl_t *opengl)
{
	if (opengl == NULL || opengl->GetError == NULL) {
		return GL_NO_ERROR;
	}
	return opengl->GetError();
}

static void gfx_opengl_clear_errors(gfx_opengl_t *opengl)
{
	if (opengl == NULL || opengl->GetError == NULL) {
		return;
	}
	for (u32 i = 0; i < 16 && opengl->GetError() != GL_NO_ERROR; i++) {
	}
}

static unsigned int gfx_opengl_compile_shader(gfx_opengl_t *opengl, unsigned int type, const char *source, int log_errors)
{
	if (log_errors && opengl->GetError == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "OpenGL diagnostic symbol glGetError is unavailable");
	}
	gfx_opengl_clear_errors(opengl);
	unsigned int shader = opengl->CreateShader(type);
	unsigned int error  = gfx_opengl_get_error(opengl);
	if (shader == 0) {
		if (log_errors) {
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to create OpenGL %s shader: glCreateShader returned 0, glGetError=0x%04x",
				  type == GL_VERTEX_SHADER ? "vertex" : "fragment",
				  error);
			gfx_opengl_log_context(opengl, "OpenGL shader creation context");
		}
		return 0;
	}
	if (log_errors && error != GL_NO_ERROR) {
		log_error("cgfx", "gfx_opengl", NULL, "OpenGL shader %u was created but glGetError=0x%04x", shader, error);
	}

	opengl->ShaderSource(shader, 1, &source, NULL);
	opengl->CompileShader(shader);
	int compiled = 0;
	opengl->GetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		if (log_errors && opengl->GetShaderInfoLog != NULL) {
			char info[1024] = {0};
			int len		= 0;
			opengl->GetShaderInfoLog(shader, (int)sizeof(info) - 1, &len, info);
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to compile OpenGL %s shader: %s",
				  type == GL_VERTEX_SHADER ? "vertex" : "fragment",
				  info);
		} else if (log_errors) {
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to compile OpenGL %s shader",
				  type == GL_VERTEX_SHADER ? "vertex" : "fragment");
		}
		opengl->DeleteShader(shader);
		return 0;
	}

	return shader;
}

static void gfx_opengl_log_program_link(gfx_opengl_t *opengl, unsigned int program)
{
	if (opengl->GetProgramInfoLog != NULL) {
		char info[1024] = {0};
		int len		= 0;
		opengl->GetProgramInfoLog(program, (int)sizeof(info) - 1, &len, info);
		log_error("cgfx", "gfx_opengl", NULL, "failed to link OpenGL shader program: %s", info);
	} else {
		log_error("cgfx", "gfx_opengl", NULL, "failed to link OpenGL shader program");
	}
}

static void gfx_opengl_buffer_free(gfx_buffer_t *buffer)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || buffer->data == NULL) {
		return;
	}
	gfx_opengl_t *opengl	       = buffer->gfx->data;
	gfx_opengl_buffer_t *gl_buffer = buffer->data;
	if (gfx_opengl_make_current(opengl, "buffer destruction")) {
		return;
	}
	if (gl_buffer->buffer != 0 && opengl->DeleteBuffers != NULL) {
		opengl->DeleteBuffers(1, &gl_buffer->buffer);
		gl_buffer->buffer = 0;
	}
	alloc_free(&buffer->gfx->alloc, gl_buffer, sizeof(gfx_opengl_buffer_t));
	buffer->data = NULL;
}

static int gfx_opengl_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = buffer->gfx->data;

	if (gfx_opengl_make_current(opengl, "buffer initialization")) {
		return 1;
	}

	gfx_opengl_buffer_t *gl_buffer = alloc_alloc(&buffer->gfx->alloc, sizeof(gfx_opengl_buffer_t));
	if (gl_buffer == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to allocate OpenGL buffer");
		return 1;
	}
	*gl_buffer   = (gfx_opengl_buffer_t){0};
	buffer->data = gl_buffer;

	switch (config->type) {
	case GFX_BUFFER_VERTEX: {
		gl_buffer->target = GL_ARRAY_BUFFER;
		break;
	}
	case GFX_BUFFER_INDEX: {
		gl_buffer->target = GL_ELEMENT_ARRAY_BUFFER;
		break;
	}
	case GFX_BUFFER_UNIFORM: {
		gl_buffer->target = GL_UNIFORM_BUFFER;
		break;
	}
	default: {
		log_error("cgfx", "gfx_opengl", NULL, "unsupported buffer type: %d", config->type);
		gfx_opengl_buffer_free(buffer);
		return 1;
	}
	}

	opengl->GenBuffers(1, &gl_buffer->buffer);
	if (gl_buffer->buffer == 0) {
		gfx_opengl_buffer_free(buffer);
		return 1;
	}
	if (config->size != 0) {
		opengl->BindBuffer(gl_buffer->target, gl_buffer->buffer);
		opengl->BufferData(gl_buffer->target,
				   config->size,
				   config->data,
				   config->usage == GFX_BUFFER_USAGE_STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
		opengl->BindBuffer(gl_buffer->target, 0);
		buffer->size = config->size;
	}

	return 0;
}

static int gfx_opengl_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || buffer->data == NULL || data == NULL || size == 0 ||
	    buffer->usage == GFX_BUFFER_USAGE_STATIC) {
		return 1;
	}

	gfx_opengl_t *opengl = buffer->gfx->data;

	if (gfx_opengl_make_current(opengl, "set buffer data")) {
		return 1;
	}

	gfx_opengl_buffer_t *gl_buffer = buffer->data;

	opengl->BindBuffer(gl_buffer->target, gl_buffer->buffer);
	opengl->BufferData(gl_buffer->target, size, data, GL_DYNAMIC_DRAW);
	opengl->BindBuffer(gl_buffer->target, 0);
	buffer->size = size;

	return 0;
}

static int gfx_opengl_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	if (frame == NULL || frame->gfx == NULL || frame->pipeline == NULL || buffer == NULL || buffer->gfx == NULL ||
	    buffer->gfx->data == NULL || buffer->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = buffer->gfx->data;
	if (gfx_opengl_make_current(opengl, "bind buffer")) {
		return 1;
	}

	gfx_opengl_buffer_t *gl_buffer	   = buffer->data;
	gfx_opengl_pipeline_t *gl_pipeline = frame->pipeline->data;

	opengl->BindBuffer(gl_buffer->target, gl_buffer->buffer);
	if (buffer->type == GFX_BUFFER_UNIFORM) {
		return 0;
	}
	size_t offset = 0;
	for (size_t i = 0; i < gl_pipeline->input_layout_size / sizeof(gfx_layout_t); i++) {
		opengl->EnableVertexAttribArray(gl_pipeline->input_layout[i].index);
		opengl->VertexAttribPointer(gl_pipeline->input_layout[i].index,
					    gl_pipeline->input_layout[i].count,
					    GL_FLOAT,
					    GL_FALSE,
					    gl_pipeline->stride,
					    (const void *)offset);
		offset += sizeof(float) * gl_pipeline->input_layout[i].count;
	}

	return 0;
}

static int gfx_opengl_bind_resources(gfx_frame_t *frame, const gfx_resource_binding_t *bindings, u32 binding_count)
{
	if (frame == NULL || frame->gfx == NULL || frame->pipeline == NULL || (bindings == NULL && binding_count != 0)) {
		return 1;
	}

	gfx_opengl_t *opengl = frame->gfx->data;
	if (gfx_opengl_make_current(opengl, "bind uniform buffer") || opengl->BindBufferBase == NULL) {
		return 1;
	}

	for (u32 i = 0; i < binding_count; i++) {
		const gfx_resource_binding_t *binding = &bindings[i];
		if (binding->type != GFX_RESOURCE_UNIFORM_BUFFER || binding->buffer == NULL || binding->buffer->gfx != frame->gfx ||
		    binding->buffer->type != GFX_BUFFER_UNIFORM || binding->buffer->data == NULL) {
			return 1;
		}
		gfx_opengl_buffer_t *gl_buffer = binding->buffer->data;
		if (gl_buffer->target != GL_UNIFORM_BUFFER) {
			return 1;
		}
		opengl->BindBufferBase(GL_UNIFORM_BUFFER, binding->binding, gl_buffer->buffer);
	}
	return 0;
}

static int gfx_opengl_shader_copy_uniform_blocks(gfx_opengl_shader_t *gl_shader, const gfx_shader_code_t *shader_code)
{
	if (gl_shader == NULL || shader_code == NULL || shader_code->buffer_count > 16) {
		return 1; // LCOV_EXCL_LINE
	}
	gl_shader->uniform_block_count = shader_code->buffer_count;
	for (u32 i = 0; i < shader_code->buffer_count; i++) {
		if (shader_code->buffers[i].name.data == NULL ||
		    shader_code->buffers[i].name.len >= sizeof(gl_shader->uniform_blocks[i].name)) {
			log_error("cgfx", "gfx_opengl", NULL, "uniform block name is too long");
			return 1;
		}
		gl_shader->uniform_blocks[i].slot = shader_code->buffers[i].slot;
		for (size_t c = 0; c < shader_code->buffers[i].name.len; c++) {
			gl_shader->uniform_blocks[i].name[c] = shader_code->buffers[i].name.data[c];
		}
		gl_shader->uniform_blocks[i].name[shader_code->buffers[i].name.len] = 0;
	}
	return 0;
}

static void gfx_opengl_shader_free(gfx_shader_t *shader)
{
	if (shader == NULL || shader->gfx == NULL || shader->gfx->data == NULL || shader->data == NULL) {
		return;
	}
	gfx_opengl_t *opengl	       = shader->gfx->data;
	gfx_opengl_shader_t *gl_shader = shader->data;
	if (gfx_opengl_make_current(opengl, "shader destruction")) {
		return;
	}
	if (gl_shader->shader != 0 && opengl->DeleteShader != NULL) {
		opengl->DeleteShader(gl_shader->shader);
		gl_shader->shader = 0;
	}
	alloc_free(&shader->gfx->alloc, gl_shader, sizeof(gfx_opengl_shader_t));
	shader->data = NULL;
}

static int gfx_opengl_shader_init(gfx_shader_t *shader, const gfx_shader_config_t *config)
{
	if (shader == NULL || shader->gfx == NULL || shader->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = shader->gfx->data;

	if (gfx_opengl_make_current(opengl, "shader initialization")) {
		return 1;
	}
	if (opengl->GetString != NULL && gfx_opengl_get_string(opengl, GL_VERSION) == NULL) {
		log_error("cgfx",
			  "gfx_opengl",
			  NULL,
			  "failed to initialize OpenGL shader: surface make_current succeeded but GL_VERSION is null");
		gfx_opengl_log_context(opengl, "OpenGL shader initialization context");
		return 1;
	}

	gfx_opengl_shader_t *gl_shader = alloc_alloc(&shader->gfx->alloc, sizeof(gfx_opengl_shader_t));
	if (gl_shader == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to allocate OpenGL shader");
		return 1;
	}
	*gl_shader   = (gfx_opengl_shader_t){0};
	shader->data = gl_shader;

	gfx_shader_code_t shader_code = {0};
	if (gfx_shader_compiler_transpile(config->compiler, config->source, config->stage, GFX_SHADER_LANGUAGE_GLSL, &shader_code)) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to transpile OpenGL shader");
		gfx_shader_code_free(&shader_code);
		gfx_opengl_shader_free(shader);
		return 1;
	}
	if (gfx_opengl_shader_copy_uniform_blocks(gl_shader, &shader_code)) {
		gfx_shader_code_free(&shader_code);
		gfx_opengl_shader_free(shader);
		return 1;
	}

	unsigned int type;
	switch (config->stage) {
	case GFX_SHADER_STAGE_VERTEX: {
		type = GL_VERTEX_SHADER;
		break;
	}
	case GFX_SHADER_STAGE_FRAGMENT: {
		type = GL_FRAGMENT_SHADER;
		break;
	}
	default: {
		log_error("cgfx", "gfx_opengl", NULL, "unsupported shader stage: %d", config->stage);
		gfx_shader_code_free(&shader_code);
		gfx_opengl_shader_free(shader);
		return 1;
	}
	}

	gl_shader->shader = gfx_opengl_compile_shader(opengl, type, shader_code.text, 1);
	if (gl_shader->shader == 0) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to build OpenGL shader");
		gfx_shader_code_free(&shader_code);
		gfx_opengl_shader_free(shader);
		return 1;
	}
	gfx_shader_code_free(&shader_code);

	return 0;
}

static void gfx_opengl_pipeline_bind_uniform_blocks(gfx_opengl_t *opengl, unsigned int program, const gfx_opengl_shader_t *shader)
{
	for (u32 i = 0; i < shader->uniform_block_count; i++) {
		unsigned int index = opengl->GetUniformBlockIndex(program, shader->uniform_blocks[i].name);
		if (index != GL_INVALID_INDEX) {
			opengl->UniformBlockBinding(program, index, shader->uniform_blocks[i].slot);
		}
	}
}

static void gfx_opengl_pipeline_free(gfx_pipeline_t *pipeline)
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL || pipeline->data == NULL) {
		return;
	}
	gfx_opengl_t *opengl		   = pipeline->gfx->data;
	gfx_opengl_pipeline_t *gl_pipeline = pipeline->data;
	if (gfx_opengl_make_current(opengl, "shader destruction")) {
		return;
	}
	if (gl_pipeline->program != 0 && opengl->DeleteProgram != NULL) {
		opengl->DeleteProgram(gl_pipeline->program);
		gl_pipeline->program = 0;
	}
	alloc_free(&pipeline->gfx->alloc, gl_pipeline, sizeof(gfx_opengl_pipeline_t));
	pipeline->data = NULL;
}

static int gfx_opengl_pipeline_init(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config)
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL || config == NULL || config->vs.data == NULL ||
	    config->fs.data == NULL || config->input_layout == NULL || config->input_layout_size == 0 ||
	    config->input_layout_size % sizeof(gfx_layout_t) != 0) {
		return 1;
	}

	gfx_opengl_t *opengl = pipeline->gfx->data;

	if (gfx_opengl_make_current(opengl, "pipeline initialization")) {
		return 1;
	}

	gfx_opengl_pipeline_t *gl_pipeline = alloc_alloc(&pipeline->gfx->alloc, sizeof(gfx_opengl_pipeline_t));
	if (gl_pipeline == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to allocate OpenGL pipeline");
		return 1;
	}
	*gl_pipeline   = (gfx_opengl_pipeline_t){0};
	pipeline->data = gl_pipeline;

	gl_pipeline->program = opengl->CreateProgram();
	if (gl_pipeline->program == 0) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to create OpenGL shader program");
		gfx_opengl_pipeline_free(pipeline);
		return 1;
	}

	gfx_opengl_shader_t *vs = config->vs.data;
	gfx_opengl_shader_t *fs = config->fs.data;

	opengl->AttachShader(gl_pipeline->program, vs->shader);
	opengl->AttachShader(gl_pipeline->program, fs->shader);

	gl_pipeline->input_layout      = config->input_layout;
	gl_pipeline->input_layout_size = config->input_layout_size;

	gl_pipeline->stride = 0;
	for (size_t i = 0; i < config->input_layout_size / sizeof(gfx_layout_t); i++) {
		size_t size;
		if (gl_pipeline->input_layout[i].type == GFX_VALUE_FLOAT32) {
			size = sizeof(float);
		} else {
			log_error("cgfx", "gfx_opengl", NULL, "unsupported value type: %d", gl_pipeline->input_layout[i].type);
			gfx_opengl_pipeline_free(pipeline);
			return 1;
		}

		size *= config->input_layout[i].count;
		if (size > (size_t)S32_MAX || gl_pipeline->stride > S32_MAX - (int)size) {
			log_error("cgfx", "gfx_opengl", NULL, "input layout stride is too large");
			gfx_opengl_pipeline_free(pipeline);
			return 1;
		}
		gl_pipeline->stride += (int)size;
	}

	opengl->LinkProgram(gl_pipeline->program);

	int linked = 0;
	opengl->GetProgramiv(gl_pipeline->program, GL_LINK_STATUS, &linked);
	if (!linked) {
		gfx_opengl_log_program_link(opengl, gl_pipeline->program);
		log_error("cgfx", "gfx_opengl", NULL, "failed to link OpenGL shader program");
		gfx_opengl_pipeline_free(pipeline);
		return 1;
	}

	gfx_opengl_pipeline_bind_uniform_blocks(opengl, gl_pipeline->program, vs);
	gfx_opengl_pipeline_bind_uniform_blocks(opengl, gl_pipeline->program, fs);

	return 0;
}

static int gfx_opengl_pipeline_bind(gfx_frame_t *frame, const gfx_pipeline_t *pipeline)
{
	if (frame == NULL || frame->gfx == NULL || pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL ||
	    pipeline->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl		   = pipeline->gfx->data;
	gfx_opengl_pipeline_t *gl_pipeline = pipeline->data;
	if (gfx_opengl_make_current(opengl, "bind pipeline")) {
		return 1;
	}

	opengl->UseProgram(gl_pipeline->program);
	if (opengl->Enable == NULL || opengl->FrontFace == NULL || (pipeline->raster.cull == GFX_CULL_NONE && opengl->Disable == NULL) ||
	    (pipeline->raster.cull != GFX_CULL_NONE && opengl->CullFace == NULL)) {
		return 1;
	}

	opengl->FrontFace(pipeline->raster.front_face == GFX_WINDING_CLOCKWISE ? GL_CW : GL_CCW);
	if (pipeline->raster.cull == GFX_CULL_NONE) {
		opengl->Disable(GL_CULL_FACE);
	} else {
		opengl->Enable(GL_CULL_FACE);
		opengl->CullFace(pipeline->raster.cull == GFX_CULL_FRONT ? GL_FRONT : GL_BACK);
	}

	if (pipeline->render_pass->depth_format != GFX_FORMAT_NONE) {
		if (pipeline->depth.test) {
			if (opengl->Enable == NULL || opengl->DepthFunc == NULL) {
				return 1;
			}
			opengl->Enable(GL_DEPTH_TEST);
			opengl->DepthFunc(GL_LESS);
		} else {
			if (opengl->Disable == NULL) {
				return 1;
			}
			opengl->Disable(GL_DEPTH_TEST);
		}
	}

	return 0;
}

static void gfx_opengl_pipeline_end(gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || frame->pipeline == NULL || frame->pipeline->data == NULL) {
		return;
	}

	gfx_opengl_t *opengl		   = frame->gfx->data;
	gfx_opengl_pipeline_t *gl_pipeline = frame->pipeline->data;

	for (size_t i = 0; i < gl_pipeline->input_layout_size / sizeof(gfx_layout_t); i++) {
		opengl->DisableVertexAttribArray(gl_pipeline->input_layout[i].index);
	}

	opengl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	opengl->BindBuffer(GL_ARRAY_BUFFER, 0);
	opengl->UseProgram(0);
}

static int gfx_opengl_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = frame->gfx->data;
	opengl->DrawArrays(GL_TRIANGLES, (int)first_vertex, (int)vertex_count);
	return 0;
}

static int gfx_opengl_draw_indexed(gfx_frame_t *frame, u32 index_count)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = frame->gfx->data;
	opengl->DrawElements(GL_TRIANGLES, (int)index_count, GL_UNSIGNED_INT, NULL);
	return 0;
}

static int gfx_opengl_end(gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = frame->gfx->data;
	if (gfx_opengl_make_current(opengl, "end")) {
		return 1;
	}
	gfx_opengl_pipeline_end(frame);

	return 0;
}

static gfx_driver_t gfx_opengl = {
	.name			= "opengl",
	.api			= GFX_API_OPENGL,
	.init			= gfx_opengl_init,
	.free			= gfx_opengl_free,
	.proc			= gfx_opengl_proc,
	.render_pass_init	= gfx_opengl_render_pass_init,
	.render_pass_free	= gfx_opengl_render_pass_free,
	.swapchain_init		= gfx_opengl_swapchain_init,
	.swapchain_free		= gfx_opengl_swapchain_free,
	.swapchain_resize	= gfx_opengl_swapchain_resize,
	.swapchain_present	= gfx_opengl_swapchain_present,
	.image_init		= gfx_opengl_image_init,
	.image_free		= gfx_opengl_image_free,
	.image_read		= gfx_opengl_image_read,
	.framebuffer_init	= gfx_opengl_framebuffer_init,
	.framebuffer_free	= gfx_opengl_framebuffer_free,
	.framebuffer_pass_begin = gfx_opengl_framebuffer_pass_begin,
	.buffer_init		= gfx_opengl_buffer_init,
	.buffer_free		= gfx_opengl_buffer_free,
	.buffer_set_data	= gfx_opengl_buffer_set_data,
	.buffer_bind		= gfx_opengl_buffer_bind,
	.bind_resources		= gfx_opengl_bind_resources,
	.shader_init		= gfx_opengl_shader_init,
	.shader_free		= gfx_opengl_shader_free,
	.pipeline_init		= gfx_opengl_pipeline_init,
	.pipeline_free		= gfx_opengl_pipeline_free,
	.pipeline_bind		= gfx_opengl_pipeline_bind,
	.draw			= gfx_opengl_draw,
	.draw_indexed		= gfx_opengl_draw_indexed,
	.end			= gfx_opengl_end,
};

GFX_DRIVER(gfx_opengl, &gfx_opengl);
