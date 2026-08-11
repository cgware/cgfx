#ifndef D3D11_H
#define D3D11_H

#include "type.h"

#include <stddef.h>

/**
 * @defgroup d3d11 D3D11
 * @ingroup graphics
 */

/**
 * @defgroup d3d11_common Common
 * @ingroup d3d11
 * @{
 */

#define D3D11_SDK_VERSION 7

typedef long HRESULT;
typedef int INT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef void *HMODULE;

typedef struct GUID_s {
	u32 Data1;
	u16 Data2;
	u16 Data3;
	u8 Data4[8];
} GUID;

typedef const GUID *REFIID;

/**
 * @}
 * @defgroup d3d11_buffer Buffer
 * @ingroup d3d11
 * @{
 */

typedef void *ID3D11Buffer;

typedef HRESULT (*PFN_Buffer_QueryInterface)(ID3D11Buffer *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_Buffer_AddRef)(ID3D11Buffer *This);
typedef ULONG (*PFN_Buffer_Release)(ID3D11Buffer *This);

typedef struct ID3D11BufferVTable_s {
	PFN_Buffer_QueryInterface QueryInterface;
	PFN_Buffer_AddRef AddRef;
	PFN_Buffer_Release Release;
} ID3D11BufferVTable;

/**
 * @}
 * @defgroup d3d11_inputlayout InputLayout
 * @ingroup d3d11
 * @{
 */

typedef void *ID3D11InputLayout;

typedef HRESULT (*PFN_InputLayout_QueryInterface)(ID3D11InputLayout *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_InputLayout_AddRef)(ID3D11InputLayout *This);
typedef ULONG (*PFN_InputLayout_Release)(ID3D11InputLayout *This);

typedef struct ID3D11InputLayoutVTable_s {
	PFN_InputLayout_QueryInterface QueryInterface;
	PFN_InputLayout_AddRef AddRef;
	PFN_InputLayout_Release Release;
} ID3D11InputLayoutVTable;

/**
 * @}
 * @defgroup d3d11_pixelshader PixelShader
 * @ingroup d3d11
 * @{
 */

typedef void *ID3D11PixelShader;

typedef HRESULT (*PFN_PixelShader_QueryInterface)(ID3D11PixelShader *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_PixelShader_AddRef)(ID3D11PixelShader *This);
typedef ULONG (*PFN_PixelShader_Release)(ID3D11PixelShader *This);

typedef struct ID3D11PixelShaderVTable_s {
	PFN_PixelShader_QueryInterface QueryInterface;
	PFN_PixelShader_AddRef AddRef;
	PFN_PixelShader_Release Release;
} ID3D11PixelShaderVTable;

/**
 * @}
 * @defgroup d3d11_rendertargetview RenderTargetView
 * @ingroup d3d11
 * @{
 */

typedef void *ID3D11RenderTargetView;
typedef void *ID3D11DepthStencilView;
typedef void *ID3D11DepthStencilState;

typedef HRESULT (*PFN_RenderTargetView_QueryInterface)(ID3D11RenderTargetView *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_RenderTargetView_AddRef)(ID3D11RenderTargetView *This);
typedef ULONG (*PFN_RenderTargetView_Release)(ID3D11RenderTargetView *This);

typedef struct ID3D11RenderTargetViewVTable_s {
	PFN_RenderTargetView_QueryInterface QueryInterface;
	PFN_RenderTargetView_AddRef AddRef;
	PFN_RenderTargetView_Release Release;
} ID3D11RenderTargetViewVTable;

/**
 * @}
 * @defgroup d3d11_texture2d Texture2D
 * @ingroup d3d11
 * @{
 */

typedef void *ID3D11Texture2D;

typedef HRESULT (*PFN_Texture2D_QueryInterface)(ID3D11Texture2D *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_Texture2D_AddRef)(ID3D11Texture2D *This);
typedef ULONG (*PFN_Texture2D_Release)(ID3D11Texture2D *This);

typedef struct ID3D11Texture2DVTable_s {
	PFN_Texture2D_QueryInterface QueryInterface;
	PFN_Texture2D_AddRef AddRef;
	PFN_Texture2D_Release Release;
} ID3D11Texture2DVTable;

