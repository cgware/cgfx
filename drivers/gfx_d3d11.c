#include "gfx_driver.h"

#include "d3d11.h"
#include "log.h"
#include "mem.h"

enum {
	GFX_D3D11_SWAPCHAIN_BUFFER_COUNT = 0,
};

typedef struct gfx_d3d11_vertex_2d_s {
	float x;
	float y;
	float r;
	float g;
	float b;
	float a;
} gfx_d3d11_vertex_2d_t;

typedef struct gfx_d3d11_s {
	void *lib;
	void *compiler_lib;
	const gfx_target_t *target;
	ID3D11Device *device;
	ID3D11DeviceContext *context;
	PFN_D3D11CreateDevice D3D11CreateDevice;
	PFN_D3DCompile D3DCompile;
} gfx_d3d11_t;

typedef struct gfx_d3d11_render_pass_s {
	gfx_format_t color_format;
	gfx_load_op_t load;
	gfx_store_op_t store;
} gfx_d3d11_render_pass_t;

typedef struct gfx_d3d11_memory_target_s {
	ID3D11Texture2D *texture;
} gfx_d3d11_memory_target_t;

typedef struct gfx_d3d11_swapchain_s {
	IDXGISwapChain *swapchain;
} gfx_d3d11_swapchain_t;

typedef struct gfx_d3d11_framebuffer_s {
	ID3D11RenderTargetView *render_target;
} gfx_d3d11_framebuffer_t;

typedef struct gfx_d3d11_buffer_s {
	ID3D11Buffer *buffer;
	gfx_buffer_type_t type;
} gfx_d3d11_buffer_t;

typedef struct gfx_d3d11_shader_s {
	ID3DBlob *code;
	gfx_shader_stage_t stage;
	union {
		ID3D11VertexShader *vertex;
		ID3D11PixelShader *pixel;
	} shader;
} gfx_d3d11_shader_t;

typedef struct gfx_d3d11_pipeline_s {
	ID3D11InputLayout *input_layout;
	ID3D11VertexShader *vertex_shader;
	ID3D11PixelShader *pixel_shader;
	UINT stride;
} gfx_d3d11_pipeline_t;

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

static int gfx_d3d11_init_free(gfx_t *gfx, gfx_d3d11_t *d3d11)
{
	if (d3d11->context != NULL) {
		d3d11_release(d3d11->context);
	}
	if (d3d11->device != NULL) {
		d3d11_release(d3d11->device);
	}
	if (d3d11->compiler_lib != NULL) {
		proc_dlclose(gfx->proc, d3d11->compiler_lib); // LCOV_EXCL_LINE
	}
	if (d3d11->lib != NULL) {
		proc_dlclose(gfx->proc, d3d11->lib);
	}
	alloc_free(&gfx->alloc, d3d11, sizeof(gfx_d3d11_t));
	gfx->data = NULL;
	return 1;
}

static int gfx_d3d11_load(gfx_t *gfx)
{
	gfx_d3d11_t *d3d11 = gfx->data;
	if (proc_dlopen(gfx->proc, STRV("d3d11.dll"), &d3d11->lib)) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to load D3D11 library");
		return 1;
	}
	if (proc_dlsym(gfx->proc, d3d11->lib, STRV("D3D11CreateDevice"), (void **)&d3d11->D3D11CreateDevice)) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to load D3D11 symbol: D3D11CreateDevice");
		return 1;
	}

	return 0;
}

static int gfx_d3d11_load_compiler(gfx_t *gfx)
{
	gfx_d3d11_t *d3d11 = gfx->data;
	if (d3d11->D3DCompile != NULL) {
		return 0;
	}
	if (proc_dlopen(gfx->proc, STRV("d3dcompiler_47.dll"), &d3d11->compiler_lib)) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to load D3DCompiler library");
		return 1;
	}
	if (proc_dlsym(gfx->proc, d3d11->compiler_lib, STRV("D3DCompile"), (void **)&d3d11->D3DCompile)) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to load D3DCompiler symbol: D3DCompile");
		proc_dlclose(gfx->proc, d3d11->compiler_lib);
		d3d11->compiler_lib = NULL;
		return 1;
	}

	return 0;
}

