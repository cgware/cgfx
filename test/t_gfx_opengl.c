#include "gfx_driver.h"

#include "log.h"
#include "test.h"

typedef void (*t_gfx_opengl_symbol_t)(void);

static int t_gl_clear_color_calls;
static int t_gl_clear_calls;
static int t_gl_gen_framebuffers_calls;
static int t_gl_delete_framebuffers_calls;
static int t_gl_bind_framebuffer_calls;
static int t_gl_check_framebuffer_status_calls;
static int t_gl_framebuffer_texture_2d_calls;
static int t_gl_gen_textures_calls;
static int t_gl_delete_textures_calls;
static int t_gl_bind_texture_calls;
static int t_gl_tex_parameteri_calls;
static int t_gl_tex_image_2d_calls;
static int t_gl_viewport_calls;
static int t_gl_read_pixels_calls;
static int t_surface_make_current_calls;
static int t_surface_clear_current_calls;
static int t_surface_present_calls;
static int t_gl_read_pixels_first_y;
static float t_gl_r;
static float t_gl_g;
static float t_gl_b;
static float t_gl_a;
static unsigned int t_gl_mask;
static unsigned int t_gl_framebuffer;
static unsigned int t_gl_texture;
static unsigned int t_gl_framebuffer_status;
static int t_gl_viewport_width;
static int t_gl_viewport_height;
static int t_gl_read_pixels_y;
static void *t_gl_read_pixels_data;
static int t_surface_make_current_ret;
static gfx_surface_t t_gfx_opengl_surface;

static void *t_gfx_opengl_alloc_fail(alloc_t *alloc, size_t size)
{
	(void)alloc;
	(void)size;
	return NULL;
}

static void *t_gfx_opengl_symbol(t_gfx_opengl_symbol_t fn)
{
	union {
		t_gfx_opengl_symbol_t fn;
		void *ptr;
	} symbol = {.fn = fn};

	return symbol.ptr;
}

static void t_glClearColor(float r, float g, float b, float a)
{
	t_gl_clear_color_calls++;
	t_gl_r = r;
	t_gl_g = g;
	t_gl_b = b;
	t_gl_a = a;
}

static void t_glClear(unsigned int mask)
{
	t_gl_clear_calls++;
	t_gl_mask = mask;
}

static void t_glGenFramebuffers(int count, unsigned int *framebuffers)
{
	t_gl_gen_framebuffers_calls++;
	if (count > 0) {
		*framebuffers = 22;
	}
}

static void t_glDeleteFramebuffers(int count, const unsigned int *framebuffers)
{
	(void)count;
	t_gl_delete_framebuffers_calls++;
	t_gl_framebuffer = *framebuffers;
}

static void t_glBindFramebuffer(unsigned int target, unsigned int framebuffer)
{
	(void)target;
	t_gl_bind_framebuffer_calls++;
	t_gl_framebuffer = framebuffer;
}

static unsigned int t_glCheckFramebufferStatus(unsigned int target)
{
	(void)target;
	t_gl_check_framebuffer_status_calls++;
	return t_gl_framebuffer_status;
}

static void t_glFramebufferTexture2D(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level)
{
	(void)target;
	(void)attachment;
	(void)textarget;
	(void)level;
	t_gl_framebuffer_texture_2d_calls++;
	t_gl_texture = texture;
}

static void t_glGenTextures(int count, unsigned int *textures)
{
	t_gl_gen_textures_calls++;
	if (count > 0) {
		*textures = 11;
	}
}

static void t_glDeleteTextures(int count, const unsigned int *textures)
{
	(void)count;
	t_gl_delete_textures_calls++;
	t_gl_texture = *textures;
}

static void t_glBindTexture(unsigned int target, unsigned int texture)
{
	(void)target;
	t_gl_bind_texture_calls++;
	t_gl_texture = texture;
}

static void t_glTexParameteri(unsigned int target, unsigned int name, int value)
{
	(void)target;
	(void)name;
	(void)value;
	t_gl_tex_parameteri_calls++;
}