/**
 * @}
 * @defgroup d3d11_vertexshader VertexShader
 * @ingroup d3d11
 * @{
 */

typedef void *ID3D11VertexShader;

typedef HRESULT (*PFN_VertexShader_QueryInterface)(ID3D11VertexShader *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_VertexShader_AddRef)(ID3D11VertexShader *This);
typedef ULONG (*PFN_VertexShader_Release)(ID3D11VertexShader *This);

typedef struct ID3D11VertexShaderVTable_s {
	PFN_VertexShader_QueryInterface QueryInterface;
	PFN_VertexShader_AddRef AddRef;
	PFN_VertexShader_Release Release;
} ID3D11VertexShaderVTable;

/**
 * @}
 * @defgroup d3d11_devicecontext DeviceContext
 * @ingroup d3d11
 * @{
 */

typedef void *ID3D11DeviceContext;

typedef HRESULT (*PFN_DeviceContext_QueryInterface)(ID3D11DeviceContext *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_DeviceContext_AddRef)(ID3D11DeviceContext *This);
typedef ULONG (*PFN_DeviceContext_Release)(ID3D11DeviceContext *This);

/**
 * @brief Sets a pixel shader to the device.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-pssetshader
 */
typedef void (*PFN_PSSetShader)(ID3D11DeviceContext *This, ID3D11PixelShader *pPixelShader, void *const *ppClassInstances,
				UINT NumClassInstances);

/**
 * @brief Set a vertex shader to the device.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-vssetshader
 */
typedef void (*PFN_VSSetShader)(ID3D11DeviceContext *This, ID3D11VertexShader *pVertexShader, void *const *ppClassInstances,
				UINT NumClassInstances);

/**
 * @brief Draw indexed, non-instanced primitives.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-drawindexed
 */
typedef void (*PFN_DrawIndexed)(ID3D11DeviceContext *This, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);

/**
 * @brief Draw non-indexed, non-instanced primitives.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-draw
 */
typedef void (*PFN_Draw)(ID3D11DeviceContext *This, UINT VertexCount, UINT StartVertexLocation);

typedef enum D3D11_MAP_e {
	D3D11_MAP_READ = 1,
} D3D11_MAP_t;
typedef UINT D3D11_MAP;

typedef struct D3D11_MAPPED_SUBRESOURCE_s {
	void *pData;
	UINT RowPitch;
	UINT DepthPitch;
} D3D11_MAPPED_SUBRESOURCE;

/**
 * @brief Gets a pointer to the data contained in a subresource, and denies the GPU access to that subresource.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-map
 */
typedef HRESULT (*PFN_Map)(ID3D11DeviceContext *This, void *pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags,
			   D3D11_MAPPED_SUBRESOURCE *pMappedResource);

/**
 * @brief Invalidate the pointer to a resource and reenable the GPU's access to that resource.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-unmap
 */
typedef void (*PFN_Unmap)(ID3D11DeviceContext *This, void *pResource, UINT Subresource);

/**
 * @brief Bind an input-layout object to the input-assembler stage.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetinputlayout
 */
typedef void (*PFN_IASetInputLayout)(ID3D11DeviceContext *This, ID3D11InputLayout *pInputLayout);

/**
 * @brief Bind an array of vertex buffers to the input-assembler stage.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetvertexbuffers
 */
typedef void (*PFN_IASetVertexBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppVertexBuffers,
				       const UINT *pStrides, const UINT *pOffsets);

/**
 * @brief Bind an index buffer to the input-assembler stage.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetindexbuffer
 */
typedef void (*PFN_IASetIndexBuffer)(ID3D11DeviceContext *This, ID3D11Buffer *pIndexBuffer, UINT Format, UINT Offset);

/**
 * @brief Sets the constant buffers used by the vertex shader pipeline stage.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-vssetconstantbuffers
 */
typedef void (*PFN_VSSetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers,
					 ID3D11Buffer *const *ppConstantBuffers);

/**
 * @brief Sets the constant buffers used by the pixel shader pipeline stage.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-pssetconstantbuffers
 */
