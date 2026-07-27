#include "gfx_driver.h"

#include "log.h"

enum {
	GL_TRUE			    = 1,
	GL_TEXTURE_2D		    = 0x0DE1,
	GL_RGBA			    = 0x1908,
	GL_VENDOR		    = 0x1F00,
	GL_RENDERER		    = 0x1F01,
	GL_VERSION		    = 0x1F02,
	GL_SHADING_LANGUAGE_VERSION = 0x8B8C,
	GL_UNSIGNED_BYTE	    = 0x1401,
	GL_NEAREST		    = 0x2600,
	GL_CLAMP_TO_EDGE	    = 0x812F,
	GL_TEXTURE_MAG_FILTER	    = 0x2800,
	GL_TEXTURE_MIN_FILTER	    = 0x2801,
	GL_TEXTURE_WRAP_S	    = 0x2802,
	GL_TEXTURE_WRAP_T	    = 0x2803,
	GL_RGBA8		    = 0x8058,
	GL_COLOR_ATTACHMENT0	    = 0x8CE0,
	GL_FRAMEBUFFER		    = 0x8D40,
	GL_FRAMEBUFFER_COMPLETE	    = 0x8CD5,
	GL_COLOR_BUFFER_BIT	    = 0x00004000,
	GL_TRIANGLES		    = 0x0004,
	GL_FLOAT		    = 0x1406,
	GL_FALSE		    = 0,
	GL_ARRAY_BUFFER		    = 0x8892,
	GL_DYNAMIC_DRAW		    = 0x88E8,
	GL_VERTEX_SHADER	    = 0x8B31,
	GL_FRAGMENT_SHADER	    = 0x8B30,
	GL_COMPILE_STATUS	    = 0x8B81,
	GL_LINK_STATUS		    = 0x8B82,
	GL_INFO_LOG_LENGTH	    = 0x8B84,
	GL_NO_ERROR		    = 0,
};

typedef struct gfx_opengl_vertex_2d_s {
	float x;
	float y;
	float r;
	float g;
	float b;
	float a;
} gfx_opengl_vertex_2d_t;

typedef struct gfx_opengl_s {
	void *gl_lib;
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
	unsigned int (*GetError)(void);
	const unsigned char *(*GetString)(unsigned int);
	unsigned int (*CreateShader)(unsigned int);
	void (*ShaderSource)(unsigned int, int, const char **, const int *);
	void (*CompileShader)(unsigned int);
	void (*GetShaderiv)(unsigned int, unsigned int, int *);
	void (*GetShaderInfoLog)(unsigned int, int, int *, char *);
	void (*DeleteShader)(unsigned int);
	unsigned int (*CreateProgram)(void);
	void (*AttachShader)(unsigned int, unsigned int);
	void (*LinkProgram)(unsigned int);
	void (*GetProgramiv)(unsigned int, unsigned int, int *);
	void (*GetProgramInfoLog)(unsigned int, int, int *, char *);
	void (*DeleteProgram)(unsigned int);
	void (*GenBuffers)(int, unsigned int *);
	void (*DeleteBuffers)(int, const unsigned int *);
	void (*BindBuffer)(unsigned int, unsigned int);
	void (*BufferData)(unsigned int, size_t, const void *, unsigned int);
	void (*UseProgram)(unsigned int);
	void (*EnableVertexAttribArray)(unsigned int);
	void (*DisableVertexAttribArray)(unsigned int);
	void (*VertexAttribPointer)(unsigned int, int, unsigned int, unsigned char, int, const void *);
	void (*DrawArrays)(unsigned int, int, int);
} gfx_opengl_t;

typedef struct gfx_opengl_buffer_s {
	unsigned int buffer;
} gfx_opengl_buffer_t;

typedef struct gfx_opengl_shader_s {
	unsigned int shader;
} gfx_opengl_shader_t;

typedef struct gfx_opengl_pipeline_s {
	unsigned int program;
	const gfx_layout_t *input_layout;
	size_t input_layout_size;
	int stride;
} gfx_opengl_pipeline_t;

static int gfx_opengl_make_current(gfx_opengl_t *opengl, const char *operation);
static int gfx_opengl_begin(gfx_t *gfx, const char *operation, gfx_opengl_t **out);

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
		return 1; // LCOV_EXCL_LINE
	}
	return surface->ops->clear_current(surface);
}

