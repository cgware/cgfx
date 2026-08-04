#include "gfx_driver.h"

#include "d3d11.h"
#include "log.h"
#include "test.h"

enum {
	S_OK = 0,
};

typedef void (*t_gfx_d3d11_symbol_t)(void);

typedef struct t_d3d11_vertex_2d_s {
	float x;
	float y;
	float r;
	float g;
	float b;
	float a;
} t_d3d11_vertex_2d_t;

typedef struct t_d3d_blob_s t_d3d_blob_t;
typedef struct t_d3d11_buffer_s t_d3d11_buffer_t;
typedef struct t_d3d11_device_s t_d3d11_device_t;
typedef struct t_d3d11_context_s t_d3d11_context_t;
typedef struct t_d3d11_input_layout_s t_d3d11_input_layout_t;
typedef struct t_d3d11_pixel_shader_s t_d3d11_pixel_shader_t;
typedef struct t_d3d11_view_s t_d3d11_view_t;
typedef struct t_d3d11_texture_s t_d3d11_texture_t;
typedef struct t_d3d11_vertex_shader_s t_d3d11_vertex_shader_t;
typedef struct t_dxgi_swapchain_s t_dxgi_swapchain_t;

static gfx_shader_compiler_t t_gfx_d3d11_compiler;
static int t_gfx_d3d11_compiler_initialized;

struct t_d3d_blob_s {
	ID3DBlobVTable *vtbl;
	const void *data;
	size_t size;
};

struct t_d3d11_buffer_s {
	ID3D11BufferVTable *vtbl;
};

struct t_d3d11_device_s {
	ID3D11DeviceVTable *vtbl;
};

struct t_d3d11_context_s {
	ID3D11DeviceContextVTable *vtbl;
};

struct t_d3d11_input_layout_s {
	ID3D11InputLayoutVTable *vtbl;
};

struct t_d3d11_pixel_shader_s {
	ID3D11PixelShaderVTable *vtbl;
};

struct t_d3d11_view_s {
	ID3D11RenderTargetViewVTable *vtbl;
};

struct t_d3d11_texture_s {
	ID3D11Texture2DVTable *vtbl;
};

struct t_d3d11_vertex_shader_s {
	ID3D11VertexShaderVTable *vtbl;
};

struct t_dxgi_swapchain_s {
	IDXGISwapChainVTable *vtbl;
};

static int t_create_device_calls;
static int t_release_device_calls;
static int t_release_context_calls;
static int t_release_view_calls;
static int t_release_texture_calls;
static int t_release_buffer_calls;
static int t_release_input_layout_calls;
static int t_release_pixel_shader_calls;
static int t_release_vertex_shader_calls;
static int t_release_blob_calls;
static int t_create_render_target_view_calls;
static int t_create_buffer_calls;
static int t_create_texture_2d_calls;
static int t_create_input_layout_calls;
static int t_create_vertex_shader_calls;
static int t_create_pixel_shader_calls;
static int t_get_buffer_calls;
static int t_resize_buffers_calls;
static int t_d3d_compile_calls;
static int t_clear_render_target_view_calls;
static int t_om_set_render_targets_calls;
static int t_ia_set_input_layout_calls;
static int t_ia_set_vertex_buffers_calls;
static int t_ia_set_index_buffer_calls;
static int t_ia_set_primitive_topology_calls;
static int t_vs_set_shader_calls;
static int t_ps_set_shader_calls;
static int t_update_subresource_calls;
static int t_copy_resource_calls;
static int t_map_calls;
static int t_unmap_calls;
static int t_draw_calls;
static int t_draw_indexed_calls;
static int t_rs_set_viewports_calls;
static int t_surface_present_calls;
static UINT t_create_buffer_bytes;
static UINT t_create_buffer_bind_flags;
static UINT t_create_texture_width;
static UINT t_create_texture_height;
static UINT t_create_texture_usage;
static UINT t_create_texture_bind_flags;
static UINT t_create_texture_cpu_access_flags;
static UINT t_copy_resource_dst_is_texture;
static UINT t_copy_resource_src_is_texture;
static UINT t_map_type;
static UINT t_map_row_pitch;
static UINT t_create_driver_type;
static UINT t_create_sdk_version;
static UINT t_resize_width;
static UINT t_resize_height;
static UINT t_input_element_count;
static const char *t_input_semantic_name[2];
static UINT t_input_semantic_index[2];
static UINT t_render_target_count;
static UINT t_vertex_buffer_start_slot;
static UINT t_vertex_buffer_count;
static UINT t_vertex_buffer_stride;
static UINT t_vertex_buffer_offset;
static UINT t_index_buffer_format;
static UINT t_index_buffer_offset;
static UINT t_primitive_topology;
static UINT t_draw_vertex_count;
static UINT t_draw_start_vertex;
static UINT t_draw_index_count;
static UINT t_draw_start_index;
static int t_draw_base_vertex;
static UINT t_viewport_count;
static t_d3d11_vertex_2d_t t_uploaded_vertices[3];
static D3D11_VIEWPORT t_viewport;
static float t_clear_color[4];
static HRESULT t_create_device_ret;
static HRESULT t_d3d_compile_ret;
static HRESULT t_create_buffer_ret;
static HRESULT t_create_texture_2d_ret;
static HRESULT t_create_input_layout_ret;
static HRESULT t_create_vertex_shader_ret;
static HRESULT t_create_pixel_shader_ret;
static HRESULT t_get_buffer_ret;
static HRESULT t_create_render_target_view_ret;
static HRESULT t_resize_buffers_ret;
static HRESULT t_map_ret;
static int t_d3d_compile_error_msgs;
static u8 t_readback_pixels[16];
static t_d3d_blob_t t_vertex_blob;
static t_d3d_blob_t t_pixel_blob;
static t_d3d11_buffer_t t_buffer;
static t_d3d11_device_t t_device;
static t_d3d11_context_t t_context;
static t_d3d11_input_layout_t t_input_layout;
static t_d3d11_pixel_shader_t t_pixel_shader;
static t_d3d11_view_t t_view;
static t_d3d11_texture_t t_texture;
static t_d3d11_vertex_shader_t t_vertex_shader;
static t_dxgi_swapchain_t t_swapchain;
static gfx_surface_t t_surface;
static gfx_render_pass_t *t_gfx_d3d11_active_render_pass;