static int gfx_d3d11_init(gfx_t *gfx, const gfx_config_t *config)
{
	if (gfx == NULL || config == NULL || gfx->proc == NULL || gfx->alloc.alloc == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = alloc_alloc(&gfx->alloc, sizeof(gfx_d3d11_t));
	if (d3d11 == NULL) {
		return 1;
	}
	*d3d11	  = (gfx_d3d11_t){0};
	gfx->data = d3d11;

	if (gfx_d3d11_load(gfx)) {
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

static int gfx_d3d11_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = gfx->data;
	d3d11_release(d3d11->context);
	d3d11_release(d3d11->device);
	if (d3d11->compiler_lib != NULL) {
		proc_dlclose(gfx->proc, d3d11->compiler_lib);
	}
	if (d3d11->lib != NULL) {
		proc_dlclose(gfx->proc, d3d11->lib);
	}
	alloc_free(&gfx->alloc, d3d11, sizeof(gfx_d3d11_t));
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

static int swapchain_valid(const gfx_swapchain_t *swapchain)
{
	return swapchain != NULL && swapchain->format == GFX_FORMAT_RGBA8 && swapchain->surface != NULL &&
	       swapchain->surface->api == GFX_API_D3D11 && swapchain->surface->handle != 0 && swapchain->surface->ops != NULL &&
	       swapchain->surface->ops->present != NULL && swapchain->width != 0 && swapchain->height != 0;
}

static int surface_target_valid(const gfx_target_t *target)
{
	return target != NULL && target->type == GFX_TARGET_SWAPCHAIN && target->swapchain != NULL && swapchain_valid(target->swapchain) &&
	       target->format == target->swapchain->format && target->width == target->swapchain->width &&
	       target->height == target->swapchain->height;
}

static int memory_target_valid(const gfx_target_t *target)
{
	return target != NULL && target->type == GFX_TARGET_MEMORY && target->format == GFX_FORMAT_RGBA8 && target->data != NULL &&
	       target->width != 0 && target->height != 0 && target->stride >= (size_t)target->width * 4;
}

static D3D11_TEXTURE2D_DESC gfx_d3d11_texture_desc(const gfx_target_t *target)
{
	return (D3D11_TEXTURE2D_DESC){
		.Width	    = target->width,
		.Height	    = target->height,
		.MipLevels  = 1,
		.ArraySize  = 1,
		.Format	    = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc = {.Count = 1},
	};
}

static int gfx_d3d11_create_render_target_view(gfx_d3d11_t *d3d11, gfx_d3d11_framebuffer_t *framebuffer, void *resource)
{
	ID3D11DeviceVTable *device = *(ID3D11DeviceVTable **)d3d11->device;
	if (device->CreateRenderTargetView == NULL) {
		return 1;
	}

	HRESULT hr = device->CreateRenderTargetView(d3d11->device, resource, NULL, &framebuffer->render_target);
	if (!hresult_ok(hr) || framebuffer->render_target == NULL) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to create D3D11 render target");
		return 1;
	}

	return 0;
}

static int gfx_d3d11_get_surface_buffer(gfx_d3d11_swapchain_t *swapchain, ID3D11Texture2D **buffer)
{
	IDXGISwapChainVTable *swap = *(IDXGISwapChainVTable **)swapchain->swapchain;
	if (buffer == NULL || !hresult_ok(swap->GetBuffer(swapchain->swapchain, 0, &IID_ID3D11Texture2D, (void **)buffer)) ||
	    *buffer == NULL) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to get D3D11 swapchain buffer");
		return 1;
	}

	return 0;
}

static int gfx_d3d11_create_memory_target(gfx_d3d11_t *d3d11, gfx_target_t *target)
{
	if (!memory_target_valid(target)) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_d3d11_memory_target_t *d3d_target = target->driver_data;
	ID3D11DeviceVTable *device	      = *(ID3D11DeviceVTable **)d3d11->device;
	if (device->CreateTexture2D == NULL) {
		return 1;
	}

	D3D11_TEXTURE2D_DESC desc = gfx_d3d11_texture_desc(target);
	desc.Usage		  = D3D11_USAGE_DEFAULT;
	desc.BindFlags		  = D3D11_BIND_RENDER_TARGET;
	if (!hresult_ok(device->CreateTexture2D(d3d11->device, &desc, NULL, &d3d_target->texture)) || d3d_target->texture == NULL) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to create D3D11 memory target texture");
		return 1;
	}

	return 0;
}

static void gfx_d3d11_target_free(gfx_target_t *target)
{
	if (target == NULL || target->gfx == NULL || target->gfx->data == NULL) {
		return;
	}

	gfx_d3d11_t *d3d11 = target->gfx->data;
	if (target->driver_data != NULL) {
		if (target->type == GFX_TARGET_SWAPCHAIN) {
			target->driver_data = NULL;
		} else {
			gfx_d3d11_memory_target_t *d3d_target = target->driver_data;
			if (d3d_target->texture != NULL) {
				d3d11_release(d3d_target->texture);
			}
			d3d_target->texture = NULL;
			alloc_free(&target->gfx->alloc, d3d_target, sizeof(gfx_d3d11_memory_target_t));
			target->driver_data = NULL;
		}
	}
	if (d3d11->target == target) {
		d3d11->target = NULL;
	}
}