static int gfx_opengl_init_free(gfx_t *gfx, gfx_opengl_t *opengl, gfx_surface_t *surface)
{
	gfx_opengl_clear_surface(surface);
	if (opengl->gl_lib != NULL) {
		proc_dlclose(gfx->proc, opengl->gl_lib);
	}
	alloc_free(&gfx->alloc, opengl, sizeof(*opengl));
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
	    LOAD_GL(gfx, opengl, surface, GenBuffers) || LOAD_GL(gfx, opengl, surface, DeleteBuffers) ||
	    LOAD_GL(gfx, opengl, surface, BindBuffer) || LOAD_GL(gfx, opengl, surface, BufferData) ||
	    LOAD_GL(gfx, opengl, surface, UseProgram) || LOAD_GL(gfx, opengl, surface, EnableVertexAttribArray) ||
	    LOAD_GL(gfx, opengl, surface, DisableVertexAttribArray) || LOAD_GL(gfx, opengl, surface, VertexAttribPointer) ||
	    LOAD_GL(gfx, opengl, surface, DrawArrays)) {
		return 1;
	}
	LOAD_GL_OPTIONAL(gfx, opengl, surface, GetShaderInfoLog);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, GetProgramInfoLog);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, GetError);
	LOAD_GL_OPTIONAL(gfx, opengl, surface, GetString);

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
			return gfx_opengl_init_free(gfx, opengl, NULL); // LCOV_EXCL_LINE
		}
		if (config->surface->ops->make_current(config->surface)) {
			log_error("cgfx", "gfx_opengl", NULL, "failed to make the OpenGL surface current"); // LCOV_EXCL_LINE
			return gfx_opengl_init_free(gfx, opengl, NULL);					    // LCOV_EXCL_LINE
		}
	}
	if (gfx_opengl_load_symbols(gfx, config->surface)) {
		return gfx_opengl_init_free(gfx, opengl, config->surface);
	}
	if (gfx_opengl_clear_surface(config->surface)) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to clear the current OpenGL surface"); // LCOV_EXCL_LINE
		return gfx_opengl_init_free(gfx, opengl, NULL);					     // LCOV_EXCL_LINE
	}

	return 0;
}

static void gfx_opengl_target_free(gfx_opengl_t *opengl)
{
	if (opengl->surface != NULL && opengl->surface->ops != NULL && opengl->surface->ops->clear_current != NULL) {
		opengl->surface->ops->clear_current(opengl->surface);
	}
	if (opengl->framebuffer != 0 && opengl->DeleteFramebuffers != NULL) {
		opengl->DeleteFramebuffers(1, &opengl->framebuffer);
		opengl->framebuffer = 0;
	}
	if (opengl->texture != 0 && opengl->DeleteTextures != NULL) {
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
		proc_dlclose(gfx->proc, opengl->gl_lib);
	}
	alloc_free(&gfx->alloc, opengl, sizeof(*opengl));
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

static int gfx_opengl_set_memory_target(gfx_opengl_t *opengl, const gfx_target_t *target)
{
	if (!memory_target_valid(target)) {
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
	gfx_opengl_t *opengl = NULL;
	if (gfx_opengl_begin(gfx, "clear color", &opengl)) {
		return 1;
	}

	opengl->ClearColor(r, g, b, a);
	return 0;
}

static int gfx_opengl_viewport(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height)
{
	gfx_opengl_t *opengl = NULL;
	if (gfx_opengl_begin(gfx, "viewport", &opengl)) {
		return 1;
	}

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

static int gfx_opengl_bind_target(gfx_opengl_t *opengl)
{
	if (opengl->target.type == GFX_TARGET_MEMORY) {
		opengl->BindFramebuffer(GL_FRAMEBUFFER, opengl->framebuffer);
		return 0;
	}
	if (opengl->target.type == GFX_TARGET_SURFACE) {
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
			  operation); // LCOV_EXCL_LINE
		return 1;	      // LCOV_EXCL_LINE
	}
	if (opengl->surface == NULL) {
		return 0;
	}
	if (opengl->surface->ops == NULL || opengl->surface->ops->make_current == NULL) {
		log_error("cgfx", // LCOV_EXCL_LINE
			  "gfx_opengl",
			  NULL,
			  "failed to make OpenGL context current for %s: surface has no make_current callback",
			  operation);
		return 1; // LCOV_EXCL_LINE
	}
	if (opengl->surface->ops->make_current(opengl->surface)) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to make OpenGL context current for %s", operation);
		return 1;
	}
	return 0;
}

static int gfx_opengl_begin(gfx_t *gfx, const char *operation, gfx_opengl_t **out)
{
	if (gfx == NULL || gfx->data == NULL || out == NULL) {
		return 1;
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
		log_error("cgfx", "gfx_opengl", NULL, "%s: glGetString is unavailable", reason); // LCOV_EXCL_LINE
		return;										 // LCOV_EXCL_LINE
	}
	const char *version  = gfx_opengl_get_string(opengl, GL_VERSION);
	const char *sl	     = gfx_opengl_get_string(opengl, GL_SHADING_LANGUAGE_VERSION);
	const char *vendor   = gfx_opengl_get_string(opengl, GL_VENDOR);
	const char *renderer = gfx_opengl_get_string(opengl, GL_RENDERER);
	log_error("cgfx", // LCOV_EXCL_LINE
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
		return GL_NO_ERROR; // LCOV_EXCL_LINE
	}
	return opengl->GetError();
}

static void gfx_opengl_clear_errors(gfx_opengl_t *opengl)
{
	if (opengl == NULL || opengl->GetError == NULL) {
		return; // LCOV_EXCL_LINE
	}
	for (u32 i = 0; i < 16 && opengl->GetError() != GL_NO_ERROR; i++) {
	}
}

static unsigned int gfx_opengl_compile_shader(gfx_opengl_t *opengl, unsigned int type, const char *source, int log_errors)
{
	if (log_errors && opengl->GetError == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "OpenGL diagnostic symbol glGetError is unavailable"); // LCOV_EXCL_LINE
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

	opengl->GenBuffers(1, &gl_buffer->buffer);
	if (gl_buffer->buffer == 0) {
		gfx_opengl_buffer_free(buffer);
		return 1;
	}

	return 0;
}

static int gfx_opengl_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = buffer->gfx->data;

	if (gfx_opengl_make_current(opengl, "set buffer data")) {
		return 1;
	}

	gfx_opengl_buffer_t *gl_buffer = buffer->data;

	opengl->BindBuffer(GL_ARRAY_BUFFER, gl_buffer->buffer);
	opengl->BufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	opengl->BindBuffer(GL_ARRAY_BUFFER, 0);

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
	alloc_free(&shader->gfx->alloc, gl_shader, sizeof(*gl_shader));
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

		gl_pipeline->stride += size * config->input_layout[i].count;
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

	return 0;
}

