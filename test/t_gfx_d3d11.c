#include "gfx_driver.h"

#include "log.h"
#include "test.h"

typedef long HRESULT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef void *HMODULE;
typedef int D3D_FEATURE_LEVEL;

enum {
	S_OK					= 0,
	T_D3D_DRIVER_TYPE_HARDWARE		= 1,
	T_D3D11_SDK_VERSION			= 7,
	T_D3D11_BIND_VERTEX_BUFFER		= 0x00000001,
	T_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
};

typedef struct GUID_s {
	u32 Data1;
	u16 Data2;
	u16 Data3;
	u8 Data4[8];
} GUID;

typedef const GUID *REFIID;
typedef void (*t_gfx_d3d11_symbol_t)(void);

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

typedef struct t_d3d_blob_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d_blob_t *self);
	void *(*GetBufferPointer)(t_d3d_blob_t *self);
	size_t (*GetBufferSize)(t_d3d_blob_t *self);
} t_d3d_blob_vtbl_t;

typedef struct t_d3d11_buffer_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_buffer_t *self);
} t_d3d11_buffer_vtbl_t;

typedef struct t_d3d11_device_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_device_t *self);
	HRESULT (*CreateBuffer)(t_d3d11_device_t *self, const D3D11_BUFFER_DESC *desc, const void *initial_data, t_d3d11_buffer_t **buffer);
	HRESULT (*CreateTexture1D)(void);
	HRESULT (*CreateTexture2D)(void);
	HRESULT (*CreateTexture3D)(void);
	HRESULT (*CreateShaderResourceView)(void);
	HRESULT (*CreateUnorderedAccessView)(void);
	HRESULT (*CreateRenderTargetView)(t_d3d11_device_t *self, void *resource, const void *desc, t_d3d11_view_t **view);
	HRESULT (*CreateDepthStencilView)(void);
	HRESULT(*CreateInputLayout)
	(t_d3d11_device_t *self, const D3D11_INPUT_ELEMENT_DESC *elements, UINT element_count, const void *shader_bytecode,
	 size_t bytecode_length, t_d3d11_input_layout_t **input_layout);
	HRESULT(*CreateVertexShader)
	(t_d3d11_device_t *self, const void *shader_bytecode, size_t bytecode_length, void *class_linkage,
	 t_d3d11_vertex_shader_t **shader);
	HRESULT (*CreateGeometryShader)(void);
	HRESULT (*CreateGeometryShaderWithStreamOutput)(void);
	HRESULT(*CreatePixelShader)
	(t_d3d11_device_t *self, const void *shader_bytecode, size_t bytecode_length, void *class_linkage, t_d3d11_pixel_shader_t **shader);
} t_d3d11_device_vtbl_t;

typedef struct t_d3d11_context_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_context_t *self);
	void (*GetDevice)(void);
	void (*GetPrivateData)(void);
	void (*SetPrivateData)(void);
	void (*SetPrivateDataInterface)(void);
	void (*unused_07)(void);
	void (*unused_08)(void);
	void (*PSSetShader)(t_d3d11_context_t *self, t_d3d11_pixel_shader_t *shader, void *const *class_instances,
			    UINT class_instance_count);
	void (*unused_10)(void);
	void (*VSSetShader)(t_d3d11_context_t *self, t_d3d11_vertex_shader_t *shader, void *const *class_instances,
			    UINT class_instance_count);
	void (*unused_12)(void);
	void (*Draw)(t_d3d11_context_t *self, UINT vertex_count, UINT start_vertex_location);
	void (*unused_14)(void);
	void (*unused_15)(void);
	void (*unused_16)(void);
	void (*IASetInputLayout)(t_d3d11_context_t *self, t_d3d11_input_layout_t *input_layout);
	void (*IASetVertexBuffers)(t_d3d11_context_t *self, UINT start_slot, UINT num_buffers, t_d3d11_buffer_t *const *buffers,
				   const UINT *strides, const UINT *offsets);
	void (*unused_19)(void);
	void (*unused_20)(void);
	void (*unused_21)(void);
	void (*unused_22)(void);
	void (*unused_23)(void);
	void (*IASetPrimitiveTopology)(t_d3d11_context_t *self, UINT topology);
	void (*unused_25)(void);
	void (*unused_26)(void);
	void (*unused_27)(void);
	void (*unused_28)(void);
	void (*unused_29)(void);
	void (*unused_30)(void);
	void (*unused_31)(void);
	void (*unused_32)(void);
	void (*OMSetRenderTargets)(t_d3d11_context_t *self, UINT num_views, t_d3d11_view_t *const *views, void *depth_stencil_view);
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
	void (*RSSetViewports)(t_d3d11_context_t *self, UINT num_viewports, const D3D11_VIEWPORT *viewports);
	void (*unused_45)(void);
	void (*unused_46)(void);
	void (*unused_47)(void);
	void (*UpdateSubresource)(t_d3d11_context_t *self, t_d3d11_buffer_t *resource, UINT subresource, const void *box, const void *data,
				  UINT row_pitch, UINT depth_pitch);
	void (*unused_49)(void);
	void (*ClearRenderTargetView)(t_d3d11_context_t *self, t_d3d11_view_t *view, const float color[4]);
} t_d3d11_context_vtbl_t;

