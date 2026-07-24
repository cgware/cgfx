#include "gfx_driver.h"
#include "gfx_shader_compiler.h"

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
	GFX_OPENGL_POS_ATTR	    = 0,
	GFX_OPENGL_COLOR_ATTR	    = 1,
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
	proc_t *proc;
	void *gl_lib;
	alloc_t alloc;
	gfx_target_t target;
	unsigned int framebuffer;
	unsigned int texture;
	unsigned int triangle_buffer;
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
	void (*BindAttribLocation)(unsigned int, unsigned int, const char *);
	void (*LinkProgram)(unsigned int);
	void (*GetProgramiv)(unsigned int, unsigned int, int *);
	void (*GetProgramInfoLog)(unsigned int, int, int *, char *);
	void (*DeleteProgram)(unsigned int);
	void (*GenBuffers)(int, unsigned int *);
	void (*DeleteBuffers)(int, const unsigned int *);
	void (*BindBuffer)(unsigned int, unsigned int);
	void (*BufferData)(unsigned int, size_t, const void *, unsigned int);
	void (*UseProgram)(unsigned int);
	int (*GetUniformLocation)(unsigned int, const char *);
	void (*Uniform2f)(int, float, float);
	void (*EnableVertexAttribArray)(unsigned int);
	void (*DisableVertexAttribArray)(unsigned int);
	void (*VertexAttribPointer)(unsigned int, int, unsigned int, unsigned char, int, const void *);
	void (*DrawArrays)(unsigned int, int, int);
} gfx_opengl_t;

typedef struct gfx_opengl_shader_s {
	unsigned int program;
	int target_size;
} gfx_opengl_shader_t;

static void gfx_opengl_draw_free(gfx_opengl_t *opengl);
static int gfx_opengl_create_draw_state(gfx_opengl_t *opengl);

static int find_gl_symbol(gfx_opengl_t *opengl, gfx_surface_t *surface, void **sym, strv_t name)
{
	if (opengl->gl_lib != NULL && proc_dlsym(opengl->proc, opengl->gl_lib, name, sym) == 0) {
		return 0;
	}
	if (surface != NULL && surface->ops != NULL && surface->ops->proc != NULL) {
		if (surface->ops->proc(surface, name, sym) == 0) {
			return 0;
		}
	}
	return 1;
}

static int load_symbol(gfx_opengl_t *opengl, gfx_surface_t *surface, void **sym, strv_t name)
{
	if (find_gl_symbol(opengl, surface, sym, name) == 0) {
		return 0;
	}

	log_error("cgfx", "gfx_opengl", NULL, "failed to load OpenGL symbol: %.*s", name.len, name.data);
	return 1;
}

#define LOAD_GL(_opengl, _surface, _name)	   load_symbol((_opengl), (_surface), (void **)&(_opengl)->_name, STRV("gl" #_name))
#define LOAD_GL_OPTIONAL(_opengl, _surface, _name) find_gl_symbol((_opengl), (_surface), (void **)&(_opengl)->_name, STRV("gl" #_name))

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
	gfx_opengl_draw_free(opengl);
	gfx_opengl_clear_surface(surface);
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

static int gfx_opengl_surface_valid(gfx_surface_t *surface)
{
	return surface != NULL && surface->api == GFX_API_OPENGL && surface->ops != NULL && surface->ops->proc != NULL &&
	       surface->ops->make_current != NULL;
}

