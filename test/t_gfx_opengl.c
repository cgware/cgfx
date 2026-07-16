#include "gfx_driver.h"

#include "log.h"
#include "test.h"

typedef void Display;
typedef void Visual;
typedef unsigned long XID;
typedef unsigned long Window;
typedef unsigned long VisualID;
typedef int Bool;

typedef struct XVisualInfo_s {
	Visual *visual;
	VisualID visualid;
	int screen;
	int depth;
	int class;
	unsigned long red_mask;
	unsigned long green_mask;
	unsigned long blue_mask;
	int colormap_size;
	int bits_per_rgb;
} XVisualInfo;

typedef XID GLXDrawable;
typedef void *GLXContext;
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
static int t_glx_create_context_calls;
static int t_glx_destroy_context_calls;
static int t_glx_make_current_calls;
static int t_glx_swap_buffers_calls;
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
static int t_glx_query_version_calls;
static Display *t_glx_display;
static XVisualInfo *t_glx_visual;
static GLXDrawable t_glx_drawable;
static GLXContext t_glx_context;
static GLXContext t_glx_create_context_ret;
static Bool t_glx_make_current_ret;
static XVisualInfo t_glx_visual_info = {
	.visualid = 7,
	.depth	  = 24,
};

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

static void t_glXQueryVersion(void)
{
	t_glx_query_version_calls++;
}

static GLXContext t_glXCreateContext(Display *display, XVisualInfo *visual, GLXContext share, Bool direct)
{
	(void)share;
	(void)direct;
	t_glx_create_context_calls++;
	t_glx_display = display;
	t_glx_visual  = visual;
	return t_glx_create_context_ret;
}

static void t_glXDestroyContext(Display *display, GLXContext context)
{
	t_glx_destroy_context_calls++;
	t_glx_display = display;
	t_glx_context = context;
}

static Bool t_glXMakeCurrent(Display *display, GLXDrawable drawable, GLXContext context)
{
	t_glx_make_current_calls++;
	t_glx_display  = display;
	t_glx_drawable = drawable;
	t_glx_context  = context;
	return t_glx_make_current_ret;
}

static void t_glXSwapBuffers(Display *display, GLXDrawable drawable)
{
	t_glx_swap_buffers_calls++;
	t_glx_display  = display;
	t_glx_drawable = drawable;
}

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
	t_glx_create_context_calls	    = 0;
	t_glx_destroy_context_calls	    = 0;
	t_glx_make_current_calls	    = 0;
	t_glx_swap_buffers_calls	    = 0;
	t_glx_query_version_calls	    = 0;
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
	t_glx_display			    = NULL;
	t_glx_visual			    = NULL;
	t_glx_drawable			    = 0;
	t_glx_context			    = NULL;
	t_glx_create_context_ret	    = (GLXContext)0x5555;
	t_glx_make_current_ret		    = 1;
}

static gfx_driver_t *t_gfx_opengl_driver(void)
{
	return gfx_driver_find(STRV("opengl"));
}

static void t_gfx_opengl_symbols(proc_t *proc)
{
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glClearColor"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glClear"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClear));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glGenFramebuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenFramebuffers));
	proc_setdlsym(
		proc, STRV("libGL.so.1"), STRV("glDeleteFramebuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteFramebuffers));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glBindFramebuffer"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindFramebuffer));
	proc_setdlsym(proc,
		      STRV("libGL.so.1"),
		      STRV("glCheckFramebufferStatus"),
		      t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCheckFramebufferStatus));
	proc_setdlsym(proc,
		      STRV("libGL.so.1"),
		      STRV("glFramebufferTexture2D"),
		      t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glFramebufferTexture2D));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glGenTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenTextures));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glDeleteTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteTextures));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glBindTexture"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindTexture));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glTexParameteri"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexParameteri));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glTexImage2D"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexImage2D));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glViewport"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glViewport));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glReadPixels"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glReadPixels));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glXQueryVersion"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glXQueryVersion));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glXCreateContext"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glXCreateContext));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glXDestroyContext"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glXDestroyContext));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glXMakeCurrent"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glXMakeCurrent));
	proc_setdlsym(proc, STRV("libGL.so.1"), STRV("glXSwapBuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glXSwapBuffers));
}

static void t_gfx_opengl_fallback_symbols(proc_t *proc)
{
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glClearColor"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glClear"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClear));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glGenFramebuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenFramebuffers));
	proc_setdlsym(
		proc, STRV("libGL.so"), STRV("glDeleteFramebuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteFramebuffers));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glBindFramebuffer"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindFramebuffer));
	proc_setdlsym(proc,
		      STRV("libGL.so"),
		      STRV("glCheckFramebufferStatus"),
		      t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCheckFramebufferStatus));
	proc_setdlsym(proc,
		      STRV("libGL.so"),
		      STRV("glFramebufferTexture2D"),
		      t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glFramebufferTexture2D));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glGenTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenTextures));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glDeleteTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteTextures));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glBindTexture"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindTexture));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glTexParameteri"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexParameteri));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glTexImage2D"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexImage2D));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glViewport"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glViewport));
	proc_setdlsym(proc, STRV("libGL.so"), STRV("glReadPixels"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glReadPixels));
}