static const gfx_layout_t t_gfx_d3d11_input_layout[] = {
	{.index = 0, .semantic = "POSITION", .count = 2, .type = GFX_VALUE_FLOAT32},
	{.index = 1, .semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
};

typedef struct t_gfx_d3d11_shader_data_s {
	t_d3d_blob_t *code;
	gfx_shader_stage_t stage;
	union {
		t_d3d11_vertex_shader_t *vertex;
		t_d3d11_pixel_shader_t *pixel;
	} shader;
} t_gfx_d3d11_shader_data_t;

typedef struct t_gfx_d3d11_memory_target_data_s {
	t_d3d11_texture_t *texture;
} t_gfx_d3d11_memory_target_data_t;

typedef struct t_gfx_d3d11_swapchain_data_s {
	t_dxgi_swapchain_t *swapchain;
} t_gfx_d3d11_swapchain_data_t;

typedef struct t_gfx_d3d11_data_s {
	void *lib;
	void *compiler_lib;
	const gfx_target_t *target;
	t_d3d11_device_t *device;
	t_d3d11_context_t *context;
	void *D3D11CreateDevice;
	void *D3DCompile;
} t_gfx_d3d11_data_t;

typedef struct t_gfx_d3d11_framebuffer_data_s {
	t_d3d11_view_t *render_target;
} t_gfx_d3d11_framebuffer_data_t;

typedef struct t_gfx_d3d11_buffer_data_s {
	t_d3d11_buffer_t *buffer;
} t_gfx_d3d11_buffer_data_t;

typedef struct t_gfx_d3d11_pipeline_data_s {
	t_d3d11_input_layout_t *input_layout;
	t_d3d11_vertex_shader_t *vertex_shader;
	t_d3d11_pixel_shader_t *pixel_shader;
	UINT stride;
} t_gfx_d3d11_pipeline_data_t;

static gfx_pipeline_config_t t_gfx_d3d11_pipeline_config(gfx_shader_t vs, gfx_shader_t fs)
{
	return (gfx_pipeline_config_t){
		.render_pass	   = t_gfx_d3d11_active_render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = t_gfx_d3d11_input_layout,
		.input_layout_size = sizeof(t_gfx_d3d11_input_layout),
	};
}

static void *t_gfx_d3d11_alloc_fail(alloc_t *alloc, size_t size)
{
	(void)alloc;
	(void)size;
	return NULL;
}

static int t_gfx_d3d11_alloc_count;
static int t_gfx_d3d11_alloc_fail_at;

static void *t_gfx_d3d11_alloc_fail_n(alloc_t *alloc, size_t size)
{
	t_gfx_d3d11_alloc_count++;
	if (t_gfx_d3d11_alloc_count == t_gfx_d3d11_alloc_fail_at) {
		return NULL;
	}
	return alloc_alloc_std(alloc, size);
}

static void *t_gfx_d3d11_symbol(t_gfx_d3d11_symbol_t fn)
{
	union {
		t_gfx_d3d11_symbol_t fn;
		void *ptr;
	} symbol = {.fn = fn};

	return symbol.ptr;
}

static ULONG t_device_release(ID3D11Device *self)
{
	(void)self;
	t_release_device_calls++;
	return 0;
}

static ULONG t_context_release(ID3D11DeviceContext *self)
{
	(void)self;
	t_release_context_calls++;
	return 0;
}

static ULONG t_view_release(ID3D11RenderTargetView *self)
{
	(void)self;
	t_release_view_calls++;
	return 0;
}

static ULONG t_texture_release(ID3D11Texture2D *self)
{
	(void)self;
	t_release_texture_calls++;
	return 0;
}

static ULONG t_buffer_release(ID3D11Buffer *self)
{
	(void)self;
	t_release_buffer_calls++;
	return 0;
}

static ULONG t_input_layout_release(ID3D11InputLayout *self)
{
	(void)self;
	t_release_input_layout_calls++;
	return 0;
}

static ULONG t_pixel_shader_release(ID3D11PixelShader *self)
{
	(void)self;
	t_release_pixel_shader_calls++;
	return 0;
}

static ULONG t_vertex_shader_release(ID3D11VertexShader *self)
{
	(void)self;
	t_release_vertex_shader_calls++;
	return 0;
}

static ULONG t_blob_release(ID3DBlob *self)
{
	(void)self;
	t_release_blob_calls++;
	return 0;
}

static void *t_blob_GetBufferPointer(ID3DBlob *self)
{
	t_d3d_blob_t *blob = (t_d3d_blob_t *)self;
	return (void *)blob->data;
}

static size_t t_blob_GetBufferSize(ID3DBlob *self)
{
	t_d3d_blob_t *blob = (t_d3d_blob_t *)self;
	return blob->size;
}

static HRESULT t_CreateBuffer(ID3D11Device *self, const D3D11_BUFFER_DESC *desc, const void *initial_data, ID3D11Buffer **buffer)
{
	(void)self;
	(void)initial_data;
	t_create_buffer_calls++;
	t_create_buffer_bytes	   = desc->ByteWidth;
	t_create_buffer_bind_flags = desc->BindFlags;
	*buffer			   = (ID3D11Buffer *)&t_buffer;
	return t_create_buffer_ret;
}

static HRESULT t_CreateRenderTargetView(ID3D11Device *self, void *resource, const void *desc, ID3D11RenderTargetView **view)
{
	(void)self;
	(void)resource;
	(void)desc;
	t_create_render_target_view_calls++;
	*view = (ID3D11RenderTargetView *)&t_view;
	return t_create_render_target_view_ret;
}

static HRESULT t_CreateTexture2D(ID3D11Device *self, const D3D11_TEXTURE2D_DESC *desc, const void *initial_data, ID3D11Texture2D **texture)
{
	(void)self;
	(void)initial_data;
	t_create_texture_2d_calls++;
	t_create_texture_width		  = desc->Width;
	t_create_texture_height		  = desc->Height;
	t_create_texture_usage		  = desc->Usage;
	t_create_texture_bind_flags	  = desc->BindFlags;
	t_create_texture_cpu_access_flags = desc->CPUAccessFlags;
	*texture			  = (ID3D11Texture2D *)&t_texture;
	return t_create_texture_2d_ret;
}

static HRESULT t_CreateInputLayout(ID3D11Device *self, const D3D11_INPUT_ELEMENT_DESC *elements, UINT element_count,
				   const void *shader_bytecode, size_t bytecode_length, ID3D11InputLayout **input_layout)
{
	(void)self;
	(void)shader_bytecode;
	(void)bytecode_length;
	t_create_input_layout_calls++;
	t_input_element_count = element_count;
	for (UINT i = 0; i < element_count && i < 2; i++) {
		t_input_semantic_name[i]  = elements[i].SemanticName;
		t_input_semantic_index[i] = elements[i].SemanticIndex;
	}
	*input_layout = (ID3D11InputLayout *)&t_input_layout;
	return t_create_input_layout_ret;
}

static HRESULT t_CreateVertexShader(ID3D11Device *self, const void *shader_bytecode, size_t bytecode_length, void *class_linkage,
				    ID3D11VertexShader **shader)
{
	(void)self;
	(void)shader_bytecode;
	(void)bytecode_length;
	(void)class_linkage;
	t_create_vertex_shader_calls++;
	*shader = (ID3D11VertexShader *)&t_vertex_shader;
	return t_create_vertex_shader_ret;
}

static HRESULT t_CreatePixelShader(ID3D11Device *self, const void *shader_bytecode, size_t bytecode_length, void *class_linkage,
				   ID3D11PixelShader **shader)
{
	(void)self;
	(void)shader_bytecode;
	(void)bytecode_length;
	(void)class_linkage;
	t_create_pixel_shader_calls++;
	*shader = (ID3D11PixelShader *)&t_pixel_shader;
	return t_create_pixel_shader_ret;
}

static void t_OMSetRenderTargets(ID3D11DeviceContext *self, UINT num_views, ID3D11RenderTargetView *const *views, void *depth_stencil_view)
{
	(void)self;
	(void)views;
	(void)depth_stencil_view;
	t_om_set_render_targets_calls++;
	t_render_target_count = num_views;
}

static void t_IASetInputLayout(ID3D11DeviceContext *self, ID3D11InputLayout *input_layout)
{
	(void)self;
	(void)input_layout;
	t_ia_set_input_layout_calls++;
}

static void t_IASetVertexBuffers(ID3D11DeviceContext *self, UINT start_slot, UINT num_buffers, ID3D11Buffer *const *buffers,
				 const UINT *strides, const UINT *offsets)
{
	(void)self;
	(void)buffers;
	t_ia_set_vertex_buffers_calls++;
	t_vertex_buffer_start_slot = start_slot;
	t_vertex_buffer_count	   = num_buffers;
	t_vertex_buffer_stride	   = strides[0];
	t_vertex_buffer_offset	   = offsets[0];
}

static void t_IASetIndexBuffer(ID3D11DeviceContext *self, ID3D11Buffer *index_buffer, UINT format, UINT offset)
{
	(void)self;
	(void)index_buffer;
	t_ia_set_index_buffer_calls++;
	t_index_buffer_format = format;
	t_index_buffer_offset = offset;
}

static void t_IASetPrimitiveTopology(ID3D11DeviceContext *self, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	(void)self;
	t_ia_set_primitive_topology_calls++;
	t_primitive_topology = topology;
}

static void t_VSSetShader(ID3D11DeviceContext *self, ID3D11VertexShader *shader, void *const *class_instances, UINT class_instance_count)
{
	(void)self;
	(void)shader;
	(void)class_instances;
	(void)class_instance_count;
	t_vs_set_shader_calls++;
}

static void t_PSSetShader(ID3D11DeviceContext *self, ID3D11PixelShader *shader, void *const *class_instances, UINT class_instance_count)
{
	(void)self;
	(void)shader;
	(void)class_instances;
	(void)class_instance_count;
	t_ps_set_shader_calls++;
}

static void t_UpdateSubresource(ID3D11DeviceContext *self, ID3D11Buffer *resource, UINT subresource, const void *box, const void *data,
				UINT row_pitch, UINT depth_pitch)
{
	(void)self;
	(void)resource;
	(void)subresource;
	(void)box;
	(void)row_pitch;
	(void)depth_pitch;
	t_update_subresource_calls++;
	const t_d3d11_vertex_2d_t *vertices = data;
	t_uploaded_vertices[0]		    = vertices[0];
	t_uploaded_vertices[1]		    = vertices[1];
	t_uploaded_vertices[2]		    = vertices[2];
}

static void t_CopyResource(ID3D11DeviceContext *self, void *dst, void *src)
{
	(void)self;
	t_copy_resource_calls++;
	t_copy_resource_dst_is_texture = dst == &t_texture;
	t_copy_resource_src_is_texture = src == &t_texture;
}

static HRESULT t_Map(ID3D11DeviceContext *self, void *resource, UINT subresource, D3D11_MAP map_type, UINT map_flags,
		     D3D11_MAPPED_SUBRESOURCE *mapped)
{
	(void)self;
	(void)resource;
	(void)subresource;
	(void)map_flags;
	t_map_calls++;
	t_map_type	 = map_type;
	mapped->pData	 = t_readback_pixels;
	mapped->RowPitch = t_map_row_pitch;
	return t_map_ret;
}

static void t_Unmap(ID3D11DeviceContext *self, void *resource, UINT subresource)
{
	(void)self;
	(void)resource;
	(void)subresource;
	t_unmap_calls++;
}

static void t_Draw(ID3D11DeviceContext *self, UINT vertex_count, UINT start_vertex_location)
{
	(void)self;
	t_draw_calls++;
	t_draw_vertex_count = vertex_count;
	t_draw_start_vertex = start_vertex_location;
}

static void t_DrawIndexed(ID3D11DeviceContext *self, UINT index_count, UINT start_index_location, INT base_vertex_location)
{
	(void)self;
	t_draw_indexed_calls++;
	t_draw_index_count = index_count;
	t_draw_start_index = start_index_location;
	t_draw_base_vertex = base_vertex_location;
}

static void t_ClearRenderTargetView(ID3D11DeviceContext *self, ID3D11RenderTargetView *view, const float color[4])
{
	(void)self;
	(void)view;
	t_clear_render_target_view_calls++;
	t_clear_color[0] = color[0];
	t_clear_color[1] = color[1];
	t_clear_color[2] = color[2];
	t_clear_color[3] = color[3];
}

static void t_RSSetViewports(ID3D11DeviceContext *self, UINT num_viewports, const D3D11_VIEWPORT *viewports)
{
	(void)self;
	t_rs_set_viewports_calls++;
	t_viewport_count = num_viewports;
	t_viewport	 = *viewports;
}

static HRESULT t_GetBuffer(IDXGISwapChain *self, UINT buffer, REFIID riid, void **surface)
{
	(void)self;
	(void)buffer;
	(void)riid;
	t_get_buffer_calls++;
	*surface = &t_texture;
	return t_get_buffer_ret;
}

static HRESULT t_ResizeBuffers(IDXGISwapChain *self, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT format, UINT flags)
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

static HRESULT t_D3D11CreateDevice(void *adapter, D3D_DRIVER_TYPE driver_type, HMODULE software, UINT flags,
				   const D3D_FEATURE_LEVEL *feature_levels, UINT feature_level_count, UINT sdk_version,
				   ID3D11Device **device, D3D_FEATURE_LEVEL *feature_level, ID3D11DeviceContext **context)
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
	*device		     = (ID3D11Device *)&t_device;
	*context	     = (ID3D11DeviceContext *)&t_context;
	return t_create_device_ret;
}

