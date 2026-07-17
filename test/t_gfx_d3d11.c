#include "gfx_driver.h"

#include "log.h"
#include "test.h"

typedef long HRESULT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef void *HMODULE;
typedef int D3D_FEATURE_LEVEL;

enum {
	S_OK			   = 0,
	T_D3D_DRIVER_TYPE_HARDWARE = 1,
	T_D3D11_SDK_VERSION	   = 7,
};

typedef struct GUID_s {
	u32 Data1;
	u16 Data2;
	u16 Data3;
	u8 Data4[8];
} GUID;

typedef const GUID *REFIID;
typedef void (*t_gfx_d3d11_symbol_t)(void);

typedef struct t_d3d11_device_s t_d3d11_device_t;
typedef struct t_d3d11_context_s t_d3d11_context_t;
typedef struct t_d3d11_view_s t_d3d11_view_t;
typedef struct t_d3d11_texture_s t_d3d11_texture_t;
typedef struct t_dxgi_swapchain_s t_dxgi_swapchain_t;

typedef struct t_d3d11_device_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_device_t *self);
	HRESULT (*CreateBuffer)(void);
	HRESULT (*CreateTexture1D)(void);
	HRESULT (*CreateTexture2D)(void);
	HRESULT (*CreateTexture3D)(void);
	HRESULT (*CreateShaderResourceView)(void);
	HRESULT (*CreateUnorderedAccessView)(void);
	HRESULT (*CreateRenderTargetView)(t_d3d11_device_t *self, void *resource, const void *desc, t_d3d11_view_t **view);
} t_d3d11_device_vtbl_t;

typedef struct t_d3d11_context_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_context_t *self);
	void (*unused_03)(void);
	void (*unused_04)(void);
	void (*unused_05)(void);
	void (*unused_06)(void);
	void (*unused_07)(void);
	void (*unused_08)(void);
	void (*unused_09)(void);
	void (*unused_10)(void);
	void (*unused_11)(void);
	void (*unused_12)(void);
	void (*unused_13)(void);
	void (*unused_14)(void);
	void (*unused_15)(void);
	void (*unused_16)(void);
	void (*unused_17)(void);
	void (*unused_18)(void);
	void (*unused_19)(void);
	void (*unused_20)(void);
	void (*unused_21)(void);
	void (*unused_22)(void);
	void (*unused_23)(void);
	void (*unused_24)(void);
	void (*unused_25)(void);
	void (*unused_26)(void);
	void (*unused_27)(void);
	void (*unused_28)(void);
	void (*unused_29)(void);
	void (*unused_30)(void);
	void (*unused_31)(void);
	void (*unused_32)(void);
	void (*unused_33)(void);
	void (*unused_34)(void);
	void (*unused_35)(void);
	void (*unused_36)(void);
	void (*unused_37)(void);
	void (*unused_38)(void);
	void (*unused_39)(void);
	void (*unused_40)(void);
	void (*unused_41)(void);
	void (*unused_42)(void);
	void (*unused_43)(void);
	void (*unused_44)(void);
	void (*unused_45)(void);
	void (*unused_46)(void);
	void (*unused_47)(void);
	void (*unused_48)(void);
	void (*unused_49)(void);
	void (*ClearRenderTargetView)(t_d3d11_context_t *self, t_d3d11_view_t *view, const float color[4]);
} t_d3d11_context_vtbl_t;

typedef struct t_d3d11_view_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_view_t *self);
} t_d3d11_view_vtbl_t;

typedef struct t_d3d11_texture_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_texture_t *self);
} t_d3d11_texture_vtbl_t;

typedef struct t_dxgi_swapchain_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(void);
	HRESULT (*SetPrivateData)(void);
	HRESULT (*SetPrivateDataInterface)(void);
	HRESULT (*GetPrivateData)(void);
	HRESULT (*GetParent)(void);
	HRESULT (*GetDevice)(void);
	HRESULT (*Present)(t_dxgi_swapchain_t *self, UINT sync_interval, UINT flags);
	HRESULT (*GetBuffer)(t_dxgi_swapchain_t *self, UINT buffer, REFIID riid, void **surface);
	HRESULT (*SetFullscreenState)(void);
	HRESULT (*GetFullscreenState)(void);
	HRESULT (*GetDesc)(void);
	HRESULT (*ResizeBuffers)(t_dxgi_swapchain_t *self, UINT buffer_count, UINT width, UINT height, UINT format, UINT flags);
} t_dxgi_swapchain_vtbl_t;

