#include "gfx_driver.h"

#include "log.h"

typedef long HRESULT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef void *HMODULE;
typedef void *ID3DBlob;
typedef void *ID3D11Buffer;
typedef void *ID3D11Device;
typedef void *ID3D11DeviceContext;
typedef void *ID3D11InputLayout;
typedef void *ID3D11PixelShader;
typedef void *ID3D11RenderTargetView;
typedef void *ID3D11Texture2D;
typedef void *ID3D11VertexShader;
typedef void *IDXGISwapChain;
typedef int D3D_FEATURE_LEVEL;

enum {
	S_OK				 = 0,
	D3D_DRIVER_TYPE_HARDWARE	 = 1,
	D3D11_SDK_VERSION		 = 7,
	D3D11_USAGE_DEFAULT		 = 0,
	D3D11_BIND_VERTEX_BUFFER	 = 0x00000001,
	D3D11_INPUT_PER_VERTEX_DATA	 = 0,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
	DXGI_FORMAT_R32G32B32A32_FLOAT	 = 2,
	DXGI_FORMAT_R32G32_FLOAT	 = 16,
	DXGI_FORMAT_UNKNOWN		 = 0,
	GFX_D3D11_SWAPCHAIN_BUFFER_COUNT = 0,
};

typedef struct GUID_s {
	u32 Data1;
	u16 Data2;
	u16 Data3;
	u8 Data4[8];
} GUID;

typedef const GUID *REFIID;

typedef struct D3D11_VIEWPORT_s {
	float TopLeftX;
	float TopLeftY;
	float Width;
	float Height;
	float MinDepth;
	float MaxDepth;
} D3D11_VIEWPORT;

typedef struct D3D11_BUFFER_DESC_s {
	UINT ByteWidth;
	UINT Usage;
	UINT BindFlags;
	UINT CPUAccessFlags;
	UINT MiscFlags;
	UINT StructureByteStride;
} D3D11_BUFFER_DESC;

typedef struct D3D11_INPUT_ELEMENT_DESC_s {
	const char *SemanticName;
	UINT SemanticIndex;
	UINT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	UINT InputSlotClass;
	UINT InstanceDataStepRate;
} D3D11_INPUT_ELEMENT_DESC;

static const GUID IID_ID3D11Texture2D = {0x6f15aaf2u, 0xd208u, 0x4e89u, {0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c}};

typedef struct ID3D11DeviceVTable_s {
	HRESULT (*QueryInterface)(ID3D11Device *self, REFIID riid, void **object);
	ULONG (*AddRef)(ID3D11Device *self);
	ULONG (*Release)(ID3D11Device *self);
	HRESULT (*CreateBuffer)(ID3D11Device *self, const D3D11_BUFFER_DESC *desc, const void *initial_data, ID3D11Buffer **buffer);
	HRESULT (*CreateTexture1D)(void);
	HRESULT (*CreateTexture2D)(void);
	HRESULT (*CreateTexture3D)(void);
	HRESULT (*CreateShaderResourceView)(void);
	HRESULT (*CreateUnorderedAccessView)(void);
	HRESULT (*CreateRenderTargetView)(ID3D11Device *self, void *resource, const void *desc, ID3D11RenderTargetView **view);
	HRESULT (*CreateDepthStencilView)(void);
	HRESULT (*CreateInputLayout)(ID3D11Device *self, const D3D11_INPUT_ELEMENT_DESC *elements, UINT element_count,
				     const void *shader_bytecode, size_t bytecode_length, ID3D11InputLayout **input_layout);
	HRESULT (*CreateVertexShader)(ID3D11Device *self, const void *shader_bytecode, size_t bytecode_length, void *class_linkage,
				      ID3D11VertexShader **shader);
	HRESULT (*CreateGeometryShader)(void);
	HRESULT (*CreateGeometryShaderWithStreamOutput)(void);
	HRESULT (*CreatePixelShader)(ID3D11Device *self, const void *shader_bytecode, size_t bytecode_length, void *class_linkage,
				     ID3D11PixelShader **shader);
} ID3D11DeviceVTable;