static void t_glTexImage2D(unsigned int target, int level, int internal_format, int width, int height, int border, unsigned int format,
			   unsigned int type, const void *data)
{
	(void)target;
	(void)level;
	(void)internal_format;
	(void)width;
	(void)height;
	(void)border;
	(void)format;
	(void)type;
	(void)data;
	t_gl_tex_image_2d_calls++;
}

static void t_glViewport(int x, int y, int width, int height)
{
	(void)x;
	(void)y;
	t_gl_viewport_calls++;
	t_gl_viewport_width  = width;
	t_gl_viewport_height = height;
}

static void t_glReadPixels(int x, int y, int width, int height, unsigned int format, unsigned int type, void *data)
{
	(void)x;
	(void)width;
	(void)height;
	(void)format;
	(void)type;
	if (t_gl_read_pixels_calls == 0) {
		t_gl_read_pixels_first_y = y;
	}
	t_gl_read_pixels_calls++;
	t_gl_read_pixels_y    = y;
	t_gl_read_pixels_data = data;
}

static void *t_gfx_opengl_surface_symbol(strv_t name)
{
	if (strv_eq(name, STRV("glGenFramebuffers"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenFramebuffers);
	}
	if (strv_eq(name, STRV("glDeleteFramebuffers"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteFramebuffers);
	}
	if (strv_eq(name, STRV("glBindFramebuffer"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindFramebuffer);
	}
	if (strv_eq(name, STRV("glCheckFramebufferStatus"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCheckFramebufferStatus);
	}
	if (strv_eq(name, STRV("glFramebufferTexture2D"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glFramebufferTexture2D);
	}
	return NULL;
}

static int t_gfx_opengl_surface_proc(gfx_surface_t *surface, strv_t name, void **proc)
{
	(void)surface;
	*proc = t_gfx_opengl_surface_symbol(name);
	return *proc == NULL;
}

static int t_gfx_opengl_surface_make_current(gfx_surface_t *surface)
{
	(void)surface;
	t_surface_make_current_calls++;
	return !t_surface_make_current_ret;
}

static int t_gfx_opengl_surface_clear_current(gfx_surface_t *surface)
{
	(void)surface;
	t_surface_clear_current_calls++;
	return 0;
}

static int t_gfx_opengl_surface_present(gfx_surface_t *surface)
{
	(void)surface;
	t_surface_present_calls++;
	return 0;
}

static const gfx_surface_ops_t t_gfx_opengl_surface_ops = {
	.proc	       = t_gfx_opengl_surface_proc,
	.make_current  = t_gfx_opengl_surface_make_current,
	.clear_current = t_gfx_opengl_surface_clear_current,
	.present       = t_gfx_opengl_surface_present,
};

static void t_gfx_opengl_reset(void)
{
	t_gl_clear_color_calls		    = 0;
	t_gl_clear_calls		    = 0;
	t_gl_gen_framebuffers_calls	    = 0;
	t_gl_delete_framebuffers_calls	    = 0;
	t_gl_bind_framebuffer_calls	    = 0;
	t_gl_check_framebuffer_status_calls = 0;
	t_gl_framebuffer_texture_2d_calls   = 0;
	t_gl_gen_textures_calls		    = 0;
	t_gl_delete_textures_calls	    = 0;
	t_gl_bind_texture_calls		    = 0;
	t_gl_tex_parameteri_calls	    = 0;
	t_gl_tex_image_2d_calls		    = 0;
	t_gl_viewport_calls		    = 0;
	t_gl_read_pixels_calls		    = 0;
	t_surface_make_current_calls	    = 0;
	t_surface_clear_current_calls	    = 0;
	t_surface_present_calls		    = 0;
	t_gl_r				    = 0.0f;
	t_gl_g				    = 0.0f;
	t_gl_b				    = 0.0f;
	t_gl_a				    = 0.0f;
	t_gl_mask			    = 0;
	t_gl_framebuffer		    = 0;
	t_gl_texture			    = 0;
	t_gl_framebuffer_status		    = 0x8CD5;
	t_gl_viewport_width		    = 0;
	t_gl_viewport_height		    = 0;
	t_gl_read_pixels_first_y	    = 0;
	t_gl_read_pixels_y		    = 0;
	t_gl_read_pixels_data		    = NULL;
	t_surface_make_current_ret	    = 1;
	t_gfx_opengl_surface		    = (gfx_surface_t){
			       .api    = GFX_API_OPENGL,
			       .handle = 0x4321,
			       .ops    = &t_gfx_opengl_surface_ops,
	       };
}

static gfx_driver_t *t_gfx_opengl_driver(void)
{
	return gfx_driver_find(STRV("opengl"));
}

static void t_gfx_opengl_gl_symbols(proc_t *proc, strv_t lib)
{
	proc_setdlsym(proc, lib, STRV("glClearColor"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));
	proc_setdlsym(proc, lib, STRV("glClear"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClear));
	proc_setdlsym(proc, lib, STRV("glGenFramebuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenFramebuffers));
	proc_setdlsym(proc, lib, STRV("glDeleteFramebuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteFramebuffers));
	proc_setdlsym(proc, lib, STRV("glBindFramebuffer"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindFramebuffer));
	proc_setdlsym(proc, lib, STRV("glCheckFramebufferStatus"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCheckFramebufferStatus));
	proc_setdlsym(proc, lib, STRV("glFramebufferTexture2D"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glFramebufferTexture2D));
	proc_setdlsym(proc, lib, STRV("glGenTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenTextures));
	proc_setdlsym(proc, lib, STRV("glDeleteTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteTextures));
	proc_setdlsym(proc, lib, STRV("glBindTexture"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindTexture));
	proc_setdlsym(proc, lib, STRV("glTexParameteri"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexParameteri));
	proc_setdlsym(proc, lib, STRV("glTexImage2D"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexImage2D));
	proc_setdlsym(proc, lib, STRV("glViewport"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glViewport));
	proc_setdlsym(proc, lib, STRV("glReadPixels"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glReadPixels));
}

static void t_gfx_opengl_core_symbols(proc_t *proc, strv_t lib)
{
	proc_setdlsym(proc, lib, STRV("glClearColor"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));
	proc_setdlsym(proc, lib, STRV("glClear"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClear));
	proc_setdlsym(proc, lib, STRV("glGenTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenTextures));
	proc_setdlsym(proc, lib, STRV("glDeleteTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteTextures));
	proc_setdlsym(proc, lib, STRV("glBindTexture"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindTexture));
	proc_setdlsym(proc, lib, STRV("glTexParameteri"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexParameteri));
	proc_setdlsym(proc, lib, STRV("glTexImage2D"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexImage2D));
	proc_setdlsym(proc, lib, STRV("glViewport"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glViewport));
	proc_setdlsym(proc, lib, STRV("glReadPixels"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glReadPixels));
}

static void t_gfx_opengl_symbols(proc_t *proc)
{
	t_gfx_opengl_gl_symbols(proc, STRV("libGL.so.1"));
}

static void t_gfx_opengl_fallback_symbols(proc_t *proc)
{
	t_gfx_opengl_gl_symbols(proc, STRV("libGL.so"));
}

static void t_gfx_opengl_glvnd_symbols(proc_t *proc)
{
	t_gfx_opengl_gl_symbols(proc, STRV("libOpenGL.so.0"));
}

static int t_gfx_opengl_init_gfx(gfx_t *gfx, proc_t *proc)
{
	t_gfx_opengl_reset();
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(proc);
	gfx_driver_t *drv = t_gfx_opengl_driver();
	return gfx_init(gfx, drv, &(gfx_config_t){.proc = proc, .alloc = ALLOC_STD}) != gfx;
}

static gfx_target_t t_gfx_opengl_memory_target(u8 *pixels, u16 width, u16 height, size_t stride)
{
	return (gfx_target_t){
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= width,
		.height = height,
		.stride = stride,
	};
}

static gfx_target_t t_gfx_opengl_surface_target(gfx_surface_t *surface, u16 width, u16 height)
{
	return (gfx_target_t){
		.type	 = GFX_TARGET_SURFACE,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = surface,
		.width	 = width,
		.height	 = height,
	};
}

TEST(gfx_opengl_driver_is_registered)
{
	START;

	EXPECT_NOT_NULL(t_gfx_opengl_driver());

	END;
}

TEST(gfx_opengl_init_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_opengl_init_null_config)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(&gfx, NULL), 1);

	END;
}

TEST(gfx_opengl_init_null_proc)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(&gfx, &(gfx_config_t){.alloc = ALLOC_STD}), 1);

	END;
}

TEST(gfx_opengl_init_alloc_failure)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = {.alloc = t_gfx_opengl_alloc_fail}}));

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_missing_library)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_fallback_library)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_fallback_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_glvnd_libraries)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_glvnd_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_windows_library_name)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_gl_symbols(&proc, STRV("opengl32.dll"));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_missing_clear)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	proc_setdlsym(&proc, STRV("libGL.so.1"), STRV("glClearColor"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_success)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->free(&gfx), 1);

	END;
}

TEST(gfx_opengl_clear_color_calls_gl)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});

	EXPECT_EQ(gfx_clear_color(&gfx, 0.1f, 0.2f, 0.3f, 0.4f), 0);
	EXPECT_EQ(t_gl_clear_color_calls, 1);
	EXPECT_EQ(t_gl_r, 0.1f);
	EXPECT_EQ(t_gl_g, 0.2f);
	EXPECT_EQ(t_gl_b, 0.3f);
	EXPECT_EQ(t_gl_a, 0.4f);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_color_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->clear_color(&gfx, 0.0f, 0.0f, 0.0f, 0.0f), 1);

	END;
}

