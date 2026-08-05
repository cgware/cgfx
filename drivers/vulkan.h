#ifndef VULKAN_H
#define VULKAN_H

#include "type.h"

#include <stddef.h>

/**
 * @defgroup vulkan Vulkan
 * @ingroup graphics
 */

/**
 * @defgroup vulkan_common Common
 * @ingroup vulkan
 * @{
 */

typedef u32 VkFlags;

typedef u32 VkBool32;

typedef enum VkStructureType_e {
	VK_STRUCTURE_TYPE_APPLICATION_INFO			    = 0,
	VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO			    = 1,
	VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO		    = 2,
	VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO			    = 3,
	VK_STRUCTURE_TYPE_SUBMIT_INFO				    = 4,
	VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO			    = 5,
	VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE			    = 6,
	VK_STRUCTURE_TYPE_FENCE_CREATE_INFO			    = 8,
	VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO			    = 12,
	VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO			    = 14,
	VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO		    = 15,
	VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO		    = 16,
	VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO	    = 18,
	VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO   = 19,
	VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO = 20,
	VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO	    = 22,
	VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO  = 23,
	VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO    = 24,
	VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO    = 26,
	VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO	    = 27,
	VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO		    = 28,
	VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO		    = 30,
	VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO		    = 37,
	VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO		    = 38,
	VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO		    = 39,
	VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO		    = 40,
	VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO		    = 42,
	VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO		    = 43,
	VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER			    = 45,
	VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR		    = 1000001000,
	VK_STRUCTURE_TYPE_PRESENT_INFO_KHR			    = 1000001001,
	VK_STRUCTURE_TYPE_MAX_ENUM				    = 0x7FFFFFFF
} VkStructureType;

typedef enum VkResult_e {
	VK_SUCCESS		 = 0,
	VK_SUBOPTIMAL_KHR	 = 1000001003,
	VK_ERROR_OUT_OF_DATE_KHR = -1000001004,
	VK_RESULT_MAX_ENUM	 = 0x7FFFFFFF
} VkResult;

typedef void (*PFN_vkVoidFunction)(void);

/**
 * @}
 * @defgroup vulkan_allocation Allocation
 * @ingroup vulkan
 * @{
 */

typedef enum VkSystemAllocationScope_e {
	VK_SYSTEM_ALLOCATION_SCOPE_MAX_ENUM = 0x7FFFFFFF
} VkSystemAllocationScope;

typedef enum VkInternalAllocationType_e {
	VK_INTERNAL_ALLOCATION_TYPE_MAX_ENUM = 0x7FFFFFFF
} VkInternalAllocationType;

typedef void *(*PFN_vkAllocationFunction)(void *pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);

typedef void (*PFN_vkFreeFunction)(void *pUserData, void *pMemory);

typedef void (*PFN_vkInternalAllocationNotification)(void *pUserData, size_t size, VkInternalAllocationType allocationType,
						     VkSystemAllocationScope allocationScope);

typedef void (*PFN_vkInternalFreeNotification)(void *pUserData, size_t size, VkInternalAllocationType allocationType,
					       VkSystemAllocationScope allocationScope);

typedef void *(*PFN_vkReallocationFunction)(void *pUserData, void *pOriginal, size_t size, size_t alignment,
					    VkSystemAllocationScope allocationScope);

typedef struct VkAllocationCallbacks_s {
	void *pUserData;
	PFN_vkAllocationFunction pfnAllocation;
	PFN_vkReallocationFunction pfnReallocation;
	PFN_vkFreeFunction pfnFree;
	PFN_vkInternalAllocationNotification pfnInternalAllocation;
	PFN_vkInternalFreeNotification pfnInternalFree;
} VkAllocationCallbacks;

/**
 * @}
 * @defgroup vulkan_instance Instance
 * @ingroup vulkan
 * @{
 */

typedef u64 VkInstance;

#define VK_MAKE_API_VERSION(variant, major, minor, patch)                                                                                  \
	((((u32)(variant)) << 29U) | (((u32)(major)) << 22U) | (((u32)(minor)) << 12U) | ((u32)(patch)))

#define VK_API_VERSION_1_0 VK_MAKE_API_VERSION(0, 1, 0, 0)

typedef VkFlags VkInstanceCreateFlags;

typedef struct VkApplicationInfo_s {
	VkStructureType sType;
	const void *pNext;
	const char *pApplicationName;
	u32 applicationVersion;
	const char *pEngineName;
	u32 engineVersion;
	u32 apiVersion;
} VkApplicationInfo;

typedef struct VkInstanceCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkInstanceCreateFlags flags;
	const VkApplicationInfo *pApplicationInfo;
	u32 enabledLayerCount;
	const char *const *ppEnabledLayerNames;
	u32 enabledExtensionCount;
	const char *const *ppEnabledExtensionNames;
} VkInstanceCreateInfo;

/**
 * @brief Create a new Vulkan instance
 * @param[out] pInstance must be freed using PFN_vkDestroyInstance()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateInstance.html
 */
typedef VkResult (*PFN_vkCreateInstance)(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
					 VkInstance *pInstance);

/**
 * @brief Destroy an instance of Vulkan
 * @param[in] instance created by PFN_vkCreateInstance()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyInstance.html
 */
typedef void (*PFN_vkDestroyInstance)(VkInstance instance, const VkAllocationCallbacks *pAllocator);

/**
 * @brief Return a function pointer for a command
 * @param[in] instance created by PFN_vkCreateInstance()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetInstanceProcAddr.html
 */
typedef PFN_vkVoidFunction (*PFN_vkGetInstanceProcAddr)(VkInstance instance, const char *pName);

/**
 * @}
 * @defgroup vulkan_surface Surface
 * @ingroup vulkan
 * @{
 */

typedef u64 VkSurfaceKHR;

/**
 * @}
 * @defgroup vulkan_physicaldevice PhysicalDevice
 * @ingroup vulkan
 * @{
 */

typedef u64 VkPhysicalDevice;

/**
 * @brief Enumerates the physical devices accessible to a Vulkan instance
 * @param[out] pPhysicalDevices
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkEnumeratePhysicalDevices.html
 */
typedef VkResult (*PFN_vkEnumeratePhysicalDevices)(VkInstance instance, u32 *pPhysicalDeviceCount, VkPhysicalDevice *pPhysicalDevices);