static int gfx_opengl_load_symbols(gfx_opengl_t *opengl, gfx_surface_t *surface)
{
	if (opengl == NULL) {
		return 1;
	}

	if (LOAD_GL(opengl, surface, ClearColor) || LOAD_GL(opengl, surface, Clear) || LOAD_GL(opengl, surface, GenFramebuffers) ||
	    LOAD_GL(opengl, surface, DeleteFramebuffers) || LOAD_GL(opengl, surface, BindFramebuffer) ||
	    LOAD_GL(opengl, surface, CheckFramebufferStatus) || LOAD_GL(opengl, surface, FramebufferTexture2D) ||
	    LOAD_GL(opengl, surface, GenTextures) || LOAD_GL(opengl, surface, DeleteTextures) || LOAD_GL(opengl, surface, BindTexture) ||
	    LOAD_GL(opengl, surface, TexParameteri) || LOAD_GL(opengl, surface, TexImage2D) || LOAD_GL(opengl, surface, Viewport) ||
	    LOAD_GL(opengl, surface, ReadPixels) || LOAD_GL(opengl, surface, CreateShader) || LOAD_GL(opengl, surface, ShaderSource) ||
	    LOAD_GL(opengl, surface, CompileShader) || LOAD_GL(opengl, surface, GetShaderiv) || LOAD_GL(opengl, surface, DeleteShader) ||
	    LOAD_GL(opengl, surface, CreateProgram) || LOAD_GL(opengl, surface, AttachShader) ||
	    LOAD_GL(opengl, surface, BindAttribLocation) || LOAD_GL(opengl, surface, LinkProgram) ||
	    LOAD_GL(opengl, surface, GetProgramiv) || LOAD_GL(opengl, surface, DeleteProgram) || LOAD_GL(opengl, surface, GenBuffers) ||
	    LOAD_GL(opengl, surface, DeleteBuffers) || LOAD_GL(opengl, surface, BindBuffer) || LOAD_GL(opengl, surface, BufferData) ||
	    LOAD_GL(opengl, surface, UseProgram) || LOAD_GL(opengl, surface, GetUniformLocation) || LOAD_GL(opengl, surface, Uniform2f) ||
	    LOAD_GL(opengl, surface, EnableVertexAttribArray) || LOAD_GL(opengl, surface, DisableVertexAttribArray) ||
	    LOAD_GL(opengl, surface, VertexAttribPointer) || LOAD_GL(opengl, surface, DrawArrays)) {
		return 1;
	}
	LOAD_GL_OPTIONAL(opengl, surface, GetShaderInfoLog);
	LOAD_GL_OPTIONAL(opengl, surface, GetProgramInfoLog);
	LOAD_GL_OPTIONAL(opengl, surface, GetError);
	LOAD_GL_OPTIONAL(opengl, surface, GetString);

	return 0;
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
		return gfx_opengl_init_free(gfx, opengl, NULL);
	}
	if (config->surface != NULL) {
		if (!gfx_opengl_surface_valid(config->surface)) {
			return gfx_opengl_init_free(gfx, opengl, NULL);
		}
		if (config->surface->ops->make_current(config->surface)) {
			log_error("cgfx", "gfx_opengl", NULL, "failed to make the OpenGL surface current");
			return gfx_opengl_init_free(gfx, opengl, NULL);
		}
	}
	if (gfx_opengl_load_symbols(opengl, config->surface)) {
		return gfx_opengl_init_free(gfx, opengl, config->surface);
	}
	if (gfx_opengl_clear_surface(config->surface)) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to clear the current OpenGL surface");
		return gfx_opengl_init_free(gfx, opengl, NULL);
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

static void gfx_opengl_draw_free(gfx_opengl_t *opengl)
{
	if (opengl->triangle_buffer != 0 && opengl->DeleteBuffers != NULL) {
		opengl->DeleteBuffers(1, &opengl->triangle_buffer);
		opengl->triangle_buffer = 0;
	}
}