static int gfx_d3d11_target_init(gfx_target_t *target)
{
	if (target == NULL || target->gfx == NULL || target->gfx->data == NULL ||
	    (!surface_target_valid(target) && !memory_target_valid(target))) {
		return 1;
	}

	if (target->type == GFX_TARGET_SWAPCHAIN) {
		target->driver_data = target->swapchain->data;
		return target->driver_data == NULL;
	}

	gfx_d3d11_t *d3d11		      = target->gfx->data;
	gfx_d3d11_memory_target_t *d3d_target = alloc_alloc(&target->gfx->alloc, sizeof(gfx_d3d11_memory_target_t));
	if (d3d_target == NULL) {
		return 1;
	}
	*d3d_target	    = (gfx_d3d11_memory_target_t){0};
	target->driver_data = d3d_target;
	int ret		    = gfx_d3d11_create_memory_target(d3d11, target);
	if (ret) {
		gfx_d3d11_target_free(target);
		return 1;
	}
	return 0;
}

static int gfx_d3d11_swapchain_init(gfx_swapchain_t *swapchain, const gfx_swapchain_config_t *config)
{
	(void)config;

	if (swapchain == NULL || swapchain->gfx == NULL || !swapchain_valid(swapchain)) {
		return 1;
	}

	gfx_d3d11_swapchain_t *d3d_swapchain = alloc_alloc(&swapchain->gfx->alloc, sizeof(gfx_d3d11_swapchain_t));
	if (d3d_swapchain == NULL) {
		return 1;
	}
	*d3d_swapchain	= (gfx_d3d11_swapchain_t){.swapchain = (IDXGISwapChain *)(uintptr_t)swapchain->surface->handle};
	swapchain->data = d3d_swapchain;
	return 0;
}

static void gfx_d3d11_swapchain_free(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->data == NULL) {
		return;
	}

	gfx_d3d11_swapchain_t *d3d_swapchain = swapchain->data;
	d3d_swapchain->swapchain	     = NULL;
	alloc_free(&swapchain->gfx->alloc, d3d_swapchain, sizeof(gfx_d3d11_swapchain_t));
	swapchain->data = NULL;
}

static int gfx_d3d11_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height)
{
	if (!swapchain_valid(swapchain) || swapchain->data == NULL) {
		return 1;
	}

	gfx_d3d11_swapchain_t *d3d_swapchain = swapchain->data;

	IDXGISwapChainVTable *swap = *(IDXGISwapChainVTable **)d3d_swapchain->swapchain;
	if (!hresult_ok(swap->ResizeBuffers(
		    d3d_swapchain->swapchain, GFX_D3D11_SWAPCHAIN_BUFFER_COUNT, width, height, DXGI_FORMAT_UNKNOWN, 0))) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to resize D3D11 swapchain");
		return 1;
	}

	return 0;
}

static int gfx_d3d11_swapchain_present(gfx_swapchain_t *swapchain)
{
	if (!swapchain_valid(swapchain)) {
		return 1;
	}

	return swapchain->surface->ops->present(swapchain->surface);
}

static void gfx_d3d11_render_pass_free(gfx_render_pass_t *render_pass)
{
	if (render_pass == NULL || render_pass->gfx == NULL || render_pass->data == NULL) {
		return;
	}

	alloc_free(&render_pass->gfx->alloc, render_pass->data, sizeof(gfx_d3d11_render_pass_t));
	render_pass->data = NULL;
}

static int gfx_d3d11_render_pass_init(gfx_render_pass_t *render_pass, const gfx_render_pass_config_t *config)
{
	if (render_pass == NULL || render_pass->gfx == NULL || config == NULL || config->color_format != GFX_FORMAT_RGBA8) {
		return 1;
	}

	gfx_d3d11_render_pass_t *d3d_render_pass = alloc_alloc(&render_pass->gfx->alloc, sizeof(gfx_d3d11_render_pass_t));
	if (d3d_render_pass == NULL) {
		return 1;
	}
	*d3d_render_pass = (gfx_d3d11_render_pass_t){
		.color_format = config->color_format,
		.load	      = config->load,
		.store	      = config->store,
	};
	render_pass->data = d3d_render_pass;
	return 0;
}