static HRESULT t_D3DCompile(const void *src_data, size_t src_data_size, const char *source_name, const void *defines, void *include,
			    const char *entrypoint, const char *target, UINT flags1, UINT flags2, ID3DBlob **code, ID3DBlob **error_msgs)
{
	(void)src_data;
	(void)src_data_size;
	(void)source_name;
	(void)defines;
	(void)include;
	(void)entrypoint;
	(void)flags1;
	(void)flags2;
	t_d3d_compile_calls++;
	if (target[0] == 'v') {
		*code = (ID3DBlob *)&t_vertex_blob;
	} else {
		*code = (ID3DBlob *)&t_pixel_blob;
	}
	*error_msgs = t_d3d_compile_error_msgs ? (ID3DBlob *)&t_pixel_blob : NULL;
	return t_d3d_compile_ret;
}

static ID3DBlobVTable t_blob_vtbl = {
	.Release	  = t_blob_release,
	.GetBufferPointer = t_blob_GetBufferPointer,
	.GetBufferSize	  = t_blob_GetBufferSize,
};

static ID3D11BufferVTable t_buffer_vtbl = {
	.Release = t_buffer_release,
};

static ID3D11DeviceVTable t_device_vtbl = {
	.Release		= t_device_release,
	.CreateBuffer		= t_CreateBuffer,
	.CreateTexture2D	= t_CreateTexture2D,
	.CreateRenderTargetView = t_CreateRenderTargetView,
	.CreateInputLayout	= t_CreateInputLayout,
	.CreateVertexShader	= t_CreateVertexShader,
	.CreatePixelShader	= t_CreatePixelShader,
};

static ID3D11DeviceContextVTable t_context_vtbl = {
	.Release		= t_context_release,
	.PSSetShader		= t_PSSetShader,
	.VSSetShader		= t_VSSetShader,
	.DrawIndexed		= t_DrawIndexed,
	.Draw			= t_Draw,
	.Map			= t_Map,
	.Unmap			= t_Unmap,
	.IASetInputLayout	= t_IASetInputLayout,
	.IASetVertexBuffers	= t_IASetVertexBuffers,
	.IASetIndexBuffer	= t_IASetIndexBuffer,
	.IASetPrimitiveTopology = t_IASetPrimitiveTopology,
	.OMSetRenderTargets	= t_OMSetRenderTargets,
	.RSSetViewports		= t_RSSetViewports,
	.CopyResource		= t_CopyResource,
	.UpdateSubresource	= t_UpdateSubresource,
	.ClearRenderTargetView	= t_ClearRenderTargetView,
};

static ID3D11InputLayoutVTable t_input_layout_vtbl = {
	.Release = t_input_layout_release,
};

static ID3D11PixelShaderVTable t_pixel_shader_vtbl = {
	.Release = t_pixel_shader_release,
};

static ID3D11RenderTargetViewVTable t_view_vtbl = {
	.Release = t_view_release,
};

static ID3D11Texture2DVTable t_texture_vtbl = {
	.Release = t_texture_release,
};

static ID3D11VertexShaderVTable t_vertex_shader_vtbl = {
	.Release = t_vertex_shader_release,
};

static IDXGISwapChainVTable t_swapchain_vtbl = {
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
	t_release_buffer_calls		  = 0;
	t_release_input_layout_calls	  = 0;
	t_release_pixel_shader_calls	  = 0;
	t_release_vertex_shader_calls	  = 0;
	t_release_blob_calls		  = 0;
	t_create_render_target_view_calls = 0;
	t_create_buffer_calls		  = 0;
	t_create_texture_2d_calls	  = 0;
	t_create_input_layout_calls	  = 0;
	t_create_vertex_shader_calls	  = 0;
	t_create_pixel_shader_calls	  = 0;
	t_get_buffer_calls		  = 0;
	t_resize_buffers_calls		  = 0;
	t_d3d_compile_calls		  = 0;
	t_clear_render_target_view_calls  = 0;
	t_om_set_render_targets_calls	  = 0;
	t_ia_set_input_layout_calls	  = 0;
	t_ia_set_vertex_buffers_calls	  = 0;
	t_ia_set_index_buffer_calls	  = 0;
	t_ia_set_primitive_topology_calls = 0;
	t_vs_set_shader_calls		  = 0;
	t_ps_set_shader_calls		  = 0;
	t_update_subresource_calls	  = 0;
	t_copy_resource_calls		  = 0;
	t_map_calls			  = 0;
	t_unmap_calls			  = 0;
	t_draw_calls			  = 0;
	t_draw_indexed_calls		  = 0;
	t_rs_set_viewports_calls	  = 0;
	t_surface_present_calls		  = 0;
	t_create_buffer_bytes		  = 0;
	t_create_buffer_bind_flags	  = 0;
	t_create_texture_width		  = 0;
	t_create_texture_height		  = 0;
	t_create_texture_usage		  = 0;
	t_create_texture_bind_flags	  = 0;
	t_create_texture_cpu_access_flags = 0;
	t_copy_resource_dst_is_texture	  = 0;
	t_copy_resource_src_is_texture	  = 0;
	t_map_type			  = 0;
	t_map_row_pitch			  = 4;
	t_create_driver_type		  = 0;
	t_create_sdk_version		  = 0;
	t_resize_width			  = 0;
	t_resize_height			  = 0;
	t_input_element_count		  = 0;
	t_input_semantic_name[0]	  = NULL;
	t_input_semantic_name[1]	  = NULL;
	t_input_semantic_index[0]	  = 0;
	t_input_semantic_index[1]	  = 0;
	t_render_target_count		  = 0;
	t_vertex_buffer_start_slot	  = 0;
	t_vertex_buffer_count		  = 0;
	t_vertex_buffer_stride		  = 0;
	t_vertex_buffer_offset		  = 0;
	t_index_buffer_format		  = 0;
	t_index_buffer_offset		  = 0;
	t_primitive_topology		  = 0;
	t_draw_vertex_count		  = 0;
	t_draw_start_vertex		  = 0;
	t_draw_index_count		  = 0;
	t_draw_start_index		  = 0;
	t_draw_base_vertex		  = 0;
	t_viewport_count		  = 0;
	t_uploaded_vertices[0]		  = (t_d3d11_vertex_2d_t){0};
	t_uploaded_vertices[1]		  = (t_d3d11_vertex_2d_t){0};
	t_uploaded_vertices[2]		  = (t_d3d11_vertex_2d_t){0};
	t_viewport			  = (D3D11_VIEWPORT){0};
	t_clear_color[0]		  = 0.0f;
	t_clear_color[1]		  = 0.0f;
	t_clear_color[2]		  = 0.0f;
	t_clear_color[3]		  = 0.0f;
	t_create_device_ret		  = S_OK;
	t_d3d_compile_ret		  = S_OK;
	t_create_buffer_ret		  = S_OK;
	t_create_texture_2d_ret		  = S_OK;
	t_create_input_layout_ret	  = S_OK;
	t_create_vertex_shader_ret	  = S_OK;
	t_create_pixel_shader_ret	  = S_OK;
	t_get_buffer_ret		  = S_OK;
	t_create_render_target_view_ret	  = S_OK;
	t_resize_buffers_ret		  = S_OK;
	t_map_ret			  = S_OK;
	t_d3d_compile_error_msgs	  = 0;
	t_readback_pixels[0]		  = 1;
	t_readback_pixels[1]		  = 2;
	t_readback_pixels[2]		  = 3;
	t_readback_pixels[3]		  = 4;
	t_vertex_blob			  = (t_d3d_blob_t){.vtbl = &t_blob_vtbl, .data = "vertex", .size = 6};
	t_pixel_blob			  = (t_d3d_blob_t){.vtbl = &t_blob_vtbl, .data = "pixel", .size = 5};
	t_buffer.vtbl			  = &t_buffer_vtbl;
	t_device.vtbl			  = &t_device_vtbl;
	t_context.vtbl			  = &t_context_vtbl;
	t_input_layout.vtbl		  = &t_input_layout_vtbl;
	t_pixel_shader.vtbl		  = &t_pixel_shader_vtbl;
	t_view.vtbl			  = &t_view_vtbl;
	t_texture.vtbl			  = &t_texture_vtbl;
	t_vertex_shader.vtbl		  = &t_vertex_shader_vtbl;
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
	proc_setdlsym(proc, STRV("d3dcompiler_47.dll"), STRV("D3DCompile"), t_gfx_d3d11_symbol((t_gfx_d3d11_symbol_t)t_D3DCompile));
}