typedef struct ID3D11DeviceContextVTable_s {
	HRESULT (*QueryInterface)(ID3D11DeviceContext *self, REFIID riid, void **object);
	ULONG (*AddRef)(ID3D11DeviceContext *self);
	ULONG (*Release)(ID3D11DeviceContext *self);
	void (*GetDevice)(void);
	void (*GetPrivateData)(void);
	void (*SetPrivateData)(void);
	void (*SetPrivateDataInterface)(void);
	void (*unused_07)(void);
	void (*unused_08)(void);
	void (*PSSetShader)(ID3D11DeviceContext *self, ID3D11PixelShader *shader, void *const *class_instances, UINT class_instance_count);
	void (*unused_10)(void);
	void (*VSSetShader)(ID3D11DeviceContext *self, ID3D11VertexShader *shader, void *const *class_instances, UINT class_instance_count);
	void (*unused_12)(void);
	void (*Draw)(ID3D11DeviceContext *self, UINT vertex_count, UINT start_vertex_location);
	void (*unused_14)(void);
	void (*unused_15)(void);
	void (*unused_16)(void);
	void (*IASetInputLayout)(ID3D11DeviceContext *self, ID3D11InputLayout *input_layout);
	void (*IASetVertexBuffers)(ID3D11DeviceContext *self, UINT start_slot, UINT num_buffers, ID3D11Buffer *const *buffers, const UINT *strides,
				   const UINT *offsets);
	void (*unused_19)(void);
	void (*unused_20)(void);
	void (*unused_21)(void);
	void (*unused_22)(void);
	void (*unused_23)(void);
	void (*IASetPrimitiveTopology)(ID3D11DeviceContext *self, UINT topology);
	void (*unused_25)(void);
	void (*unused_26)(void);
	void (*unused_27)(void);
	void (*unused_28)(void);
	void (*unused_29)(void);
	void (*unused_30)(void);
	void (*unused_31)(void);
	void (*unused_32)(void);
	void (*OMSetRenderTargets)(ID3D11DeviceContext *self, UINT num_views, ID3D11RenderTargetView *const *views, void *depth_stencil_view);
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
	void (*RSSetViewports)(ID3D11DeviceContext *self, UINT num_viewports, const D3D11_VIEWPORT *viewports);
	void (*unused_45)(void);
	void (*unused_46)(void);
	void (*unused_47)(void);
	void (*UpdateSubresource)(ID3D11DeviceContext *self, ID3D11Buffer *resource, UINT subresource, const void *box, const void *data,
				  UINT row_pitch, UINT depth_pitch);
	void (*unused_49)(void);
	void (*ClearRenderTargetView)(ID3D11DeviceContext *self, ID3D11RenderTargetView *view, const float color[4]);
} ID3D11DeviceContextVTable;

typedef struct ID3DBlobVTable_s {
	HRESULT (*QueryInterface)(ID3DBlob *self, REFIID riid, void **object);
	ULONG (*AddRef)(ID3DBlob *self);
	ULONG (*Release)(ID3DBlob *self);
	void *(*GetBufferPointer)(ID3DBlob *self);
	size_t (*GetBufferSize)(ID3DBlob *self);
} ID3DBlobVTable;

typedef struct ID3D11BufferVTable_s {
	HRESULT (*QueryInterface)(ID3D11Buffer *self, REFIID riid, void **object);
	ULONG (*AddRef)(ID3D11Buffer *self);
	ULONG (*Release)(ID3D11Buffer *self);
} ID3D11BufferVTable;

typedef struct ID3D11InputLayoutVTable_s {
	HRESULT (*QueryInterface)(ID3D11InputLayout *self, REFIID riid, void **object);
	ULONG (*AddRef)(ID3D11InputLayout *self);
	ULONG (*Release)(ID3D11InputLayout *self);
} ID3D11InputLayoutVTable;

typedef struct ID3D11PixelShaderVTable_s {
	HRESULT (*QueryInterface)(ID3D11PixelShader *self, REFIID riid, void **object);
	ULONG (*AddRef)(ID3D11PixelShader *self);
	ULONG (*Release)(ID3D11PixelShader *self);
} ID3D11PixelShaderVTable;

typedef struct ID3D11RenderTargetViewVTable_s {
	HRESULT (*QueryInterface)(ID3D11RenderTargetView *self, REFIID riid, void **object);
	ULONG (*AddRef)(ID3D11RenderTargetView *self);
	ULONG (*Release)(ID3D11RenderTargetView *self);
} ID3D11RenderTargetViewVTable;

typedef struct ID3D11Texture2DVTable_s {
	HRESULT (*QueryInterface)(ID3D11Texture2D *self, REFIID riid, void **object);
	ULONG (*AddRef)(ID3D11Texture2D *self);
	ULONG (*Release)(ID3D11Texture2D *self);
} ID3D11Texture2DVTable;