struct t_d3d11_device_s {
	t_d3d11_device_vtbl_t *vtbl;
};

struct t_d3d11_context_s {
	t_d3d11_context_vtbl_t *vtbl;
};

struct t_d3d11_view_s {
	t_d3d11_view_vtbl_t *vtbl;
};

struct t_d3d11_texture_s {
	t_d3d11_texture_vtbl_t *vtbl;
};

struct t_dxgi_swapchain_s {
	t_dxgi_swapchain_vtbl_t *vtbl;
};

static int t_create_device_calls;
static int t_release_device_calls;
static int t_release_context_calls;
static int t_release_view_calls;
static int t_release_texture_calls;
static int t_create_render_target_view_calls;
static int t_get_buffer_calls;
static int t_resize_buffers_calls;
static int t_clear_render_target_view_calls;
static int t_surface_present_calls;
static UINT t_create_driver_type;
static UINT t_create_sdk_version;
static UINT t_resize_width;
static UINT t_resize_height;
static float t_clear_color[4];
static HRESULT t_create_device_ret;
static HRESULT t_get_buffer_ret;
static HRESULT t_create_render_target_view_ret;
static HRESULT t_resize_buffers_ret;
static t_d3d11_device_t t_device;
static t_d3d11_context_t t_context;
static t_d3d11_view_t t_view;
static t_d3d11_texture_t t_texture;
static t_dxgi_swapchain_t t_swapchain;
static gfx_surface_t t_surface;

static void *t_gfx_d3d11_alloc_fail(alloc_t *alloc, size_t size)
{
	(void)alloc;
	(void)size;
	return NULL;
}

static void *t_gfx_d3d11_symbol(t_gfx_d3d11_symbol_t fn)
{
	union {
		t_gfx_d3d11_symbol_t fn;
		void *ptr;
	} symbol = {.fn = fn};

	return symbol.ptr;
}

static ULONG t_device_release(t_d3d11_device_t *self)
{
	(void)self;
	t_release_device_calls++;
	return 0;
}

static ULONG t_context_release(t_d3d11_context_t *self)
{
	(void)self;
	t_release_context_calls++;
	return 0;
}

static ULONG t_view_release(t_d3d11_view_t *self)
{
	(void)self;
	t_release_view_calls++;
	return 0;
}

static ULONG t_texture_release(t_d3d11_texture_t *self)
{
	(void)self;
	t_release_texture_calls++;
	return 0;
}

static HRESULT t_CreateRenderTargetView(t_d3d11_device_t *self, void *resource, const void *desc, t_d3d11_view_t **view)
{
	(void)self;
	(void)resource;
	(void)desc;
	t_create_render_target_view_calls++;
	*view = &t_view;
	return t_create_render_target_view_ret;
}

static void t_ClearRenderTargetView(t_d3d11_context_t *self, t_d3d11_view_t *view, const float color[4])
{
	(void)self;
	(void)view;
	t_clear_render_target_view_calls++;
	t_clear_color[0] = color[0];
	t_clear_color[1] = color[1];
	t_clear_color[2] = color[2];
	t_clear_color[3] = color[3];
}

static HRESULT t_GetBuffer(t_dxgi_swapchain_t *self, UINT buffer, REFIID riid, void **surface)
{
	(void)self;
	(void)buffer;
	(void)riid;
	t_get_buffer_calls++;
	*surface = &t_texture;
	return t_get_buffer_ret;
}

static HRESULT t_ResizeBuffers(t_dxgi_swapchain_t *self, UINT buffer_count, UINT width, UINT height, UINT format, UINT flags)
{
	(void)self;
	(void)buffer_count;
	(void)format;
	(void)flags;
	t_resize_buffers_calls++;
	t_resize_width	= width;
	t_resize_height = height;
	return t_resize_buffers_ret;
}

static int t_surface_present(gfx_surface_t *surface)
{
	(void)surface;
	t_surface_present_calls++;
	return 0;
}

static HRESULT t_D3D11CreateDevice(void *adapter, UINT driver_type, HMODULE software, UINT flags, const D3D_FEATURE_LEVEL *feature_levels,
				   UINT feature_level_count, UINT sdk_version, t_d3d11_device_t **device, D3D_FEATURE_LEVEL *feature_level,
				   t_d3d11_context_t **context)
{
	(void)adapter;
	(void)software;
	(void)flags;
	(void)feature_levels;
	(void)feature_level_count;
	(void)feature_level;
	t_create_device_calls++;
	t_create_driver_type = driver_type;
	t_create_sdk_version = sdk_version;
	*device		     = &t_device;
	*context	     = &t_context;
	return t_create_device_ret;
}