TEST(gfx_opengl_proc_loads_symbol)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	void *sym = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("glClearColor"), &sym), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_proc_sets_symbol)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	void *sym = NULL;

	gfx_proc(&gfx, STRV("glClearColor"), &sym);
	EXPECT_PTR(sym, t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_proc_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	void *sym = NULL;

	EXPECT_EQ(gfx.drv->proc(&gfx, STRV("glClearColor"), &sym), 1);

	END;
}

TEST(gfx_opengl_proc_uses_surface_proc)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_core_symbols(&proc, STRV("libGL.so.1"));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 1, 1);
	gfx_set_target(&gfx, &target);
	void *sym = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("glGenFramebuffers"), &sym), 0);
	EXPECT_PTR(sym, t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenFramebuffers));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_proc_missing_symbol)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	void *sym = NULL;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_proc(&gfx, STRV("glMissing"), &sym), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_target_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);

	EXPECT_EQ(gfx.drv->set_target(&gfx, &target), 1);

	END;
}

TEST(gfx_opengl_set_target_invalid_format)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = {
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_NONE,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_memory_target_missing_framebuffer_symbol)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_core_symbols(&proc, STRV("libGL.so.1"));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_target_success)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_target_reuses_framebuffer_symbols)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = {
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	gfx_set_target(&gfx, &target);

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_target_creates_texture)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);

	gfx_set_target(&gfx, &target);

	EXPECT_EQ(t_gl_gen_textures_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_target_creates_framebuffer)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);

	gfx_set_target(&gfx, &target);

	EXPECT_EQ(t_gl_gen_framebuffers_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_target_incomplete_framebuffer)
{
	START;

	t_gfx_opengl_reset();
	t_gl_framebuffer_status = 0;
	proc_t proc		= {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_free_deletes_framebuffer)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	gfx_set_target(&gfx, &target);
	t_gl_delete_framebuffers_calls = 0;

	gfx_free(&gfx);

	EXPECT_EQ(t_gl_delete_framebuffers_calls, 1);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_free_deletes_texture)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	gfx_set_target(&gfx, &target);
	t_gl_delete_textures_calls = 0;

	gfx_free(&gfx);

	EXPECT_EQ(t_gl_delete_textures_calls, 1);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_maps_color_buffer)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 0);
	EXPECT_EQ(t_gl_clear_calls, 1);
	EXPECT_EQ(t_gl_mask, 0x00004000);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_memory_binds_framebuffer)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	gfx_set_target(&gfx, &target);
	t_gl_bind_framebuffer_calls = 0;

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_gl_bind_framebuffer_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_memory_sets_viewport)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[24]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 3, 2, 12);
	gfx_set_target(&gfx, &target);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_gl_viewport_width, 3);
	EXPECT_EQ(t_gl_viewport_height, 2);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_memory_reads_rows)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[16]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 2, 8);
	gfx_set_target(&gfx, &target);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_gl_read_pixels_calls, 2);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_memory_flips_y)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[16]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 2, 8);
	gfx_set_target(&gfx, &target);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_gl_read_pixels_first_y, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_invalid_surface)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_surface_t surface = {
		.api = GFX_API_SOFTWARE,
		.ops = &t_gfx_opengl_surface_ops,
	};
	gfx_target_t target = t_gfx_opengl_surface_target(&surface, 1, 1);

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_accepts_surface)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_missing_framebuffer_symbol)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_core_symbols(&proc, STRV("libGL.so.1"));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_surface_t surface = {
		.api = GFX_API_OPENGL,
		.ops =
			&(gfx_surface_ops_t){
				.make_current = t_gfx_opengl_surface_make_current,
				.present      = t_gfx_opengl_surface_present,
			},
	};
	gfx_target_t target = t_gfx_opengl_surface_target(&surface, 1, 1);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_makes_current)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);

	gfx_set_target(&gfx, &target);

	EXPECT_EQ(t_surface_make_current_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_uses_library_framebuffer_symbols)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_fallback_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_uses_glvnd_library)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_glvnd_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);
	EXPECT_EQ(t_surface_make_current_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_uses_default_library)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);
	EXPECT_EQ(t_surface_make_current_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_present_calls_surface_present)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);
	gfx_set_target(&gfx, &target);

	EXPECT_EQ(gfx_present(&gfx), 0);
	EXPECT_EQ(t_surface_present_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_surface_make_current_ret = 0;
	gfx_target_t target	   = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_reuses_surface_on_resize)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t first_target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);
	gfx_set_target(&gfx, &first_target);
	t_surface_make_current_calls  = 0;
	t_surface_clear_current_calls = 0;
	gfx_target_t resized_target   = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 7, 5);

	EXPECT_EQ(gfx_set_target(&gfx, &resized_target), 0);
	EXPECT_EQ(t_surface_make_current_calls, 0);
	EXPECT_EQ(t_surface_clear_current_calls, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_updates_size_on_resize)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t first_target   = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);
	gfx_target_t resized_target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 7, 5);
	gfx_set_target(&gfx, &first_target);
	gfx_set_target(&gfx, &resized_target);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_gl_viewport_width, 7);
	EXPECT_EQ(t_gl_viewport_height, 5);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_surface_binds_default_framebuffer)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);
	gfx_set_target(&gfx, &target);
	t_gl_bind_framebuffer_calls = 0;

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_gl_bind_framebuffer_calls, 1);
	EXPECT_EQ(t_gl_framebuffer, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_surface_sets_viewport)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);
	gfx_set_target(&gfx, &target);

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_gl_viewport_width, 3);
	EXPECT_EQ(t_gl_viewport_height, 2);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_present_swaps_buffers)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);
	gfx_set_target(&gfx, &target);

	EXPECT_EQ(gfx_present(&gfx), 0);
	EXPECT_EQ(t_surface_present_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_free_clears_surface_current)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);
	gfx_set_target(&gfx, &target);
	t_surface_clear_current_calls = 0;

	gfx_free(&gfx);

	EXPECT_EQ(t_surface_clear_current_calls, 1);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_target_none_clears_surface_current)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_target_t surface_target = t_gfx_opengl_surface_target(&t_gfx_opengl_surface, 3, 2);
	gfx_set_target(&gfx, &surface_target);
	t_surface_clear_current_calls = 0;
	gfx_target_t target	      = {
			  .type = GFX_TARGET_NONE,
	  };

	EXPECT_EQ(gfx_set_target(&gfx, &target), 0);
	EXPECT_EQ(t_surface_clear_current_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_opengl_set_target_unknown_type)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_target_t target = {
		.type = (gfx_target_type_t)99,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_without_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_present_without_surface_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx_present(&gfx), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_present_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->present(&gfx), 1);

	END;
}