typedef struct t_d3d11_input_layout_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_input_layout_t *self);
} t_d3d11_input_layout_vtbl_t;

typedef struct t_d3d11_pixel_shader_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_pixel_shader_t *self);
} t_d3d11_pixel_shader_vtbl_t;

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

typedef struct t_d3d11_vertex_shader_vtbl_s {
	HRESULT (*QueryInterface)(void);
	ULONG (*AddRef)(void);
	ULONG (*Release)(t_d3d11_vertex_shader_t *self);
} t_d3d11_vertex_shader_vtbl_t;

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

struct t_d3d_blob_s {
	t_d3d_blob_vtbl_t *vtbl;
	const void *data;
	size_t size;
};

struct t_d3d11_buffer_s {
	t_d3d11_buffer_vtbl_t *vtbl;
};

struct t_d3d11_device_s {
	t_d3d11_device_vtbl_t *vtbl;
};

struct t_d3d11_context_s {
	t_d3d11_context_vtbl_t *vtbl;
};

struct t_d3d11_input_layout_s {
	t_d3d11_input_layout_vtbl_t *vtbl;
};

struct t_d3d11_pixel_shader_s {
	t_d3d11_pixel_shader_vtbl_t *vtbl;
};

struct t_d3d11_view_s {
	t_d3d11_view_vtbl_t *vtbl;
};

struct t_d3d11_texture_s {
	t_d3d11_texture_vtbl_t *vtbl;
};

struct t_d3d11_vertex_shader_s {
	t_d3d11_vertex_shader_vtbl_t *vtbl;
};