typedef enum VkQueueFlagBits_e {
	VK_QUEUE_GRAPHICS_BIT	    = 0x00000001,
	VK_QUEUE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkQueueFlagBits;
typedef VkFlags VkQueueFlags;

typedef struct VkExtent3D_s {
	u32 width;
	u32 height;
	u32 depth;
} VkExtent3D;

typedef struct VkQueueFamilyProperties_s {
	VkQueueFlags queueFlags;
	u32 queueCount;
	u32 timestampValidBits;
	VkExtent3D minImageTransferGranularity;
} VkQueueFamilyProperties;

/**
 * @brief Reports properties of the queues of the specified physical device
 * @param[in] physicalDevice created by PFN_vkEnumeratePhysicalDevices()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetPhysicalDeviceQueueFamilyProperties.html
 */
typedef void (*PFN_vkGetPhysicalDeviceQueueFamilyProperties)(VkPhysicalDevice physicalDevice, u32 *pQueueFamilyPropertyCount,
							     VkQueueFamilyProperties *pQueueFamilyProperties);

#define VK_MAX_MEMORY_TYPES 32U
#define VK_MAX_MEMORY_HEAPS 16U

typedef enum VkMemoryPropertyFlagBits_e {
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT   = 0x00000002,
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT  = 0x00000004,
	VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkMemoryPropertyFlagBits;
typedef VkFlags VkMemoryPropertyFlags;

typedef struct VkMemoryType_s {
	VkMemoryPropertyFlags propertyFlags;
	u32 heapIndex;
} VkMemoryType;

typedef u64 VkDeviceSize;

typedef VkFlags VkMemoryHeapFlags;

typedef struct VkMemoryHeap_s {
	VkDeviceSize size;
	VkMemoryHeapFlags flags;
} VkMemoryHeap;

typedef struct VkPhysicalDeviceMemoryProperties_s {
	u32 memoryTypeCount;
	VkMemoryType memoryTypes[VK_MAX_MEMORY_TYPES];
	u32 memoryHeapCount;
	VkMemoryHeap memoryHeaps[VK_MAX_MEMORY_HEAPS];
} VkPhysicalDeviceMemoryProperties;

/**
 * @brief Reports memory information for the specified physical device
 * @param[in] physicalDevice created by PFN_vkEnumeratePhysicalDevices()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetPhysicalDeviceMemoryProperties.html
 */
typedef void (*PFN_vkGetPhysicalDeviceMemoryProperties)(VkPhysicalDevice physicalDevice,
							VkPhysicalDeviceMemoryProperties *pMemoryProperties);

typedef enum VkFormatFeatureFlagBits_e {
	VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT = 0x00000080,
	VK_FORMAT_FEATURE_TRANSFER_DST_BIT     = 0x00008000,
	VK_FORMAT_FEATURE_FLAG_BITS_MAX_ENUM   = 0x7FFFFFFF
} VkFormatFeatureFlagBits;
typedef VkFlags VkFormatFeatureFlags;

typedef struct VkFormatProperties_s {
	VkFormatFeatureFlags linearTilingFeatures;
	VkFormatFeatureFlags optimalTilingFeatures;
	VkFormatFeatureFlags bufferFeatures;
} VkFormatProperties;

typedef enum VkFormat_e {
	VK_FORMAT_UNDEFINED	      = 0,
	VK_FORMAT_R8G8B8A8_UNORM      = 37,
	VK_FORMAT_R8G8B8A8_SRGB	      = 43,
	VK_FORMAT_B8G8R8A8_UNORM      = 44,
	VK_FORMAT_B8G8R8A8_SRGB	      = 50,
	VK_FORMAT_R32G32_SFLOAT	      = 103,
	VK_FORMAT_R32G32B32A32_SFLOAT = 109,
	VK_FORMAT_MAX_ENUM	      = 0x7FFFFFFF
} VkFormat;

/**
 * @brief Lists physical device’s format capabilities
 * @param[in] physicalDevice created by PFN_vkEnumeratePhysicalDevices()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetPhysicalDeviceFormatProperties.html
 */
typedef void (*PFN_vkGetPhysicalDeviceFormatProperties)(VkPhysicalDevice physicalDevice, VkFormat format,
							VkFormatProperties *pFormatProperties);

/**
 * @brief Query if presentation is supported
 * @param[in] physicalDevice created by PFN_vkEnumeratePhysicalDevices()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetPhysicalDeviceSurfaceSupportKHR.html
 */
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(VkPhysicalDevice physicalDevice, u32 queueFamilyIndex, VkSurfaceKHR surface,
							     VkBool32 *pSupported);

typedef struct VkExtent2D_s {
	u32 width;
	u32 height;
} VkExtent2D;

typedef enum VkSurfaceTransformFlagBitsKHR_e {
	VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR = 0x7FFFFFFF
} VkSurfaceTransformFlagBitsKHR;
typedef VkFlags VkSurfaceTransformFlagsKHR;

typedef enum VkCompositeAlphaFlagBitsKHR_e {
	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR	  = 0x00000001,
	VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR = 0x7FFFFFFF
} VkCompositeAlphaFlagBitsKHR;
typedef VkFlags VkCompositeAlphaFlagsKHR;

typedef enum VkImageUsageFlagBits_e {
	VK_IMAGE_USAGE_TRANSFER_DST_BIT	    = 0x00000002,
	VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00000010,
	VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM   = 0x7FFFFFFF
} VkImageUsageFlagBits;
typedef VkFlags VkImageUsageFlags;

typedef struct VkSurfaceCapabilitiesKHR_s {
	u32 minImageCount;
	u32 maxImageCount;
	VkExtent2D currentExtent;
	VkExtent2D minImageExtent;
	VkExtent2D maxImageExtent;
	u32 maxImageArrayLayers;
	VkSurfaceTransformFlagsKHR supportedTransforms;
	VkSurfaceTransformFlagBitsKHR currentTransform;
	VkCompositeAlphaFlagsKHR supportedCompositeAlpha;
	VkImageUsageFlags supportedUsageFlags;
} VkSurfaceCapabilitiesKHR;

/**
 * @brief Query surface capabilities
 * @param[in] physicalDevice created by PFN_vkEnumeratePhysicalDevices()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetPhysicalDeviceSurfaceCapabilitiesKHR.html
 */
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
								  VkSurfaceCapabilitiesKHR *pSurfaceCapabilities);

typedef enum VkPresentModeKHR_e {
	VK_PRESENT_MODE_IMMEDIATE_KHR = 0,
	VK_PRESENT_MODE_MAILBOX_KHR   = 1,
	VK_PRESENT_MODE_FIFO_KHR      = 2,
	VK_PRESENT_MODE_MAX_ENUM_KHR  = 0x7FFFFFFF
} VkPresentModeKHR;

/**
 * @brief Query supported presentation modes
 * @param[in] physicalDevice created by PFN_vkEnumeratePhysicalDevices()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetPhysicalDeviceSurfacePresentModesKHR.html
 */
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
								  u32 *pPresentModeCount, VkPresentModeKHR *pPresentModes);

typedef enum VkColorSpaceKHR_e {
	VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0,
	VK_COLOR_SPACE_MAX_ENUM_KHR	  = 0x7FFFFFFF
} VkColorSpaceKHR;

typedef struct VkSurfaceFormatKHR_s {
	VkFormat format;
	VkColorSpaceKHR colorSpace;
} VkSurfaceFormatKHR;

/**
 * @brief Query color formats supported by surface
 * @param[in] physicalDevice created by PFN_vkEnumeratePhysicalDevices()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetPhysicalDeviceSurfaceFormatsKHR.html
 */
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
							     u32 *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats);

/**
 * @}
 * @defgroup vulkan_device Device
 * @ingroup vulkan
 * @{
 */

typedef u64 VkDevice;

typedef VkFlags VkDeviceCreateFlags;

typedef VkFlags VkDeviceQueueCreateFlags;

typedef struct VkDeviceQueueCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkDeviceQueueCreateFlags flags;
	u32 queueFamilyIndex;
	u32 queueCount;
	const float *pQueuePriorities;
} VkDeviceQueueCreateInfo;

typedef struct VkPhysicalDeviceFeatures_s {
	VkBool32 robustBufferAccess;
	VkBool32 fullDrawIndexUint32;
	VkBool32 imageCubeArray;
	VkBool32 independentBlend;
	VkBool32 geometryShader;
	VkBool32 tessellationShader;
	VkBool32 sampleRateShading;
	VkBool32 dualSrcBlend;
	VkBool32 logicOp;
	VkBool32 multiDrawIndirect;
	VkBool32 drawIndirectFirstInstance;
	VkBool32 depthClamp;
	VkBool32 depthBiasClamp;
	VkBool32 fillModeNonSolid;
	VkBool32 depthBounds;
	VkBool32 wideLines;
	VkBool32 largePoints;
	VkBool32 alphaToOne;
	VkBool32 multiViewport;
	VkBool32 samplerAnisotropy;
	VkBool32 textureCompressionETC2;
	VkBool32 textureCompressionASTC_LDR;
	VkBool32 textureCompressionBC;
	VkBool32 occlusionQueryPrecise;
	VkBool32 pipelineStatisticsQuery;
	VkBool32 vertexPipelineStoresAndAtomics;
	VkBool32 fragmentStoresAndAtomics;
	VkBool32 shaderTessellationAndGeometryPointSize;
	VkBool32 shaderImageGatherExtended;
	VkBool32 shaderStorageImageExtendedFormats;
	VkBool32 shaderStorageImageMultisample;
	VkBool32 shaderStorageImageReadWithoutFormat;
	VkBool32 shaderStorageImageWriteWithoutFormat;
	VkBool32 shaderUniformBufferArrayDynamicIndexing;
	VkBool32 shaderSampledImageArrayDynamicIndexing;
	VkBool32 shaderStorageBufferArrayDynamicIndexing;
	VkBool32 shaderStorageImageArrayDynamicIndexing;
	VkBool32 shaderClipDistance;
	VkBool32 shaderCullDistance;
	VkBool32 shaderFloat64;
	VkBool32 shaderInt64;
	VkBool32 shaderInt16;
	VkBool32 shaderResourceResidency;
	VkBool32 shaderResourceMinLod;
	VkBool32 sparseBinding;
	VkBool32 sparseResidencyBuffer;
	VkBool32 sparseResidencyImage2D;
	VkBool32 sparseResidencyImage3D;
	VkBool32 sparseResidency2Samples;
	VkBool32 sparseResidency4Samples;
	VkBool32 sparseResidency8Samples;
	VkBool32 sparseResidency16Samples;
	VkBool32 sparseResidencyAliased;
	VkBool32 variableMultisampleRate;
	VkBool32 inheritedQueries;
} VkPhysicalDeviceFeatures;