typedef void (*PFN_PSSetConstantBuffers)(ID3D11DeviceContext *This, UINT StartSlot, UINT NumBuffers,
					 ID3D11Buffer *const *ppConstantBuffers);

typedef enum D3D11_PRIMITIVE_TOPOLOGY_e {
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
} D3D11_PRIMITIVE_TOPOLOGY_t;
typedef UINT D3D11_PRIMITIVE_TOPOLOGY;

/**
 * @brief Bind information about the primitive type, and data order that describes input data for the input assembler stage.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetprimitivetopology
 */
typedef void (*PFN_IASetPrimitiveTopology)(ID3D11DeviceContext *This, D3D11_PRIMITIVE_TOPOLOGY Topology);

/**
 * @brief Bind one or more render targets atomically and the depth-stencil buffer to the output-merger stage.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-omsetrendertargets
 */
typedef void (*PFN_OMSetRenderTargets)(ID3D11DeviceContext *This, UINT NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews,
				       ID3D11DepthStencilView pDepthStencilView);

typedef void (*PFN_OMSetDepthStencilState)(ID3D11DeviceContext *This, ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef);

typedef struct D3D11_VIEWPORT_s {
	float TopLeftX;
	float TopLeftY;
	float Width;
	float Height;
	float MinDepth;
	float MaxDepth;
} D3D11_VIEWPORT;

/**
 * @brief Bind an array of viewports to the rasterizer stage of the pipeline.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-rssetviewports
 */
typedef void (*PFN_RSSetViewports)(ID3D11DeviceContext *This, UINT NumViewports, const D3D11_VIEWPORT *pViewports);

/**
 * @brief Copy the entire contents of the source resource to the destination resource using the GPU.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-copyresource
 */
typedef void (*PFN_CopyResource)(ID3D11DeviceContext *This, void *pDstResource, void *pSrcResource);

/**
 * @brief The CPU copies data from memory to a subresource created in non-mappable memory.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-updatesubresource
 */
typedef void (*PFN_UpdateSubresource)(ID3D11DeviceContext *This, ID3D11Buffer *pDstResource, UINT DstSubresource, const void *pDstBox,
				      const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch);

/**
 * @brief Set all the elements in a render target to one value.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-clearrendertargetview
 */
typedef void (*PFN_ClearRenderTargetView)(ID3D11DeviceContext *This, ID3D11RenderTargetView *pRenderTargetView, const float ColorRGBA[4]);

typedef enum D3D11_CLEAR_FLAG_e {
	D3D11_CLEAR_DEPTH = 0x1,
} D3D11_CLEAR_FLAG_t;
typedef UINT D3D11_CLEAR_FLAG;

typedef void (*PFN_ClearDepthStencilView)(ID3D11DeviceContext *This, ID3D11DepthStencilView pDepthStencilView, D3D11_CLEAR_FLAG ClearFlags,
					  float Depth, u8 Stencil);

typedef struct ID3D11DeviceContextVTable_s {
	PFN_DeviceContext_QueryInterface QueryInterface;
	PFN_DeviceContext_AddRef AddRef;
	PFN_DeviceContext_Release Release;
	void (*GetDevice)(void);
	void (*GetPrivateData)(void);
	void (*SetPrivateData)(void);
	void (*SetPrivateDataInterface)(void);
	PFN_VSSetConstantBuffers VSSetConstantBuffers;
	void (*unused_08)(void);
	PFN_PSSetShader PSSetShader;
	void (*unused_10)(void);
	PFN_VSSetShader VSSetShader;
	PFN_DrawIndexed DrawIndexed;
	PFN_Draw Draw;
	PFN_Map Map;
	PFN_Unmap Unmap;
	PFN_PSSetConstantBuffers PSSetConstantBuffers;
	PFN_IASetInputLayout IASetInputLayout;
	PFN_IASetVertexBuffers IASetVertexBuffers;
	PFN_IASetIndexBuffer IASetIndexBuffer;
	void (*unused_20)(void);
	void (*unused_21)(void);
	void (*unused_22)(void);
	void (*unused_23)(void);
	PFN_IASetPrimitiveTopology IASetPrimitiveTopology;
	void (*unused_25)(void);
	void (*unused_26)(void);
	void (*unused_27)(void);
	void (*unused_28)(void);
	void (*unused_29)(void);
	void (*unused_30)(void);
	void (*unused_31)(void);
	void (*unused_32)(void);
	PFN_OMSetRenderTargets OMSetRenderTargets;
	void (*unused_34)(void);
	void (*unused_35)(void);
	PFN_OMSetDepthStencilState OMSetDepthStencilState;
	void (*unused_37)(void);
	void (*unused_38)(void);
	void (*unused_39)(void);
	void (*unused_40)(void);
	void (*unused_41)(void);
	void (*unused_42)(void);
	void (*unused_43)(void);
	PFN_RSSetViewports RSSetViewports;
	void (*unused_45)(void);
	void (*unused_46)(void);
	PFN_CopyResource CopyResource;
	PFN_UpdateSubresource UpdateSubresource;
	void (*unused_49)(void);
	PFN_ClearRenderTargetView ClearRenderTargetView;
	void (*unused_51)(void);
	void (*unused_52)(void);
	PFN_ClearDepthStencilView ClearDepthStencilView;
} ID3D11DeviceContextVTable;