typedef struct ID3D11VertexShaderVTable_s {
	HRESULT (*QueryInterface)(ID3D11VertexShader *self, REFIID riid, void **object);
	ULONG (*AddRef)(ID3D11VertexShader *self);
	ULONG (*Release)(ID3D11VertexShader *self);
} ID3D11VertexShaderVTable;

typedef struct IDXGISwapChainVTable_s {
	HRESULT (*QueryInterface)(IDXGISwapChain *self, REFIID riid, void **object);
	ULONG (*AddRef)(IDXGISwapChain *self);
	ULONG (*Release)(IDXGISwapChain *self);
	HRESULT (*SetPrivateData)(void);
	HRESULT (*SetPrivateDataInterface)(void);
	HRESULT (*GetPrivateData)(void);
	HRESULT (*GetParent)(void);
	HRESULT (*GetDevice)(void);
	HRESULT (*Present)(IDXGISwapChain *self, UINT sync_interval, UINT flags);
	HRESULT (*GetBuffer)(IDXGISwapChain *self, UINT buffer, REFIID riid, void **surface);
	HRESULT (*SetFullscreenState)(void);
	HRESULT (*GetFullscreenState)(void);
	HRESULT (*GetDesc)(void);
	HRESULT (*ResizeBuffers)(IDXGISwapChain *self, UINT buffer_count, UINT width, UINT height, UINT format, UINT flags);
} IDXGISwapChainVTable;

typedef HRESULT (*PFN_D3D11CreateDevice)(void *adapter, UINT driver_type, HMODULE software, UINT flags,
					 const D3D_FEATURE_LEVEL *feature_levels, UINT feature_level_count, UINT sdk_version,
					 ID3D11Device **device, D3D_FEATURE_LEVEL *feature_level, ID3D11DeviceContext **context);
typedef HRESULT (*PFN_D3DCompile)(const void *src_data, size_t src_data_size, const char *source_name, const void *defines, void *include,
				  const char *entrypoint, const char *target, UINT flags1, UINT flags2, ID3DBlob **code,
				  ID3DBlob **error_msgs);

typedef struct gfx_d3d11_vertex_2d_s {
	float x;
	float y;
	float r;
	float g;
	float b;
	float a;
} gfx_d3d11_vertex_2d_t;

typedef struct gfx_d3d11_s {
	proc_t *proc;
	void *lib;
	void *compiler_lib;
	alloc_t alloc;
	gfx_target_t target;
	ID3D11Device *device;
	ID3D11DeviceContext *context;
	IDXGISwapChain *swapchain;
	ID3D11RenderTargetView *render_target;
	ID3D11InputLayout *triangle_input_layout;
	ID3D11VertexShader *triangle_vertex_shader;
	ID3D11PixelShader *triangle_pixel_shader;
	ID3D11Buffer *triangle_buffer;
	float color[4];
	PFN_D3D11CreateDevice D3D11CreateDevice;
	PFN_D3DCompile D3DCompile;
} gfx_d3d11_t;

static int hresult_ok(HRESULT hr)
{
	return hr >= 0;
}

static ULONG d3d11_release(void *object)
{
	void ***iface		 = object;
	ULONG (**vtable)(void *) = (ULONG(**)(void *)) * iface;
	return vtable[2](object);
}

static size_t cstr_len(const char *str)
{
	size_t len = 0;
	while (str[len] != '\0') {
		len++;
	}
	return len;
}

static void gfx_d3d11_draw_free(gfx_d3d11_t *d3d11)
{
	if (d3d11->triangle_buffer != NULL) {
		d3d11_release(d3d11->triangle_buffer);
	}
	if (d3d11->triangle_pixel_shader != NULL) {
		d3d11_release(d3d11->triangle_pixel_shader);
	}
	if (d3d11->triangle_vertex_shader != NULL) {
		d3d11_release(d3d11->triangle_vertex_shader);
	}
	if (d3d11->triangle_input_layout != NULL) {
		d3d11_release(d3d11->triangle_input_layout);
	}
	d3d11->triangle_buffer	      = NULL;
	d3d11->triangle_pixel_shader  = NULL;
	d3d11->triangle_vertex_shader = NULL;
	d3d11->triangle_input_layout  = NULL;
}