struct t_dxgi_swapchain_s {
	t_dxgi_swapchain_vtbl_t *vtbl;
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
static int t_ia_set_primitive_topology_calls;
static int t_vs_set_shader_calls;
static int t_ps_set_shader_calls;
static int t_update_subresource_calls;
static int t_draw_calls;
static int t_rs_set_viewports_calls;
static int t_surface_present_calls;
static UINT t_create_buffer_bytes;
static UINT t_create_buffer_bind_flags;
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
static UINT t_primitive_topology;
static UINT t_draw_vertex_count;
static UINT t_draw_start_vertex;
static UINT t_viewport_count;
static t_d3d11_vertex_2d_t t_uploaded_vertices[3];
static D3D11_VIEWPORT t_viewport;
static float t_clear_color[4];
static HRESULT t_create_device_ret;
static HRESULT t_d3d_compile_ret;
static HRESULT t_create_buffer_ret;
static HRESULT t_create_input_layout_ret;
static HRESULT t_create_vertex_shader_ret;
static HRESULT t_create_pixel_shader_ret;
static HRESULT t_get_buffer_ret;
static HRESULT t_create_render_target_view_ret;
static HRESULT t_resize_buffers_ret;
static int t_d3d_compile_error_msgs;
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
static gfx_target_t t_gfx_d3d11_target;

static const gfx_layout_t t_gfx_d3d11_input_layout[] = {
	{.index = 0, .semantic = "POSITION", .count = 2, .type = GFX_VALUE_FLOAT32},
	{.index = 1, .semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
};

static gfx_pipeline_config_t t_gfx_d3d11_pipeline_config(gfx_shader_t vs, gfx_shader_t fs)
{
	return (gfx_pipeline_config_t){
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

static ULONG t_buffer_release(t_d3d11_buffer_t *self)
{
	(void)self;
	t_release_buffer_calls++;
	return 0;
}

static ULONG t_input_layout_release(t_d3d11_input_layout_t *self)
{
	(void)self;
	t_release_input_layout_calls++;
	return 0;
}

static ULONG t_pixel_shader_release(t_d3d11_pixel_shader_t *self)
{
	(void)self;
	t_release_pixel_shader_calls++;
	return 0;
}

static ULONG t_vertex_shader_release(t_d3d11_vertex_shader_t *self)
{
	(void)self;
	t_release_vertex_shader_calls++;
	return 0;
}

static ULONG t_blob_release(t_d3d_blob_t *self)
{
	(void)self;
	t_release_blob_calls++;
	return 0;
}

static void *t_blob_GetBufferPointer(t_d3d_blob_t *self)
{
	return (void *)self->data;
}

static size_t t_blob_GetBufferSize(t_d3d_blob_t *self)
{
	return self->size;
}

static HRESULT t_CreateBuffer(t_d3d11_device_t *self, const D3D11_BUFFER_DESC *desc, const void *initial_data, t_d3d11_buffer_t **buffer)
{
	(void)self;
	(void)initial_data;
	t_create_buffer_calls++;
	t_create_buffer_bytes	   = desc->ByteWidth;
	t_create_buffer_bind_flags = desc->BindFlags;
	*buffer			   = &t_buffer;
	return t_create_buffer_ret;
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

static HRESULT t_CreateInputLayout(t_d3d11_device_t *self, const D3D11_INPUT_ELEMENT_DESC *elements, UINT element_count,
				   const void *shader_bytecode, size_t bytecode_length, t_d3d11_input_layout_t **input_layout)
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
	*input_layout = &t_input_layout;
	return t_create_input_layout_ret;
}

static HRESULT t_CreateVertexShader(t_d3d11_device_t *self, const void *shader_bytecode, size_t bytecode_length, void *class_linkage,
				    t_d3d11_vertex_shader_t **shader)
{
	(void)self;
	(void)shader_bytecode;
	(void)bytecode_length;
	(void)class_linkage;
	t_create_vertex_shader_calls++;
	*shader = &t_vertex_shader;
	return t_create_vertex_shader_ret;
}

static HRESULT t_CreatePixelShader(t_d3d11_device_t *self, const void *shader_bytecode, size_t bytecode_length, void *class_linkage,
				   t_d3d11_pixel_shader_t **shader)
{
	(void)self;
	(void)shader_bytecode;
	(void)bytecode_length;
	(void)class_linkage;
	t_create_pixel_shader_calls++;
	*shader = &t_pixel_shader;
	return t_create_pixel_shader_ret;
}

static void t_OMSetRenderTargets(t_d3d11_context_t *self, UINT num_views, t_d3d11_view_t *const *views, void *depth_stencil_view)
{
	(void)self;
	(void)views;
	(void)depth_stencil_view;
	t_om_set_render_targets_calls++;
	t_render_target_count = num_views;
}

static void t_IASetInputLayout(t_d3d11_context_t *self, t_d3d11_input_layout_t *input_layout)
{
	(void)self;
	(void)input_layout;
	t_ia_set_input_layout_calls++;
}

static void t_IASetVertexBuffers(t_d3d11_context_t *self, UINT start_slot, UINT num_buffers, t_d3d11_buffer_t *const *buffers,
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

static void t_IASetPrimitiveTopology(t_d3d11_context_t *self, UINT topology)
{
	(void)self;
	t_ia_set_primitive_topology_calls++;
	t_primitive_topology = topology;
}

static void t_VSSetShader(t_d3d11_context_t *self, t_d3d11_vertex_shader_t *shader, void *const *class_instances, UINT class_instance_count)
{
	(void)self;
	(void)shader;
	(void)class_instances;
	(void)class_instance_count;
	t_vs_set_shader_calls++;
}

static void t_PSSetShader(t_d3d11_context_t *self, t_d3d11_pixel_shader_t *shader, void *const *class_instances, UINT class_instance_count)
{
	(void)self;
	(void)shader;
	(void)class_instances;
	(void)class_instance_count;
	t_ps_set_shader_calls++;
}

static void t_UpdateSubresource(t_d3d11_context_t *self, t_d3d11_buffer_t *resource, UINT subresource, const void *box, const void *data,
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

static void t_Draw(t_d3d11_context_t *self, UINT vertex_count, UINT start_vertex_location)
{
	(void)self;
	t_draw_calls++;
	t_draw_vertex_count = vertex_count;
	t_draw_start_vertex = start_vertex_location;
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

static void t_RSSetViewports(t_d3d11_context_t *self, UINT num_viewports, const D3D11_VIEWPORT *viewports)
{
	(void)self;
	t_rs_set_viewports_calls++;
	t_viewport_count = num_viewports;
	t_viewport	 = *viewports;
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

static HRESULT t_D3DCompile(const void *src_data, size_t src_data_size, const char *source_name, const void *defines, void *include,
			    const char *entrypoint, const char *target, UINT flags1, UINT flags2, t_d3d_blob_t **code,
			    t_d3d_blob_t **error_msgs)
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
		*code = &t_vertex_blob;
	} else {
		*code = &t_pixel_blob;
	}
	*error_msgs = t_d3d_compile_error_msgs ? &t_pixel_blob : NULL;
	return t_d3d_compile_ret;
}

static t_d3d_blob_vtbl_t t_blob_vtbl = {
	.Release	  = t_blob_release,
	.GetBufferPointer = t_blob_GetBufferPointer,
	.GetBufferSize	  = t_blob_GetBufferSize,
};

static t_d3d11_buffer_vtbl_t t_buffer_vtbl = {
	.Release = t_buffer_release,
};

static t_d3d11_device_vtbl_t t_device_vtbl = {
	.Release		= t_device_release,
	.CreateBuffer		= t_CreateBuffer,
	.CreateRenderTargetView = t_CreateRenderTargetView,
	.CreateInputLayout	= t_CreateInputLayout,
	.CreateVertexShader	= t_CreateVertexShader,
	.CreatePixelShader	= t_CreatePixelShader,
};

static t_d3d11_context_vtbl_t t_context_vtbl = {
	.Release		= t_context_release,
	.PSSetShader		= t_PSSetShader,
	.VSSetShader		= t_VSSetShader,
	.Draw			= t_Draw,
	.IASetInputLayout	= t_IASetInputLayout,
	.IASetVertexBuffers	= t_IASetVertexBuffers,
	.IASetPrimitiveTopology = t_IASetPrimitiveTopology,
	.OMSetRenderTargets	= t_OMSetRenderTargets,
	.RSSetViewports		= t_RSSetViewports,
	.UpdateSubresource	= t_UpdateSubresource,
	.ClearRenderTargetView	= t_ClearRenderTargetView,
};

static t_d3d11_input_layout_vtbl_t t_input_layout_vtbl = {
	.Release = t_input_layout_release,
};

static t_d3d11_pixel_shader_vtbl_t t_pixel_shader_vtbl = {
	.Release = t_pixel_shader_release,
};

static t_d3d11_view_vtbl_t t_view_vtbl = {
	.Release = t_view_release,
};

static t_d3d11_texture_vtbl_t t_texture_vtbl = {
	.Release = t_texture_release,
};

static t_d3d11_vertex_shader_vtbl_t t_vertex_shader_vtbl = {
	.Release = t_vertex_shader_release,
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
	t_release_buffer_calls		  = 0;
	t_release_input_layout_calls	  = 0;
	t_release_pixel_shader_calls	  = 0;
	t_release_vertex_shader_calls	  = 0;
	t_release_blob_calls		  = 0;
	t_create_render_target_view_calls = 0;
	t_create_buffer_calls		  = 0;
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
	t_ia_set_primitive_topology_calls = 0;
	t_vs_set_shader_calls		  = 0;
	t_ps_set_shader_calls		  = 0;
	t_update_subresource_calls	  = 0;
	t_draw_calls			  = 0;
	t_rs_set_viewports_calls	  = 0;
	t_surface_present_calls		  = 0;
	t_create_buffer_bytes		  = 0;
	t_create_buffer_bind_flags	  = 0;
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
	t_primitive_topology		  = 0;
	t_draw_vertex_count		  = 0;
	t_draw_start_vertex		  = 0;
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
	t_create_input_layout_ret	  = S_OK;
	t_create_vertex_shader_ret	  = S_OK;
	t_create_pixel_shader_ret	  = S_OK;
	t_get_buffer_ret		  = S_OK;
	t_create_render_target_view_ret	  = S_OK;
	t_resize_buffers_ret		  = S_OK;
	t_d3d_compile_error_msgs	  = 0;
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

static int t_gfx_d3d11_draw(gfx_t *gfx, const gfx_target_t *target, const gfx_vertex_2d_t vertices[3])
{
	if (vertices == NULL) {
		return 1;
	}
	gfx_buffer_t buffer = {0};
	if (gfx_buffer_init(&buffer, gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}) != &buffer) {
		return 1;
	}
	if (gfx_buffer_set_data(&buffer, vertices, sizeof(gfx_vertex_2d_t) * 3)) {
		gfx_buffer_free(&buffer);
		return 1;
	}
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	if (t_gfx_d3d11_shader(gfx, &vs, GFX_SHADER_STAGE_VERTEX)) {
		gfx_buffer_free(&buffer);
		return 1;
	}
	if (t_gfx_d3d11_shader(gfx, &fs, GFX_SHADER_STAGE_FRAGMENT)) {
		gfx_shader_free(&vs);
		gfx_buffer_free(&buffer);
		return 1;
	}
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = t_gfx_d3d11_pipeline_config(vs, fs);
	int ret			     = gfx_pipeline_init(&pipeline, gfx, &config) != &pipeline;
	gfx_frame_t frame	     = {0};
	int began		     = 0;
	if (ret == 0) {
		ret = gfx_begin(gfx, &frame, &(gfx_frame_config_t){.target = target});
	}
	if (ret == 0) {
		began = 1;
		ret   = gfx_pipeline_bind(&frame, &pipeline);
	}
	if (ret == 0) {
		ret = gfx_buffer_bind(&frame, &buffer);
	}
	if (ret == 0) {
		ret = gfx_draw(&frame, 3, 0);
	}
	if (began && gfx_end(&frame)) {
		ret = 1;
	}
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_buffer_free(&buffer);
	return ret;
}

static int t_gfx_d3d11_set_surface_target(gfx_t *gfx, u16 width, u16 height)
{
	t_gfx_d3d11_target = (gfx_target_t){
		.type	 = GFX_TARGET_SURFACE,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_surface,
		.width	 = width,
		.height	 = height,
	};
	return gfx_set_target(gfx, &t_gfx_d3d11_target);
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

TEST(gfx_d3d11_viewport_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->viewport(&gfx, 1, 2, 3, 4), 1);

	END;
}

TEST(gfx_d3d11_viewport_missing_context_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_context_vtbl.RSSetViewports = NULL;

	EXPECT_EQ(gfx_viewport(&gfx, 1, 2, 3, 4), 1);

	t_context_vtbl.RSSetViewports = t_RSSetViewports;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_viewport_calls_context)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx_viewport(&gfx, 1, 2, 3, 4), 0);
	EXPECT_EQ(t_rs_set_viewports_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_viewport_passes_count)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	gfx_viewport(&gfx, 1, 2, 3, 4);

	EXPECT_EQ(t_viewport_count, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_viewport_passes_x)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	gfx_viewport(&gfx, 1, 2, 3, 4);

	EXPECT_EQ(t_viewport.TopLeftX, 1.0f);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_viewport_passes_y)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	gfx_viewport(&gfx, 1, 2, 3, 4);

	EXPECT_EQ(t_viewport.TopLeftY, 2.0f);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_viewport_passes_width)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	gfx_viewport(&gfx, 1, 2, 3, 4);

	EXPECT_EQ(t_viewport.Width, 3.0f);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_viewport_passes_height)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	gfx_viewport(&gfx, 1, 2, 3, 4);

	EXPECT_EQ(t_viewport.Height, 4.0f);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_viewport_sets_min_depth)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	gfx_viewport(&gfx, 1, 2, 3, 4);