/**
 * @}
 * @defgroup d3d11_device Device
 * @ingroup d3d11
 * @{
 */

typedef void *ID3D11Device;

typedef int D3D_FEATURE_LEVEL;

typedef enum D3D_DRIVER_TYPE_e {
	D3D_DRIVER_TYPE_HARDWARE = 1,
} D3D_DRIVER_TYPE_t;
typedef UINT D3D_DRIVER_TYPE;

/**
 * @brief Creates a device that represents the display adapter.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdevice
 */
typedef HRESULT (*PFN_D3D11CreateDevice)(void *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
					 const D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion,
					 ID3D11Device **ppDevice, D3D_FEATURE_LEVEL *pFeatureLevel,
					 ID3D11DeviceContext **ppImmediateContext);

typedef HRESULT (*PFN_Device_QueryInterface)(ID3D11Device *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_Device_AddRef)(ID3D11Device *This);
typedef ULONG (*PFN_Device_Release)(ID3D11Device *This);

typedef struct D3D11_BUFFER_DESC_s {
	UINT ByteWidth;
	UINT Usage;
	UINT BindFlags;
	UINT CPUAccessFlags;
	UINT MiscFlags;
	UINT StructureByteStride;
} D3D11_BUFFER_DESC;

typedef struct D3D11_SUBRESOURCE_DATA_s {
	const void *pSysMem;
	UINT SysMemPitch;
	UINT SysMemSlicePitch;
} D3D11_SUBRESOURCE_DATA;

/**
 * @brief Creates a buffer (vertex buffer, index buffer, or shader-constant buffer).
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer
 */
typedef HRESULT (*PFN_CreateBuffer)(ID3D11Device *This, const D3D11_BUFFER_DESC *pDesc, const void *pInitialData, ID3D11Buffer **ppBuffer);

/**
 * @brief Creates an array of 1D textures.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createtexture1d
 */
typedef HRESULT (*PFN_CreateTexture1D)(void);

typedef struct DXGI_SAMPLE_DESC_s {
	UINT Count;
	UINT Quality;
} DXGI_SAMPLE_DESC;

typedef enum D3D11_USAGE_e {
	D3D11_USAGE_DEFAULT   = 0,
	D3D11_USAGE_IMMUTABLE = 1,
	D3D11_USAGE_STAGING   = 3,
} D3D11_USAGE_t;
typedef UINT D3D11_USAGE;

typedef enum D3D11_BIND_FLAG_e {
	D3D11_BIND_VERTEX_BUFFER   = 0x00000001,
	D3D11_BIND_INDEX_BUFFER	   = 0x00000002,
	D3D11_BIND_CONSTANT_BUFFER = 0x00000004,
	D3D11_BIND_RENDER_TARGET   = 0x00000020,
	D3D11_BIND_DEPTH_STENCIL   = 0x00000040,
} D3D11_BIND_FLAG_t;
typedef UINT D3D11_BIND_FLAG;

typedef enum D3D11_CPU_ACCESS_FLAG_e {
	D3D11_CPU_ACCESS_READ = 0x00020000,
} D3D11_CPU_ACCESS_FLAG_t;
typedef UINT D3D11_CPU_ACCESS_FLAG;

