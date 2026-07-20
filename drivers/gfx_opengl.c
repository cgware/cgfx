#include "gfx_driver.h"

#include "log.h"

enum {
	GL_TRUE			= 1,
	GL_TEXTURE_2D		= 0x0DE1,
	GL_RGBA			= 0x1908,
	GL_UNSIGNED_BYTE	= 0x1401,
	GL_NEAREST		= 0x2600,
	GL_CLAMP_TO_EDGE	= 0x812F,
	GL_TEXTURE_MAG_FILTER	= 0x2800,
	GL_TEXTURE_MIN_FILTER	= 0x2801,
	GL_TEXTURE_WRAP_S	= 0x2802,
	GL_TEXTURE_WRAP_T	= 0x2803,
	GL_RGBA8		= 0x8058,
	GL_COLOR_ATTACHMENT0	= 0x8CE0,
	GL_FRAMEBUFFER		= 0x8D40,
	GL_FRAMEBUFFER_COMPLETE = 0x8CD5,
	GL_COLOR_BUFFER_BIT	= 0x00004000,
};

typedef struct gfx_opengl_s {
	proc_t *proc;
	void *gl_lib;
	alloc_t alloc;
	gfx_target_t target;
	unsigned int framebuffer;
	unsigned int texture;
	gfx_surface_t *surface;
	void (*ClearColor)(float, float, float, float);
	void (*Clear)(unsigned int);
	void (*GenFramebuffers)(int, unsigned int *);
	void (*DeleteFramebuffers)(int, const unsigned int *);
	void (*BindFramebuffer)(unsigned int, unsigned int);
	unsigned int (*CheckFramebufferStatus)(unsigned int);
	void (*FramebufferTexture2D)(unsigned int, unsigned int, unsigned int, unsigned int, int);
	void (*GenTextures)(int, unsigned int *);
	void (*DeleteTextures)(int, const unsigned int *);
	void (*BindTexture)(unsigned int, unsigned int);
	void (*TexParameteri)(unsigned int, unsigned int, int);
	void (*TexImage2D)(unsigned int, int, int, int, int, int, unsigned int, unsigned int, const void *);
	void (*Viewport)(int, int, int, int);
	void (*ReadPixels)(int, int, int, int, unsigned int, unsigned int, void *);
} gfx_opengl_t;

static int load_symbol(gfx_opengl_t *opengl, void *lib, void **sym, strv_t name)
{
	if (proc_dlsym(opengl->proc, lib, name, sym) == 0) {
		return 0;
	}

	log_error("cgfx", "gfx_opengl", NULL, "failed to load OpenGL symbol: %.*s", name.len, name.data);
	return 1;
}

static int find_gl_symbol(gfx_opengl_t *opengl, void **sym, strv_t name)
{
	if (opengl->gl_lib != NULL && proc_dlsym(opengl->proc, opengl->gl_lib, name, sym) == 0) {
		return 0;
	}
	if (opengl->surface != NULL && opengl->surface->ops != NULL && opengl->surface->ops->proc != NULL) {
		if (opengl->surface->ops->proc(opengl->surface, name, sym) == 0) {
			return 0;
		}
	}
	return 1;
}

static int symbol_missing(strv_t name)
{
	log_error("cgfx", "gfx_opengl", NULL, "failed to load OpenGL symbol: %.*s", name.len, name.data);
	return 1;
}