static int gfx_opengl_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = gfx->data;
	gfx_opengl_draw_free(opengl);
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
	if (find_gl_symbol(opengl, opengl->surface, proc, name) == 0) {
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
		log_error("cgfx", "gfx_opengl", NULL, "failed to make OpenGL context current for %s: driver data is null", operation);
		return 1;
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

static const char *gfx_opengl_get_string(gfx_opengl_t *opengl, unsigned int name)
{
	if (opengl == NULL || opengl->GetString == NULL) {
		return NULL;
	}
	const unsigned char *str = opengl->GetString(name);
	return str != NULL ? (const char *)str : NULL;
}

static void gfx_opengl_log_context(gfx_opengl_t *opengl, const char *reason)
{
	if (opengl == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "%s: OpenGL driver data is null", reason);
		return;
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

static int gfx_opengl_create_draw_state(gfx_opengl_t *opengl)
{
	if (opengl->triangle_buffer != 0) {
		return 0;
	}

	unsigned int buffer = 0;
	opengl->GenBuffers(1, &buffer);
	if (buffer == 0) {
		return 1;
	}
	opengl->triangle_buffer = buffer;
	return 0;
}

static const char *gfx_opengl_shader_language_name(gfx_shader_language_t language)
{
	if (language == GFX_SHADER_LANGUAGE_GLSL) {
		return "GLSL 120";
	}
	return "unknown GLSL";
}

static int gfx_opengl_shader_build(gfx_opengl_t *opengl, gfx_shader_compiler_t *compiler, strv_t source, gfx_shader_language_t language,
				   gfx_opengl_shader_t *gl_shader, int log_errors)
{
	if (opengl == NULL || compiler == NULL || source.data == NULL || gl_shader == NULL) {
		if (log_errors) {
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to build OpenGL shader: invalid arguments opengl=%p compiler=%p source=%p shader=%p",
				  (void *)opengl,
				  (void *)compiler,
				  (const void *)source.data,
				  (void *)gl_shader);
		}
		return 1;
	}

	gfx_shader_code_t vertex   = {0};
	gfx_shader_code_t fragment = {0};
	if (gfx_shader_compiler_transpile(compiler, source, GFX_SHADER_STAGE_VERTEX, language, &vertex)) {
		if (log_errors) {
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to transpile OpenGL vertex shader to %s",
				  gfx_opengl_shader_language_name(language));
		}
		gfx_shader_code_free(&vertex);
		return 1;
	}
	if (gfx_shader_compiler_transpile(compiler, source, GFX_SHADER_STAGE_FRAGMENT, language, &fragment)) {
		if (log_errors) {
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to transpile OpenGL fragment shader to %s",
				  gfx_opengl_shader_language_name(language));
		}
		gfx_shader_code_free(&fragment);
		gfx_shader_code_free(&vertex);
		return 1;
	}

	unsigned int vertex_shader = gfx_opengl_compile_shader(opengl, GL_VERTEX_SHADER, vertex.text, log_errors);
	if (vertex_shader == 0) {
		if (log_errors) {
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to build OpenGL vertex shader for %s",
				  gfx_opengl_shader_language_name(language));
		}
		gfx_shader_code_free(&fragment);
		gfx_shader_code_free(&vertex);
		return 1;
	}
	unsigned int fragment_shader = gfx_opengl_compile_shader(opengl, GL_FRAGMENT_SHADER, fragment.text, log_errors);
	gfx_shader_code_free(&fragment);
	gfx_shader_code_free(&vertex);
	if (fragment_shader == 0) {
		if (log_errors) {
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to build OpenGL fragment shader for %s",
				  gfx_opengl_shader_language_name(language));
		}
		opengl->DeleteShader(vertex_shader);
		return 1;
	}

	unsigned int program = opengl->CreateProgram();
	if (program == 0) {
		if (log_errors) {
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to create OpenGL shader program for %s",
				  gfx_opengl_shader_language_name(language));
		}
		opengl->DeleteShader(fragment_shader);
		opengl->DeleteShader(vertex_shader);
		return 1;
	}

	opengl->AttachShader(program, vertex_shader);
	opengl->AttachShader(program, fragment_shader);
	opengl->BindAttribLocation(program, GFX_OPENGL_POS_ATTR, "a_pos");
	opengl->BindAttribLocation(program, GFX_OPENGL_COLOR_ATTR, "a_color");
	opengl->LinkProgram(program);
	opengl->DeleteShader(fragment_shader);
	opengl->DeleteShader(vertex_shader);

	int linked = 0;
	opengl->GetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		if (log_errors) {
			gfx_opengl_log_program_link(opengl, program);
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to link OpenGL shader program for %s",
				  gfx_opengl_shader_language_name(language));
		}
		opengl->DeleteProgram(program);
		return 1;
	}

	gl_shader->program     = program;
	gl_shader->target_size = opengl->GetUniformLocation(program, "u_target_size");
	if (gl_shader->target_size < 0) {
		if (log_errors) {
			log_error("cgfx",
				  "gfx_opengl",
				  NULL,
				  "failed to find OpenGL shader uniform for %s: u_target_size",
				  gfx_opengl_shader_language_name(language));
		}
		opengl->DeleteProgram(program);
		return 1;
	}

	return 0;
}

static int gfx_opengl_shader_init(gfx_shader_t *shader, const gfx_shader_config_t *config)
{
	if (shader == NULL || shader->gfx == NULL || shader->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl = shader->gfx->data;
	if (config->compiler == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to initialize OpenGL shader: shader compiler is unavailable");
		return 1;
	}

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

	gfx_opengl_shader_t *gl_shader = alloc_alloc(&opengl->alloc, sizeof(*gl_shader));
	if (gl_shader == NULL) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to allocate OpenGL shader");
		return 1;
	}
	*gl_shader = (gfx_opengl_shader_t){0};

	if (gfx_opengl_shader_build(opengl, config->compiler, config->source, GFX_SHADER_LANGUAGE_GLSL, gl_shader, 1) &&
	    gfx_opengl_shader_build(opengl, config->compiler, config->source, GFX_SHADER_LANGUAGE_GLSL, gl_shader, 1)) {
		log_error("cgfx", "gfx_opengl", NULL, "failed to initialize OpenGL shader: all GLSL variants failed");
		alloc_free(&opengl->alloc, gl_shader, sizeof(*gl_shader));
		return 1;
	}

	shader->data = gl_shader;
	return 0;
}