static int gfx_d3d11_init_free(gfx_t *gfx, gfx_d3d11_t *d3d11)
{
	gfx_d3d11_draw_free(d3d11);
	if (d3d11->context != NULL) {
		d3d11_release(d3d11->context);
	}
	if (d3d11->device != NULL) {
		d3d11_release(d3d11->device);
	}
	if (d3d11->compiler_lib != NULL) {
		proc_dlclose(d3d11->proc, d3d11->compiler_lib);
	}
	if (d3d11->lib != NULL) {
		proc_dlclose(d3d11->proc, d3d11->lib);
	}
	alloc_free(&d3d11->alloc, d3d11, sizeof(*d3d11));
	gfx->data = NULL;
	return 1;
}

static int gfx_d3d11_load(gfx_d3d11_t *d3d11)
{
	if (proc_dlopen(d3d11->proc, STRV("d3d11.dll"), &d3d11->lib)) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to load D3D11 library");
		return 1;
	}
	if (proc_dlsym(d3d11->proc, d3d11->lib, STRV("D3D11CreateDevice"), (void **)&d3d11->D3D11CreateDevice)) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to load D3D11 symbol: D3D11CreateDevice");
		return 1;
	}

	return 0;
}

static int gfx_d3d11_load_compiler(gfx_d3d11_t *d3d11)
{
	if (d3d11->D3DCompile != NULL) {
		return 0;
	}
	if (proc_dlopen(d3d11->proc, STRV("d3dcompiler_47.dll"), &d3d11->compiler_lib)) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to load D3DCompiler library");
		return 1;
	}
	if (proc_dlsym(d3d11->proc, d3d11->compiler_lib, STRV("D3DCompile"), (void **)&d3d11->D3DCompile)) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to load D3DCompiler symbol: D3DCompile");
		proc_dlclose(d3d11->proc, d3d11->compiler_lib);
		d3d11->compiler_lib = NULL;
		return 1;
	}

	return 0;
}

static int gfx_d3d11_init(gfx_t *gfx, const gfx_config_t *config)
{
	if (gfx == NULL || config == NULL || config->proc == NULL || config->alloc.alloc == NULL) {
		return 1;
	}

	alloc_t alloc	   = config->alloc;
	gfx_d3d11_t *d3d11 = alloc_alloc(&alloc, sizeof(*d3d11));
	if (d3d11 == NULL) {
		return 1;
	}
	*d3d11 = (gfx_d3d11_t){
		.proc  = config->proc,
		.alloc = alloc,
		.color = {0.0f, 0.0f, 0.0f, 1.0f},
	};
	gfx->data = d3d11;

	if (gfx_d3d11_load(d3d11)) {
		return gfx_d3d11_init_free(gfx, d3d11);
	}
	if (!hresult_ok(d3d11->D3D11CreateDevice(
		    NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &d3d11->device, NULL, &d3d11->context)) ||
	    d3d11->device == NULL || d3d11->context == NULL) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to create D3D11 device");
		return gfx_d3d11_init_free(gfx, d3d11);
	}

	return 0;
}

static void gfx_d3d11_target_free(gfx_d3d11_t *d3d11)
{
	if (d3d11->render_target != NULL) {
		d3d11_release(d3d11->render_target);
	}
	d3d11->render_target = NULL;
	d3d11->swapchain     = NULL;
	d3d11->target	     = (gfx_target_t){0};
}

static int gfx_d3d11_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = gfx->data;
	gfx_d3d11_draw_free(d3d11);
	gfx_d3d11_target_free(d3d11);
	d3d11_release(d3d11->context);
	d3d11_release(d3d11->device);
	if (d3d11->compiler_lib != NULL) {
		proc_dlclose(d3d11->proc, d3d11->compiler_lib);
	}
	if (d3d11->lib != NULL) {
		proc_dlclose(d3d11->proc, d3d11->lib);
	}
	alloc_free(&d3d11->alloc, d3d11, sizeof(*d3d11));
	gfx->data = NULL;
	return 0;
}