typedef struct VkDeviceCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkDeviceCreateFlags flags;
	u32 queueCreateInfoCount;
	const VkDeviceQueueCreateInfo *pQueueCreateInfos;
	// enabledLayerCount is legacy and not used
	u32 enabledLayerCount;
	// ppEnabledLayerNames is legacy and not used
	const char *const *ppEnabledLayerNames;
	u32 enabledExtensionCount;
	const char *const *ppEnabledExtensionNames;
	const VkPhysicalDeviceFeatures *pEnabledFeatures;
} VkDeviceCreateInfo;

/**
 * @brief Create a new device instance
 * @param[out] pDevice must be freed using PFN_vkDestroyDevice()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateDevice.html
 */
typedef VkResult (*PFN_vkCreateDevice)(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo,
				       const VkAllocationCallbacks *pAllocator, VkDevice *pDevice);

/**
 * @brief Destroy a logical device
 * @param[in] device created by PFN_vkCreateDevice()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyDevice.html
 */
typedef void (*PFN_vkDestroyDevice)(VkDevice device, const VkAllocationCallbacks *pAllocator);

/**
 * @brief Return a function pointer for a command
 * @param[in] device created by PFN_vkCreateDevice()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetDeviceProcAddr.html
 */
typedef PFN_vkVoidFunction (*PFN_vkGetDeviceProcAddr)(VkDevice device, const char *pName);

/**
 * @brief Wait for a device to become idle
 * @param[in] device created by PFN_vkCreateDevice()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDeviceWaitIdle.html
 */
typedef VkResult (*PFN_vkDeviceWaitIdle)(VkDevice device);

/**
 * @}
 * @defgroup vulkan_commandpool CommandPool
 * @ingroup vulkan
 * @{
 */

typedef u64 VkCommandPool;

typedef enum VkCommandPoolCreateFlagBits_e {
	VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT = 0x00000002,
	VK_COMMAND_POOL_CREATE_FLAG_BITS_MAX_ENUM	= 0x7FFFFFFF
} VkCommandPoolCreateFlagBits;
typedef VkFlags VkCommandPoolCreateFlags;

typedef struct VkCommandPoolCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkCommandPoolCreateFlags flags;
	u32 queueFamilyIndex;
} VkCommandPoolCreateInfo;

/**
 * @brief Create a new command pool object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pCommandPool must be freed using PFN_vkDestroyCommandPool()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateCommandPool.html
 */
typedef VkResult (*PFN_vkCreateCommandPool)(VkDevice device, const VkCommandPoolCreateInfo *pCreateInfo,
					    const VkAllocationCallbacks *pAllocator, VkCommandPool *pCommandPool);

/**
 * @brief Destroy a command pool object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] commandPool created by PFN_vkCreateCommandPool()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyCommandPool.html
 */
typedef void (*PFN_vkDestroyCommandPool)(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks *pAllocator);

/**
 * @}
 * @defgroup vulkan_fence Fence
 * @ingroup vulkan
 * @{
 */

typedef u64 VkFence;

typedef enum VkFenceCreateFlagBits_e {
	VK_FENCE_CREATE_SIGNALED_BIT	   = 0x00000001,
	VK_FENCE_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkFenceCreateFlagBits;
typedef VkFlags VkFenceCreateFlags;

typedef struct VkFenceCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkFenceCreateFlags flags;
} VkFenceCreateInfo;

/**
 * @brief Create a new fence object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pFence must be freed using PFN_vkDestroyFence()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateFence.html
 */
typedef VkResult (*PFN_vkCreateFence)(VkDevice device, const VkFenceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
				      VkFence *pFence);

/**
 * @brief Destroy a fence object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] fence created by PFN_vkCreateFence()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyFence.html
 */
typedef void (*PFN_vkDestroyFence)(VkDevice device, VkFence fence, const VkAllocationCallbacks *pAllocator);

/**
 * @brief Resets one or more fence objects
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] pFences created by PFN_vkCreateFence()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkResetFences.html
 */
typedef VkResult (*PFN_vkResetFences)(VkDevice device, u32 fenceCount, const VkFence *pFences);

/**
 * @brief Wait for one or more fences to become signaled
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] pFences created by PFN_vkCreateFence()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkWaitForFences.html
 */
typedef VkResult (*PFN_vkWaitForFences)(VkDevice device, u32 fenceCount, const VkFence *pFences, VkBool32 waitAll, u64 timeout);

/**
 * @}
 * @defgroup vulkan_image Image
 * @ingroup vulkan
 * @{
 */

typedef u64 VkImage;

typedef VkFlags VkImageCreateFlags;

typedef enum VkImageType_e {
	VK_IMAGE_TYPE_2D       = 1,
	VK_IMAGE_TYPE_MAX_ENUM = 0x7FFFFFFF
} VkImageType;

typedef enum VkSampleCountFlagBits_e {
	VK_SAMPLE_COUNT_1_BIT		   = 0x00000001,
	VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkSampleCountFlagBits;

typedef enum VkImageTiling_e {
	VK_IMAGE_TILING_LINEAR	 = 1,
	VK_IMAGE_TILING_MAX_ENUM = 0x7FFFFFFF
} VkImageTiling;

typedef enum VkSharingMode_e {
	VK_SHARING_MODE_EXCLUSIVE = 0,
	VK_SHARING_MODE_MAX_ENUM  = 0x7FFFFFFF
} VkSharingMode;

typedef enum VkImageLayout_e {
	VK_IMAGE_LAYOUT_UNDEFINED		 = 0,
	VK_IMAGE_LAYOUT_GENERAL			 = 1,
	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR		 = 1000001002,
	VK_IMAGE_LAYOUT_MAX_ENUM		 = 0x7FFFFFFF
} VkImageLayout;

typedef struct VkImageCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkImageCreateFlags flags;
	VkImageType imageType;
	VkFormat format;
	VkExtent3D extent;
	u32 mipLevels;
	u32 arrayLayers;
	VkSampleCountFlagBits samples;
	VkImageTiling tiling;
	VkImageUsageFlags usage;
	VkSharingMode sharingMode;
	u32 queueFamilyIndexCount;
	const u32 *pQueueFamilyIndices;
	VkImageLayout initialLayout;
} VkImageCreateInfo;

/**
 * @brief Create a new image object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pImage must be freed using PFN_vkDestroyImage()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateImage.html
 */
typedef VkResult (*PFN_vkCreateImage)(VkDevice device, const VkImageCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
				      VkImage *pImage);

/**
 * @brief Destroy an image object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] image created by PFN_vkCreateImage()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyImage.html
 */
typedef void (*PFN_vkDestroyImage)(VkDevice device, VkImage image, const VkAllocationCallbacks *pAllocator);