	EXPECT_EQ(t_viewport.MinDepth, 0.0f);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_viewport_sets_max_depth)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	gfx_viewport(&gfx, 1, 2, 3, 4);

	EXPECT_EQ(t_viewport.MaxDepth, 1.0f);

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

TEST(gfx_d3d11_draw_null_buffer)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(t_gfx_d3d11_draw(&gfx, NULL, NULL), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_without_target)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_EQ(t_gfx_d3d11_draw(&gfx, NULL, vertices), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_begin_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_d3d11_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->begin(NULL), 1);

	END;
}

TEST(gfx_d3d11_begin_missing_render_target_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	void (*saved)(t_d3d11_context_t *, UINT, t_d3d11_view_t *const *, void *) = t_context_vtbl.OMSetRenderTargets;
	t_context_vtbl.OMSetRenderTargets					  = NULL;
	gfx_frame_t frame							  = {.gfx = &gfx, .active = 1};

	EXPECT_EQ(gfx.drv->begin(&frame), 1);

	t_context_vtbl.OMSetRenderTargets = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_clear_missing_clear_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	void (*saved)(t_d3d11_context_t *, t_d3d11_view_t *, const float[4]) = t_context_vtbl.ClearRenderTargetView;
	t_context_vtbl.ClearRenderTargetView				     = NULL;

	EXPECT_EQ(gfx_clear(&gfx, GFX_CLEAR_COLOR_BUFFER), 1);

	t_context_vtbl.ClearRenderTargetView = saved;
	gfx_free(&gfx);
	proc_free(&proc);
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
	EXPECT_NULL(gfx_shader_init(&shader,
				    &gfx,
				    &(gfx_shader_config_t){
					    .compiler = &t_gfx_d3d11_compiler,
					    .source   = STRV("not shader source\n"),
					    .stage    = GFX_SHADER_STAGE_VERTEX,
				    }));
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

TEST(gfx_d3d11_draw_success)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_EQ(t_gfx_d3d11_draw(&gfx, &t_gfx_d3d11_target, vertices), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_compiles_shaders)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	t_gfx_d3d11_draw(&gfx, &t_gfx_d3d11_target, vertices);

	EXPECT_EQ(t_d3d_compile_calls, 2);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_creates_buffer)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	t_gfx_d3d11_draw(&gfx, &t_gfx_d3d11_target, vertices);

	EXPECT_EQ(t_create_buffer_calls, 1);
	EXPECT_EQ(t_create_buffer_bytes, sizeof(t_d3d11_vertex_2d_t) * 3);
	EXPECT_EQ(t_create_buffer_bind_flags, T_D3D11_BIND_VERTEX_BUFFER);

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
	void (*saved)(t_d3d11_context_t *, t_d3d11_buffer_t *, UINT, const void *, const void *, UINT, UINT) =
		t_context_vtbl.UpdateSubresource;
	t_context_vtbl.UpdateSubresource = NULL;
	gfx_vertex_2d_t vertices[3]	 = {0};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	t_context_vtbl.UpdateSubresource = saved;
	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_creates_input_layout)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	t_gfx_d3d11_draw(&gfx, &t_gfx_d3d11_target, vertices);

	EXPECT_EQ(t_create_input_layout_calls, 1);
	EXPECT_EQ(t_input_element_count, 2);
	EXPECT_STR(t_input_semantic_name[0], "POSITION");
	EXPECT_EQ(t_input_semantic_index[0], 0);
	EXPECT_STR(t_input_semantic_name[1], "COLOR");
	EXPECT_EQ(t_input_semantic_index[1], 0);

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