static int t_gfx_opengl_init_gfx(gfx_t *gfx, proc_t *proc)
{
	t_gfx_opengl_reset();
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(proc);
	gfx_driver_t *drv = t_gfx_opengl_driver();
	return gfx_init(gfx, drv, &(gfx_config_t){.proc = proc, .alloc = ALLOC_STD}) != gfx;
}

TEST(gfx_opengl_driver_is_registered)
{
	START;

	EXPECT_NE(t_gfx_opengl_driver(), NULL);

	END;
}

TEST(gfx_opengl_init_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_opengl_init_null_config)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(drv->init(&gfx, NULL), 1);

	END;
}

TEST(gfx_opengl_init_null_proc)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NE(drv, NULL);

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
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = {.alloc = t_gfx_opengl_alloc_fail}}), NULL);

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
	EXPECT_NE(drv, NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), NULL);
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
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), &gfx);

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
	EXPECT_NE(drv, NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), NULL);
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
	EXPECT_NE(drv, NULL);

	EXPECT_EQ(gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), &gfx);

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
	EXPECT_NE(gfx.drv, NULL);

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
	EXPECT_NE(drv, NULL);
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
	EXPECT_NE(gfx.drv, NULL);

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	void *sym = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("glXQueryVersion"), &sym), 0);

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	void *sym = NULL;

	gfx_proc(&gfx, STRV("glXQueryVersion"), &sym);

	EXPECT_EQ(sym, t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glXQueryVersion));

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
	EXPECT_NE(gfx.drv, NULL);
	void *sym = NULL;

	EXPECT_EQ(gfx.drv->proc(&gfx, STRV("glXQueryVersion"), &sym), 1);

	END;
}