#define LOAD_GL(_opengl, _name) load_symbol((_opengl), (_opengl)->gl_lib, (void **)&(_opengl)->_name, STRV("gl" #_name))
#define LOAD_GL_PROC(_opengl, _name)                                                                                                       \
	do {                                                                                                                               \
		if (find_gl_symbol((_opengl), (void **)&(_opengl)->_name, STRV("gl" #_name))) {                                            \
			return symbol_missing(STRV("gl" #_name));                                                                          \
		}                                                                                                                          \
	} while (0)

static int gfx_opengl_init_free(gfx_t *gfx, gfx_opengl_t *opengl)
{
	if (opengl->gl_lib != NULL) {
		proc_dlclose(opengl->proc, opengl->gl_lib);
	}
	alloc_free(&opengl->alloc, opengl, sizeof(*opengl));
	gfx->data = NULL;
	return 1;
}

static int gfx_opengl_load_gl(gfx_opengl_t *opengl)
{
	if (proc_dlopen(opengl->proc, STRV("opengl32.dll"), &opengl->gl_lib) == 0) {
		return 0;
	}
	if (proc_dlopen(opengl->proc, STRV("libOpenGL.so.0"), &opengl->gl_lib) == 0) {
		return 0;
	}
	if (proc_dlopen(opengl->proc, STRV("libGL.so.1"), &opengl->gl_lib) == 0) {
		return 0;
	}
	if (proc_dlopen(opengl->proc, STRV("libGL.so"), &opengl->gl_lib) == 0) {
		return 0;
	}

	log_error("cgfx", "gfx_opengl", NULL, "failed to load OpenGL library");
	return 1;
}

static int gfx_opengl_init(gfx_t *gfx, const gfx_config_t *config)
{
	if (gfx == NULL || config == NULL || config->proc == NULL || config->alloc.alloc == NULL) {
		return 1;
	}

	alloc_t alloc	     = config->alloc;
	gfx_opengl_t *opengl = alloc_alloc(&alloc, sizeof(gfx_opengl_t));
	if (opengl == NULL) {
		return 1;
	}
	*opengl = (gfx_opengl_t){
		.proc  = config->proc,
		.alloc = alloc,
	};
	gfx->data = opengl;

	if (gfx_opengl_load_gl(opengl)) {
		return gfx_opengl_init_free(gfx, opengl);
	}

	if (LOAD_GL(opengl, ClearColor) || LOAD_GL(opengl, Clear) || LOAD_GL(opengl, GenTextures) || LOAD_GL(opengl, DeleteTextures) ||
	    LOAD_GL(opengl, BindTexture) || LOAD_GL(opengl, TexParameteri) || LOAD_GL(opengl, TexImage2D) || LOAD_GL(opengl, Viewport) ||
	    LOAD_GL(opengl, ReadPixels)) {
		return gfx_opengl_init_free(gfx, opengl);
	}

	return 0;
}

static void gfx_opengl_target_free(gfx_opengl_t *opengl)
{
	if (opengl->surface != NULL && opengl->surface->ops != NULL && opengl->surface->ops->clear_current != NULL) {
		opengl->surface->ops->clear_current(opengl->surface);
	}
	if (opengl->framebuffer != 0) {
		opengl->DeleteFramebuffers(1, &opengl->framebuffer);
		opengl->framebuffer = 0;
	}
	if (opengl->texture != 0) {
		opengl->DeleteTextures(1, &opengl->texture);
		opengl->texture = 0;
	}
	opengl->surface = NULL;
	opengl->target	= (gfx_target_t){0};
}

static int gfx_opengl_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = gfx->data;
	gfx_opengl_target_free(opengl);
	if (opengl->gl_lib != NULL) {
		proc_dlclose(opengl->proc, opengl->gl_lib);
	}
	alloc_free(&opengl->alloc, opengl, sizeof(*opengl));
	gfx->data = NULL;
	return 0;
}

static int gfx_opengl_proc(gfx_t *gfx, strv_t name, void **proc)
{
	if (gfx == NULL || gfx->data == NULL || proc == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = gfx->data;
	if (find_gl_symbol(opengl, proc, name) == 0) {
		return 0;
	}
	return symbol_missing(name);
}

static int memory_target_valid(const gfx_target_t *target)
{
	return target != NULL && target->type == GFX_TARGET_MEMORY && target->format == GFX_FORMAT_RGBA8 && target->data != NULL &&
	       target->width != 0 && target->height != 0 && target->stride >= (size_t)target->width * 4;
}

static int surface_target_valid(const gfx_target_t *target)
{
	return target != NULL && target->type == GFX_TARGET_SURFACE && target->format == GFX_FORMAT_RGBA8 && target->surface != NULL &&
	       target->surface->api == GFX_API_OPENGL && target->surface->ops != NULL && target->surface->ops->make_current != NULL &&
	       target->surface->ops->present != NULL && target->width != 0 && target->height != 0;
}

static int surface_target_same(const gfx_opengl_t *opengl, const gfx_target_t *target)
{
	return opengl->target.type == GFX_TARGET_SURFACE && opengl->surface != NULL && opengl->target.format == target->format &&
	       opengl->target.surface == target->surface;
}

static int gfx_opengl_load_framebuffer(gfx_opengl_t *opengl)
{
	if (opengl->GenFramebuffers != NULL && opengl->DeleteFramebuffers != NULL && opengl->BindFramebuffer != NULL &&
	    opengl->CheckFramebufferStatus != NULL && opengl->FramebufferTexture2D != NULL) {
		return 0;
	}

	LOAD_GL_PROC(opengl, GenFramebuffers);
	LOAD_GL_PROC(opengl, DeleteFramebuffers);
	LOAD_GL_PROC(opengl, BindFramebuffer);
	LOAD_GL_PROC(opengl, CheckFramebufferStatus);
	LOAD_GL_PROC(opengl, FramebufferTexture2D);
	return 0;
}

static int gfx_opengl_set_memory_target(gfx_opengl_t *opengl, const gfx_target_t *target)
{
	if (!memory_target_valid(target)) {
		return 1;
	}
	if (gfx_opengl_load_framebuffer(opengl)) {
		return 1;
	}

	gfx_opengl_target_free(opengl);
	opengl->target = *target;

	opengl->GenTextures(1, &opengl->texture);
	opengl->BindTexture(GL_TEXTURE_2D, opengl->texture);
	opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	opengl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	opengl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, target->width, target->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	opengl->GenFramebuffers(1, &opengl->framebuffer);
	opengl->BindFramebuffer(GL_FRAMEBUFFER, opengl->framebuffer);
	opengl->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opengl->texture, 0);
	if (opengl->CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to create complete framebuffer");
		gfx_opengl_target_free(opengl);
		return 1;
	}

	return 0;
}

static int gfx_opengl_set_surface_target(gfx_opengl_t *opengl, const gfx_target_t *target)
{
	if (!surface_target_valid(target)) {
		return 1;
	}

	if (surface_target_same(opengl, target)) {
		opengl->target = *target;
		return 0;
	}

	gfx_opengl_target_free(opengl);
	opengl->surface = target->surface;
	if (opengl->surface->ops->make_current(opengl->surface)) {
		gfx_opengl_target_free(opengl);
		log_error("cgfx", "gfx_opengl", NULL, "failed to make the OpenGL surface current");
		return 1;
	}
	if (gfx_opengl_load_framebuffer(opengl)) {
		gfx_opengl_target_free(opengl);
		return 1;
	}

	opengl->target = *target;
	return 0;
}

static int gfx_opengl_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || gfx->data == NULL || target == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = gfx->data;
	switch (target->type) {
	case GFX_TARGET_NONE:
		gfx_opengl_target_free(opengl);
		return 0;
	case GFX_TARGET_MEMORY:
		return gfx_opengl_set_memory_target(opengl, target);
	case GFX_TARGET_SURFACE:
		return gfx_opengl_set_surface_target(opengl, target);
	default:
		return 1;
	}
}

static int gfx_opengl_clear_color(gfx_t *gfx, float r, float g, float b, float a)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = gfx->data;
	opengl->ClearColor(r, g, b, a);
	return 0;
}

static int gfx_opengl_viewport(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = gfx->data;
	opengl->Viewport(x, y, width, height);
	return 0;
}

static int gfx_opengl_read_memory(gfx_opengl_t *opengl)
{
	for (u16 y = 0; y < opengl->target.height; y++) {
		u8 *row = (u8 *)opengl->target.data + (size_t)y * opengl->target.stride;
		opengl->ReadPixels(0, opengl->target.height - 1 - y, opengl->target.width, 1, GL_RGBA, GL_UNSIGNED_BYTE, row);
	}

	return 0;
}

static int gfx_opengl_clear(gfx_t *gfx, u32 buffers)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	unsigned int mask = 0;
	if (buffers & GFX_CLEAR_COLOR_BUFFER) {
		mask |= GL_COLOR_BUFFER_BIT;
	}

	gfx_opengl_t *opengl = gfx->data;
	if (opengl->target.type == GFX_TARGET_MEMORY) {
		opengl->BindFramebuffer(GL_FRAMEBUFFER, opengl->framebuffer);
		opengl->Viewport(0, 0, opengl->target.width, opengl->target.height);
	} else if (opengl->target.type == GFX_TARGET_SURFACE) {
		opengl->BindFramebuffer(GL_FRAMEBUFFER, 0);
		opengl->Viewport(0, 0, opengl->target.width, opengl->target.height);
	}

	opengl->Clear(mask);
	if ((buffers & GFX_CLEAR_COLOR_BUFFER) == 0 || opengl->target.type != GFX_TARGET_MEMORY) {
		return 0;
	}

	return gfx_opengl_read_memory(opengl);
}

static int gfx_opengl_present(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = gfx->data;
	if (opengl->target.type != GFX_TARGET_SURFACE || opengl->surface == NULL || opengl->surface->ops == NULL ||
	    opengl->surface->ops->present == NULL) {
		return 1;
	}

	return opengl->surface->ops->present(opengl->surface);
}

static gfx_driver_t gfx_opengl = {
	.name	     = "opengl",
	.api	     = GFX_API_OPENGL,
	.init	     = gfx_opengl_init,
	.free	     = gfx_opengl_free,
	.proc	     = gfx_opengl_proc,
	.set_target  = gfx_opengl_set_target,
	.viewport    = gfx_opengl_viewport,
	.clear_color = gfx_opengl_clear_color,
	.clear	     = gfx_opengl_clear,
	.present     = gfx_opengl_present,
};

GFX_DRIVER(gfx_opengl, &gfx_opengl);