static int t_gfx_d3d11_init_gfx(gfx_t *gfx, proc_t *proc)
{
	t_gfx_d3d11_reset();
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_d3d11_symbols(proc);
	return gfx_init(gfx, t_gfx_d3d11_driver(), &(gfx_config_t){0}, proc, ALLOC_STD) != gfx;
}

static gfx_target_t *t_gfx_d3d11_init_swapchain_target(gfx_t *gfx, gfx_swapchain_t *swapchain, gfx_target_t *target, u16 width, u16 height)
{
	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_surface,
		.width	 = width,
		.height	 = height,
	};
	if (gfx_swapchain_init(swapchain, gfx, &swapchain_config) != swapchain) {
		return NULL;
	}
	if (gfx_target_init_swapchain(target, swapchain) != target) {
		gfx_swapchain_free(swapchain);
		return NULL;
	}
	return target;
}

static int t_gfx_d3d11_compiler_init(void)
{
	if (t_gfx_d3d11_compiler_initialized) {
		return 0;
	}
	if (gfx_shader_compiler_init(&t_gfx_d3d11_compiler, ALLOC_STD) == NULL) {
		return 1;
	}
	t_gfx_d3d11_compiler_initialized = 1;
	return 0;
}

static void t_gfx_d3d11_compiler_free(void)
{
	if (!t_gfx_d3d11_compiler_initialized) {
		return;
	}
	gfx_shader_compiler_free(&t_gfx_d3d11_compiler);
	t_gfx_d3d11_compiler		 = (gfx_shader_compiler_t){0};
	t_gfx_d3d11_compiler_initialized = 0;
}

static int t_gfx_d3d11_shader(gfx_t *gfx, gfx_shader_t *shader, gfx_shader_stage_t stage)
{
	if (!t_gfx_d3d11_compiler_initialized) {
		return 1;
	}
	const char *triangle_src = "vs_in 0 VertexIn {\n"
				   "\tvec2f position : POSITION;\n"
				   "\tvec4f color : COLOR0;\n"
				   "}\n"
				   "vs_out VertexOut {\n"
				   "\tvec4f position : POSITION;\n"
				   "\tvec4f color : COLOR0;\n"
				   "}\n"
				   "fs_in FragmentIn {\n"
				   "\tvec4f color : COLOR0;\n"
				   "}\n"
				   "fs_out FragmentOut {\n"
				   "\tvec4f color : COLOR0;\n"
				   "}\n"
				   "VertexOut vertex(VertexIn input) {\n"
				   "\tVertexOut output;\n"
				   "\toutput.position = vec4f(input.position.x, input.position.y, 0.0f, 1.0f);\n"
				   "\toutput.color = input.color;\n"
				   "\treturn output;\n"
				   "}\n"
				   "FragmentOut fragment(FragmentIn input) {\n"
				   "\tFragmentOut output;\n"
				   "\toutput.color = input.color;\n"
				   "\treturn output;\n"
				   "}\n";

	gfx_shader_config_t config = {
		.compiler = &t_gfx_d3d11_compiler,
		.source	  = strv_cstr(triangle_src),
		.stage	  = stage,
	};
	return gfx_shader_init(shader, gfx, &config) != shader;
}

TEST(gfx_d3d11_driver_is_registered)
{
	START;

	EXPECT_NOT_NULL(t_gfx_d3d11_driver());

	END;
}

TEST(gfx_d3d11_init_null_gfx)
{
	START;

	EXPECT_EQ(t_gfx_d3d11_driver()->init(NULL, &(gfx_config_t){0}), 1);

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

	EXPECT_EQ(t_gfx_d3d11_driver()->init(&(gfx_t){0}, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_d3d11_init_null_alloc)
{
	START;

	EXPECT_EQ(t_gfx_d3d11_driver()->init(&(gfx_t){0}, &(gfx_config_t){0}), 1);

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

	EXPECT_NULL(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){0}, &proc, (alloc_t){.alloc = t_gfx_d3d11_alloc_fail}));

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
	EXPECT_NULL(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD));
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
	EXPECT_NULL(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD));
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
	EXPECT_NULL(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD));
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
	gfx_init(&(gfx_t){0}, t_gfx_d3d11_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD);
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
	gfx_init(&(gfx_t){0}, t_gfx_d3d11_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD);
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

	EXPECT_EQ(t_create_driver_type, D3D_DRIVER_TYPE_HARDWARE);

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

	EXPECT_EQ(t_create_sdk_version, D3D11_SDK_VERSION);

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

TEST(gfx_d3d11_set_target_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	gfx_target_t target = {0};

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	END;
}

TEST(gfx_d3d11_render_pass_free_null_data)
{
	START;

	gfx_driver_t *drv = t_gfx_d3d11_driver();
	EXPECT_NOT_NULL(drv);

	drv->render_pass_free(NULL);

	END;
}

TEST(gfx_d3d11_render_pass_init_invalid_config)
{
	START;

	gfx_t gfx		      = {0};
	gfx_render_pass_t render_pass = {
		.gfx = &gfx,
	};

	EXPECT_EQ(t_gfx_d3d11_driver()->render_pass_init(&render_pass, &(gfx_render_pass_config_t){.color_format = GFX_FORMAT_NONE}), 1);

	END;
}

TEST(gfx_d3d11_render_pass_init_alloc_failure)
{
	START;

	gfx_t gfx = {
		.alloc = {.alloc = t_gfx_d3d11_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std},
	};
	gfx_render_pass_t render_pass = {
		.gfx = &gfx,
	};

	EXPECT_EQ(t_gfx_d3d11_driver()->render_pass_init(&render_pass, &(gfx_render_pass_config_t){.color_format = GFX_FORMAT_RGBA8}), 1);

	END;
}

TEST(gfx_d3d11_memory_target_read_invalid_config)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx.drv->target_read(&(gfx_target_t){.gfx = &gfx}, NULL), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_memory_target_init_creates_texture)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 pixels[8]	    = {0};
	gfx_target_t target = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	EXPECT_EQ(t_create_texture_2d_calls, 1);
	EXPECT_EQ(t_create_texture_width, 2);
	EXPECT_EQ(t_create_texture_height, 1);
	EXPECT_EQ(t_create_texture_usage, D3D11_USAGE_DEFAULT);
	EXPECT_EQ(t_create_texture_bind_flags, D3D11_BIND_RENDER_TARGET);

	gfx_target_free(&target);
	EXPECT_EQ(t_release_texture_calls, 1);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_memory_target_init_missing_create_texture_callback)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	PFN_CreateTexture2D saved     = t_device_vtbl.CreateTexture2D;
	t_device_vtbl.CreateTexture2D = NULL;
	u8 pixels[4]		      = {0};
	gfx_target_t target	      = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));
	EXPECT_NULL(target.driver_data);

	t_device_vtbl.CreateTexture2D = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_memory_target_init_create_texture_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_create_texture_2d_ret = -1;
	u8 pixels[4]		= {0};
	gfx_target_t target	= {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);
	EXPECT_NULL(target.driver_data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_swapchain_resize_calls_dxgi_swapchain)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_d3d11_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);

	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 800, 600), 0);
	EXPECT_EQ(t_resize_buffers_calls, 1);
	EXPECT_EQ(t_resize_width, 800);
	EXPECT_EQ(t_resize_height, 600);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_swapchain_init_invalid_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx.drv->swapchain_init(NULL, NULL), 1);
	EXPECT_EQ(gfx.drv->swapchain_init(&(gfx_swapchain_t){.gfx = &gfx}, NULL), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_swapchain_init_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx.alloc		  = (alloc_t){.alloc = t_gfx_d3d11_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_swapchain_t swapchain = {0};

	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_surface,
		.width	 = 640,
		.height	 = 480,
	};
	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &swapchain_config));

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_swapchain_resize_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_d3d11_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	t_resize_buffers_ret = -1;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 800, 600), 1);
	log_set_quiet(0, 0);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_swapchain_free_invalid_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_d3d11_driver();
	EXPECT_NOT_NULL(drv);

	drv->swapchain_free(NULL);
	drv->swapchain_free(&(gfx_swapchain_t){0});

	END;
}

TEST(gfx_d3d11_swapchain_present_calls_surface)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_d3d11_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);

	EXPECT_EQ(gfx_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_surface_present_calls, 1);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_target_free_null_data_direct)
{
	START;

	t_gfx_d3d11_driver()->target_free(NULL);

	END;
}