typedef struct D3D11_TEXTURE2D_DESC_s {
	UINT Width;
	UINT Height;
	UINT MipLevels;
	UINT ArraySize;
	UINT Format;
	DXGI_SAMPLE_DESC SampleDesc;
	D3D11_USAGE Usage;
	D3D11_BIND_FLAG BindFlags;
	D3D11_CPU_ACCESS_FLAG CPUAccessFlags;
	UINT MiscFlags;
} D3D11_TEXTURE2D_DESC;

/**
 * @brief Create an array of 2D textures.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createtexture2d
 */
typedef HRESULT (*PFN_CreateTexture2D)(ID3D11Device *This, const D3D11_TEXTURE2D_DESC *pDesc, const void *pInitialData,
				       ID3D11Texture2D **ppTexture2D);

/**
 * @brief Create a single 3D texture.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createtexture3d
 */
typedef HRESULT (*PFN_CreateTexture3D)(void);

/**
 * @brief Create a shader-resource view for accessing data in a resource.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createshaderresourceview
 */
typedef HRESULT (*PFN_CreateShaderResourceView)(void);

/**
 * @brief Creates a view for accessing an unordered access resource.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createunorderedaccessview
 */
typedef HRESULT (*PFN_CreateUnorderedAccessView)(void);

/**
 * @brief Creates a render-target view for accessing resource data.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createrendertargetview
 */
typedef HRESULT (*PFN_CreateRenderTargetView)(ID3D11Device *This, void *pResource, const void *pDesc, ID3D11RenderTargetView **ppRTView);

/**
 * @brief Create a depth-stencil view for accessing resource data.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createdepthstencilview
 */
typedef HRESULT (*PFN_CreateDepthStencilView)(ID3D11Device *This, void *pResource, const void *pDesc,
					      ID3D11DepthStencilView **ppDepthStencilView);

typedef enum D3D11_DEPTH_WRITE_MASK_e {
	D3D11_DEPTH_WRITE_MASK_ZERO = 0,
	D3D11_DEPTH_WRITE_MASK_ALL  = 1,
} D3D11_DEPTH_WRITE_MASK_t;
typedef UINT D3D11_DEPTH_WRITE_MASK;

typedef enum D3D11_COMPARISON_FUNC_e {
	D3D11_COMPARISON_LESS = 2,
} D3D11_COMPARISON_FUNC_t;
typedef UINT D3D11_COMPARISON_FUNC;

typedef enum D3D11_STENCIL_OP_e {
	D3D11_STENCIL_OP_KEEP = 1,
} D3D11_STENCIL_OP_t;
typedef UINT D3D11_STENCIL_OP;

typedef struct D3D11_DEPTH_STENCILOP_DESC_s {
	D3D11_STENCIL_OP StencilFailOp;
	D3D11_STENCIL_OP StencilDepthFailOp;
	D3D11_STENCIL_OP StencilPassOp;
	D3D11_COMPARISON_FUNC StencilFunc;
} D3D11_DEPTH_STENCILOP_DESC;

typedef struct D3D11_DEPTH_STENCIL_DESC_s {
	int DepthEnable;
	D3D11_DEPTH_WRITE_MASK DepthWriteMask;
	D3D11_COMPARISON_FUNC DepthFunc;
	int StencilEnable;
	u8 StencilReadMask;
	u8 StencilWriteMask;
	D3D11_DEPTH_STENCILOP_DESC FrontFace;
	D3D11_DEPTH_STENCILOP_DESC BackFace;
} D3D11_DEPTH_STENCIL_DESC;

typedef HRESULT (*PFN_CreateDepthStencilState)(ID3D11Device *This, const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
					       ID3D11DepthStencilState **ppDepthStencilState);

typedef enum DXGI_FORMAT_e {
	DXGI_FORMAT_UNKNOWN	       = 0,
	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	DXGI_FORMAT_R32G32B32_FLOAT    = 6,
	DXGI_FORMAT_R32G32_FLOAT       = 16,
	DXGI_FORMAT_R8G8B8A8_UNORM     = 28,
	DXGI_FORMAT_D32_FLOAT	       = 40,
	DXGI_FORMAT_R32_UINT	       = 42,
	DXGI_FORMAT_R8_UINT	       = 62,
} DXGI_FORMAT_t;
typedef UINT DXGI_FORMAT;