TEST(gfx_d3d11_pipeline_bind_missing_shader_callback)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = t_gfx_d3d11_pipeline_config(vs, fs);
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &config), &pipeline);
	void (*saved)(t_d3d11_context_t *, t_d3d11_vertex_shader_t *, void *const *, UINT) = t_context_vtbl.VSSetShader;
	t_context_vtbl.VSSetShader							   = NULL;
	gfx_frame_t frame								   = {.gfx = &gfx, .active = 1};

	EXPECT_EQ(gfx.drv->pipeline_bind(&frame, &pipeline), 1);

	t_context_vtbl.VSSetShader = saved;
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_binds_render_target)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	t_gfx_d3d11_draw(&gfx, &t_gfx_d3d11_target, vertices);

	EXPECT_EQ(t_om_set_render_targets_calls, 1);
	EXPECT_EQ(t_render_target_count, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_binds_vertex_buffer)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	t_gfx_d3d11_draw(&gfx, &t_gfx_d3d11_target, vertices);

	EXPECT_EQ(t_ia_set_vertex_buffers_calls, 1);
	EXPECT_EQ(t_vertex_buffer_start_slot, 0);
	EXPECT_EQ(t_vertex_buffer_count, 1);
	EXPECT_EQ(t_vertex_buffer_stride, sizeof(t_d3d11_vertex_2d_t));
	EXPECT_EQ(t_vertex_buffer_offset, 0);

	gfx_free(&gfx);
	proc_free(&proc);
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