TEST(gfx_opengl_set_target_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NE(gfx.drv, NULL);
	u8 pixels[4] = {0};

	EXPECT_EQ(gfx.drv->set_target(&gfx,
				      &(gfx_target_t){
					      .type   = GFX_TARGET_MEMORY,
					      .format = GFX_FORMAT_RGBA8,
					      .data   = pixels,
					      .width  = 1,
					      .height = 1,
					      .stride = 4,
				      }),
		  1);

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4] = {0};

	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	 = GFX_TARGET_MEMORY,
					 .format = GFX_FORMAT_NONE,
					 .data	 = pixels,
					 .width	 = 1,
					 .height = 1,
					 .stride = 4,
				 }),
		  1);

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4] = {0};

	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	 = GFX_TARGET_MEMORY,
					 .format = GFX_FORMAT_RGBA8,
					 .data	 = pixels,
					 .width	 = 1,
					 .height = 1,
					 .stride = 4,
				 }),
		  0);

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4] = {0};

	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 1,
			       .height = 1,
			       .stride = 4,
		       });

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4] = {0};

	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 1,
			       .height = 1,
			       .stride = 4,
		       });

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4] = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	 = GFX_TARGET_MEMORY,
					 .format = GFX_FORMAT_RGBA8,
					 .data	 = pixels,
					 .width	 = 1,
					 .height = 1,
					 .stride = 4,
				 }),
		  1);
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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4] = {0};
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 1,
			       .height = 1,
			       .stride = 4,
		       });
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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4] = {0};
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 1,
			       .height = 1,
			       .stride = 4,
		       });
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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 0);
	EXPECT_EQ(t_gl_clear_calls, 1);
	EXPECT_EQ(t_gl_mask, 0x00004000u);

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[4] = {0};
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 1,
			       .height = 1,
			       .stride = 4,
		       });
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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[24] = {0};
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 3,
			       .height = 2,
			       .stride = 12,
		       });

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[16] = {0};
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 1,
			       .height = 2,
			       .stride = 8,
		       });

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	u8 pixels[16] = {0};
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type   = GFX_TARGET_MEMORY,
			       .format = GFX_FORMAT_RGBA8,
			       .data   = pixels,
			       .width  = 1,
			       .height = 2,
			       .stride = 8,
		       });

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_gl_read_pixels_first_y, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_creates_context)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});

	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	  = GFX_TARGET_SURFACE,
					 .format  = GFX_FORMAT_RGBA8,
					 .display = (void *)0x1234,
					 .visual  = &t_glx_visual_info,
					 .surface = 0x4321,
					 .width	  = 3,
					 .height  = 2,
				 }),
		  0);
	EXPECT_EQ(t_glx_create_context_calls, 1);

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});

	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type	= GFX_TARGET_SURFACE,
			       .format	= GFX_FORMAT_RGBA8,
			       .display = (void *)0x1234,
			       .visual	= &t_glx_visual_info,
			       .surface = 0x4321,
			       .width	= 3,
			       .height	= 2,
		       });

	EXPECT_EQ(t_glx_make_current_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_missing_glx_symbol)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_fallback_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	  = GFX_TARGET_SURFACE,
					 .format  = GFX_FORMAT_RGBA8,
					 .display = (void *)0x1234,
					 .visual  = &t_glx_visual_info,
					 .surface = 0x4321,
					 .width	  = 3,
					 .height  = 2,
				 }),
		  1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_create_context_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_glx_create_context_ret = NULL;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	  = GFX_TARGET_SURFACE,
					 .format  = GFX_FORMAT_RGBA8,
					 .display = (void *)0x1234,
					 .visual  = &t_glx_visual_info,
					 .surface = 0x4321,
					 .width	  = 3,
					 .height  = 2,
				 }),
		  1);
	log_set_quiet(0, 0);

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
	t_glx_make_current_ret = 0;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	  = GFX_TARGET_SURFACE,
					 .format  = GFX_FORMAT_RGBA8,
					 .display = (void *)0x1234,
					 .visual  = &t_glx_visual_info,
					 .surface = 0x4321,
					 .width	  = 3,
					 .height  = 2,
				 }),
		  1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_surface_target_reuses_context_on_resize)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type	= GFX_TARGET_SURFACE,
			       .format	= GFX_FORMAT_RGBA8,
			       .display = (void *)0x1234,
			       .visual	= &t_glx_visual_info,
			       .surface = 0x4321,
			       .width	= 3,
			       .height	= 2,
		       });
	t_glx_create_context_calls  = 0;
	t_glx_destroy_context_calls = 0;

	EXPECT_EQ(gfx_set_target(&gfx,
				 &(gfx_target_t){
					 .type	  = GFX_TARGET_SURFACE,
					 .format  = GFX_FORMAT_RGBA8,
					 .display = (void *)0x1234,
					 .visual  = &t_glx_visual_info,
					 .surface = 0x4321,
					 .width	  = 7,
					 .height  = 5,
				 }),
		  0);
	EXPECT_EQ(t_glx_create_context_calls, 0);
	EXPECT_EQ(t_glx_destroy_context_calls, 0);

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type	= GFX_TARGET_SURFACE,
			       .format	= GFX_FORMAT_RGBA8,
			       .display = (void *)0x1234,
			       .visual	= &t_glx_visual_info,
			       .surface = 0x4321,
			       .width	= 3,
			       .height	= 2,
		       });
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type	= GFX_TARGET_SURFACE,
			       .format	= GFX_FORMAT_RGBA8,
			       .display = (void *)0x1234,
			       .visual	= &t_glx_visual_info,
			       .surface = 0x4321,
			       .width	= 7,
			       .height	= 5,
		       });

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type	= GFX_TARGET_SURFACE,
			       .format	= GFX_FORMAT_RGBA8,
			       .display = (void *)0x1234,
			       .visual	= &t_glx_visual_info,
			       .surface = 0x4321,
			       .width	= 3,
			       .height	= 2,
		       });
	t_gl_bind_framebuffer_calls = 0;

	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_gl_bind_framebuffer_calls, 1);
	EXPECT_EQ(t_gl_framebuffer, 0u);

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type	= GFX_TARGET_SURFACE,
			       .format	= GFX_FORMAT_RGBA8,
			       .display = (void *)0x1234,
			       .visual	= &t_glx_visual_info,
			       .surface = 0x4321,
			       .width	= 3,
			       .height	= 2,
		       });

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
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type	= GFX_TARGET_SURFACE,
			       .format	= GFX_FORMAT_RGBA8,
			       .display = (void *)0x1234,
			       .visual	= &t_glx_visual_info,
			       .surface = 0x4321,
			       .width	= 3,
			       .height	= 2,
		       });

	EXPECT_EQ(gfx_present(&gfx), 0);
	EXPECT_EQ(t_glx_swap_buffers_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_free_destroys_context)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type	= GFX_TARGET_SURFACE,
			       .format	= GFX_FORMAT_RGBA8,
			       .display = (void *)0x1234,
			       .visual	= &t_glx_visual_info,
			       .surface = 0x4321,
			       .width	= 3,
			       .height	= 2,
		       });
	t_glx_destroy_context_calls = 0;

	gfx_free(&gfx);

	EXPECT_EQ(t_glx_destroy_context_calls, 1);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_set_target_none_destroys_context)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NE(drv, NULL);
	gfx_init(&gfx, drv, &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	gfx_set_target(&gfx,
		       &(gfx_target_t){
			       .type	= GFX_TARGET_SURFACE,
			       .format	= GFX_FORMAT_RGBA8,
			       .display = (void *)0x1234,
			       .visual	= &t_glx_visual_info,
			       .surface = 0x4321,
			       .width	= 3,
			       .height	= 2,
		       });
	t_glx_destroy_context_calls = 0;

	EXPECT_EQ(gfx_set_target(&gfx, &(gfx_target_t){.type = GFX_TARGET_NONE}), 0);
	EXPECT_EQ(t_glx_destroy_context_calls, 1);

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
	EXPECT_NE(gfx.drv, NULL);

	EXPECT_EQ(gfx.drv->clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_opengl_set_target_unknown_type)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx_set_target(&gfx, &(gfx_target_t){.type = (gfx_target_type_t)99}), 1);

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
	EXPECT_NE(gfx.drv, NULL);

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
	RUN(gfx_opengl_init_missing_clear);
	RUN(gfx_opengl_init_success);
	RUN(gfx_opengl_free_null_data);
	RUN(gfx_opengl_clear_color_calls_gl);
	RUN(gfx_opengl_clear_color_null_data);
	RUN(gfx_opengl_proc_loads_symbol);
	RUN(gfx_opengl_proc_sets_symbol);
	RUN(gfx_opengl_proc_null_data);
	RUN(gfx_opengl_set_target_null_data);
	RUN(gfx_opengl_set_target_invalid_format);
	RUN(gfx_opengl_set_target_success);
	RUN(gfx_opengl_set_target_unknown_type);
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
	RUN(gfx_opengl_set_surface_target_creates_context);
	RUN(gfx_opengl_set_surface_target_makes_current);
	RUN(gfx_opengl_set_surface_target_missing_glx_symbol);
	RUN(gfx_opengl_set_surface_target_create_context_failure);
	RUN(gfx_opengl_set_surface_target_make_current_failure);
	RUN(gfx_opengl_set_surface_target_reuses_context_on_resize);
	RUN(gfx_opengl_set_surface_target_updates_size_on_resize);
	RUN(gfx_opengl_clear_surface_binds_default_framebuffer);
	RUN(gfx_opengl_clear_surface_sets_viewport);
	RUN(gfx_opengl_present_swaps_buffers);
	RUN(gfx_opengl_free_destroys_context);
	RUN(gfx_opengl_set_target_none_destroys_context);
	RUN(gfx_opengl_clear_null_data);
	RUN(gfx_opengl_clear_without_target);
	RUN(gfx_opengl_present_without_surface_target);
	RUN(gfx_opengl_present_null_data);

	SEND;
}