typedef struct VkMemoryRequirements_s {
	VkDeviceSize size;
	VkDeviceSize alignment;
	u32 memoryTypeBits;
} VkMemoryRequirements;

/**
 * @brief Returns the memory requirements for specified Vulkan object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] image created by PFN_vkCreateImage()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetImageMemoryRequirements.html
 */
typedef void (*PFN_vkGetImageMemoryRequirements)(VkDevice device, VkImage image, VkMemoryRequirements *pMemoryRequirements);

typedef enum VkImageAspectFlagBits_e {
	VK_IMAGE_ASPECT_COLOR_BIT	   = 0x00000001,
	VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkImageAspectFlagBits;
typedef VkFlags VkImageAspectFlags;

typedef struct VkImageSubresource_s {
	VkImageAspectFlags aspectMask;
	u32 mipLevel;
	u32 arrayLayer;
} VkImageSubresource;

typedef struct VkSubresourceLayout_s {
	VkDeviceSize offset;
	VkDeviceSize size;
	VkDeviceSize rowPitch;
	VkDeviceSize arrayPitch;
	VkDeviceSize depthPitch;
} VkSubresourceLayout;

/**
 * @brief Retrieve information about an image subresource
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] image created by PFN_vkCreateImage()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetImageSubresourceLayout.html
 */
typedef void (*PFN_vkGetImageSubresourceLayout)(VkDevice device, VkImage image, const VkImageSubresource *pSubresource,
						VkSubresourceLayout *pLayout);

/**
 * @}
 * @defgroup vulkan_buffer Buffer
 * @ingroup vulkan
 * @{
 */

typedef u64 VkBuffer;

typedef VkFlags VkBufferCreateFlags;

typedef enum VkBufferUsageFlagBits_e {
	VK_BUFFER_USAGE_INDEX_BUFFER_BIT   = 0x00000040,
	VK_BUFFER_USAGE_VERTEX_BUFFER_BIT  = 0x00000080,
	VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkBufferUsageFlagBits;
typedef VkFlags VkBufferUsageFlags;

typedef struct VkBufferCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkBufferCreateFlags flags;
	VkDeviceSize size;
	VkBufferUsageFlags usage;
	VkSharingMode sharingMode;
	u32 queueFamilyIndexCount;
	const u32 *pQueueFamilyIndices;
} VkBufferCreateInfo;

/**
 * @brief Create a new buffer object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pBuffer must be freed using PFN_vkDestroyBuffer()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateBuffer.html
 */
typedef VkResult (*PFN_vkCreateBuffer)(VkDevice device, const VkBufferCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
				       VkBuffer *pBuffer);

/**
 * @brief Destroy a buffer object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] buffer created by PFN_vkCreateBuffer()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyBuffer.html
 */
typedef void (*PFN_vkDestroyBuffer)(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks *pAllocator);

/**
 * @brief Returns the memory requirements for specified Vulkan object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] buffer created by PFN_vkCreateBuffer()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetBufferMemoryRequirements.html
 */
typedef void (*PFN_vkGetBufferMemoryRequirements)(VkDevice device, VkBuffer buffer, VkMemoryRequirements *pMemoryRequirements);

/**
 * @}
 * @defgroup vulkan_devicememory DeviceMemory
 * @ingroup vulkan
 * @{
 */

typedef u64 VkDeviceMemory;

typedef struct VkMemoryAllocateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkDeviceSize allocationSize;
	u32 memoryTypeIndex;
} VkMemoryAllocateInfo;

/**
 * @brief Allocate device memory
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pMemory must be freed using PFN_vkFreeMemory()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkAllocateMemory.html
 */
typedef VkResult (*PFN_vkAllocateMemory)(VkDevice device, const VkMemoryAllocateInfo *pAllocateInfo,
					 const VkAllocationCallbacks *pAllocator, VkDeviceMemory *pMemory);

/**
 * @brief Free device memory
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] memory created by PFN_vkAllocateMemory()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkFreeMemory.html
 */
typedef void (*PFN_vkFreeMemory)(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks *pAllocator);

typedef VkFlags VkMemoryMapFlags;

/**
 * @brief Map a memory object into application address space
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] memory created by PFN_vkAllocateMemory()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkMapMemory.html
 */
typedef VkResult (*PFN_vkMapMemory)(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags,
				    void **ppData);

/**
 * @brief Unmap a previously mapped memory object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] memory created by PFN_vkAllocateMemory()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkUnmapMemory.html
 */
typedef void (*PFN_vkUnmapMemory)(VkDevice device, VkDeviceMemory memory);

typedef struct VkMappedMemoryRange_s {
	VkStructureType sType;
	const void *pNext;
	VkDeviceMemory memory;
	VkDeviceSize offset;
	VkDeviceSize size;
} VkMappedMemoryRange;

/**
 * @brief Flush mapped memory ranges
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] pMemoryRanges created by PFN_vkAllocateMemory()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkFlushMappedMemoryRanges.html
 */
typedef VkResult (*PFN_vkFlushMappedMemoryRanges)(VkDevice device, u32 memoryRangeCount, const VkMappedMemoryRange *pMemoryRanges);

/**
 * @brief Invalidate ranges of mapped memory objects
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] pMemoryRanges created by PFN_vkAllocateMemory()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkInvalidateMappedMemoryRanges.html
 */
typedef VkResult (*PFN_vkInvalidateMappedMemoryRanges)(VkDevice device, u32 memoryRangeCount, const VkMappedMemoryRange *pMemoryRanges);

/**
 * @brief Bind device memory to an image object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] image created by PFN_vkCreateImage()
 * @param[in] memory created by PFN_vkAllocateMemory()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkBindImageMemory.html
 */
typedef VkResult (*PFN_vkBindImageMemory)(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset);

/**
 * @brief Bind device memory to a buffer object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] buffer created by PFN_vkCreateBuffer()
 * @param[in] memory created by PFN_vkAllocateMemory()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkBindBufferMemory.html
 */
typedef VkResult (*PFN_vkBindBufferMemory)(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset);

/**
 * @}
 * @defgroup vulkan_imageview ImageView
 * @ingroup vulkan
 * @{
 */

typedef u64 VkImageView;

typedef VkFlags VkImageViewCreateFlags;

typedef enum VkImageViewType_e {
	VK_IMAGE_VIEW_TYPE_2D	    = 1,
	VK_IMAGE_VIEW_TYPE_MAX_ENUM = 0x7FFFFFFF
} VkImageViewType;

typedef enum VkComponentSwizzle_e {
	VK_COMPONENT_SWIZZLE_MAX_ENUM = 0x7FFFFFFF
} VkComponentSwizzle;

typedef struct VkComponentMapping_s {
	VkComponentSwizzle r;
	VkComponentSwizzle g;
	VkComponentSwizzle b;
	VkComponentSwizzle a;
} VkComponentMapping;

typedef struct VkImageSubresourceRange_s {
	VkImageAspectFlags aspectMask;
	u32 baseMipLevel;
	u32 levelCount;
	u32 baseArrayLayer;
	u32 layerCount;
} VkImageSubresourceRange;

typedef struct VkImageViewCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkImageViewCreateFlags flags;
	VkImage image;
	VkImageViewType viewType;
	VkFormat format;
	VkComponentMapping components;
	VkImageSubresourceRange subresourceRange;
} VkImageViewCreateInfo;

/**
 * @brief Create an image view from an existing image
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pView must be freed using PFN_vkDestroyImageView()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateImageView.html
 */
typedef VkResult (*PFN_vkCreateImageView)(VkDevice device, const VkImageViewCreateInfo *pCreateInfo,
					  const VkAllocationCallbacks *pAllocator, VkImageView *pView);

/**
 * @brief Destroy an image view object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] imageView created by PFN_vkCreateImageView()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyImageView.html
 */
typedef void (*PFN_vkDestroyImageView)(VkDevice device, VkImageView imageView, const VkAllocationCallbacks *pAllocator);