static int gfx_d3d11_target_read(gfx_target_t *target, const gfx_memory_readback_config_t *config)
{
	if (!memory_target_valid(target) || target->gfx == NULL || target->gfx->data == NULL || config == NULL || config->data == NULL ||
	    config->stride < (size_t)target->width * 4) {
		return 1;
	}

	gfx_d3d11_t *d3d11		      = target->gfx->data;
	gfx_d3d11_memory_target_t *d3d_target = target->driver_data;
	if (d3d11->target != target || d3d_target == NULL || d3d_target->texture == NULL) {
		return 1;
	}

	ID3D11DeviceVTable *device	   = *(ID3D11DeviceVTable **)d3d11->device;
	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	if (device->CreateTexture2D == NULL || context->CopyResource == NULL || context->Map == NULL || context->Unmap == NULL) {
		return 1;
	}

	D3D11_TEXTURE2D_DESC desc = gfx_d3d11_texture_desc(target);
	desc.Usage		  = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags	  = D3D11_CPU_ACCESS_READ;

	ID3D11Texture2D *staging = NULL;
	if (!hresult_ok(device->CreateTexture2D(d3d11->device, &desc, NULL, &staging)) || staging == NULL) {
		log_error("cgfx", "gfx_d3d11", NULL, "failed to create D3D11 readback texture");
		return 1;
	}

	context->CopyResource(d3d11->context, staging, d3d_target->texture);

	D3D11_MAPPED_SUBRESOURCE mapped = {0};
	if (!hresult_ok(context->Map(d3d11->context, staging, 0, D3D11_MAP_READ, 0, &mapped)) || mapped.pData == NULL) {
		d3d11_release(staging);
		return 1;
	}
	if (mapped.RowPitch < (UINT)target->width * 4) {
		context->Unmap(d3d11->context, staging, 0);
		d3d11_release(staging);
		return 1;
	}

	for (u16 y = 0; y < target->height; y++) {
		u8 *dst	      = (u8 *)config->data + (size_t)y * config->stride;
		const u8 *src = (const u8 *)mapped.pData + (size_t)y * mapped.RowPitch;
		mem_copy(dst, config->stride, src, (size_t)target->width * 4);
	}

	context->Unmap(d3d11->context, staging, 0);
	d3d11_release(staging);
	return 0;
}

static void gfx_d3d11_framebuffer_free(gfx_framebuffer_t *framebuffer)
{
	if (framebuffer == NULL || framebuffer->data == NULL) {
		return;
	}

	gfx_d3d11_framebuffer_t *d3d_framebuffer = framebuffer->data;
	if (d3d_framebuffer->render_target != NULL) {
		d3d11_release(d3d_framebuffer->render_target);
		d3d_framebuffer->render_target = NULL;
	}
	alloc_free(&framebuffer->gfx->alloc, d3d_framebuffer, sizeof(gfx_d3d11_framebuffer_t));
	framebuffer->data = NULL;
}