typedef enum D3D11_INPUT_CLASSIFICATION_e {
	D3D11_INPUT_PER_VERTEX_DATA,
} D3D11_INPUT_CLASSIFICATION_t;
typedef UINT D3D11_INPUT_CLASSIFICATION;

typedef struct D3D11_INPUT_ELEMENT_DESC_s {
	const char *SemanticName;
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	D3D11_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
} D3D11_INPUT_ELEMENT_DESC;

/**
 * @brief Create an input-layout object to describe the input-buffer data for the input-assembler stage.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createinputlayout
 */
typedef HRESULT (*PFN_CreateInputLayout)(ID3D11Device *This, const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements,
					 const void *pShaderBytecodeWithInputSignature, size_t BytecodeLength,
					 ID3D11InputLayout **ppInputLayout);

/**
 * @brief Create a vertex-shader object from a compiled shader.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createvertexshader
 */
typedef HRESULT (*PFN_CreateVertexShader)(ID3D11Device *This, const void *pShaderBytecode, size_t BytecodeLength, void *pClassLinkage,
					  ID3D11VertexShader **ppVertexShader);

/**
 * @brief Create a geometry shader.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-creategeometryshader
 */
typedef HRESULT (*PFN_CreateGeometryShader)(void);

/**
 * @brief Creates a geometry shader that can write to streaming output buffers.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-creategeometryshaderwithstreamoutput
 */
typedef HRESULT (*PFN_CreateGeometryShaderWithStreamOutput)(void);

/**
 * @brief Create a pixel shader.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createpixelshader
 */
typedef HRESULT (*PFN_CreatePixelShader)(ID3D11Device *This, const void *pShaderBytecode, size_t BytecodeLength, void *pClassLinkage,
					 ID3D11PixelShader **ppPixelShader);

/**
 * @}
 * @ingroup d3d11_device
 */
typedef struct ID3D11DeviceVTable_s {
	PFN_Device_QueryInterface QueryInterface;
	PFN_Device_AddRef AddRef;
	PFN_Device_Release Release;
	PFN_CreateBuffer CreateBuffer;
	PFN_CreateTexture1D CreateTexture1D;
	PFN_CreateTexture2D CreateTexture2D;
	PFN_CreateTexture3D CreateTexture3D;
	PFN_CreateShaderResourceView CreateShaderResourceView;
	PFN_CreateUnorderedAccessView CreateUnorderedAccessView;
	PFN_CreateRenderTargetView CreateRenderTargetView;
	PFN_CreateDepthStencilView CreateDepthStencilView;
	PFN_CreateInputLayout CreateInputLayout;
	PFN_CreateVertexShader CreateVertexShader;
	PFN_CreateGeometryShader CreateGeometryShader;
	PFN_CreateGeometryShaderWithStreamOutput CreateGeometryShaderWithStreamOutput;
	PFN_CreatePixelShader CreatePixelShader;
	void (*CreateHullShader)(void);
	void (*CreateDomainShader)(void);
	void (*CreateComputeShader)(void);
	void (*CreateClassLinkage)(void);
	void (*CreateBlendState)(void);
	PFN_CreateDepthStencilState CreateDepthStencilState;
} ID3D11DeviceVTable;

/**
 * @}
 * @defgroup d3d11_blob Blob
 * @ingroup d3d11
 * @{
 */

typedef void *ID3DBlob;

typedef HRESULT (*PFN_Blob_QueryInterface)(ID3DBlob *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_Blob_AddRef)(ID3DBlob *This);
typedef ULONG (*PFN_Blob_Release)(ID3DBlob *This);

/**
 * @brief Retrieves a pointer to the blob's data.
 * @see https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ff728744(v=vs.85)
 */
typedef void *(*PFN_GetBufferPointer)(ID3DBlob *This);

/**
 * @brief Retrieves the size, in bytes, of the blob's data.
 * @see https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ff728745(v=vs.85)
 */