/**
 * @}
 * @defgroup vulkan_shadermodule ShaderModule
 * @ingroup vulkan
 * @{
 */

typedef u64 VkShaderModule;

typedef VkFlags VkShaderModuleCreateFlags;

typedef struct VkShaderModuleCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkShaderModuleCreateFlags flags;
	size_t codeSize;
	const u32 *pCode;
} VkShaderModuleCreateInfo;

/**
 * @brief Creates a new shader module object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pShaderModule must be freed using PFN_vkDestroyShaderModule()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateShaderModule.html
 */
typedef VkResult (*PFN_vkCreateShaderModule)(VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo,
					     const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule);

/**
 * @brief Destroy a shader module
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] shaderModule created by PFN_vkCreateShaderModule()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyShaderModule.html
 */
typedef void (*PFN_vkDestroyShaderModule)(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks *pAllocator);

/**
 * @}
 * @defgroup vulkan_renderpass RenderPass
 * @ingroup vulkan
 * @{
 */

typedef u64 VkRenderPass;

typedef VkFlags VkRenderPassCreateFlags;

typedef VkFlags VkAttachmentDescriptionFlags;

typedef enum VkAttachmentLoadOp_e {
	VK_ATTACHMENT_LOAD_OP_LOAD	= 0,
	VK_ATTACHMENT_LOAD_OP_CLEAR	= 1,
	VK_ATTACHMENT_LOAD_OP_DONT_CARE = 2,
	VK_ATTACHMENT_LOAD_OP_MAX_ENUM	= 0x7FFFFFFF
} VkAttachmentLoadOp;

typedef enum VkAttachmentStoreOp_e {
	VK_ATTACHMENT_STORE_OP_STORE	 = 0,
	VK_ATTACHMENT_STORE_OP_DONT_CARE = 1,
	VK_ATTACHMENT_STORE_OP_MAX_ENUM	 = 0x7FFFFFFF
} VkAttachmentStoreOp;

typedef struct VkAttachmentDescription_s {
	VkAttachmentDescriptionFlags flags;
	VkFormat format;
	VkSampleCountFlagBits samples;
	VkAttachmentLoadOp loadOp;
	VkAttachmentStoreOp storeOp;
	VkAttachmentLoadOp stencilLoadOp;
	VkAttachmentStoreOp stencilStoreOp;
	VkImageLayout initialLayout;
	VkImageLayout finalLayout;
} VkAttachmentDescription;

typedef VkFlags VkSubpassDescriptionFlags;

typedef enum VkPipelineBindPoint_e {
	VK_PIPELINE_BIND_POINT_GRAPHICS = 0,
	VK_PIPELINE_BIND_POINT_MAX_ENUM = 0x7FFFFFFF
} VkPipelineBindPoint;

typedef struct VkAttachmentReference_s {
	u32 attachment;
	VkImageLayout layout;
} VkAttachmentReference;

typedef struct VkSubpassDescription_s {
	VkSubpassDescriptionFlags flags;
	VkPipelineBindPoint pipelineBindPoint;
	u32 inputAttachmentCount;
	const VkAttachmentReference *pInputAttachments;
	u32 colorAttachmentCount;
	const VkAttachmentReference *pColorAttachments;
	const VkAttachmentReference *pResolveAttachments;
	const VkAttachmentReference *pDepthStencilAttachment;
	u32 preserveAttachmentCount;
	const u32 *pPreserveAttachments;
} VkSubpassDescription;

typedef enum VkPipelineStageFlagBits_e {
	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT	      = 0x00000001,
	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT	      = 0x00002000,
	VK_PIPELINE_STAGE_HOST_BIT		      = 0x00004000,
	VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM	      = 0x7FFFFFFF
} VkPipelineStageFlagBits;
typedef VkFlags VkPipelineStageFlags;

typedef enum VkAccessFlagBits_e {
	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT = 0x00000100,
	VK_ACCESS_FLAG_BITS_MAX_ENUM	     = 0x7FFFFFFF
} VkAccessFlagBits;
typedef VkFlags VkAccessFlags;

typedef VkFlags VkDependencyFlags;

typedef struct VkSubpassDependency_s {
	u32 srcSubpass;
	u32 dstSubpass;
	VkPipelineStageFlags srcStageMask;
	VkPipelineStageFlags dstStageMask;
	VkAccessFlags srcAccessMask;
	VkAccessFlags dstAccessMask;
	VkDependencyFlags dependencyFlags;
} VkSubpassDependency;

typedef struct VkRenderPassCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkRenderPassCreateFlags flags;
	u32 attachmentCount;
	const VkAttachmentDescription *pAttachments;
	u32 subpassCount;
	const VkSubpassDescription *pSubpasses;
	u32 dependencyCount;
	const VkSubpassDependency *pDependencies;
} VkRenderPassCreateInfo;

/**
 * @brief Create a new render pass object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pRenderPass must be freed using PFN_vkDestroyRenderPass()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateRenderPass.html
 */
typedef VkResult (*PFN_vkCreateRenderPass)(VkDevice device, const VkRenderPassCreateInfo *pCreateInfo,
					   const VkAllocationCallbacks *pAllocator, VkRenderPass *pRenderPass);

/**
 * @brief Destroy a render pass object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] renderPass created by PFN_vkCreateRenderPass()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyRenderPass.html
 */
typedef void (*PFN_vkDestroyRenderPass)(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks *pAllocator);

/**
 * @}
 * @defgroup vulkan_framebuffer Framebuffer
 * @ingroup vulkan
 * @{
 */

typedef u64 VkFramebuffer;

typedef VkFlags VkFramebufferCreateFlags;

typedef struct VkFramebufferCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkFramebufferCreateFlags flags;
	VkRenderPass renderPass;
	u32 attachmentCount;
	const VkImageView *pAttachments;
	u32 width;
	u32 height;
	u32 layers;
} VkFramebufferCreateInfo;

/**
 * @brief Create a new framebuffer object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pFramebuffer must be freed using PFN_vkDestroyFramebuffer()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateFramebuffer.html
 */
typedef VkResult (*PFN_vkCreateFramebuffer)(VkDevice device, const VkFramebufferCreateInfo *pCreateInfo,
					    const VkAllocationCallbacks *pAllocator, VkFramebuffer *pFramebuffer);

/**
 * @brief Destroy a framebuffer object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] framebuffer created by PFN_vkCreateFramebuffer()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyFramebuffer.html
 */
typedef void (*PFN_vkDestroyFramebuffer)(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks *pAllocator);

/**
 * @}
 * @defgroup vulkan_pipelinelayout PipelineLayout
 * @ingroup vulkan
 * @{
 */

typedef u64 VkPipelineLayout;

typedef VkFlags VkPipelineLayoutCreateFlags;

typedef u64 VkDescriptorSetLayout;

typedef VkFlags VkShaderStageFlags;

typedef struct VkPushConstantRange_s {
	VkShaderStageFlags stageFlags;
	u32 offset;
	u32 size;
} VkPushConstantRange;

typedef struct VkPipelineLayoutCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineLayoutCreateFlags flags;
	u32 setLayoutCount;
	const VkDescriptorSetLayout *pSetLayouts;
	u32 pushConstantRangeCount;
	const VkPushConstantRange *pPushConstantRanges;
} VkPipelineLayoutCreateInfo;

/**
 * @brief Creates a new pipeline layout object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pPipelineLayout must be freed using PFN_vkDestroyPipelineLayout()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreatePipelineLayout.html
 */
typedef VkResult (*PFN_vkCreatePipelineLayout)(VkDevice device, const VkPipelineLayoutCreateInfo *pCreateInfo,
					       const VkAllocationCallbacks *pAllocator, VkPipelineLayout *pPipelineLayout);

/**
 * @brief Destroy a pipeline layout object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] pipelineLayout created by PFN_vkCreatePipelineLayout()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyPipelineLayout.html
 */
typedef void (*PFN_vkDestroyPipelineLayout)(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks *pAllocator);