static int gfx_d3d11_framebuffer_init(gfx_framebuffer_t *framebuffer)
{
	if (framebuffer == NULL || framebuffer->gfx == NULL || framebuffer->gfx->data == NULL || framebuffer->target == NULL ||
	    framebuffer->target->driver_data == NULL || framebuffer->render_pass == NULL || framebuffer->render_pass->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11			 = framebuffer->gfx->data;
	gfx_d3d11_framebuffer_t *d3d_framebuffer = alloc_alloc(&framebuffer->gfx->alloc, sizeof(gfx_d3d11_framebuffer_t));
	if (d3d_framebuffer == NULL) {
		return 1;
	}
	*d3d_framebuffer  = (gfx_d3d11_framebuffer_t){0};
	framebuffer->data = d3d_framebuffer;

	switch (framebuffer->target->type) {
	case GFX_TARGET_MEMORY: {
		gfx_d3d11_memory_target_t *d3d_target = framebuffer->target->driver_data;
		if (d3d_target->texture == NULL || gfx_d3d11_create_render_target_view(d3d11, d3d_framebuffer, d3d_target->texture)) {
			gfx_d3d11_framebuffer_free(framebuffer);
			return 1;
		}
		break;
	}
	case GFX_TARGET_SWAPCHAIN: {
		gfx_d3d11_swapchain_t *d3d_swapchain = framebuffer->target->driver_data;
		if (d3d_swapchain->swapchain == NULL) {
			gfx_d3d11_framebuffer_free(framebuffer);
			return 1;
		}
		ID3D11Texture2D *buffer = NULL;
		if (gfx_d3d11_get_surface_buffer(d3d_swapchain, &buffer) ||
		    gfx_d3d11_create_render_target_view(d3d11, d3d_framebuffer, buffer)) {
			if (buffer != NULL) {
				d3d11_release(buffer);
			}
			gfx_d3d11_framebuffer_free(framebuffer);
			return 1;
		}
		d3d11_release(buffer);
		break;
	}
	default:
		gfx_d3d11_framebuffer_free(framebuffer);
		return 1;
	}

	return 0;
}

static int gfx_d3d11_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || framebuffer == NULL || framebuffer->target == NULL ||
	    framebuffer->render_pass == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11			 = frame->gfx->data;
	d3d11->target				 = framebuffer->target;
	void *d3d_target			 = d3d11->target->driver_data;
	gfx_d3d11_framebuffer_t *d3d_framebuffer = framebuffer->data;
	if ((d3d11->target->type != GFX_TARGET_SWAPCHAIN && d3d11->target->type != GFX_TARGET_MEMORY) || d3d_target == NULL ||
	    d3d_framebuffer == NULL || d3d_framebuffer->render_target == NULL || d3d11->target->width == 0 || d3d11->target->height == 0) {
		return 1;
	}

	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	if (context->OMSetRenderTargets == NULL || context->RSSetViewports == NULL) {
		return 1;
	}

	ID3D11RenderTargetView *views[1] = {d3d_framebuffer->render_target};
	context->OMSetRenderTargets(d3d11->context, 1, views, NULL);
	D3D11_VIEWPORT viewport = {
		.TopLeftX = (float)frame->pass.viewport.x,
		.TopLeftY = (float)frame->pass.viewport.y,
		.Width	  = (float)frame->pass.viewport.width,
		.Height	  = (float)frame->pass.viewport.height,
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f,
	};
	context->RSSetViewports(d3d11->context, 1, &viewport);
	if (framebuffer->render_pass->load == GFX_LOAD_CLEAR) {
		if (context->ClearRenderTargetView == NULL) {
			return 1;
		}
		float color[4] = {
			frame->pass.clear.r,
			frame->pass.clear.g,
			frame->pass.clear.b,
			frame->pass.clear.a,
		};
		context->ClearRenderTargetView(d3d11->context, d3d_framebuffer->render_target, color);
	}
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
	if (!hresult_ok(d3d11->D3DCompile(source, cstr_len(source), NULL, NULL, NULL, "main", target, 0, 0, code, &errors)) ||
	    *code == NULL) {
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

static void gfx_d3d11_buffer_free(gfx_buffer_t *buffer)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->data == NULL) {
		return;
	}

	gfx_d3d11_buffer_t *d3d_buffer = buffer->data;

	if (d3d_buffer->buffer != NULL) {
		d3d11_release(d3d_buffer->buffer);
		d3d_buffer->buffer = NULL;
	}
	alloc_free(&buffer->gfx->alloc, d3d_buffer, sizeof(gfx_d3d11_buffer_t));
	buffer->data = NULL;
}

static int gfx_d3d11_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = buffer->gfx->data;

	ID3D11DeviceVTable *device = *(ID3D11DeviceVTable **)d3d11->device;
	if (device->CreateBuffer == NULL) {
		return 1;
	}

	UINT flags = 0;
	switch (config->type) {
	case GFX_BUFFER_VERTEX: {
		flags |= D3D11_BIND_VERTEX_BUFFER;
		break;
	}
	case GFX_BUFFER_INDEX: {
		flags |= D3D11_BIND_INDEX_BUFFER;
		break;
	}
	default: {
		log_error("cgfx", "gfx_d3d11", NULL, "unsupported buffer type: %d", config->type);
		return 1;
	}
	}

	gfx_d3d11_buffer_t *d3d_buffer = alloc_alloc(&buffer->gfx->alloc, sizeof(gfx_d3d11_buffer_t));
	if (d3d_buffer == NULL) {
		return 1;
	}
	*d3d_buffer = (gfx_d3d11_buffer_t){
		.type = config->type,
	};
	buffer->data = d3d_buffer;

	D3D11_BUFFER_DESC desc = {
		.ByteWidth = (UINT)(sizeof(gfx_d3d11_vertex_2d_t) * 3),
		.Usage	   = D3D11_USAGE_DEFAULT,
		.BindFlags = flags,
	};
	if (!hresult_ok(device->CreateBuffer(d3d11->device, &desc, NULL, &d3d_buffer->buffer)) || d3d_buffer->buffer == NULL) {
		gfx_d3d11_buffer_free(buffer);
		return 1;
	}
	return 0;
}