TEST(gfx_d3d11_target_init_alloc_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_d3d11_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	u8 pixels[4]	    = {0};
	gfx_target_t target = {
		.gfx	= &gfx,
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_swapchain_resize_invalid_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx.drv->swapchain_resize(NULL, 1, 1), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_free_null_data_direct)
{
	START;

	t_gfx_d3d11_driver()->framebuffer_free(NULL);

	END;
}

TEST(gfx_d3d11_framebuffer_init_invalid_config_direct)
{
	START;

	EXPECT_EQ(t_gfx_d3d11_driver()->framebuffer_init(NULL), 1);

	END;
}

TEST(gfx_d3d11_framebuffer_init_alloc_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx.alloc		      = (alloc_t){.alloc = t_gfx_d3d11_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	t_gfx_d3d11_memory_target_data_t driver_target = {
		.texture = &t_texture,
	};
	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_MEMORY,
		.format	     = GFX_FORMAT_RGBA8,
		.driver_data = &driver_target,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_init_missing_render_target_callback_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_device_vtbl.CreateRenderTargetView = NULL;

	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	t_gfx_d3d11_memory_target_data_t driver_target = {.texture = &t_texture};

	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_MEMORY,
		.format	     = GFX_FORMAT_RGBA8,
		.driver_data = &driver_target,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
	};
	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	t_device_vtbl.CreateRenderTargetView = t_CreateRenderTargetView;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_init_memory_without_texture_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	t_gfx_d3d11_memory_target_data_t driver_target = {0};

	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_MEMORY,
		.format	     = GFX_FORMAT_RGBA8,
		.driver_data = &driver_target,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_init_surface_without_swapchain_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	t_gfx_d3d11_swapchain_data_t driver_target = {0};

	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_SWAPCHAIN,
		.format	     = GFX_FORMAT_RGBA8,
		.driver_data = &driver_target,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_init_unknown_target_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	t_gfx_d3d11_memory_target_data_t driver_target = {0};

	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = (gfx_target_type_t)99,
		.format	     = GFX_FORMAT_RGBA8,
		.driver_data = &driver_target,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_init_memory_creates_render_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]		      = {0};
	gfx_swapchain_t swapchain     = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);

	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(t_create_render_target_view_calls, 1);

	gfx_framebuffer_free(&framebuffer);
	EXPECT_EQ(t_release_view_calls, 1);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_init_render_target_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]					= {0};
	gfx_swapchain_t swapchain			= {0};
	gfx_target_t target				= {0};
	gfx_render_pass_t render_pass			= {0};
	gfx_framebuffer_t framebuffer			= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_create_render_target_view_ret = -1;

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	log_set_quiet(0, 0);

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_init_surface_gets_buffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain     = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(t_gfx_d3d11_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);

	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(t_get_buffer_calls, 1);
	EXPECT_EQ(t_create_render_target_view_calls, 1);
	EXPECT_EQ(t_release_texture_calls, 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_init_surface_get_buffer_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain     = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(t_gfx_d3d11_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_get_buffer_ret = -1;

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	log_set_quiet(0, 0);

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_init_surface_render_target_failure_releases_buffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain     = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(t_gfx_d3d11_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_create_render_target_view_ret = -1;

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	log_set_quiet(0, 0);
	EXPECT_EQ(t_release_texture_calls, 1);

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_pass_begin_invalid_target_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	t_gfx_d3d11_memory_target_data_t driver_target = {
		.texture = &t_texture,
	};
	struct {
		t_d3d11_view_t *render_target;
	} driver_framebuffer = {
		.render_target = &t_view,
	};
	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = (gfx_target_type_t)99,
		.format	     = GFX_FORMAT_RGBA8,
		.driver_data = &driver_target,
		.width	     = 1,
		.height	     = 1,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &driver_framebuffer,
	};
	gfx_frame_t frame = {
		.gfx = &gfx,
	};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_pass_begin_sets_targets_and_clears)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]					= {0};
	gfx_swapchain_t swapchain			= {0};
	gfx_target_t target				= {0};
	gfx_render_pass_t render_pass			= {0};
	gfx_framebuffer_t framebuffer			= {0};
	gfx_frame_t frame				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_pass_config_t pass_config = {
		.clear	  = {.r = 0.1f, .g = 0.2f, .b = 0.3f, .a = 0.4f},
		.viewport = {.x = 1, .y = 2, .width = 3, .height = 4},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(t_om_set_render_targets_calls, 1);
	EXPECT_EQ(t_render_target_count, 1);
	EXPECT_EQ(t_rs_set_viewports_calls, 1);
	EXPECT_EQ(t_viewport_count, 1);
	EXPECT_EQ(t_viewport.TopLeftX, 1.0f);
	EXPECT_EQ(t_viewport.TopLeftY, 2.0f);
	EXPECT_EQ(t_viewport.Width, 3.0f);
	EXPECT_EQ(t_viewport.Height, 4.0f);
	EXPECT_EQ(t_clear_render_target_view_calls, 1);
	EXPECT_EQ(t_clear_color[0], 0.1f);
	EXPECT_EQ(t_clear_color[1], 0.2f);
	EXPECT_EQ(t_clear_color[2], 0.3f);
	EXPECT_EQ(t_clear_color[3], 0.4f);

	gfx_end(&frame);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_pass_begin_missing_render_target_callbacks)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.load = GFX_LOAD_LOAD,
		.data = &render_pass,
	};
	t_gfx_d3d11_memory_target_data_t driver_target	  = {.texture = &t_texture};
	t_gfx_d3d11_framebuffer_data_t driver_framebuffer = {.render_target = &t_view};

	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_MEMORY,
		.driver_data = &driver_target,
		.width	     = 1,
		.height	     = 1,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &driver_framebuffer,
	};
	gfx_frame_t frame		  = {.gfx = &gfx};
	PFN_OMSetRenderTargets saved	  = t_context_vtbl.OMSetRenderTargets;
	t_context_vtbl.OMSetRenderTargets = NULL;

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	t_context_vtbl.OMSetRenderTargets = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_framebuffer_pass_begin_missing_clear_callback)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.load = GFX_LOAD_CLEAR,
		.data = &render_pass,
	};
	t_gfx_d3d11_memory_target_data_t driver_target	  = {.texture = &t_texture};
	t_gfx_d3d11_framebuffer_data_t driver_framebuffer = {.render_target = &t_view};

	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_MEMORY,
		.driver_data = &driver_target,
		.width	     = 1,
		.height	     = 1,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &driver_framebuffer,
	};
	gfx_frame_t frame		     = {.gfx = &gfx};
	PFN_ClearRenderTargetView saved	     = t_context_vtbl.ClearRenderTargetView;
	t_context_vtbl.ClearRenderTargetView = NULL;

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	t_context_vtbl.ClearRenderTargetView = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_target_read_copies_memory)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 target_pixels[4]				= {0};
	u8 read_pixels[4]				= {0};
	gfx_swapchain_t swapchain			= {0};
	gfx_target_t target				= {0};
	gfx_render_pass_t render_pass			= {0};
	gfx_framebuffer_t framebuffer			= {0};
	gfx_frame_t frame				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= target_pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}), 0);

	EXPECT_EQ(gfx.drv->target_read(&target, &(gfx_memory_readback_config_t){.data = read_pixels, .stride = 4}), 0);
	EXPECT_EQ(t_copy_resource_calls, 1);
	EXPECT_EQ(t_copy_resource_dst_is_texture, 1);
	EXPECT_EQ(t_copy_resource_src_is_texture, 1);
	EXPECT_EQ(t_map_calls, 1);
	EXPECT_EQ(t_map_type, D3D11_MAP_READ);
	EXPECT_EQ(t_unmap_calls, 1);
	EXPECT_EQ(read_pixels[0], 1);
	EXPECT_EQ(read_pixels[1], 2);
	EXPECT_EQ(read_pixels[2], 3);
	EXPECT_EQ(read_pixels[3], 4);

	gfx_end(&frame);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_target_read_requires_active_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]					= {0};
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);

	EXPECT_EQ(gfx.drv->target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);

	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_target_read_missing_callbacks)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]	    = {0};
	gfx_target_t target = {
		.gfx	= &gfx,
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	t_gfx_d3d11_memory_target_data_t driver_target = {.texture = &t_texture};
	target.driver_data			       = &driver_target;
	PFN_CopyResource saved			       = t_context_vtbl.CopyResource;
	t_context_vtbl.CopyResource		       = NULL;
	((t_gfx_d3d11_data_t *)gfx.data)->target       = &target;

	EXPECT_EQ(gfx.drv->target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);

	t_context_vtbl.CopyResource = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_target_read_create_staging_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]					= {0};
	gfx_swapchain_t swapchain			= {0};
	gfx_target_t target				= {0};
	gfx_render_pass_t render_pass			= {0};
	gfx_framebuffer_t framebuffer			= {0};
	gfx_frame_t frame				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}), 0);
	t_create_texture_2d_ret = -1;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);
	log_set_quiet(0, 0);

	gfx_end(&frame);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_target_read_map_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]					 = {0};
	gfx_swapchain_t swapchain			 = {0};
	gfx_target_t target				 = {0};
	gfx_render_pass_t render_pass			 = {0};
	gfx_framebuffer_t framebuffer			 = {0};
	gfx_frame_t frame				 = {0};
	gfx_memory_target_config_t memory_target_config0 = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config0), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}), 0);
	t_map_ret = -1;

	EXPECT_EQ(gfx.drv->target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);
	EXPECT_EQ(t_release_texture_calls, 1);

	gfx_end(&frame);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_target_read_rejects_short_row_pitch)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]					 = {0};
	gfx_swapchain_t swapchain			 = {0};
	gfx_target_t target				 = {0};
	gfx_render_pass_t render_pass			 = {0};
	gfx_framebuffer_t framebuffer			 = {0};
	gfx_frame_t frame				 = {0};
	gfx_memory_target_config_t memory_target_config1 = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config1), &target);
	gfx_render_pass_config_t render_pass_config0 = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config0), &render_pass);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}), 0);
	t_map_row_pitch = 3;

	EXPECT_EQ(gfx.drv->target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);
	EXPECT_EQ(t_unmap_calls, 1);

	gfx_end(&frame);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