/**
 * @}
 * @defgroup vulkan_pipeline Pipeline
 * @ingroup vulkan
 * @{
 */

typedef u64 VkPipeline;
typedef u64 VkPipelineCache;

typedef VkFlags VkPipelineCreateFlags;

typedef VkFlags VkPipelineShaderStageCreateFlags;

typedef enum VkShaderStageFlagBits_e {
	VK_SHADER_STAGE_VERTEX_BIT	   = 0x00000001,
	VK_SHADER_STAGE_FRAGMENT_BIT	   = 0x00000010,
	VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkShaderStageFlagBits;

typedef struct VkSpecializationMapEntry_s {
	u32 constantID;
	u32 offset;
	size_t size;
} VkSpecializationMapEntry;

typedef struct VkSpecializationInfo_s {
	u32 mapEntryCount;
	const VkSpecializationMapEntry *pMapEntries;
	size_t dataSize;
	const void *pData;
} VkSpecializationInfo;

typedef struct VkPipelineShaderStageCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineShaderStageCreateFlags flags;
	VkShaderStageFlagBits stage;
	VkShaderModule module;
	const char *pName;
	const VkSpecializationInfo *pSpecializationInfo;
} VkPipelineShaderStageCreateInfo;

typedef VkFlags VkPipelineVertexInputStateCreateFlags;

typedef enum VkVertexInputRate_e {
	VK_VERTEX_INPUT_RATE_VERTEX   = 0,
	VK_VERTEX_INPUT_RATE_MAX_ENUM = 0x7FFFFFFF
} VkVertexInputRate;

typedef struct VkVertexInputBindingDescription_s {
	u32 binding;
	u32 stride;
	VkVertexInputRate inputRate;
} VkVertexInputBindingDescription;

typedef struct VkVertexInputAttributeDescription_s {
	u32 location;
	u32 binding;
	VkFormat format;
	u32 offset;
} VkVertexInputAttributeDescription;

typedef struct VkPipelineVertexInputStateCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineVertexInputStateCreateFlags flags;
	u32 vertexBindingDescriptionCount;
	const VkVertexInputBindingDescription *pVertexBindingDescriptions;
	u32 vertexAttributeDescriptionCount;
	const VkVertexInputAttributeDescription *pVertexAttributeDescriptions;
} VkPipelineVertexInputStateCreateInfo;

typedef VkFlags VkPipelineInputAssemblyStateCreateFlags;

typedef enum VkPrimitiveTopology_e {
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
	VK_PRIMITIVE_TOPOLOGY_MAX_ENUM	    = 0x7FFFFFFF
} VkPrimitiveTopology;

typedef struct VkPipelineInputAssemblyStateCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineInputAssemblyStateCreateFlags flags;
	VkPrimitiveTopology topology;
	VkBool32 primitiveRestartEnable;
} VkPipelineInputAssemblyStateCreateInfo;

typedef VkFlags VkPipelineTessellationStateCreateFlags;

typedef struct VkPipelineTessellationStateCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineTessellationStateCreateFlags flags;
	u32 patchControlPoints;
} VkPipelineTessellationStateCreateInfo;

typedef VkFlags VkPipelineViewportStateCreateFlags;

typedef struct VkViewport_s {
	float x;
	float y;
	float width;
	float height;
	float minDepth;
	float maxDepth;
} VkViewport;

typedef struct VkOffset2D_s {
	s32 x;
	s32 y;
} VkOffset2D;

typedef struct VkRect2D_s {
	VkOffset2D offset;
	VkExtent2D extent;
} VkRect2D;

typedef struct VkPipelineViewportStateCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineViewportStateCreateFlags flags;
	u32 viewportCount;
	const VkViewport *pViewports;
	u32 scissorCount;
	const VkRect2D *pScissors;
} VkPipelineViewportStateCreateInfo;

typedef VkFlags VkPipelineRasterizationStateCreateFlags;

typedef enum VkPolygonMode_e {
	VK_POLYGON_MODE_FILL	 = 0,
	VK_POLYGON_MODE_MAX_ENUM = 0x7FFFFFFF
} VkPolygonMode;

typedef enum VkCullModeFlagBits_e {
	VK_CULL_MODE_NONE		= 0,
	VK_CULL_MODE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkCullModeFlagBits;
typedef VkFlags VkCullModeFlags;

typedef enum VkFrontFace_e {
	VK_FRONT_FACE_COUNTER_CLOCKWISE = 0,
	VK_FRONT_FACE_MAX_ENUM		= 0x7FFFFFFF
} VkFrontFace;

typedef struct VkPipelineRasterizationStateCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineRasterizationStateCreateFlags flags;
	VkBool32 depthClampEnable;
	VkBool32 rasterizerDiscardEnable;
	VkPolygonMode polygonMode;
	VkCullModeFlags cullMode;
	VkFrontFace frontFace;
	VkBool32 depthBiasEnable;
	float depthBiasConstantFactor;
	float depthBiasClamp;
	float depthBiasSlopeFactor;
	float lineWidth;
} VkPipelineRasterizationStateCreateInfo;

typedef VkFlags VkPipelineMultisampleStateCreateFlags;

typedef u32 VkSampleMask;

typedef struct VkPipelineMultisampleStateCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineMultisampleStateCreateFlags flags;
	VkSampleCountFlagBits rasterizationSamples;
	VkBool32 sampleShadingEnable;
	float minSampleShading;
	const VkSampleMask *pSampleMask;
	VkBool32 alphaToCoverageEnable;
	VkBool32 alphaToOneEnable;
} VkPipelineMultisampleStateCreateInfo;

typedef VkFlags VkPipelineDepthStencilStateCreateFlags;

typedef enum VkCompareOp_e {
	VK_COMPARE_OP_MAX_ENUM = 0x7FFFFFFF
} VkCompareOp;

typedef enum VkStencilOp_e {
	VK_STENCIL_OP_MAX_ENUM = 0x7FFFFFFF
} VkStencilOp;

typedef struct VkStencilOpState_s {
	VkStencilOp failOp;
	VkStencilOp passOp;
	VkStencilOp depthFailOp;
	VkCompareOp compareOp;
	u32 compareMask;
	u32 writeMask;
	u32 reference;
} VkStencilOpState;

typedef struct VkPipelineDepthStencilStateCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineDepthStencilStateCreateFlags flags;
	VkBool32 depthTestEnable;
	VkBool32 depthWriteEnable;
	VkCompareOp depthCompareOp;
	VkBool32 depthBoundsTestEnable;
	VkBool32 stencilTestEnable;
	VkStencilOpState front;
	VkStencilOpState back;
	float minDepthBounds;
	float maxDepthBounds;
} VkPipelineDepthStencilStateCreateInfo;

typedef VkFlags VkPipelineColorBlendStateCreateFlags;

typedef enum VkLogicOp_e {
	VK_LOGIC_OP_MAX_ENUM = 0x7FFFFFFF
} VkLogicOp;

typedef enum VkBlendFactor_e {
	VK_BLEND_FACTOR_ZERO	 = 0,
	VK_BLEND_FACTOR_ONE	 = 1,
	VK_BLEND_FACTOR_MAX_ENUM = 0x7FFFFFFF
} VkBlendFactor;

typedef enum VkBlendOp_e {
	VK_BLEND_OP_ADD	     = 0,
	VK_BLEND_OP_MAX_ENUM = 0x7FFFFFFF
} VkBlendOp;