static t_d3d11_device_vtbl_t t_device_vtbl = {
	.Release		= t_device_release,
	.CreateRenderTargetView = t_CreateRenderTargetView,
};

static t_d3d11_context_vtbl_t t_context_vtbl = {
	.Release	       = t_context_release,
	.ClearRenderTargetView = t_ClearRenderTargetView,
};

static t_d3d11_view_vtbl_t t_view_vtbl = {
	.Release = t_view_release,
};

static t_d3d11_texture_vtbl_t t_texture_vtbl = {
	.Release = t_texture_release,
};

static t_dxgi_swapchain_vtbl_t t_swapchain_vtbl = {
	.GetBuffer     = t_GetBuffer,
	.ResizeBuffers = t_ResizeBuffers,
};

static const gfx_surface_ops_t t_surface_ops = {
	.present = t_surface_present,
};

static void t_gfx_d3d11_reset(void)
{
	t_create_device_calls		  = 0;
	t_release_device_calls		  = 0;
	t_release_context_calls		  = 0;
	t_release_view_calls		  = 0;
	t_release_texture_calls		  = 0;
	t_create_render_target_view_calls = 0;
	t_get_buffer_calls		  = 0;
	t_resize_buffers_calls		  = 0;
	t_clear_render_target_view_calls  = 0;
	t_surface_present_calls		  = 0;
	t_create_driver_type		  = 0;
	t_create_sdk_version		  = 0;
	t_resize_width			  = 0;
	t_resize_height			  = 0;
	t_clear_color[0]		  = 0.0f;
	t_clear_color[1]		  = 0.0f;
	t_clear_color[2]		  = 0.0f;
	t_clear_color[3]		  = 0.0f;
	t_create_device_ret		  = S_OK;
	t_get_buffer_ret		  = S_OK;
	t_create_render_target_view_ret	  = S_OK;
	t_resize_buffers_ret		  = S_OK;
	t_device.vtbl			  = &t_device_vtbl;
	t_context.vtbl			  = &t_context_vtbl;
	t_view.vtbl			  = &t_view_vtbl;
	t_texture.vtbl			  = &t_texture_vtbl;
	t_swapchain.vtbl		  = &t_swapchain_vtbl;

	t_surface = (gfx_surface_t){
		.api	= GFX_API_D3D11,
		.handle = (u64)(uintptr_t)&t_swapchain,
		.ops	= &t_surface_ops,
	};
}

static gfx_driver_t *t_gfx_d3d11_driver(void)
{
	return gfx_driver_find(STRV("d3d11"));
}

static void t_gfx_d3d11_symbols(proc_t *proc)
{
	proc_setdlsym(proc, STRV("d3d11.dll"), STRV("D3D11CreateDevice"), t_gfx_d3d11_symbol((t_gfx_d3d11_symbol_t)t_D3D11CreateDevice));
}

static int t_gfx_d3d11_init_gfx(gfx_t *gfx, proc_t *proc)
{
	t_gfx_d3d11_reset();
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_d3d11_symbols(proc);
	return gfx_init(gfx, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = proc, .alloc = ALLOC_STD}) != gfx;
}

static int t_gfx_d3d11_set_surface_target(gfx_t *gfx, u16 width, u16 height)
{
	gfx_target_t target = {
		.type	 = GFX_TARGET_SURFACE,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_surface,
		.width	 = width,
		.height	 = height,
	};
	return gfx_set_target(gfx, &target);
}

TEST(gfx_d3d11_driver_is_registered)
{
	START;

	EXPECT_NE(t_gfx_d3d11_driver(), NULL);

	END;
}

TEST(gfx_d3d11_init_null_gfx)
{
	START;

	EXPECT_EQ(t_gfx_d3d11_driver()->init(NULL, &(gfx_config_t){.alloc = ALLOC_STD}), 1);

	END;
}

TEST(gfx_d3d11_init_null_config)
{
	START;

	EXPECT_EQ(t_gfx_d3d11_driver()->init(&(gfx_t){0}, NULL), 1);

	END;
}