STEST(gfx_opengl)
{
	SSTART;

	RUN(gfx_opengl_driver_is_registered);
	RUN(gfx_opengl_init_null_gfx);
	RUN(gfx_opengl_init_null_config);
	RUN(gfx_opengl_init_null_proc);
	RUN(gfx_opengl_init_alloc_failure);
	RUN(gfx_opengl_init_missing_library);
	RUN(gfx_opengl_init_fallback_library);
	RUN(gfx_opengl_init_glvnd_libraries);
	RUN(gfx_opengl_init_windows_library_name);
	RUN(gfx_opengl_init_missing_clear);
	RUN(gfx_opengl_init_success);
	RUN(gfx_opengl_free_null_data);
	RUN(gfx_opengl_clear_color_calls_gl);
	RUN(gfx_opengl_clear_color_null_data);
	RUN(gfx_opengl_proc_loads_symbol);
	RUN(gfx_opengl_proc_sets_symbol);
	RUN(gfx_opengl_proc_null_data);
	RUN(gfx_opengl_proc_uses_surface_proc);
	RUN(gfx_opengl_proc_missing_symbol);
	RUN(gfx_opengl_set_target_null_data);
	RUN(gfx_opengl_set_target_invalid_format);
	RUN(gfx_opengl_set_memory_target_missing_framebuffer_symbol);
	RUN(gfx_opengl_set_target_success);
	RUN(gfx_opengl_set_target_unknown_type);
	RUN(gfx_opengl_set_target_reuses_framebuffer_symbols);
	RUN(gfx_opengl_set_target_creates_texture);
	RUN(gfx_opengl_set_target_creates_framebuffer);
	RUN(gfx_opengl_set_target_incomplete_framebuffer);
	RUN(gfx_opengl_free_deletes_framebuffer);
	RUN(gfx_opengl_free_deletes_texture);
	RUN(gfx_opengl_clear_maps_color_buffer);
	RUN(gfx_opengl_clear_memory_binds_framebuffer);
	RUN(gfx_opengl_clear_memory_sets_viewport);
	RUN(gfx_opengl_clear_memory_reads_rows);
	RUN(gfx_opengl_clear_memory_flips_y);
	RUN(gfx_opengl_set_surface_target_invalid_surface);
	RUN(gfx_opengl_set_surface_target_accepts_surface);
	RUN(gfx_opengl_set_surface_target_missing_framebuffer_symbol);
	RUN(gfx_opengl_set_surface_target_makes_current);
	RUN(gfx_opengl_set_surface_target_uses_library_framebuffer_symbols);
	RUN(gfx_opengl_set_surface_target_uses_glvnd_library);
	RUN(gfx_opengl_set_surface_target_uses_default_library);
	RUN(gfx_opengl_present_calls_surface_present);
	RUN(gfx_opengl_set_surface_target_make_current_failure);
	RUN(gfx_opengl_set_surface_target_reuses_surface_on_resize);
	RUN(gfx_opengl_set_surface_target_updates_size_on_resize);
	RUN(gfx_opengl_clear_surface_binds_default_framebuffer);
	RUN(gfx_opengl_clear_surface_sets_viewport);
	RUN(gfx_opengl_present_swaps_buffers);
	RUN(gfx_opengl_free_clears_surface_current);
	RUN(gfx_opengl_set_target_none_clears_surface_current);
	RUN(gfx_opengl_clear_null_data);
	RUN(gfx_opengl_clear_without_target);
	RUN(gfx_opengl_present_without_surface_target);
	RUN(gfx_opengl_present_null_data);

	SEND;
}