typedef enum VkColorComponentFlagBits_e {
	VK_COLOR_COMPONENT_R_BIT	      = 0x00000001,
	VK_COLOR_COMPONENT_G_BIT	      = 0x00000002,
	VK_COLOR_COMPONENT_B_BIT	      = 0x00000004,
	VK_COLOR_COMPONENT_A_BIT	      = 0x00000008,
	VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VkColorComponentFlagBits;
typedef VkFlags VkColorComponentFlags;

typedef struct VkPipelineColorBlendAttachmentState_s {
	VkBool32 blendEnable;
	VkBlendFactor srcColorBlendFactor;
	VkBlendFactor dstColorBlendFactor;
	VkBlendOp colorBlendOp;
	VkBlendFactor srcAlphaBlendFactor;
	VkBlendFactor dstAlphaBlendFactor;
	VkBlendOp alphaBlendOp;
	VkColorComponentFlags colorWriteMask;
} VkPipelineColorBlendAttachmentState;

typedef struct VkPipelineColorBlendStateCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineColorBlendStateCreateFlags flags;
	VkBool32 logicOpEnable;
	VkLogicOp logicOp;
	u32 attachmentCount;
	const VkPipelineColorBlendAttachmentState *pAttachments;
	float blendConstants[4];
} VkPipelineColorBlendStateCreateInfo;

typedef VkFlags VkPipelineDynamicStateCreateFlags;

typedef enum VkDynamicState_e {
	VK_DYNAMIC_STATE_VIEWPORT = 0,
	VK_DYNAMIC_STATE_SCISSOR  = 1,
	VK_DYNAMIC_STATE_MAX_ENUM = 0x7FFFFFFF
} VkDynamicState;

typedef struct VkPipelineDynamicStateCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineDynamicStateCreateFlags flags;
	u32 dynamicStateCount;
	const VkDynamicState *pDynamicStates;
} VkPipelineDynamicStateCreateInfo;

typedef struct VkGraphicsPipelineCreateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkPipelineCreateFlags flags;
	u32 stageCount;
	const VkPipelineShaderStageCreateInfo *pStages;
	const VkPipelineVertexInputStateCreateInfo *pVertexInputState;
	const VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState;
	const VkPipelineTessellationStateCreateInfo *pTessellationState;
	const VkPipelineViewportStateCreateInfo *pViewportState;
	const VkPipelineRasterizationStateCreateInfo *pRasterizationState;
	const VkPipelineMultisampleStateCreateInfo *pMultisampleState;
	const VkPipelineDepthStencilStateCreateInfo *pDepthStencilState;
	const VkPipelineColorBlendStateCreateInfo *pColorBlendState;
	const VkPipelineDynamicStateCreateInfo *pDynamicState;
	VkPipelineLayout layout;
	VkRenderPass renderPass;
	u32 subpass;
	VkPipeline basePipelineHandle;
	s32 basePipelineIndex;
} VkGraphicsPipelineCreateInfo;

/**
 * @brief Create graphics pipelines
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pPipelines must be freed using PFN_vkDestroyPipeline()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateGraphicsPipelines.html
 */
typedef VkResult (*PFN_vkCreateGraphicsPipelines)(VkDevice device, VkPipelineCache pipelineCache, u32 createInfoCount,
						  const VkGraphicsPipelineCreateInfo *pCreateInfos, const VkAllocationCallbacks *pAllocator,
						  VkPipeline *pPipelines);

/**
 * @brief Destroy a pipeline object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] pipeline created by PFN_vkCreateGraphicsPipelines()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroyPipeline.html
 */
typedef void (*PFN_vkDestroyPipeline)(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks *pAllocator);

/**
 * @}
 * @defgroup vulkan_commandbuffer CommandBuffer
 * @ingroup vulkan
 * @{
 */

typedef u64 VkCommandBuffer;

typedef enum VkCommandBufferLevel_e {
	VK_COMMAND_BUFFER_LEVEL_PRIMARY	 = 0,
	VK_COMMAND_BUFFER_LEVEL_MAX_ENUM = 0x7FFFFFFF
} VkCommandBufferLevel;

typedef struct VkCommandBufferAllocateInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkCommandPool commandPool;
	VkCommandBufferLevel level;
	u32 commandBufferCount;
} VkCommandBufferAllocateInfo;

/**
 * @brief Allocate command buffers from an existing command pool
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pCommandBuffers must be freed using PFN_vkFreeCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkAllocateCommandBuffers.html
 */
typedef VkResult (*PFN_vkAllocateCommandBuffers)(VkDevice device, const VkCommandBufferAllocateInfo *pAllocateInfo,
						 VkCommandBuffer *pCommandBuffers);

/**
 * @brief Free command buffers
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] pCommandBuffers created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkFreeCommandBuffers.html
 */
typedef void (*PFN_vkFreeCommandBuffers)(VkDevice device, VkCommandPool commandPool, u32 commandBufferCount,
					 const VkCommandBuffer *pCommandBuffers);

typedef enum VkCommandBufferUsageFlagBits_e {
	VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT = 0x00000001,
	VK_COMMAND_BUFFER_USAGE_FLAG_BITS_MAX_ENUM  = 0x7FFFFFFF
} VkCommandBufferUsageFlagBits;
typedef VkFlags VkCommandBufferUsageFlags;

typedef VkFlags VkQueryControlFlags;

typedef VkFlags VkQueryPipelineStatisticFlags;

typedef struct VkCommandBufferInheritanceInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkRenderPass renderPass;
	u32 subpass;
	VkFramebuffer framebuffer;
	VkBool32 occlusionQueryEnable;
	VkQueryControlFlags queryFlags;
	VkQueryPipelineStatisticFlags pipelineStatistics;
} VkCommandBufferInheritanceInfo;

typedef struct VkCommandBufferBeginInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkCommandBufferUsageFlags flags;
	const VkCommandBufferInheritanceInfo *pInheritanceInfo;
} VkCommandBufferBeginInfo;

/**
 * @brief Start recording a command buffer
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkBeginCommandBuffer.html
 */
typedef VkResult (*PFN_vkBeginCommandBuffer)(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo *pBeginInfo);

/**
 * @brief Finish recording a command buffer
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkEndCommandBuffer.html
 */
typedef VkResult (*PFN_vkEndCommandBuffer)(VkCommandBuffer commandBuffer);

typedef VkFlags VkCommandBufferResetFlags;

/**
 * @brief Reset a command buffer to the initial state
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkResetCommandBuffer.html
 */
typedef VkResult (*PFN_vkResetCommandBuffer)(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags);

#define VK_QUEUE_FAMILY_IGNORED (~0U)

typedef struct VkMemoryBarrier_s {
	VkStructureType sType;
	const void *pNext;
	VkAccessFlags srcAccessMask;
	VkAccessFlags dstAccessMask;
} VkMemoryBarrier;

typedef struct VkBufferMemoryBarrier_s {
	VkStructureType sType;
	const void *pNext;
	VkAccessFlags srcAccessMask;
	VkAccessFlags dstAccessMask;
	u32 srcQueueFamilyIndex;
	u32 dstQueueFamilyIndex;
	VkBuffer buffer;
	VkDeviceSize offset;
	VkDeviceSize size;
} VkBufferMemoryBarrier;

typedef struct VkImageMemoryBarrier_s {
	VkStructureType sType;
	const void *pNext;
	VkAccessFlags srcAccessMask;
	VkAccessFlags dstAccessMask;
	VkImageLayout oldLayout;
	VkImageLayout newLayout;
	u32 srcQueueFamilyIndex;
	u32 dstQueueFamilyIndex;
	VkImage image;
	VkImageSubresourceRange subresourceRange;
} VkImageMemoryBarrier;

/**
 * @brief Insert a memory dependency
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdPipelineBarrier.html
 */
typedef void (*PFN_vkCmdPipelineBarrier)(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask,
					 VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, u32 memoryBarrierCount,
					 const VkMemoryBarrier *pMemoryBarriers, u32 bufferMemoryBarrierCount,
					 const VkBufferMemoryBarrier *pBufferMemoryBarriers, u32 imageMemoryBarrierCount,
					 const VkImageMemoryBarrier *pImageMemoryBarriers);

typedef union VkClearColorValue_u {
	float float32[4];
	s32 int32[4];
	u32 uint32[4];
} VkClearColorValue;

/**
 * @brief Clear regions of a color image
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @param[in] image created by PFN_vkCreateImage()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdClearColorImage.html
 */
