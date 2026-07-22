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
	HRESULT (*CreateInputLayout)
	(t_d3d11_device_t *self, const D3D11_INPUT_ELEMENT_DESC *elements, UINT element_count, const void *shader_bytecode,
	 size_t bytecode_length, t_d3d11_input_layout_t **input_layout);
	HRESULT (*CreateVertexShader)
	(t_d3d11_device_t *self, const void *shader_bytecode, size_t bytecode_length, void *class_linkage,
	 t_d3d11_vertex_shader_t **shader);
	HRESULT (*CreateGeometryShader)(void);
	HRESULT (*CreateGeometryShaderWithStreamOutput)(void);
	HRESULT (*CreatePixelShader)
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
	(void)elements;
	(void)shader_bytecode;
	(void)bytecode_length;
	t_create_input_layout_calls++;
	t_input_element_count = element_count;
	*input_layout	      = &t_input_layout;
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
	*error_msgs = NULL;
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

	EXPECT_NOT_NULL(t_gfx_d3d11_driver());

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

	EXPECT_NULL(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = {.alloc = t_gfx_d3d11_alloc_fail}}));

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
	EXPECT_NULL(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}));
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
	EXPECT_NULL(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}));
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
	EXPECT_NULL(gfx_init(&gfx, t_gfx_d3d11_driver(), &(gfx_config_t){.proc = &proc, .alloc = ALLOC_STD}));
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

TEST(gfx_d3d11_draw_triangle_2d_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_d3d11_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_EQ(gfx.drv->draw_triangle_2d(&gfx, vertices), 1);

	END;
}

TEST(gfx_d3d11_draw_triangle_2d_null_vertices)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx.drv->draw_triangle_2d(&gfx, NULL), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_without_target)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_EQ(gfx_draw_triangle_2d(&gfx, vertices), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_success)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_EQ(gfx_draw_triangle_2d(&gfx, vertices), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_compiles_shaders)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_draw_triangle_2d(&gfx, vertices);

	EXPECT_EQ(t_d3d_compile_calls, 2);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_creates_buffer)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_draw_triangle_2d(&gfx, vertices);

	EXPECT_EQ(t_create_buffer_calls, 1);
	EXPECT_EQ(t_create_buffer_bytes, sizeof(t_d3d11_vertex_2d_t) * 3);
	EXPECT_EQ(t_create_buffer_bind_flags, T_D3D11_BIND_VERTEX_BUFFER);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_creates_input_layout)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_draw_triangle_2d(&gfx, vertices);

	EXPECT_EQ(t_create_input_layout_calls, 1);
	EXPECT_EQ(t_input_element_count, 2);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_binds_render_target)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_draw_triangle_2d(&gfx, vertices);

	EXPECT_EQ(t_om_set_render_targets_calls, 1);
	EXPECT_EQ(t_render_target_count, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_binds_vertex_buffer)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_draw_triangle_2d(&gfx, vertices);

	EXPECT_EQ(t_ia_set_vertex_buffers_calls, 1);
	EXPECT_EQ(t_vertex_buffer_start_slot, 0);
	EXPECT_EQ(t_vertex_buffer_count, 1);
	EXPECT_EQ(t_vertex_buffer_stride, sizeof(t_d3d11_vertex_2d_t));
	EXPECT_EQ(t_vertex_buffer_offset, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_uses_triangle_list)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_draw_triangle_2d(&gfx, vertices);

	EXPECT_EQ(t_ia_set_primitive_topology_calls, 1);
	EXPECT_EQ(t_primitive_topology, T_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_draws_three_vertices)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 640, 480);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_draw_triangle_2d(&gfx, vertices);

	EXPECT_EQ(t_draw_calls, 1);
	EXPECT_EQ(t_draw_vertex_count, 3);
	EXPECT_EQ(t_draw_start_vertex, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_d3d11_draw_triangle_2d_uploads_first_vertex)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_d3d11_init_gfx(&gfx, &proc), 0);
	t_gfx_d3d11_set_surface_target(&gfx, 200, 100);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 50.0f, .y = 25.0f, .r = 0.25f, .g = 0.5f, .b = 0.75f, .a = 1.0f},
		{.x = 0.0f, .y = 0.0f},
		{.x = 0.0f, .y = 0.0f},
	};
	gfx_draw_triangle_2d(&gfx, vertices);

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
	RUN(gfx_d3d11_draw_triangle_2d_null_data);
	RUN(gfx_d3d11_draw_triangle_2d_null_vertices);
	RUN(gfx_d3d11_draw_triangle_2d_without_target);
	RUN(gfx_d3d11_draw_triangle_2d_success);
	RUN(gfx_d3d11_draw_triangle_2d_compiles_shaders);
	RUN(gfx_d3d11_draw_triangle_2d_creates_buffer);
	RUN(gfx_d3d11_draw_triangle_2d_creates_input_layout);
	RUN(gfx_d3d11_draw_triangle_2d_binds_render_target);
	RUN(gfx_d3d11_draw_triangle_2d_binds_vertex_buffer);
	RUN(gfx_d3d11_draw_triangle_2d_uses_triangle_list);
	RUN(gfx_d3d11_draw_triangle_2d_draws_three_vertices);
	RUN(gfx_d3d11_draw_triangle_2d_uploads_first_vertex);
	RUN(gfx_d3d11_present_null_data);
	RUN(gfx_d3d11_present_without_target);
	RUN(gfx_d3d11_present_calls_surface);
	RUN(gfx_d3d11_free_null_data);
	RUN(gfx_d3d11_free_releases_context);
	RUN(gfx_d3d11_free_releases_device);

	SEND;
}