static int gfx_d3d11_native(gfx_t *gfx, gfx_native_t *native)
{
	if (gfx == NULL || gfx->data == NULL || native == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = gfx->data;

	*native = (gfx_native_t){
		.api	= GFX_API_D3D11,
		.device = (u64)(uintptr_t)d3d11->device,
	};
	return 0;
}

static int surface_target_valid(const gfx_target_t *target)
{
	return target != NULL && target->type == GFX_TARGET_SURFACE && target->format == GFX_FORMAT_RGBA8 && target->surface != NULL &&
	       target->surface->api == GFX_API_D3D11 && target->surface->handle != 0 && target->surface->ops != NULL &&
	       target->surface->ops->present != NULL && target->width != 0 && target->height != 0;
}

static int gfx_d3d11_create_render_target(gfx_d3d11_t *d3d11)
{
	ID3D11Texture2D *buffer	   = NULL;
	IDXGISwapChainVTable *swap = *(IDXGISwapChainVTable **)d3d11->swapchain;
	if (!hresult_ok(swap->GetBuffer(d3d11->swapchain, 0, &IID_ID3D11Texture2D, (void **)&buffer)) || buffer == NULL) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to get D3D11 swapchain buffer");
		return 1;
	}

	ID3D11DeviceVTable *device = *(ID3D11DeviceVTable **)d3d11->device;
	HRESULT hr		   = device->CreateRenderTargetView(d3d11->device, buffer, NULL, &d3d11->render_target);
	d3d11_release(buffer);
	if (!hresult_ok(hr) || d3d11->render_target == NULL) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to create D3D11 render target");
		return 1;
	}

	return 0;
}

static int gfx_d3d11_set_surface_target(gfx_d3d11_t *d3d11, const gfx_target_t *target)
{
	if (!surface_target_valid(target)) {
		return 1;
	}

	IDXGISwapChain *swapchain = (IDXGISwapChain *)(uintptr_t)target->surface->handle;
	if (d3d11->target.type == GFX_TARGET_SURFACE && d3d11->swapchain == swapchain && d3d11->target.width == target->width &&
	    d3d11->target.height == target->height) {
		d3d11->target = *target;
		return 0;
	}

	if (d3d11->render_target != NULL) {
		d3d11_release(d3d11->render_target);
		d3d11->render_target = NULL;
	}
	if (d3d11->target.type == GFX_TARGET_SURFACE && d3d11->swapchain == swapchain) {
		IDXGISwapChainVTable *swap = *(IDXGISwapChainVTable **)swapchain;
		if (!hresult_ok(swap->ResizeBuffers(
			    swapchain, GFX_D3D11_SWAPCHAIN_BUFFER_COUNT, target->width, target->height, DXGI_FORMAT_UNKNOWN, 0))) {
			log_error("cgfx", "gfx_d3d11", NULL, "failed to resize D3D11 swapchain");
			d3d11->target	 = (gfx_target_t){0};
			d3d11->swapchain = NULL;
			return 1;
		}
	}

	d3d11->swapchain = swapchain;
	if (gfx_d3d11_create_render_target(d3d11)) {
		gfx_d3d11_target_free(d3d11);
		return 1;
	}
	d3d11->target = *target;
	return 0;
}

static int gfx_d3d11_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || gfx->data == NULL || target == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = gfx->data;
	if (target->type == GFX_TARGET_NONE) {
		gfx_d3d11_target_free(d3d11);
		return 0;
	}
	if (target->type == GFX_TARGET_SURFACE) {
		return gfx_d3d11_set_surface_target(d3d11, target);
	}

	return 1;
}

static int gfx_d3d11_clear_color(gfx_t *gfx, float r, float g, float b, float a)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = gfx->data;
	d3d11->color[0]	   = r;
	d3d11->color[1]	   = g;
	d3d11->color[2]	   = b;
	d3d11->color[3]	   = a;
	return 0;
}

static int gfx_d3d11_viewport(gfx_t *gfx, u16 x, u16 y, u16 width, u16 height)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11		   = gfx->data;
	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	if (context->RSSetViewports == NULL) {
		return 1;
	}

	D3D11_VIEWPORT viewport = {
		.TopLeftX = (float)x,
		.TopLeftY = (float)y,
		.Width	  = (float)width,
		.Height	  = (float)height,
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f,
	};
	context->RSSetViewports(d3d11->context, 1, &viewport);
	return 0;
}

static int gfx_d3d11_clear(gfx_t *gfx, u32 buffers)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}
	if ((buffers & GFX_CLEAR_COLOR_BUFFER) == 0) {
		return 0;
	}

	gfx_d3d11_t *d3d11 = gfx->data;
	if (d3d11->target.type != GFX_TARGET_SURFACE || d3d11->render_target == NULL) {
		return 1;
	}

	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	context->ClearRenderTargetView(d3d11->context, d3d11->render_target, d3d11->color);
	return 0;
}