static void gfx_opengl_shader_free(gfx_shader_t *shader)
{
	if (shader == NULL || shader->gfx || shader->gfx->data || shader->data == NULL) {
		return;
	}
	gfx_opengl_t *opengl	       = shader->gfx->data;
	gfx_opengl_shader_t *gl_shader = shader->data;
	if (gfx_opengl_make_current(opengl, "shader destruction")) {
		return;
	}
	if (gl_shader->program != 0 && opengl->DeleteProgram != NULL) {
		opengl->DeleteProgram(gl_shader->program);
	}
	alloc_free(&opengl->alloc, gl_shader, sizeof(*gl_shader));
	shader->data = NULL;
}

static int gfx_opengl_draw_triangle_2d(const gfx_shader_t *shader, const gfx_vertex_2d_t vertices[3])
{
	if (shader == NULL || shader->gfx == NULL || shader->gfx->data == NULL || shader->data == NULL || vertices == NULL) {
		return 1;
	}

	gfx_opengl_t *opengl	       = shader->gfx->data;
	gfx_opengl_shader_t *gl_shader = shader->data;
	if (gfx_opengl_make_current(opengl, "triangle draw")) {
		return 1;
	}
	if (gfx_opengl_create_draw_state(opengl)) {
		return 1;
	}
	if (gfx_opengl_bind_target(opengl)) {
		return 1;
	}

	gfx_opengl_vertex_2d_t gl_vertices[3];
	for (u32 i = 0; i < 3; i++) {
		gl_vertices[i] = (gfx_opengl_vertex_2d_t){
			.x = vertices[i].x,
			.y = vertices[i].y,
			.r = vertices[i].r,
			.g = vertices[i].g,
			.b = vertices[i].b,
			.a = vertices[i].a,
		};
	}

	opengl->UseProgram(gl_shader->program);
	opengl->Uniform2f(gl_shader->target_size, (float)opengl->target.width, (float)opengl->target.height);
	opengl->BindBuffer(GL_ARRAY_BUFFER, opengl->triangle_buffer);
	opengl->BufferData(GL_ARRAY_BUFFER, sizeof(gl_vertices), gl_vertices, GL_DYNAMIC_DRAW);
	opengl->EnableVertexAttribArray(GFX_OPENGL_POS_ATTR);
	opengl->EnableVertexAttribArray(GFX_OPENGL_COLOR_ATTR);
	opengl->VertexAttribPointer(GFX_OPENGL_POS_ATTR, 2, GL_FLOAT, GL_FALSE, sizeof(gfx_opengl_vertex_2d_t), (const void *)0);
	opengl->VertexAttribPointer(
		GFX_OPENGL_COLOR_ATTR, 4, GL_FLOAT, GL_FALSE, sizeof(gfx_opengl_vertex_2d_t), (const void *)(2 * sizeof(float)));
	opengl->DrawArrays(GL_TRIANGLES, 0, 3);
	opengl->DisableVertexAttribArray(GFX_OPENGL_COLOR_ATTR);
	opengl->DisableVertexAttribArray(GFX_OPENGL_POS_ATTR);
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
	.name		  = "opengl",
	.api		  = GFX_API_OPENGL,
	.init		  = gfx_opengl_init,
	.free		  = gfx_opengl_free,
	.proc		  = gfx_opengl_proc,
	.set_target	  = gfx_opengl_set_target,
	.viewport	  = gfx_opengl_viewport,
	.clear_color	  = gfx_opengl_clear_color,
	.clear		  = gfx_opengl_clear,
	.shader_init	  = gfx_opengl_shader_init,
	.shader_free	  = gfx_opengl_shader_free,
	.draw_triangle_2d = gfx_opengl_draw_triangle_2d,
	.present	  = gfx_opengl_present,
};

GFX_DRIVER(gfx_opengl, &gfx_opengl);