typedef size_t (*PFN_GetBufferSize)(ID3DBlob *This);

typedef struct ID3DBlobVTable_s {
	PFN_Blob_QueryInterface QueryInterface;
	PFN_Blob_AddRef AddRef;
	PFN_Blob_Release Release;
	PFN_GetBufferPointer GetBufferPointer;
	PFN_GetBufferSize GetBufferSize;
} ID3DBlobVTable;

/**
 * @}
 * @defgroup d3d11_swapchain SwapChain
 * @ingroup d3d11
 * @{
 */

static const GUID IID_ID3D11Texture2D = {0x6f15aaf2u, 0xd208u, 0x4e89u, {0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c}};

typedef void *IDXGISwapChain;

typedef HRESULT (*PFN_SwapChain_QueryInterface)(IDXGISwapChain *This, REFIID riid, void **ppvObject);
typedef ULONG (*PFN_SwapChain_AddRef)(IDXGISwapChain *This);
typedef ULONG (*PFN_SwapChain_Release)(IDXGISwapChain *This);
typedef HRESULT (*PFN_SwapChain_SetPrivateData)(void);
typedef HRESULT (*PFN_SwapChain_SetPrivateDataInterface)(void);
typedef HRESULT (*PFN_SwapChain_GetPrivateData)(void);
typedef HRESULT (*PFN_SwapChain_GetParent)(void);
typedef HRESULT (*PFN_SwapChain_GetDevice)(void);

/**
 * @brief Presents a rendered image to the user.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-present
 */
typedef HRESULT (*PFN_Present)(IDXGISwapChain *This, UINT SyncInterval, UINT Flags);

/**
 * @brief Accesses one of the swap-chain's back buffers.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-getbuffer
 */
typedef HRESULT (*PFN_GetBuffer)(IDXGISwapChain *This, UINT Buffer, REFIID riid, void **ppSurface);

/**
 * @brief Sets the display state to windowed or full screen.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-setfullscreenstate
 */
typedef HRESULT (*PFN_SetFullscreenState)(void);

/**
 * @brief Get the state associated with full-screen mode.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-getfullscreenstate
 */
typedef HRESULT (*PFN_GetFullscreenState)(void);

/**
 * @brief Get a description of the swap chain.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-getdesc
 */
typedef HRESULT (*PFN_GetDesc)(void);

/**
 * @brief Changes the swap chain's back buffer size, format, and number of buffers. This should be called when the application window is
 * resized.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
 */
typedef HRESULT (*PFN_ResizeBuffers)(IDXGISwapChain *This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat,
				     UINT SwapChainFlags);

typedef struct IDXGISwapChainVTable_s {
	PFN_SwapChain_QueryInterface QueryInterface;
	PFN_SwapChain_AddRef AddRef;
	PFN_SwapChain_Release Release;
	PFN_SwapChain_SetPrivateData SetPrivateData;
	PFN_SwapChain_SetPrivateDataInterface SetPrivateDataInterface;
	PFN_SwapChain_GetPrivateData GetPrivateData;
	PFN_SwapChain_GetParent GetParent;
	PFN_SwapChain_GetDevice GetDevice;
	PFN_Present Present;
	PFN_GetBuffer GetBuffer;
	PFN_SetFullscreenState SetFullscreenState;
	PFN_GetFullscreenState GetFullscreenState;
	PFN_GetDesc GetDesc;
	PFN_ResizeBuffers ResizeBuffers;
} IDXGISwapChainVTable;

/**
 * @}
 * @defgroup d3d11_compiler Compiler
 * @ingroup d3d11
 * @{
 */

/**
 * @brief Compile HLSL code or an effect file into bytecode for a given target.
 * @see https://learn.microsoft.com/en-us/windows/win32/api/d3dcompiler/nf-d3dcompiler-d3dcompile
 */
typedef HRESULT (*PFN_D3DCompile)(const void *pSrcData, size_t SrcDataSize, const char *pSourceName, const void *pDefines, void *pInclude,
				  const char *pEntrypoint, const char *pTarget, UINT Flags1, UINT Flags2, ID3DBlob **ppCode,
				  ID3DBlob **ppErrorMsgs);

/**
 * @}
 */

#endif