TEST(gfx_d3d11_init_null_proc)
{
	START;

	EXPECT_EQ(t_gfx_d3d11_driver()->init(&(gfx_t){0}, &(gfx_config_t){.alloc = ALLOC_STD}), 1);

	END;
}

TEST(gfx_d3d11_init_null_alloc)
{
	START;

	proc_t proc = {0};

	EXPECT_EQ(t_gfx_d3d11_driver()->init(&(gfx_t){0}, &(gfx_config_t){.proc = &proc}), 1);

	END;
}

TEST(gfx_d3d11_init_alloc_failure)
{
	START;

	t_gfx_d3d11_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_d3d11_symbols(&proc);
	gfx_t gfx = {0};

	EXPECT_EQ(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = {.alloc = t_gfx_d3d11_alloc_fail}}), NULL);

	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_init_loads_library)
{
	START;

	t_gfx_d3d11_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	gfx_t gfx = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), NULL);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_init_missing_create_device_symbol)
{
	START;

	t_gfx_d3d11_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	proc_setdlsym(&proc, STRV("d3d11.dll"), STRV("unused"), &t_device);
	gfx_t gfx = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), NULL);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_init_create_device_failure)
{
	START;

	t_gfx_d3d11_reset();
	t_create_device_ret = -1;
	proc_t proc	    = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_d3d11_symbols(&proc);
	gfx_t gfx = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}), NULL);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_init_failure_releases_context)
{
	START;

	t_gfx_d3d11_reset();
	t_create_device_ret = -1;
	proc_t proc	    = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_d3d11_symbols(&proc);

	log_set_quiet(0, 1);
	gfx_init(&(gfx_t){0}, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	log_set_quiet(0, 0);

	EXPECT_EQ(t_release_context_calls, 1);

	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_init_failure_releases_device)
{
	START;

	t_gfx_d3d11_reset();
	t_create_device_ret = -1;
	proc_t proc	    = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_d3d11_symbols(&proc);

	log_set_quiet(0, 1);
	gfx_init(&(gfx_t){0}, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD});
	log_set_quiet(0, 0);

	EXPECT_EQ(t_release_device_calls, 1);

	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_init_creates_device)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(t_create_device_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_init_uses_hardware_driver)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(t_create_driver_type, T_D3D_DRIVER_TYPE_HARDWARE);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_init_uses_sdk_version)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(t_create_sdk_version, T_D3D11_SDK_VERSION);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_native_sets_api)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_native_t native = {0};
	gfx_native(&gfx, &native);

	EXPECT_EQ(native.api, GFX_API_D3D11);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_native_sets_device)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_native_t native = {0};
	gfx_native(&gfx, &native);

	EXPECT_EQ(native.device, (u64)(uintptr_t)&t_device);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_native_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	gfx_native_t native = {0};

	EXPECT_EQ(gfx.drv->native(&gfx, &native), 1);

	END;
}

TEST(gfx_d3d11_set_surface_target_gets_buffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);

	EXPECT_EQ(t_get_buffer_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_surface_target_creates_render_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);

	EXPECT_EQ(t_create_render_target_view_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_surface_target_releases_buffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);

	EXPECT_EQ(t_release_texture_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_surface_target_resizes_swapchain)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	t_gfx_d3d11_set_surface_target(&gfx, 800, 600);

	EXPECT_EQ(t_resize_buffers_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_surface_target_passes_resize_width)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	t_gfx_d3d11_set_surface_target(&gfx, 800, 600);

	EXPECT_EQ(t_resize_width, 800);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_target_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	gfx_target_t target = {0};

	EXPECT_EQ(gfx.drv->set_target(&gfx, &target), 1);

	END;
}