static int gfx_d3d11_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	(void)size;

	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || buffer->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11	       = buffer->gfx->data;
	gfx_d3d11_buffer_t *d3d_buffer = buffer->data;

	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	if (context->UpdateSubresource == NULL) {
		return 1;
	}

	context->UpdateSubresource(d3d11->context, d3d_buffer->buffer, 0, NULL, data, 0, 0);

	return 0;
}

static int gfx_d3d11_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	if (frame == NULL || frame->pipeline == NULL || frame->pipeline->data == NULL || buffer == NULL || buffer->gfx == NULL ||
	    buffer->gfx->data == NULL || buffer->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11		   = buffer->gfx->data;
	gfx_d3d11_buffer_t *d3d_buffer	   = buffer->data;
	gfx_d3d11_pipeline_t *d3d_pipeline = frame->pipeline->data;
	if (d3d_pipeline->stride == 0) {
		return 1;
	}

	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	ID3D11Buffer *buffers[1]	   = {d3d_buffer->buffer};
	UINT strides[1]			   = {d3d_pipeline->stride};
	UINT offsets[1]			   = {0};

	switch (buffer->type) {
	case GFX_BUFFER_VERTEX: {
		if (context->IASetVertexBuffers == NULL) {
			return 1;
		}
		context->IASetVertexBuffers(d3d11->context, 0, 1, buffers, strides, offsets);
		break;
	}
	case GFX_BUFFER_INDEX: {
		if (context->IASetIndexBuffer == NULL) {
			return 1;
		}
		context->IASetIndexBuffer(d3d11->context, d3d_buffer->buffer, DXGI_FORMAT_R32_UINT, 0);
		break;
	}
	default: {
		log_error("cgfx", "gfx_d3d11", NULL, "unsupported buffer type: %d", buffer->type);
		return 1;
	}
	}

	return 0;
}

static void gfx_d3d11_shader_free(gfx_shader_t *shader)
{
	if (shader == NULL || shader->gfx == NULL || shader->data == NULL) {
		return;
	}

	gfx_d3d11_shader_t *d3d_shader = shader->data;

	if (d3d_shader->stage == GFX_SHADER_STAGE_VERTEX && d3d_shader->shader.vertex != NULL) {
		d3d11_release(d3d_shader->shader.vertex);
		d3d_shader->shader.vertex = NULL;
	}
	if (d3d_shader->stage == GFX_SHADER_STAGE_FRAGMENT && d3d_shader->shader.pixel != NULL) {
		d3d11_release(d3d_shader->shader.pixel);
		d3d_shader->shader.pixel = NULL;
	}
	if (d3d_shader->code != NULL) {
		d3d11_release(d3d_shader->code);
		d3d_shader->code = NULL;
	}
	alloc_free(&shader->gfx->alloc, d3d_shader, sizeof(gfx_d3d11_shader_t));
	shader->data = NULL;
}