TEST(gfx_d3d11_buffer_bind_zero_stride)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	struct {
		void *input_layout;
		void *vertex_shader;
		void *pixel_shader;
		UINT stride;
	} pipeline_data		= {0};
	gfx_pipeline_t pipeline = {.gfx = &gfx, .data = &pipeline_data};
	gfx_frame_t frame	= {.gfx = &gfx, .pipeline = &pipeline, .active = 1};

	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 1);

	gfx_buffer_free(&buffer);
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
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_d3d11_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = t_gfx_d3d11_pipeline_config(vs, fs);
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &config), &pipeline);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	void (*saved)(t_d3d11_context_t *, UINT, UINT, t_d3d11_buffer_t *const *, const UINT *, const UINT *) =
		t_context_vtbl.IASetVertexBuffers;
	t_context_vtbl.IASetVertexBuffers = NULL;
	gfx_frame_t frame		  = {.gfx = &gfx, .pipeline = &pipeline, .active = 1};

	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 1);

	t_context_vtbl.IASetVertexBuffers = saved;
	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_uses_triangle_list)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	t_gfx_d3d11_draw(&gfx, &t_gfx_d3d11_target, vertices);

	EXPECT_EQ(t_ia_set_primitive_topology_calls, 1);
	EXPECT_EQ(t_primitive_topology, T_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
	void (*saved)(t_d3d11_context_t *, UINT, UINT) = t_context_vtbl.Draw;
	t_context_vtbl.Draw			       = NULL;
	gfx_frame_t frame			       = {.gfx = &gfx, .active = 1};

	EXPECT_EQ(gfx.drv->draw(&frame, 3, 0), 1);

	t_context_vtbl.Draw = saved;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_draws_three_vertices)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	t_gfx_d3d11_draw(&gfx, &t_gfx_d3d11_target, vertices);

	EXPECT_EQ(t_draw_calls, 1);
	EXPECT_EQ(t_draw_vertex_count, 3);
	EXPECT_EQ(t_draw_start_vertex, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_uploads_first_vertex)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 200, 100);
	gfx_vertex_2d_t vertices[3] = {
		{.x = -0.5f, .y = 0.5f, .r = 0.25f, .g = 0.5f, .b = 0.75f, .a = 1.0f},
		{.x = 0.0f, .y = 0.0f},
		{.x = 0.0f, .y = 0.0f},
	};
	t_gfx_d3d11_draw(&gfx, &t_gfx_d3d11_target, vertices);

	EXPECT_EQ(t_update_subresource_calls, 1);
	EXPECT_EQ(t_uploaded_vertices[0].x, -0.5f);
	EXPECT_EQ(t_uploaded_vertices[0].y, 0.5f);
	EXPECT_EQ(t_uploaded_vertices[0].r, 0.25f);

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
	RUN(gfx_d3d11_viewport_null_data);
	RUN(gfx_d3d11_viewport_missing_context_callback);
	RUN(gfx_d3d11_viewport_calls_context);
	RUN(gfx_d3d11_viewport_passes_count);
	RUN(gfx_d3d11_viewport_passes_x);
	RUN(gfx_d3d11_viewport_passes_y);
	RUN(gfx_d3d11_viewport_passes_width);
	RUN(gfx_d3d11_viewport_passes_height);
	RUN(gfx_d3d11_viewport_sets_min_depth);
	RUN(gfx_d3d11_viewport_sets_max_depth);
	RUN(gfx_d3d11_clear_null_data);
	RUN(gfx_d3d11_clear_zero_buffers);
	RUN(gfx_d3d11_clear_without_target);
	RUN(gfx_d3d11_clear_calls_context);
	RUN(gfx_d3d11_clear_uses_red);
	RUN(gfx_d3d11_begin_null_frame);
	RUN(gfx_d3d11_begin_missing_render_target_callback);
	RUN(gfx_d3d11_clear_missing_clear_callback);
	RUN(gfx_d3d11_draw_null_data);
	RUN(gfx_d3d11_end_null_frame);
	RUN(gfx_d3d11_draw_null_buffer);
	RUN(gfx_d3d11_draw_without_target);
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
	RUN(gfx_d3d11_pipeline_init_missing_input_layout_callback);
	RUN(gfx_d3d11_pipeline_init_alloc_failure);
	RUN(gfx_d3d11_pipeline_init_element_alloc_failure);
	RUN(gfx_d3d11_pipeline_init_create_input_layout_failure);
	RUN(gfx_d3d11_pipeline_init_missing_layout_semantic);
	RUN(gfx_d3d11_pipeline_init_unsupported_input_layout);
	RUN(gfx_d3d11_draw_success);
	RUN(gfx_d3d11_draw_compiles_shaders);
	RUN(gfx_d3d11_draw_creates_buffer);
	RUN(gfx_d3d11_buffer_set_data_missing_update_callback);
	RUN(gfx_d3d11_draw_creates_input_layout);
	RUN(gfx_d3d11_pipeline_bind_null_frame);
	RUN(gfx_d3d11_pipeline_bind_missing_shader_callback);
	RUN(gfx_d3d11_draw_binds_render_target);
	RUN(gfx_d3d11_draw_binds_vertex_buffer);
	RUN(gfx_d3d11_buffer_bind_null_frame);
	RUN(gfx_d3d11_buffer_bind_zero_stride);
	RUN(gfx_d3d11_buffer_bind_missing_vertex_buffer_callback);
	RUN(gfx_d3d11_draw_uses_triangle_list);
	RUN(gfx_d3d11_draw_missing_draw_callback);
	RUN(gfx_d3d11_draw_draws_three_vertices);
	RUN(gfx_d3d11_draw_uploads_first_vertex);
	RUN(gfx_d3d11_present_null_data);
	RUN(gfx_d3d11_present_without_target);
	RUN(gfx_d3d11_present_calls_surface);
	RUN(gfx_d3d11_free_null_data);
	RUN(gfx_d3d11_free_releases_context);
	RUN(gfx_d3d11_free_releases_device);

	t_gfx_d3d11_compiler_free();

	SEND;
}