static gfx_pipeline_config_t t_gfx_d3d11_direct_pipeline_config(gfx_shader_t *vs, gfx_shader_t *fs)
{
	t_gfx_d3d11_shader_data_t *vs_data = vs->data;
	t_gfx_d3d11_shader_data_t *fs_data = fs->data;

	*vs_data = (t_gfx_d3d11_shader_data_t){
		.code	       = &t_vertex_blob,
		.stage	       = GFX_SHADER_STAGE_VERTEX,
		.shader.vertex = &t_vertex_shader,
	};
	*fs_data = (t_gfx_d3d11_shader_data_t){
		.code	      = &t_pixel_blob,
		.stage	      = GFX_SHADER_STAGE_FRAGMENT,
		.shader.pixel = &t_pixel_shader,
	};
	return t_gfx_d3d11_pipeline_config(*vs, *fs);
}

TEST(gfx_d3d11_pipeline_init_invalid_config_direct)
{
	START;

	gfx_t gfx		= {0};
	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};

	EXPECT_EQ(t_gfx_d3d11_driver()->pipeline_init(&pipeline, &(gfx_pipeline_config_t){0}), 1);

	END;
}

TEST(gfx_d3d11_pipeline_init_missing_shader_callback_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_device_vtbl.CreateVertexShader  = NULL;
	t_gfx_d3d11_shader_data_t vs_data = {0};
	t_gfx_d3d11_shader_data_t fs_data = {0};
	gfx_shader_t vs			  = {.data = &vs_data};
	gfx_shader_t fs			  = {.data = &fs_data};
	gfx_pipeline_config_t config	  = t_gfx_d3d11_direct_pipeline_config(&vs, &fs);

	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &config), 1);

	t_device_vtbl.CreateVertexShader = t_CreateVertexShader;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_alloc_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_shader_data_t vs_data = {0};
	t_gfx_d3d11_shader_data_t fs_data = {0};
	gfx_shader_t vs			  = {.data = &vs_data};
	gfx_shader_t fs			  = {.data = &fs_data};
	gfx_pipeline_config_t config	  = t_gfx_d3d11_direct_pipeline_config(&vs, &fs);

	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};
	gfx.alloc = (alloc_t){.alloc = t_gfx_d3d11_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &config), 1);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_element_alloc_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_shader_data_t vs_data = {0};
	t_gfx_d3d11_shader_data_t fs_data = {0};
	gfx_shader_t vs			  = {.data = &vs_data};
	gfx_shader_t fs			  = {.data = &fs_data};
	gfx_pipeline_config_t config	  = t_gfx_d3d11_direct_pipeline_config(&vs, &fs);

	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};
	t_gfx_d3d11_alloc_count	  = 0;
	t_gfx_d3d11_alloc_fail_at = 2;
	gfx.alloc		  = (alloc_t){.alloc = t_gfx_d3d11_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std};

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &config), 1);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_missing_layout_semantic_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_shader_data_t vs_data = {0};
	t_gfx_d3d11_shader_data_t fs_data = {0};
	gfx_shader_t vs			  = {.data = &vs_data};
	gfx_shader_t fs			  = {.data = &fs_data};

	const gfx_layout_t layout[] = {
		{.index = 0, .semantic = NULL, .count = 2, .type = GFX_VALUE_FLOAT32},
	};
	gfx_pipeline_config_t config = t_gfx_d3d11_direct_pipeline_config(&vs, &fs);
	config.input_layout	     = layout;
	config.input_layout_size     = sizeof(layout);

	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &config), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_unsupported_input_layout_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_shader_data_t vs_data = {0};
	t_gfx_d3d11_shader_data_t fs_data = {0};
	gfx_shader_t vs			  = {.data = &vs_data};
	gfx_shader_t fs			  = {.data = &fs_data};

	const gfx_layout_t layout[] = {
		{.index = 0, .semantic = "POSITION", .count = 3, .type = GFX_VALUE_FLOAT32},
	};
	gfx_pipeline_config_t config = t_gfx_d3d11_direct_pipeline_config(&vs, &fs);
	config.input_layout	     = layout;
	config.input_layout_size     = sizeof(layout);

	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &config), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_create_input_layout_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_create_input_layout_ret	  = -1;
	t_gfx_d3d11_shader_data_t vs_data = {0};
	t_gfx_d3d11_shader_data_t fs_data = {0};
	gfx_shader_t vs			  = {.data = &vs_data};
	gfx_shader_t fs			  = {.data = &fs_data};
	gfx_pipeline_config_t config	  = t_gfx_d3d11_direct_pipeline_config(&vs, &fs);

	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &config), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_clear_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}

TEST(gfx_d3d11_draw_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->draw(NULL, 3, 0), 1);

	END;
}

TEST(gfx_d3d11_end_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_d3d11_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->end(NULL), 1);

	END;
}

TEST(gfx_d3d11_begin_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_d3d11_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}

TEST(gfx_d3d11_buffer_init_unsupported_type)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_UNKNOWN}));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_free_null_data)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {.gfx = &gfx};

	gfx_buffer_free(&buffer);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_init_null_config)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, NULL));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_init_missing_create_buffer_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	t_device_vtbl.CreateBuffer = NULL;
	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));
	t_device_vtbl.CreateBuffer = t_CreateBuffer;

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_init_alloc_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_d3d11_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_init_create_buffer_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_create_buffer_ret = -1;
	gfx_buffer_t buffer = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));
	EXPECT_NULL(buffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_set_data_null_data)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_buffer_t buffer	    = {.gfx = &gfx};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_free_releases_pixel_shader)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t shader = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &shader, GFX_SHADER_STAGE_FRAGMENT), 0);

	gfx_shader_free(&shader);

	EXPECT_EQ(t_release_pixel_shader_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_shader_t shader = {.gfx = &gfx};

	gfx_shader_free(&shader);

	END;
}

TEST(gfx_d3d11_shader_init_null_config)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, NULL));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_init_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){0}));
	log_set_quiet(0, 0);

	END;
}

TEST(gfx_d3d11_shader_init_missing_compiler_library)
{
	START;

	t_gfx_d3d11_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	proc_setdlsym(&proc, STRV("d3d11.dll"), STRV("D3D11CreateDevice"), t_gfx_d3d11_symbol((t_gfx_d3d11_symbol_t)t_D3D11CreateDevice));
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_init_missing_compiler_symbol)
{
	START;

	t_gfx_d3d11_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	proc_setdlsym(&proc, STRV("d3d11.dll"), STRV("D3D11CreateDevice"), t_gfx_d3d11_symbol((t_gfx_d3d11_symbol_t)t_D3D11CreateDevice));
	proc_setdlsym(&proc, STRV("d3dcompiler_47.dll"), STRV("unused"), &t_device);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_init_transpile_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	gfx_shader_config_t shader_config = {
		.compiler = &t_gfx_d3d11_compiler,
		.source	  = STRV("not shader source\n"),
		.stage	  = GFX_SHADER_STAGE_VERTEX,
	};
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, &shader_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_init_unsupported_stage)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &shader, (gfx_shader_stage_t)99), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_init_alloc_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_d3d11_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_init_compile_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_d3d_compile_ret	 = -1;
	t_d3d_compile_error_msgs = 1;
	gfx_shader_t shader	 = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_init_releases_compile_messages)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_d3d_compile_error_msgs = 1;
	gfx_shader_t shader	 = {0};

	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 0);

	EXPECT_EQ(t_release_blob_calls, 1);

	gfx_shader_free(&shader);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_init_create_vertex_shader_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_create_vertex_shader_ret = -1;
	gfx_shader_t shader	   = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_shader_init_create_pixel_shader_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_create_pixel_shader_ret = -1;
	gfx_shader_t shader	  = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &shader, GFX_SHADER_STAGE_FRAGMENT), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_pipeline_t pipeline = {.gfx = &gfx};

	gfx_pipeline_free(&pipeline);

	END;
}

TEST(gfx_d3d11_pipeline_init_null_config)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_pipeline_t pipeline = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, NULL));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_pipeline_t pipeline = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){0}));

	END;
}