static int gfx_d3d11_shader_init(gfx_shader_t *shader, const gfx_shader_config_t *config)
{
	if (shader == NULL || shader->gfx == NULL || shader->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = shader->gfx->data;
	if (gfx_d3d11_load_compiler(shader->gfx)) {
		return 1;
	}
	ID3D11DeviceVTable *device = *(ID3D11DeviceVTable **)d3d11->device;

	gfx_shader_code_t shader_code = {0};
	if (gfx_shader_compiler_transpile(config->compiler, config->source, config->stage, GFX_SHADER_LANGUAGE_HLSL, &shader_code)) {
		gfx_shader_code_free(&shader_code);
		return 1;
	}

	const char *target;
	switch (config->stage) {
	case GFX_SHADER_STAGE_VERTEX: {
		target = "vs_4_0";
		break;
	}
	case GFX_SHADER_STAGE_FRAGMENT: {
		target = "ps_4_0";
		break;
	}
	default: {
		log_error("cgfx", "gfx_d3d11", NULL, "unsupported shader stage: %d", config->stage);
		gfx_shader_code_free(&shader_code);
		return 1;
	}
	}

	gfx_d3d11_shader_t *d3d_shader = alloc_alloc(&shader->gfx->alloc, sizeof(gfx_d3d11_shader_t));
	if (d3d_shader == NULL) {
		gfx_shader_code_free(&shader_code);
		return 1;
	}
	*d3d_shader	  = (gfx_d3d11_shader_t){0};
	d3d_shader->stage = config->stage;
	shader->data	  = d3d_shader;

	if (gfx_d3d11_compile_shader(d3d11, shader_code.text, target, &d3d_shader->code)) {
		gfx_shader_code_free(&shader_code);
		gfx_d3d11_shader_free(shader);
		return 1;
	}

	gfx_shader_code_free(&shader_code);

	switch (config->stage) {
	case GFX_SHADER_STAGE_VERTEX: {
		HRESULT hr = device->CreateVertexShader(d3d11->device,
							d3d11_blob_data(d3d_shader->code),
							d3d11_blob_size(d3d_shader->code),
							NULL,
							&d3d_shader->shader.vertex);
		if (!hresult_ok(hr) || d3d_shader->shader.vertex == NULL) {
			gfx_d3d11_shader_free(shader);
			return 1;
		}
		break;
	}
	case GFX_SHADER_STAGE_FRAGMENT: {
		HRESULT hr = device->CreatePixelShader(d3d11->device,
						       d3d11_blob_data(d3d_shader->code),
						       d3d11_blob_size(d3d_shader->code),
						       NULL,
						       &d3d_shader->shader.pixel);
		if (!hresult_ok(hr) || d3d_shader->shader.pixel == NULL) {
			gfx_d3d11_shader_free(shader);
			return 1;
		}
		break;
	}
	}

	return 0;
}

static void gfx_d3d11_pipeline_free(gfx_pipeline_t *pipeline)
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->data == NULL) {
		return;
	}

	gfx_d3d11_pipeline_t *d3d_pipeline = pipeline->data;
	if (d3d_pipeline->input_layout != NULL) {
		d3d11_release(d3d_pipeline->input_layout);
		d3d_pipeline->input_layout = NULL;
	}
	alloc_free(&pipeline->gfx->alloc, d3d_pipeline, sizeof(gfx_d3d11_pipeline_t));
	pipeline->data = NULL;
}

static int gfx_d3d11_pipeline_init(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config)
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL || config == NULL || config->vs.data == NULL ||
	    config->fs.data == NULL || config->input_layout == NULL || config->input_layout_size == 0 ||
	    config->input_layout_size % sizeof(gfx_layout_t) != 0) {
		return 1;
	}

	gfx_d3d11_t *d3d11 = pipeline->gfx->data;

	ID3D11DeviceVTable *device = *(ID3D11DeviceVTable **)d3d11->device;
	if (device->CreateInputLayout == NULL || device->CreateVertexShader == NULL || device->CreatePixelShader == NULL) {
		return 1;
	}

	gfx_d3d11_pipeline_t *d3d_pipeline = alloc_alloc(&pipeline->gfx->alloc, sizeof(gfx_d3d11_pipeline_t));
	if (d3d_pipeline == NULL) {
		return 1;
	}
	*d3d_pipeline  = (gfx_d3d11_pipeline_t){0};
	pipeline->data = d3d_pipeline;

	size_t layout_cnt = config->input_layout_size / sizeof(gfx_layout_t);

	D3D11_INPUT_ELEMENT_DESC *elements = alloc_alloc(&pipeline->gfx->alloc, layout_cnt * sizeof(D3D11_INPUT_ELEMENT_DESC));
	if (elements == NULL) {
		gfx_d3d11_pipeline_free(pipeline);
		return 1;
	}

	size_t offset = 0;
	for (size_t i = 0; i < layout_cnt; i++) {
		if (config->input_layout[i].semantic == NULL) {
			alloc_free(&pipeline->gfx->alloc, elements, layout_cnt * sizeof(D3D11_INPUT_ELEMENT_DESC));
			gfx_d3d11_pipeline_free(pipeline);
			return 1;
		}

		elements[i] = (D3D11_INPUT_ELEMENT_DESC){
			.SemanticName	   = config->input_layout[i].semantic,
			.SemanticIndex	   = config->input_layout[i].semantic_index,
			.InputSlot	   = 0,
			.AlignedByteOffset = offset,
			.InputSlotClass	   = D3D11_INPUT_PER_VERTEX_DATA,
		};

		if (config->input_layout[i].type == GFX_VALUE_FLOAT32 && config->input_layout[i].count == 2) {
			elements[i].Format = DXGI_FORMAT_R32G32_FLOAT;
		} else if (config->input_layout[i].type == GFX_VALUE_FLOAT32 && config->input_layout[i].count == 4) {
			elements[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		} else {
			log_error("cgfx",
				  "gfx_d3d11",
				  NULL,
				  "unsupported input layout: %d x %d",
				  config->input_layout[i].count,
				  config->input_layout[i].type);
			alloc_free(&pipeline->gfx->alloc, elements, layout_cnt * sizeof(D3D11_INPUT_ELEMENT_DESC));
			gfx_d3d11_pipeline_free(pipeline);
			return 1;
		}

		offset += sizeof(float) * config->input_layout[i].count;
	}

	d3d_pipeline->stride = offset;

	gfx_d3d11_shader_t *vs = config->vs.data;
	gfx_d3d11_shader_t *fs = config->fs.data;

	HRESULT hr = device->CreateInputLayout(
		d3d11->device, elements, layout_cnt, d3d11_blob_data(vs->code), d3d11_blob_size(vs->code), &d3d_pipeline->input_layout);
	alloc_free(&pipeline->gfx->alloc, elements, layout_cnt * sizeof(D3D11_INPUT_ELEMENT_DESC));
	if (!hresult_ok(hr) || d3d_pipeline->input_layout == NULL) {
		gfx_d3d11_pipeline_free(pipeline);
		return 1;
	}

	d3d_pipeline->vertex_shader = vs->shader.vertex;
	d3d_pipeline->pixel_shader  = fs->shader.pixel;

	return 0;
}