static void *d3d11_blob_data(ID3DBlob *blob)
{
	ID3DBlobVTable *vtable = *(ID3DBlobVTable **)blob;
	return vtable->GetBufferPointer(blob);
}

static size_t d3d11_blob_size(ID3DBlob *blob)
{
	ID3DBlobVTable *vtable = *(ID3DBlobVTable **)blob;
	return vtable->GetBufferSize(blob);
}

static int gfx_d3d11_compile_shader(gfx_d3d11_t *d3d11, const char *source, const char *target, ID3DBlob **code)
{
	ID3DBlob *errors = NULL;
	if (!hresult_ok(d3d11->D3DCompile(source, cstr_len(source), NULL, NULL, NULL, "main", target, 0, 0, code, &errors)) || *code == NULL) {
		if (errors != NULL) {
			d3d11_release(errors);
		}
		return 1;
	}
	if (errors != NULL) {
		d3d11_release(errors);
	}
	return 0;
}

static int gfx_d3d11_create_draw_state(gfx_d3d11_t *d3d11)
{
	static const char *vertex_source = "struct VSIn {\n"
					   "    float2 position : POSITION;\n"
					   "    float4 color : COLOR0;\n"
					   "};\n"
					   "struct VSOut {\n"
					   "    float4 position : SV_POSITION;\n"
					   "    float4 color : COLOR0;\n"
					   "};\n"
					   "VSOut main(VSIn input) {\n"
					   "    VSOut output;\n"
					   "    output.position = float4(input.position, 0.0, 1.0);\n"
					   "    output.color = input.color;\n"
					   "    return output;\n"
					   "}\n";
	static const char *pixel_source  = "struct PSIn {\n"
					   "    float4 position : SV_POSITION;\n"
					   "    float4 color : COLOR0;\n"
					   "};\n"
					   "float4 main(PSIn input) : SV_TARGET {\n"
					   "    return input.color;\n"
					   "}\n";

	if (d3d11->triangle_input_layout != NULL && d3d11->triangle_vertex_shader != NULL && d3d11->triangle_pixel_shader != NULL &&
	    d3d11->triangle_buffer != NULL) {
		return 0;
	}
	if (gfx_d3d11_load_compiler(d3d11)) {
		return 1;
	}

	ID3D11DeviceVTable *device = *(ID3D11DeviceVTable **)d3d11->device;
	if (device->CreateInputLayout == NULL || device->CreateVertexShader == NULL || device->CreatePixelShader == NULL ||
	    device->CreateBuffer == NULL) {
		return 1;
	}

	ID3DBlob *vertex_code = NULL;
	ID3DBlob *pixel_code  = NULL;
	if (gfx_d3d11_compile_shader(d3d11, vertex_source, "vs_4_0", &vertex_code)) {
		return 1;
	}
	if (gfx_d3d11_compile_shader(d3d11, pixel_source, "ps_4_0", &pixel_code)) {
		d3d11_release(vertex_code);
		return 1;
	}

	HRESULT hr = device->CreateVertexShader(
		d3d11->device, d3d11_blob_data(vertex_code), d3d11_blob_size(vertex_code), NULL, &d3d11->triangle_vertex_shader);
	if (!hresult_ok(hr) || d3d11->triangle_vertex_shader == NULL) {
		d3d11_release(pixel_code);
		d3d11_release(vertex_code);
		gfx_d3d11_draw_free(d3d11);
		return 1;
	}
	hr = device->CreatePixelShader(
		d3d11->device, d3d11_blob_data(pixel_code), d3d11_blob_size(pixel_code), NULL, &d3d11->triangle_pixel_shader);
	d3d11_release(pixel_code);
	if (!hresult_ok(hr) || d3d11->triangle_pixel_shader == NULL) {
		d3d11_release(vertex_code);
		gfx_d3d11_draw_free(d3d11);
		return 1;
	}

	D3D11_INPUT_ELEMENT_DESC elements[2] = {
		{
			.SemanticName	     = "POSITION",
			.Format		     = DXGI_FORMAT_R32G32_FLOAT,
			.InputSlotClass     = D3D11_INPUT_PER_VERTEX_DATA,
		},
		{
			.SemanticName	     = "COLOR",
			.Format		     = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.AlignedByteOffset  = 2 * sizeof(float),
			.InputSlotClass     = D3D11_INPUT_PER_VERTEX_DATA,
		},
	};
	hr = device->CreateInputLayout(
		d3d11->device, elements, 2, d3d11_blob_data(vertex_code), d3d11_blob_size(vertex_code), &d3d11->triangle_input_layout);
	d3d11_release(vertex_code);
	if (!hresult_ok(hr) || d3d11->triangle_input_layout == NULL) {
		gfx_d3d11_draw_free(d3d11);
		return 1;
	}

	D3D11_BUFFER_DESC buffer = {
		.ByteWidth = (UINT)(sizeof(gfx_d3d11_vertex_2d_t) * 3),
		.Usage	   = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
	};
	if (!hresult_ok(device->CreateBuffer(d3d11->device, &buffer, NULL, &d3d11->triangle_buffer)) || d3d11->triangle_buffer == NULL) {
		gfx_d3d11_draw_free(d3d11);
		return 1;
	}

	return 0;
}