TEST(gfx_d3d11_pipeline_init_success)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs		      = {0};
	gfx_shader_t fs		      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_pipeline_t pipeline	      = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_render_pass_config_t render_pass_config1 = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config1), &render_pass);
	t_gfx_d3d11_active_render_pass = &render_pass;
	gfx_pipeline_config_t config   = t_gfx_d3d11_pipeline_config(vs, fs);

	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &config), &pipeline);
	EXPECT_EQ(t_create_input_layout_calls, 1);
	EXPECT_EQ(t_input_element_count, 2);
	EXPECT_STR(t_input_semantic_name[0], "POSITION");
	EXPECT_STR(t_input_semantic_name[1], "COLOR");

	gfx_pipeline_free(&pipeline);
	t_gfx_d3d11_active_render_pass = NULL;
	gfx_render_pass_free(&render_pass);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_missing_input_layout_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	t_device_vtbl.CreateInputLayout = NULL;
	gfx_pipeline_t pipeline		= {0};
	gfx_pipeline_config_t config	= t_gfx_d3d11_pipeline_config(vs, fs);

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));

	t_device_vtbl.CreateInputLayout = t_CreateInputLayout;
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_alloc_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx.alloc		     = (alloc_t){.alloc = t_gfx_d3d11_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = t_gfx_d3d11_pipeline_config(vs, fs);

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));

	gfx.alloc = ALLOC_STD;
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_element_alloc_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx.alloc		     = (alloc_t){.alloc = t_gfx_d3d11_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std};
	t_gfx_d3d11_alloc_count	     = 0;
	t_gfx_d3d11_alloc_fail_at    = 2;
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = t_gfx_d3d11_pipeline_config(vs, fs);

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));

	gfx.alloc = ALLOC_STD;
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_create_input_layout_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	t_create_input_layout_ret    = -1;
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = t_gfx_d3d11_pipeline_config(vs, fs);

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));

	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_missing_layout_semantic)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	const gfx_layout_t layout[] = {
		{.index = 0, .count = 2, .type = GFX_VALUE_FLOAT32},
	};
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = t_gfx_d3d11_pipeline_config(vs, fs);
	config.input_layout	     = layout;
	config.input_layout_size     = sizeof(layout);

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));

	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_init_unsupported_input_layout)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	const gfx_layout_t layout[] = {
		{.index = 0, .semantic = "POSITION", .count = 3, .type = GFX_VALUE_FLOAT32},
	};
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = t_gfx_d3d11_pipeline_config(vs, fs);
	config.input_layout	     = layout;
	config.input_layout_size     = sizeof(layout);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));
	log_set_quiet(0, 0);

	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}
TEST(gfx_d3d11_buffer_set_data_missing_update_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	PFN_UpdateSubresource saved	 = t_context_vtbl.UpdateSubresource;
	t_context_vtbl.UpdateSubresource = NULL;
	gfx_vertex_2d_t vertices[3]	 = {0};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	t_context_vtbl.UpdateSubresource = saved;
	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_set_data_uploads_vertices)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 1.0f, .y = 2.0f, .r = 3.0f, .g = 4.0f, .b = 5.0f, .a = 6.0f},
		{.x = 7.0f, .y = 8.0f},
		{.x = 9.0f, .y = 10.0f},
	};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);
	EXPECT_EQ(t_update_subresource_calls, 1);
	EXPECT_EQ(t_uploaded_vertices[0].x, 1.0f);
	EXPECT_EQ(t_uploaded_vertices[0].y, 2.0f);
	EXPECT_EQ(t_uploaded_vertices[2].x, 9.0f);
	EXPECT_EQ(t_uploaded_vertices[2].y, 10.0f);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_init_creates_index_buffer)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX}), &buffer);
	EXPECT_EQ(t_create_buffer_calls, 1);
	EXPECT_EQ(t_create_buffer_bind_flags, D3D11_BIND_INDEX_BUFFER);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_bind_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_d3d11_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->pipeline_bind(NULL, NULL), 1);

	END;
}

TEST(gfx_d3d11_buffer_bind_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_d3d11_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->buffer_bind(NULL, NULL), 1);

	END;
}

TEST(gfx_d3d11_pipeline_bind_binds_shaders)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs		      = {0};
	gfx_shader_t fs		      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_pipeline_t pipeline	      = {0};
	gfx_frame_t frame	      = {.gfx = &gfx, .render_pass = &render_pass, .active = 1};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_render_pass_config_t render_pass_config2 = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config2), &render_pass);
	t_gfx_d3d11_active_render_pass = &render_pass;
	gfx_pipeline_config_t config   = t_gfx_d3d11_pipeline_config(vs, fs);
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &config), &pipeline);
	gfx.frame = &frame;

	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(t_ia_set_input_layout_calls, 1);
	EXPECT_EQ(t_vs_set_shader_calls, 1);
	EXPECT_EQ(t_ps_set_shader_calls, 1);

	gfx.frame = NULL;
	gfx_pipeline_free(&pipeline);
	t_gfx_d3d11_active_render_pass = NULL;
	gfx_render_pass_free(&render_pass);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_bind_binds_vertex_buffer)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs		      = {0};
	gfx_shader_t fs		      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_pipeline_t pipeline	      = {0};
	gfx_buffer_t buffer	      = {0};
	gfx_frame_t frame	      = {.gfx = &gfx, .render_pass = &render_pass, .active = 1};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_render_pass_config_t render_pass_config3 = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config3), &render_pass);
	t_gfx_d3d11_active_render_pass = &render_pass;
	gfx_pipeline_config_t config   = t_gfx_d3d11_pipeline_config(vs, fs);
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &config), &pipeline);
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	gfx.frame = &frame;
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);

	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(t_ia_set_vertex_buffers_calls, 1);
	EXPECT_EQ(t_vertex_buffer_start_slot, 0);
	EXPECT_EQ(t_vertex_buffer_count, 1);
	EXPECT_EQ(t_vertex_buffer_stride, sizeof(t_d3d11_vertex_2d_t));
	EXPECT_EQ(t_vertex_buffer_offset, 0);

	gfx.frame = NULL;
	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	t_gfx_d3d11_active_render_pass = NULL;
	gfx_render_pass_free(&render_pass);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_bind_binds_index_buffer)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_buffer_data_t driver_buffer = {.buffer = &t_buffer};

	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &(t_gfx_d3d11_pipeline_data_t){.stride = sizeof(t_d3d11_vertex_2d_t)},
	};
	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.type = GFX_BUFFER_INDEX,
		.data = &driver_buffer,
	};
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = &pipeline,
		.active	  = 1,
	};

	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(t_ia_set_index_buffer_calls, 1);
	EXPECT_EQ(t_index_buffer_format, DXGI_FORMAT_R32_UINT);
	EXPECT_EQ(t_index_buffer_offset, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_bind_missing_index_buffer_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_buffer_data_t driver_buffer = {.buffer = &t_buffer};

	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &(t_gfx_d3d11_pipeline_data_t){.stride = sizeof(t_d3d11_vertex_2d_t)},
	};
	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.type = GFX_BUFFER_INDEX,
		.data = &driver_buffer,
	};
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = &pipeline,
		.active	  = 1,
	};
	PFN_IASetIndexBuffer saved	= t_context_vtbl.IASetIndexBuffer;
	t_context_vtbl.IASetIndexBuffer = NULL;

	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 1);

	t_context_vtbl.IASetIndexBuffer = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_bind_rejects_unknown_type)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_buffer_data_t driver_buffer = {.buffer = &t_buffer};

	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &(t_gfx_d3d11_pipeline_data_t){.stride = sizeof(t_d3d11_vertex_2d_t)},
	};
	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.type = GFX_BUFFER_UNKNOWN,
		.data = &driver_buffer,
	};
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = &pipeline,
		.active	  = 1,
	};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_bind_rejects_zero_stride)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_pipeline_data_t driver_pipeline = {
		.input_layout = &t_input_layout,
		.stride	      = 0,
	};
	t_gfx_d3d11_buffer_data_t driver_buffer = {.buffer = &t_buffer};

	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &driver_pipeline,
	};
	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.type = GFX_BUFFER_VERTEX,
		.data = &driver_buffer,
	};
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = &pipeline,
		.active	  = 1,
	};

	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_buffer_bind_missing_vertex_buffer_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_pipeline_data_t driver_pipeline = {
		.input_layout = &t_input_layout,
		.stride	      = sizeof(t_d3d11_vertex_2d_t),
	};
	t_gfx_d3d11_buffer_data_t driver_buffer = {.buffer = &t_buffer};

	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &driver_pipeline,
	};
	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.type = GFX_BUFFER_VERTEX,
		.data = &driver_buffer,
	};
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = &pipeline,
		.active	  = 1,
	};
	PFN_IASetVertexBuffers saved	  = t_context_vtbl.IASetVertexBuffers;
	t_context_vtbl.IASetVertexBuffers = NULL;

	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 1);

	t_context_vtbl.IASetVertexBuffers = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_pipeline_bind_missing_shader_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_pipeline_data_t driver_pipeline = {
		.input_layout  = &t_input_layout,
		.stride	       = sizeof(t_d3d11_vertex_2d_t),
		.vertex_shader = &t_vertex_shader,
		.pixel_shader  = &t_pixel_shader,
	};
	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &driver_pipeline,
	};
	gfx_frame_t frame		= {.gfx = &gfx, .active = 1};
	PFN_IASetInputLayout saved	= t_context_vtbl.IASetInputLayout;
	t_context_vtbl.IASetInputLayout = NULL;

	EXPECT_EQ(gfx.drv->pipeline_bind(&frame, &pipeline), 1);

	t_context_vtbl.IASetInputLayout = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_missing_draw_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	PFN_Draw saved	    = t_context_vtbl.Draw;
	t_context_vtbl.Draw = NULL;
	gfx_frame_t frame   = {.gfx = &gfx, .active = 1};

	EXPECT_EQ(gfx.drv->draw(&frame, 3, 0), 1);

	t_context_vtbl.Draw = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_calls_context)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_frame_t frame = {.gfx = &gfx, .active = 1};

	EXPECT_EQ(gfx.drv->draw(&frame, 3, 2), 0);
	EXPECT_EQ(t_ia_set_primitive_topology_calls, 1);
	EXPECT_EQ(t_primitive_topology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	EXPECT_EQ(t_draw_calls, 1);
	EXPECT_EQ(t_draw_vertex_count, 3);
	EXPECT_EQ(t_draw_start_vertex, 2);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_indexed_null_data)
{
	START;

	gfx_driver_t *drv = t_gfx_d3d11_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->draw_indexed(NULL, 3), 1);
	EXPECT_EQ(drv->draw_indexed(&(gfx_frame_t){.gfx = &(gfx_t){0}}, 3), 1);

	END;
}