static int gfx_d3d11_pipeline_bind(gfx_frame_t *frame, const gfx_pipeline_t *pipeline)
{
	if (frame == NULL || pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL || pipeline->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11		   = pipeline->gfx->data;
	gfx_d3d11_pipeline_t *d3d_pipeline = pipeline->data;
	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	if (context->IASetInputLayout == NULL || context->VSSetShader == NULL || context->PSSetShader == NULL) {
		return 1;
	}

	context->IASetInputLayout(d3d11->context, d3d_pipeline->input_layout);
	context->VSSetShader(d3d11->context, d3d_pipeline->vertex_shader, NULL, 0);
	context->PSSetShader(d3d11->context, d3d_pipeline->pixel_shader, NULL, 0);

	return 0;
}

static int gfx_d3d11_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11		   = frame->gfx->data;
	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	if (context->IASetPrimitiveTopology == NULL || context->Draw == NULL) {
		return 1;
	}

	context->IASetPrimitiveTopology(d3d11->context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(d3d11->context, vertex_count, first_vertex);
	return 0;
}

static int gfx_d3d11_draw_indexed(gfx_frame_t *frame, u32 index_count)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	gfx_d3d11_t *d3d11		   = frame->gfx->data;
	ID3D11DeviceContextVTable *context = *(ID3D11DeviceContextVTable **)d3d11->context;
	if (context->IASetPrimitiveTopology == NULL || context->DrawIndexed == NULL) {
		return 1;
	}

	context->IASetPrimitiveTopology(d3d11->context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(d3d11->context, index_count, 0, 0);
	return 0;
}

static int gfx_d3d11_end(gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	return 0;
}

static gfx_driver_t gfx_d3d11 = {
	.name			= "d3d11",
	.api			= GFX_API_D3D11,
	.init			= gfx_d3d11_init,
	.free			= gfx_d3d11_free,
	.native			= gfx_d3d11_native,
	.render_pass_init	= gfx_d3d11_render_pass_init,
	.render_pass_free	= gfx_d3d11_render_pass_free,
	.swapchain_init		= gfx_d3d11_swapchain_init,
	.swapchain_free		= gfx_d3d11_swapchain_free,
	.swapchain_resize	= gfx_d3d11_swapchain_resize,
	.swapchain_present	= gfx_d3d11_swapchain_present,
	.target_init		= gfx_d3d11_target_init,
	.target_free		= gfx_d3d11_target_free,
	.target_read		= gfx_d3d11_target_read,
	.framebuffer_init	= gfx_d3d11_framebuffer_init,
	.framebuffer_free	= gfx_d3d11_framebuffer_free,
	.framebuffer_pass_begin = gfx_d3d11_framebuffer_pass_begin,
	.buffer_init		= gfx_d3d11_buffer_init,
	.buffer_free		= gfx_d3d11_buffer_free,
	.buffer_set_data	= gfx_d3d11_buffer_set_data,
	.buffer_bind		= gfx_d3d11_buffer_bind,
	.shader_init		= gfx_d3d11_shader_init,
	.shader_free		= gfx_d3d11_shader_free,
	.pipeline_init		= gfx_d3d11_pipeline_init,
	.pipeline_free		= gfx_d3d11_pipeline_free,
	.pipeline_bind		= gfx_d3d11_pipeline_bind,
	.draw			= gfx_d3d11_draw,
	.draw_indexed		= gfx_d3d11_draw_indexed,
	.end			= gfx_d3d11_end,
};

GFX_DRIVER(gfx_d3d11, &gfx_d3d11);