static int gfx_d3d11_draw_triangle_2d(gfx_t *gfx, const gfx_vertex_2d_t vertices[3])
{
	if (gfx == NULL || gfx->data == NULL || vertices == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = gfx->data;
	if (d3d11->target.type != GFX_TARGET_SURFACE || d3d11->render_target == NULL || d3d11->target.width == 0 ||
	    d3d11->target.height == 0) {
		return 1;
	}
	if (gfx_d3d11_create_draw_state(d3d11)) {
		return 1;
	}

	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	if (context->OMSetRenderTargets == NULL || context->IASetInputLayout == NULL || context->IASetVertexBuffers == NULL ||
	    context->IASetPrimitiveTopology == NULL || context->VSSetShader == NULL || context->PSSetShader == NULL ||
	    context->UpdateSubresource == NULL || context->Draw == NULL) {
		return 1;
	}

	gfx_d3d11_vertex_2d_t d3d_vertices[3];
	for (u32 i = 0; i < 3; i++) {
		d3d_vertices[i] = (gfx_d3d11_vertex_2d_t){
			.x = vertices[i].x / (float)d3d11->target.width * 2.0f - 1.0f,
			.y = 1.0f - vertices[i].y / (float)d3d11->target.height * 2.0f,
			.r = vertices[i].r,
			.g = vertices[i].g,
			.b = vertices[i].b,
			.a = vertices[i].a,
		};
	}

	ID3D11RenderTargetView *views[1] = {d3d11->render_target};
	ID3D11Buffer *buffers[1]	       = {d3d11->triangle_buffer};
	UINT strides[1]		       = {sizeof(gfx_d3d11_vertex_2d_t)};
	UINT offsets[1]		       = {0};

	context->OMSetRenderTargets(d3d11->context, 1, views, NULL);
	context->IASetInputLayout(d3d11->context, d3d11->triangle_input_layout);
	context->IASetVertexBuffers(d3d11->context, 0, 1, buffers, strides, offsets);
	context->IASetPrimitiveTopology(d3d11->context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(d3d11->context, d3d11->triangle_vertex_shader, NULL, 0);
	context->PSSetShader(d3d11->context, d3d11->triangle_pixel_shader, NULL, 0);
	context->UpdateSubresource(d3d11->context, d3d11->triangle_buffer, 0, NULL, d3d_vertices, 0, 0);
	context->Draw(d3d11->context, 3, 0);
	return 0;
}

static int gfx_d3d11_present(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = gfx->data;
	if (d3d11->target.type != GFX_TARGET_SURFACE || d3d11->target.surface == NULL || d3d11->target.surface->ops == NULL ||
	    d3d11->target.surface->ops->present == NULL) {
		return 1;
	}

	return d3d11->target.surface->ops->present(d3d11->target.surface);
}

static gfx_driver_t gfx_d3d11 = {
	.name	     = "d3d11",
	.api	     = GFX_API_D3D11,
	.init	     = gfx_d3d11_init,
	.free	     = gfx_d3d11_free,
	.native	     = gfx_d3d11_native,
	.set_target  = gfx_d3d11_set_target,
	.viewport    = gfx_d3d11_viewport,
	.clear_color = gfx_d3d11_clear_color,
	.clear	     = gfx_d3d11_clear,
	.draw_triangle_2d = gfx_d3d11_draw_triangle_2d,
	.present     = gfx_d3d11_present,
};

GFX_DRIVER(gfx_d3d11, &gfx_d3d11);