TEST(gfx_d3d11_set_target_unknown_type)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_target_t target = {
		.type = GFX_TARGET_MEMORY,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_target_none_clears_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);

	EXPECT_EQ(gfx_set_target(&gfx, &(gfx_target_t){.type = GFX_TARGET_NONE}), 0);
	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_surface_target_invalid_surface_api)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_surface_t surface = t_surface;
	surface.api	      = GFX_API_OPENGL;

	gfx_target_t target = {
		.type	 = GFX_TARGET_SURFACE,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &surface,
		.width	 = 640,
		.height	 = 480,
	};

	EXPECT_EQ(gfx_set_target(&gfx, &target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_surface_target_get_buffer_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_get_buffer_ret = -1;

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_set_surface_target(&gfx, 640, 480), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_surface_target_render_target_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_create_render_target_view_ret = -1;

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_set_surface_target(&gfx, 640, 480), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_surface_target_reuses_existing_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);

	EXPECT_EQ(t_get_buffer_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_set_surface_target_resize_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	t_resize_buffers_ret = -1;

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_set_surface_target(&gfx, 800, 600), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_clear_calls_context)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_clear_render_target_view_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_clear_uses_red)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_clear_color(&gfx, 0.25f, 0.5f, 0.75f, 1.0f);
	gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER);

	EXPECT_EQ(t_clear_color[0], 0.25f);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_clear_color_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};

	EXPECT_EQ(gfx.drv->clear_color(&gfx, 0.0f, 0.0f, 0.0f, 0.0f), 1);

	END;
}

TEST(gfx_d3d11_clear_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};

	EXPECT_EQ(gfx.drv->clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	END;
}

TEST(gfx_d3d11_clear_zero_buffers)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx_clear(&gfx, 0), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_clear_without_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_present_calls_surface)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_present(&gfx);

	EXPECT_EQ(t_surface_present_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_present_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};

	EXPECT_EQ(gfx.drv->present(&gfx), 1);

	END;
}

TEST(gfx_d3d11_present_without_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx_present(&gfx), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_free_releases_context)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	gfx_free(&gfx);

	EXPECT_EQ(t_release_context_calls, 1);

	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};

	EXPECT_EQ(gfx.drv->free(&gfx), 1);

	END;
}

TEST(gfx_d3d11_free_releases_device)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	gfx_free(&gfx);

	EXPECT_EQ(t_release_device_calls, 1);

	proc_free(&proc);
	END;
}

STEST(gfx_d3d11)
{
	SSTART;

	RUN(gfx_d3d11_driver_is_registered);
	RUN(gfx_d3d11_init_null_gfx);
	RUN(gfx_d3d11_init_null_config);
	RUN(gfx_d3d11_init_null_proc);
	RUN(gfx_d3d11_init_null_alloc);
	RUN(gfx_d3d11_init_alloc_failure);
	RUN(gfx_d3d11_init_loads_library);
	RUN(gfx_d3d11_init_missing_create_device_symbol);
	RUN(gfx_d3d11_init_create_device_failure);
	RUN(gfx_d3d11_init_failure_releases_context);
	RUN(gfx_d3d11_init_failure_releases_device);
	RUN(gfx_d3d11_init_creates_device);
	RUN(gfx_d3d11_init_uses_hardware_driver);
	RUN(gfx_d3d11_init_uses_sdk_version);
	RUN(gfx_d3d11_native_sets_api);
	RUN(gfx_d3d11_native_sets_device);
	RUN(gfx_d3d11_native_null_data);
	RUN(gfx_d3d11_set_target_null_data);
	RUN(gfx_d3d11_set_target_unknown_type);
	RUN(gfx_d3d11_set_target_none_clears_target);
	RUN(gfx_d3d11_set_surface_target_invalid_surface_api);
	RUN(gfx_d3d11_set_surface_target_get_buffer_failure);
	RUN(gfx_d3d11_set_surface_target_render_target_failure);
	RUN(gfx_d3d11_set_surface_target_gets_buffer);
	RUN(gfx_d3d11_set_surface_target_creates_render_target);
	RUN(gfx_d3d11_set_surface_target_releases_buffer);
	RUN(gfx_d3d11_set_surface_target_reuses_existing_target);
	RUN(gfx_d3d11_set_surface_target_resizes_swapchain);
	RUN(gfx_d3d11_set_surface_target_passes_resize_width);
	RUN(gfx_d3d11_set_surface_target_resize_failure);
	RUN(gfx_d3d11_clear_color_null_data);
	RUN(gfx_d3d11_clear_null_data);
	RUN(gfx_d3d11_clear_zero_buffers);
	RUN(gfx_d3d11_clear_without_target);
	RUN(gfx_d3d11_clear_calls_context);
	RUN(gfx_d3d11_clear_uses_red);
	RUN(gfx_d3d11_present_null_data);
	RUN(gfx_d3d11_present_without_target);
	RUN(gfx_d3d11_present_calls_surface);
	RUN(gfx_d3d11_free_null_data);
	RUN(gfx_d3d11_free_releases_context);
	RUN(gfx_d3d11_free_releases_device);

	SEND;
}