typedef void (*PFN_vkCmdClearColorImage)(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
					 const VkClearColorValue *pColor, u32 rangeCount, const VkImageSubresourceRange *pRanges);

typedef struct VkClearDepthStencilValue_s {
	float depth;
	u32 stencil;
} VkClearDepthStencilValue;

typedef union VkClearValue_u {
	VkClearColorValue color;
	VkClearDepthStencilValue depthStencil;
} VkClearValue;

typedef struct VkRenderPassBeginInfo_s {
	VkStructureType sType;
	const void *pNext;
	VkRenderPass renderPass;
	VkFramebuffer framebuffer;
	VkRect2D renderArea;
	u32 clearValueCount;
	const VkClearValue *pClearValues;
} VkRenderPassBeginInfo;

typedef enum VkSubpassContents_e {
	VK_SUBPASS_CONTENTS_INLINE   = 0,
	VK_SUBPASS_CONTENTS_MAX_ENUM = 0x7FFFFFFF
} VkSubpassContents;

/**
 * @brief Begin a new render pass
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdBeginRenderPass.html
 */
typedef void (*PFN_vkCmdBeginRenderPass)(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo *pRenderPassBegin,
					 VkSubpassContents contents);

/**
 * @brief End the current render pass
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdEndRenderPass.html
 */
typedef void (*PFN_vkCmdEndRenderPass)(VkCommandBuffer commandBuffer);

/**
 * @brief Bind a pipeline object to a command buffer
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdBindPipeline.html
 */
typedef void (*PFN_vkCmdBindPipeline)(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline);

/**
 * @brief Bind vertex buffers to a command buffer
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdBindVertexBuffers.html
 */
typedef void (*PFN_vkCmdBindVertexBuffers)(VkCommandBuffer commandBuffer, u32 firstBinding, u32 bindingCount, const VkBuffer *pBuffers,
					   const VkDeviceSize *pOffsets);

typedef enum VkIndexType_e {
	VK_INDEX_TYPE_UINT32   = 1,
	VK_INDEX_TYPE_MAX_ENUM = 0x7FFFFFFF
} VkIndexType;

/**
 * @brief Bind an index buffer to a command buffer
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @param[in] buffer created by PFN_vkCreateBuffer()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdBindIndexBuffer.html
 */
typedef void (*PFN_vkCmdBindIndexBuffer)(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);

/**
 * @brief Set the viewport dynamically for a command buffer
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdSetViewport.html
 */
typedef void (*PFN_vkCmdSetViewport)(VkCommandBuffer commandBuffer, u32 firstViewport, u32 viewportCount, const VkViewport *pViewports);

/**
 * @brief Set scissor rectangles dynamically for a command buffer
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdSetScissor.html
 */
typedef void (*PFN_vkCmdSetScissor)(VkCommandBuffer commandBuffer, u32 firstScissor, u32 scissorCount, const VkRect2D *pScissors);

/**
 * @brief Draw primitives
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdDraw.html
 */
typedef void (*PFN_vkCmdDraw)(VkCommandBuffer commandBuffer, u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);

/**
 * @brief Draw primitives with indexed vertices
 * @param[in] commandBuffer created by PFN_vkAllocateCommandBuffers()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdDrawIndexed.html
 */
typedef void (*PFN_vkCmdDrawIndexed)(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, s32 vertexOffset,
				     u32 firstInstance);

/**
 * @}
 * @defgroup vulkan_semaphore Semaphore
 * @ingroup vulkan
 * @{
 */

typedef u64 VkSemaphore;

/**
 * @}
 * @defgroup vulkan_swapchain Swapchain
 * @ingroup vulkan
 * @{
 */

typedef u64 VkSwapchainKHR;

typedef VkFlags VkSwapchainCreateFlagsKHR;

typedef struct VkSwapchainCreateInfoKHR_s {
	VkStructureType sType;
	const void *pNext;
	VkSwapchainCreateFlagsKHR flags;
	VkSurfaceKHR surface;
	u32 minImageCount;
	VkFormat imageFormat;
	VkColorSpaceKHR imageColorSpace;
	VkExtent2D imageExtent;
	u32 imageArrayLayers;
	VkImageUsageFlags imageUsage;
	VkSharingMode imageSharingMode;
	u32 queueFamilyIndexCount;
	const u32 *pQueueFamilyIndices;
	VkSurfaceTransformFlagBitsKHR preTransform;
	VkCompositeAlphaFlagBitsKHR compositeAlpha;
	VkPresentModeKHR presentMode;
	VkBool32 clipped;
	VkSwapchainKHR oldSwapchain;
} VkSwapchainCreateInfoKHR;

/**
 * @brief Create a swapchain
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pSwapchain must be freed using PFN_vkDestroySwapchainKHR()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateSwapchainKHR.html
 */
typedef VkResult (*PFN_vkCreateSwapchainKHR)(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo,
					     const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain);

/**
 * @brief Destroy a swapchain object
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] swapchain created by PFN_vkCreateSwapchainKHR()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkDestroySwapchainKHR.html
 */
typedef void (*PFN_vkDestroySwapchainKHR)(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks *pAllocator);

/**
 * @brief Obtain the array of presentable images associated with a swapchain
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] swapchain created by PFN_vkCreateSwapchainKHR()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetSwapchainImagesKHR.html
 */
typedef VkResult (*PFN_vkGetSwapchainImagesKHR)(VkDevice device, VkSwapchainKHR swapchain, u32 *pSwapchainImageCount,
						VkImage *pSwapchainImages);

/**
 * @brief Retrieve the index of the next available presentable image
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[in] swapchain created by PFN_vkCreateSwapchainKHR()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkAcquireNextImageKHR.html
 */
typedef VkResult (*PFN_vkAcquireNextImageKHR)(VkDevice device, VkSwapchainKHR swapchain, u64 timeout, VkSemaphore semaphore, VkFence fence,
					      u32 *pImageIndex);

/**
 * @}
 * @defgroup vulkan_queue Queue
 * @ingroup vulkan
 * @{
 */

typedef u64 VkQueue;

/**
 * @brief Get a queue handle from a device
 * @param[in] device created by PFN_vkCreateDevice()
 * @param[out] pQueue
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkGetDeviceQueue.html
 */
typedef void (*PFN_vkGetDeviceQueue)(VkDevice device, u32 queueFamilyIndex, u32 queueIndex, VkQueue *pQueue);

typedef struct VkSubmitInfo_s {
	VkStructureType sType;
	const void *pNext;
	u32 waitSemaphoreCount;
	const VkSemaphore *pWaitSemaphores;
	const VkPipelineStageFlags *pWaitDstStageMask;
	u32 commandBufferCount;
	const VkCommandBuffer *pCommandBuffers;
	u32 signalSemaphoreCount;
	const VkSemaphore *pSignalSemaphores;
} VkSubmitInfo;

/**
 * @brief Submits a sequence of semaphores or command buffers to a queue
 * @param[in] queue created by PFN_vkGetDeviceQueue()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkQueueSubmit.html
 */
typedef VkResult (*PFN_vkQueueSubmit)(VkQueue queue, u32 submitCount, const VkSubmitInfo *pSubmits, VkFence fence);

typedef struct VkPresentInfoKHR_s {
	VkStructureType sType;
	const void *pNext;
	u32 waitSemaphoreCount;
	const VkSemaphore *pWaitSemaphores;
	u32 swapchainCount;
	const VkSwapchainKHR *pSwapchains;
	const u32 *pImageIndices;
	VkResult *pResults;
} VkPresentInfoKHR;

/**
 * @brief Queue an image for presentation
 * @param[in] queue created by PFN_vkGetDeviceQueue()
 * @see https://docs.vulkan.org/refpages/latest/refpages/source/vkQueuePresentKHR.html
 */
typedef VkResult (*PFN_vkQueuePresentKHR)(VkQueue queue, const VkPresentInfoKHR *pPresentInfo);

/**
 * @}
 */

#endif