TEST(gfx_d3d11_draw_indexed_missing_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	PFN_DrawIndexed saved	   = t_context_vtbl.DrawIndexed;
	t_context_vtbl.DrawIndexed = NULL;
	gfx_frame_t frame	   = {.gfx = &gfx, .active = 1};

	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 3), 1);

	t_context_vtbl.DrawIndexed = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_indexed_calls_context)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_frame_t frame = {.gfx = &gfx, .active = 1};

	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 5), 0);
	EXPECT_EQ(t_ia_set_primitive_topology_calls, 1);
	EXPECT_EQ(t_primitive_topology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	EXPECT_EQ(t_draw_indexed_calls, 1);
	EXPECT_EQ(t_draw_index_count, 5);
	EXPECT_EQ(t_draw_start_index, 0);
	EXPECT_EQ(t_draw_base_vertex, 0);

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

	EXPECT_EQ(gfx.drv->swapchain_present(&(gfx_swapchain_t){0}), 1);

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

	(void)t_gfx_d3d11_compiler_init();

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
	RUN(gfx_d3d11_render_pass_free_null_data);
	RUN(gfx_d3d11_render_pass_init_invalid_config);
	RUN(gfx_d3d11_render_pass_init_alloc_failure);
	RUN(gfx_d3d11_memory_target_read_invalid_config);
	RUN(gfx_d3d11_memory_target_init_creates_texture);
	RUN(gfx_d3d11_memory_target_init_missing_create_texture_callback);
	RUN(gfx_d3d11_memory_target_init_create_texture_failure);
	RUN(gfx_d3d11_swapchain_resize_calls_dxgi_swapchain);
	RUN(gfx_d3d11_swapchain_init_invalid_direct);
	RUN(gfx_d3d11_swapchain_init_alloc_failure);
	RUN(gfx_d3d11_swapchain_resize_failure);
	RUN(gfx_d3d11_swapchain_free_invalid_direct);
	RUN(gfx_d3d11_swapchain_present_calls_surface);
	RUN(gfx_d3d11_target_free_null_data_direct);
	RUN(gfx_d3d11_target_init_alloc_failure_direct);
	RUN(gfx_d3d11_swapchain_resize_invalid_direct);
	RUN(gfx_d3d11_framebuffer_free_null_data_direct);
	RUN(gfx_d3d11_framebuffer_init_invalid_config_direct);
	RUN(gfx_d3d11_framebuffer_init_alloc_failure_direct);
	RUN(gfx_d3d11_framebuffer_init_missing_render_target_callback_direct);
	RUN(gfx_d3d11_framebuffer_init_memory_without_texture_direct);
	RUN(gfx_d3d11_framebuffer_init_surface_without_swapchain_direct);
	RUN(gfx_d3d11_framebuffer_init_unknown_target_direct);
	RUN(gfx_d3d11_framebuffer_init_memory_creates_render_target);
	RUN(gfx_d3d11_framebuffer_init_render_target_failure);
	RUN(gfx_d3d11_framebuffer_init_surface_gets_buffer);
	RUN(gfx_d3d11_framebuffer_init_surface_get_buffer_failure);
	RUN(gfx_d3d11_framebuffer_init_surface_render_target_failure_releases_buffer);
	RUN(gfx_d3d11_framebuffer_pass_begin_invalid_target_direct);
	RUN(gfx_d3d11_framebuffer_pass_begin_sets_targets_and_clears);
	RUN(gfx_d3d11_framebuffer_pass_begin_missing_render_target_callbacks);
	RUN(gfx_d3d11_framebuffer_pass_begin_missing_clear_callback);
	RUN(gfx_d3d11_target_read_copies_memory);
	RUN(gfx_d3d11_target_read_requires_active_target);
	RUN(gfx_d3d11_target_read_missing_callbacks);
	RUN(gfx_d3d11_target_read_create_staging_failure);
	RUN(gfx_d3d11_target_read_map_failure);
	RUN(gfx_d3d11_target_read_rejects_short_row_pitch);
	RUN(gfx_d3d11_pipeline_init_invalid_config_direct);
	RUN(gfx_d3d11_pipeline_init_missing_shader_callback_direct);
	RUN(gfx_d3d11_pipeline_init_alloc_failure_direct);
	RUN(gfx_d3d11_pipeline_init_element_alloc_failure_direct);
	RUN(gfx_d3d11_pipeline_init_missing_layout_semantic_direct);
	RUN(gfx_d3d11_pipeline_init_unsupported_input_layout_direct);
	RUN(gfx_d3d11_pipeline_init_create_input_layout_failure_direct);
	RUN(gfx_d3d11_clear_null_data);
	RUN(gfx_d3d11_begin_null_frame);
	RUN(gfx_d3d11_draw_null_data);
	RUN(gfx_d3d11_end_null_frame);
	RUN(gfx_d3d11_buffer_init_unsupported_type);
	RUN(gfx_d3d11_buffer_free_null_data);
	RUN(gfx_d3d11_buffer_init_null_config);
	RUN(gfx_d3d11_buffer_init_missing_create_buffer_callback);
	RUN(gfx_d3d11_buffer_init_alloc_failure);
	RUN(gfx_d3d11_buffer_init_create_buffer_failure);
	RUN(gfx_d3d11_buffer_set_data_null_data);
	RUN(gfx_d3d11_shader_free_releases_pixel_shader);
	RUN(gfx_d3d11_shader_free_null_data);
	RUN(gfx_d3d11_shader_init_null_config);
	RUN(gfx_d3d11_shader_init_null_data);
	RUN(gfx_d3d11_shader_init_missing_compiler_library);
	RUN(gfx_d3d11_shader_init_missing_compiler_symbol);
	RUN(gfx_d3d11_shader_init_transpile_failure);
	RUN(gfx_d3d11_shader_init_unsupported_stage);
	RUN(gfx_d3d11_shader_init_alloc_failure);
	RUN(gfx_d3d11_shader_init_compile_failure);
	RUN(gfx_d3d11_shader_init_releases_compile_messages);
	RUN(gfx_d3d11_shader_init_create_vertex_shader_failure);
	RUN(gfx_d3d11_shader_init_create_pixel_shader_failure);
	RUN(gfx_d3d11_pipeline_free_null_data);
	RUN(gfx_d3d11_pipeline_init_null_config);
	RUN(gfx_d3d11_pipeline_init_null_data);
	RUN(gfx_d3d11_pipeline_init_success);
	RUN(gfx_d3d11_pipeline_init_missing_input_layout_callback);
	RUN(gfx_d3d11_pipeline_init_alloc_failure);
	RUN(gfx_d3d11_pipeline_init_element_alloc_failure);
	RUN(gfx_d3d11_pipeline_init_create_input_layout_failure);
	RUN(gfx_d3d11_pipeline_init_missing_layout_semantic);
	RUN(gfx_d3d11_pipeline_init_unsupported_input_layout);
	RUN(gfx_d3d11_buffer_set_data_missing_update_callback);
	RUN(gfx_d3d11_buffer_set_data_uploads_vertices);
	RUN(gfx_d3d11_buffer_init_creates_index_buffer);
	RUN(gfx_d3d11_pipeline_bind_null_frame);
	RUN(gfx_d3d11_buffer_bind_null_frame);
	RUN(gfx_d3d11_pipeline_bind_binds_shaders);
	RUN(gfx_d3d11_buffer_bind_binds_vertex_buffer);
	RUN(gfx_d3d11_buffer_bind_binds_index_buffer);
	RUN(gfx_d3d11_buffer_bind_missing_index_buffer_callback);
	RUN(gfx_d3d11_buffer_bind_rejects_unknown_type);
	RUN(gfx_d3d11_buffer_bind_rejects_zero_stride);
	RUN(gfx_d3d11_buffer_bind_missing_vertex_buffer_callback);
	RUN(gfx_d3d11_pipeline_bind_missing_shader_callback);
	RUN(gfx_d3d11_draw_missing_draw_callback);
	RUN(gfx_d3d11_draw_calls_context);
	RUN(gfx_d3d11_draw_indexed_null_data);
	RUN(gfx_d3d11_draw_indexed_missing_callback);
	RUN(gfx_d3d11_draw_indexed_calls_context);
	RUN(gfx_d3d11_present_null_data);
	RUN(gfx_d3d11_free_null_data);
	RUN(gfx_d3d11_free_releases_context);
	RUN(gfx_d3d11_free_releases_device);

	t_gfx_d3d11_compiler_free();

	SEND;
}