static int gfx_opengl_draw_triangle_2d(const gfx_pipeline_t *pipeline, const gfx_buffer_t *buffer)
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL || pipeline->data == NULL || buffer == NULL ||
	    buffer->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl		   = pipeline->gfx->data;
	gfx_opengl_pipeline_t *gl_pipeline = pipeline->data;
	gfx_opengl_buffer_t *gl_buffer	   = buffer->data;
	if (gfx_opengl_make_current(opengl, "triangle draw")) {
		return 1;
	}
	if (gfx_opengl_bind_target(opengl)) {
		return 1;
	}

	opengl->UseProgram(gl_pipeline->program);
	opengl->BindBuffer(GL_ARRAY_BUFFER, gl_buffer->buffer);

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
	opengl->DrawArrays(GL_TRIANGLES, 0, 3);
	for (size_t i = 0; i < gl_pipeline->input_layout_size / sizeof(gfx_layout_t); i++) {
		opengl->DisableVertexAttribArray(gl_pipeline->input_layout[i].index);
	}
	opengl->BindBuffer(GL_ARRAY_BUFFER, 0);
	opengl->UseProgram(0);

	if (opengl->target.type == GFX_TARGET_MEMORY) {
		return gfx_opengl_read_memory(opengl);
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

	gfx_opengl_t *opengl = NULL;
	if (gfx_opengl_begin(gfx, "clear", &opengl)) {
		return 1;
	}
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
	.name		  = "opengl",
	.api		  = GFX_API_OPENGL,
	.init		  = gfx_opengl_init,
	.free		  = gfx_opengl_free,
	.proc		  = gfx_opengl_proc,
	.set_target	  = gfx_opengl_set_target,
	.viewport	  = gfx_opengl_viewport,
	.clear_color	  = gfx_opengl_clear_color,
	.clear		  = gfx_opengl_clear,
	.buffer_init	  = gfx_opengl_buffer_init,
	.buffer_free	  = gfx_opengl_buffer_free,
	.buffer_set_data  = gfx_opengl_buffer_set_data,
	.shader_init	  = gfx_opengl_shader_init,
	.shader_free	  = gfx_opengl_shader_free,
	.pipeline_init	  = gfx_opengl_pipeline_init,
	.pipeline_free	  = gfx_opengl_pipeline_free,
	.draw_triangle_2d = gfx_opengl_draw_triangle_2d,
	.present	  = gfx_opengl_present,
};

GFX_DRIVER(gfx_opengl, &gfx_opengl);
