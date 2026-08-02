#include "gfx_buffer.h"
#include "gfx_driver.h"

#include "log.h"
#include "mem.h"

typedef u64 VkDeviceSize;
typedef u32 VkBool32;
typedef u32 VkFlags;
typedef u64 VkImage;
typedef u64 VkInstance;
typedef u64 VkPhysicalDevice;
typedef u64 VkDevice;
typedef u64 VkQueue;
typedef u64 VkDeviceMemory;
typedef u64 VkCommandPool;
typedef u64 VkCommandBuffer;
typedef u64 VkFence;
typedef u64 VkSurfaceKHR;
typedef u64 VkSwapchainKHR;
typedef u64 VkBuffer;
typedef u64 VkImageView;
typedef u64 VkRenderPass;
typedef u64 VkFramebuffer;
typedef u64 VkShaderModule;
typedef u64 VkPipelineLayout;
typedef u64 VkPipeline;
typedef u32 VkColorSpaceKHR;
typedef u32 VkPresentModeKHR;

typedef enum VkResult_e {
	VK_SUCCESS		 = 0,
	VK_SUBOPTIMAL_KHR	 = 1000001003,
	VK_ERROR_OUT_OF_DATE_KHR = -1000001004,
} VkResult;

enum {
	VK_QUEUE_GRAPHICS_BIT					    = 0x00000001,
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT			    = 0x00000002,
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT			    = 0x00000004,
	VK_BUFFER_USAGE_INDEX_BUFFER_BIT			    = 0x0040,
	VK_BUFFER_USAGE_VERTEX_BUFFER_BIT			    = 0x00000080,
	VK_IMAGE_USAGE_TRANSFER_DST_BIT				    = 0x00000002,
	VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT			    = 0x00000010,
	VK_FORMAT_FEATURE_TRANSFER_DST_BIT			    = 0x00004000,
	VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT			    = 0x00000080,
	VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT		    = 0x00000002,
	VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT		    = 0x00000001,
	VK_IMAGE_ASPECT_COLOR_BIT				    = 0x00000001,
	VK_ACCESS_TRANSFER_WRITE_BIT				    = 0x00001000,
	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT			    = 0x00000100,
	VK_ACCESS_HOST_READ_BIT					    = 0x00002000,
	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT			    = 0x00000001,
	VK_PIPELINE_STAGE_TRANSFER_BIT				    = 0x00001000,
	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT		    = 0x00000400,
	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT			    = 0x00002000,
	VK_PIPELINE_STAGE_HOST_BIT				    = 0x00004000,
	VK_FENCE_CREATE_SIGNALED_BIT				    = 0x00000001,
	VK_STRUCTURE_TYPE_APPLICATION_INFO			    = 0,
	VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO			    = 1,
	VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO		    = 2,
	VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO			    = 3,
	VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO			    = 14,
	VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO			    = 12,
	VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO			    = 5,
	VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO		    = 39,
	VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO		    = 40,
	VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO		    = 42,
	VK_STRUCTURE_TYPE_SUBMIT_INFO				    = 4,
	VK_STRUCTURE_TYPE_FENCE_CREATE_INFO			    = 8,
	VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE			    = 6,
	VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER			    = 44,
	VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO		    = 15,
	VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO		    = 16,
	VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO		    = 38,
	VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO		    = 37,
	VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO	    = 18,
	VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO   = 19,
	VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO = 20,
	VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO	    = 22,
	VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO  = 23,
	VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO    = 24,
	VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO    = 26,
	VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO	    = 27,
	VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO		    = 30,
	VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO		    = 28,
	VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO		    = 43,
	VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR		    = 1000001000,
	VK_STRUCTURE_TYPE_PRESENT_INFO_KHR			    = 1000001001,
	VK_IMAGE_TYPE_2D					    = 0,
	VK_FORMAT_R8G8B8A8_UNORM				    = 37,
	VK_FORMAT_R8G8B8A8_SRGB					    = 43,
	VK_FORMAT_B8G8R8A8_UNORM				    = 44,
	VK_FORMAT_B8G8R8A8_SRGB					    = 50,
	VK_IMAGE_TILING_LINEAR					    = 1,
	VK_IMAGE_LAYOUT_UNDEFINED				    = 0,
	VK_IMAGE_LAYOUT_GENERAL					    = 1,
	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL		    = 2,
	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL			    = 7,
	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR				    = 1000001002,
	VK_SHARING_MODE_EXCLUSIVE				    = 0,
	VK_SAMPLE_COUNT_1_BIT					    = 1,
	VK_COMMAND_BUFFER_LEVEL_PRIMARY				    = 0,
	VK_IMAGE_VIEW_TYPE_2D					    = 1,
	VK_ATTACHMENT_LOAD_OP_LOAD				    = 0,
	VK_ATTACHMENT_LOAD_OP_CLEAR				    = 1,
	VK_ATTACHMENT_STORE_OP_STORE				    = 0,
	VK_ATTACHMENT_LOAD_OP_DONT_CARE				    = 2,
	VK_ATTACHMENT_STORE_OP_DONT_CARE			    = 1,
	VK_PIPELINE_BIND_POINT_GRAPHICS				    = 0,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST			    = 3,
	VK_POLYGON_MODE_FILL					    = 0,
	VK_CULL_MODE_NONE					    = 0,
	VK_FRONT_FACE_COUNTER_CLOCKWISE				    = 1,
	VK_BLEND_FACTOR_ONE					    = 1,
	VK_BLEND_FACTOR_ZERO					    = 0,
	VK_BLEND_OP_ADD						    = 0,
	VK_COLOR_COMPONENT_R_BIT				    = 0x00000001,
	VK_COLOR_COMPONENT_G_BIT				    = 0x00000002,
	VK_COLOR_COMPONENT_B_BIT				    = 0x00000004,
	VK_COLOR_COMPONENT_A_BIT				    = 0x00000008,
	VK_DYNAMIC_STATE_VIEWPORT				    = 0,
	VK_DYNAMIC_STATE_SCISSOR				    = 1,
	VK_VERTEX_INPUT_RATE_VERTEX				    = 0,
	VK_FORMAT_R32G32_SFLOAT					    = 103,
	VK_FORMAT_R32G32B32A32_SFLOAT				    = 109,
	VK_SHADER_STAGE_VERTEX_BIT				    = 0x00000001,
	VK_SHADER_STAGE_FRAGMENT_BIT				    = 0x00000010,
	VK_SUBPASS_CONTENTS_INLINE				    = 0,
	VK_API_VERSION_1_0					    = 1u << 22,
	VK_COLOR_SPACE_SRGB_NONLINEAR_KHR			    = 0,
	VK_PRESENT_MODE_FIFO_KHR				    = 2,
	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR			    = 0x00000001,
};

#define VK_QUEUE_FAMILY_IGNORED ((u32)~0u)

typedef struct VkExtent3D_s {
	u32 width;
	u32 height;
	u32 depth;
} VkExtent3D;

typedef struct VkExtent2D_s {
	u32 width;
	u32 height;
} VkExtent2D;

typedef struct VkApplicationInfo_s {
	u32 sType;
	const void *pNext;
	const char *pApplicationName;
	u32 applicationVersion;
	const char *pEngineName;
	u32 engineVersion;
	u32 apiVersion;
} VkApplicationInfo;

typedef struct VkInstanceCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	const VkApplicationInfo *pApplicationInfo;
	u32 enabledLayerCount;
	const char *const *ppEnabledLayerNames;
	u32 enabledExtensionCount;
	const char *const *ppEnabledExtensionNames;
} VkInstanceCreateInfo;

typedef struct VkQueueFamilyProperties_s {
	VkFlags queueFlags;
	u32 queueCount;
	u32 timestampValidBits;
	struct {
		u32 width;
		u32 height;
		u32 depth;
	} minImageTransferGranularity;
} VkQueueFamilyProperties;

typedef struct VkPhysicalDeviceMemoryProperties_s {
	u32 memoryTypeCount;
	struct {
		VkFlags propertyFlags;
		u32 heapIndex;
	} memoryTypes[32];
	u32 memoryHeapCount;
	struct {
		VkDeviceSize size;
		VkFlags flags;
	} memoryHeaps[16];
} VkPhysicalDeviceMemoryProperties;

typedef struct VkFormatProperties_s {
	VkFlags linearTilingFeatures;
	VkFlags optimalTilingFeatures;
	VkFlags bufferFeatures;
} VkFormatProperties;

typedef struct VkSurfaceCapabilitiesKHR_s {
	u32 minImageCount;
	u32 maxImageCount;
	VkExtent2D currentExtent;
	VkExtent2D minImageExtent;
	VkExtent2D maxImageExtent;
	u32 maxImageArrayLayers;
	VkFlags supportedTransforms;
	VkFlags currentTransform;
	VkFlags supportedCompositeAlpha;
	VkFlags supportedUsageFlags;
} VkSurfaceCapabilitiesKHR;

typedef struct VkSurfaceFormatKHR_s {
	u32 format;
	VkColorSpaceKHR colorSpace;
} VkSurfaceFormatKHR;

typedef struct VkDeviceQueueCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 queueFamilyIndex;
	u32 queueCount;
	const float *pQueuePriorities;
} VkDeviceQueueCreateInfo;

typedef struct VkDeviceCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 queueCreateInfoCount;
	const VkDeviceQueueCreateInfo *pQueueCreateInfos;
	u32 enabledLayerCount;
	const char *const *ppEnabledLayerNames;
	u32 enabledExtensionCount;
	const char *const *ppEnabledExtensionNames;
	const void *pEnabledFeatures;
} VkDeviceCreateInfo;

typedef struct VkImageCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 imageType;
	u32 format;
	VkExtent3D extent;
	u32 mipLevels;
	u32 arrayLayers;
	u32 samples;
	u32 tiling;
	VkFlags usage;
	u32 sharingMode;
	u32 queueFamilyIndexCount;
	const u32 *pQueueFamilyIndices;
	u32 initialLayout;
} VkImageCreateInfo;

typedef struct VkBufferCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	VkDeviceSize size;
	VkFlags usage;
	u32 sharingMode;
	u32 queueFamilyIndexCount;
	const u32 *pQueueFamilyIndices;
} VkBufferCreateInfo;

typedef struct VkSwapchainCreateInfoKHR_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	VkSurfaceKHR surface;
	u32 minImageCount;
	u32 imageFormat;
	VkColorSpaceKHR imageColorSpace;
	VkExtent2D imageExtent;
	u32 imageArrayLayers;
	VkFlags imageUsage;
	u32 imageSharingMode;
	u32 queueFamilyIndexCount;
	const u32 *pQueueFamilyIndices;
	VkFlags preTransform;
	VkFlags compositeAlpha;
	VkPresentModeKHR presentMode;
	VkBool32 clipped;
	VkSwapchainKHR oldSwapchain;
} VkSwapchainCreateInfoKHR;

typedef struct VkMemoryRequirements_s {
	VkDeviceSize size;
	VkDeviceSize alignment;
	u32 memoryTypeBits;
} VkMemoryRequirements;

typedef struct VkMemoryAllocateInfo_s {
	u32 sType;
	const void *pNext;
	VkDeviceSize allocationSize;
	u32 memoryTypeIndex;
} VkMemoryAllocateInfo;

typedef struct VkCommandPoolCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 queueFamilyIndex;
} VkCommandPoolCreateInfo;

typedef struct VkCommandBufferAllocateInfo_s {
	u32 sType;
	const void *pNext;
	VkCommandPool commandPool;
	u32 level;
	u32 commandBufferCount;
} VkCommandBufferAllocateInfo;

typedef struct VkCommandBufferBeginInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	const void *pInheritanceInfo;
} VkCommandBufferBeginInfo;

typedef struct VkComponentMapping_s {
	u32 r;
	u32 g;
	u32 b;
	u32 a;
} VkComponentMapping;

typedef struct VkImageSubresourceRange_s {
	VkFlags aspectMask;
	u32 baseMipLevel;
	u32 levelCount;
	u32 baseArrayLayer;
	u32 layerCount;
} VkImageSubresourceRange;

typedef struct VkImageViewCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	VkImage image;
	u32 viewType;
	u32 format;
	VkComponentMapping components;
	VkImageSubresourceRange subresourceRange;
} VkImageViewCreateInfo;

typedef struct VkShaderModuleCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	size_t codeSize;
	const u32 *pCode;
} VkShaderModuleCreateInfo;

typedef struct VkAttachmentDescription_s {
	VkFlags flags;
	u32 format;
	u32 samples;
	u32 loadOp;
	u32 storeOp;
	u32 stencilLoadOp;
	u32 stencilStoreOp;
	u32 initialLayout;
	u32 finalLayout;
} VkAttachmentDescription;

typedef struct VkAttachmentReference_s {
	u32 attachment;
	u32 layout;
} VkAttachmentReference;

typedef struct VkSubpassDescription_s {
	VkFlags flags;
	u32 pipelineBindPoint;
	u32 inputAttachmentCount;
	const void *pInputAttachments;
	u32 colorAttachmentCount;
	const VkAttachmentReference *pColorAttachments;
	const void *pResolveAttachments;
	const void *pDepthStencilAttachment;
	u32 preserveAttachmentCount;
	const u32 *pPreserveAttachments;
} VkSubpassDescription;

typedef struct VkRenderPassCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 attachmentCount;
	const VkAttachmentDescription *pAttachments;
	u32 subpassCount;
	const VkSubpassDescription *pSubpasses;
	u32 dependencyCount;
	const void *pDependencies;
} VkRenderPassCreateInfo;

typedef struct VkFramebufferCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	VkRenderPass renderPass;
	u32 attachmentCount;
	const VkImageView *pAttachments;
	u32 width;
	u32 height;
	u32 layers;
} VkFramebufferCreateInfo;

typedef struct VkVertexInputBindingDescription_s {
	u32 binding;
	u32 stride;
	u32 inputRate;
} VkVertexInputBindingDescription;

typedef struct VkVertexInputAttributeDescription_s {
	u32 location;
	u32 binding;
	u32 format;
	u32 offset;
} VkVertexInputAttributeDescription;

typedef struct VkPipelineShaderStageCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 stage;
	VkShaderModule module;
	const char *pName;
	const void *pSpecializationInfo;
} VkPipelineShaderStageCreateInfo;

typedef struct VkPipelineVertexInputStateCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 vertexBindingDescriptionCount;
	const VkVertexInputBindingDescription *pVertexBindingDescriptions;
	u32 vertexAttributeDescriptionCount;
	const VkVertexInputAttributeDescription *pVertexAttributeDescriptions;
} VkPipelineVertexInputStateCreateInfo;

typedef struct VkPipelineInputAssemblyStateCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 topology;
	VkBool32 primitiveRestartEnable;
} VkPipelineInputAssemblyStateCreateInfo;

typedef struct VkViewport_s {
	float x;
	float y;
	float width;
	float height;
	float minDepth;
	float maxDepth;
} VkViewport;

typedef struct VkOffset2D_s {
	int x;
	int y;
} VkOffset2D;

typedef struct VkRect2D_s {
	VkOffset2D offset;
	VkExtent2D extent;
} VkRect2D;

typedef struct VkPipelineViewportStateCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 viewportCount;
	const VkViewport *pViewports;
	u32 scissorCount;
	const VkRect2D *pScissors;
} VkPipelineViewportStateCreateInfo;

typedef struct VkPipelineRasterizationStateCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	VkBool32 depthClampEnable;
	VkBool32 rasterizerDiscardEnable;
	u32 polygonMode;
	VkFlags cullMode;
	u32 frontFace;
	VkBool32 depthBiasEnable;
	float depthBiasConstantFactor;
	float depthBiasClamp;
	float depthBiasSlopeFactor;
	float lineWidth;
} VkPipelineRasterizationStateCreateInfo;

typedef struct VkPipelineMultisampleStateCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 rasterizationSamples;
	VkBool32 sampleShadingEnable;
	float minSampleShading;
	const void *pSampleMask;
	VkBool32 alphaToCoverageEnable;
	VkBool32 alphaToOneEnable;
} VkPipelineMultisampleStateCreateInfo;

typedef struct VkPipelineColorBlendAttachmentState_s {
	VkBool32 blendEnable;
	u32 srcColorBlendFactor;
	u32 dstColorBlendFactor;
	u32 colorBlendOp;
	u32 srcAlphaBlendFactor;
	u32 dstAlphaBlendFactor;
	u32 alphaBlendOp;
	VkFlags colorWriteMask;
} VkPipelineColorBlendAttachmentState;

typedef struct VkPipelineColorBlendStateCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	VkBool32 logicOpEnable;
	u32 logicOp;
	u32 attachmentCount;
	const VkPipelineColorBlendAttachmentState *pAttachments;
	float blendConstants[4];
} VkPipelineColorBlendStateCreateInfo;

typedef struct VkPipelineDynamicStateCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 dynamicStateCount;
	const u32 *pDynamicStates;
} VkPipelineDynamicStateCreateInfo;

typedef struct VkPipelineLayoutCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 setLayoutCount;
	const void *pSetLayouts;
	u32 pushConstantRangeCount;
	const void *pPushConstantRanges;
} VkPipelineLayoutCreateInfo;

typedef struct VkGraphicsPipelineCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	u32 stageCount;
	const VkPipelineShaderStageCreateInfo *pStages;
	const VkPipelineVertexInputStateCreateInfo *pVertexInputState;
	const VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState;
	const void *pTessellationState;
	const VkPipelineViewportStateCreateInfo *pViewportState;
	const VkPipelineRasterizationStateCreateInfo *pRasterizationState;
	const VkPipelineMultisampleStateCreateInfo *pMultisampleState;
	const void *pDepthStencilState;
	const VkPipelineColorBlendStateCreateInfo *pColorBlendState;
	const VkPipelineDynamicStateCreateInfo *pDynamicState;
	VkPipelineLayout layout;
	VkRenderPass renderPass;
	u32 subpass;
	VkPipeline basePipelineHandle;
	int basePipelineIndex;
} VkGraphicsPipelineCreateInfo;

typedef struct VkRenderPassBeginInfo_s {
	u32 sType;
	const void *pNext;
	VkRenderPass renderPass;
	VkFramebuffer framebuffer;
	VkRect2D renderArea;
	u32 clearValueCount;
	const void *pClearValues;
} VkRenderPassBeginInfo;

typedef union VkClearColorValue_u {
	float float32[4];
	int int32[4];
	u32 uint32[4];
} VkClearColorValue;

typedef struct VkImageMemoryBarrier_s {
	u32 sType;
	const void *pNext;
	VkFlags srcAccessMask;
	VkFlags dstAccessMask;
	u32 oldLayout;
	u32 newLayout;
	u32 srcQueueFamilyIndex;
	u32 dstQueueFamilyIndex;
	VkImage image;
	VkImageSubresourceRange subresourceRange;
} VkImageMemoryBarrier;

typedef struct VkSubmitInfo_s {
	u32 sType;
	const void *pNext;
	u32 waitSemaphoreCount;
	const void *pWaitSemaphores;
	const void *pWaitDstStageMask;
	u32 commandBufferCount;
	const VkCommandBuffer *pCommandBuffers;
	u32 signalSemaphoreCount;
	const void *pSignalSemaphores;
} VkSubmitInfo;

typedef struct VkPresentInfoKHR_s {
	u32 sType;
	const void *pNext;
	u32 waitSemaphoreCount;
	const void *pWaitSemaphores;
	u32 swapchainCount;
	const VkSwapchainKHR *pSwapchains;
	const u32 *pImageIndices;
	VkResult *pResults;
} VkPresentInfoKHR;

typedef struct VkFenceCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
} VkFenceCreateInfo;

typedef struct VkMappedMemoryRange_s {
	u32 sType;
	const void *pNext;
	VkDeviceMemory memory;
	VkDeviceSize offset;
	VkDeviceSize size;
} VkMappedMemoryRange;

typedef struct VkImageSubresource_s {
	VkFlags aspectMask;
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

typedef enum VkIndexType {
	VK_INDEX_TYPE_UINT16 = 0,
	VK_INDEX_TYPE_UINT32 = 1,
	VK_INDEX_TYPE_UINT8  = 1000265000,
} VkIndexType;

typedef void (*PFN_vkVoidFunction)(void);
typedef PFN_vkVoidFunction (*PFN_vkGetInstanceProcAddr)(VkInstance, const char *);
typedef PFN_vkVoidFunction (*PFN_vkGetDeviceProcAddr)(VkDevice, const char *);
typedef VkResult (*PFN_vkCreateInstance)(const VkInstanceCreateInfo *, const void *, VkInstance *);
typedef void (*PFN_vkDestroyInstance)(VkInstance, const void *);
typedef VkResult (*PFN_vkEnumeratePhysicalDevices)(VkInstance, u32 *, VkPhysicalDevice *);
typedef void (*PFN_vkGetPhysicalDeviceQueueFamilyProperties)(VkPhysicalDevice, u32 *, VkQueueFamilyProperties *);
typedef void (*PFN_vkGetPhysicalDeviceMemoryProperties)(VkPhysicalDevice, VkPhysicalDeviceMemoryProperties *);
typedef void (*PFN_vkGetPhysicalDeviceFormatProperties)(VkPhysicalDevice, u32, VkFormatProperties *);
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(VkPhysicalDevice, u32, VkSurfaceKHR, VkBool32 *);
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(VkPhysicalDevice, VkSurfaceKHR, VkSurfaceCapabilitiesKHR *);
typedef VkResult (*PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(VkPhysicalDevice, VkSurfaceKHR, u32 *, VkSurfaceFormatKHR *);
typedef VkResult (*PFN_vkCreateDevice)(VkPhysicalDevice, const VkDeviceCreateInfo *, const void *, VkDevice *);
typedef void (*PFN_vkDestroyDevice)(VkDevice, const void *);
typedef VkResult (*PFN_vkDeviceWaitIdle)(VkDevice);
typedef void (*PFN_vkGetDeviceQueue)(VkDevice, u32, u32, VkQueue *);
typedef VkResult (*PFN_vkCreateCommandPool)(VkDevice, const VkCommandPoolCreateInfo *, const void *, VkCommandPool *);
typedef void (*PFN_vkDestroyCommandPool)(VkDevice, VkCommandPool, const void *);
typedef VkResult (*PFN_vkAllocateCommandBuffers)(VkDevice, const VkCommandBufferAllocateInfo *, VkCommandBuffer *);
typedef void (*PFN_vkFreeCommandBuffers)(VkDevice, VkCommandPool, u32, const VkCommandBuffer *);
typedef VkResult (*PFN_vkCreateFence)(VkDevice, const VkFenceCreateInfo *, const void *, VkFence *);
typedef void (*PFN_vkDestroyFence)(VkDevice, VkFence, const void *);
typedef VkResult (*PFN_vkResetFences)(VkDevice, u32, const VkFence *);
typedef VkResult (*PFN_vkWaitForFences)(VkDevice, u32, const VkFence *, VkBool32, u64);
typedef VkResult (*PFN_vkCreateImage)(VkDevice, const VkImageCreateInfo *, const void *, VkImage *);
typedef void (*PFN_vkDestroyImage)(VkDevice, VkImage, const void *);
typedef void (*PFN_vkGetImageMemoryRequirements)(VkDevice, VkImage, VkMemoryRequirements *);
typedef VkResult (*PFN_vkCreateBuffer)(VkDevice, const VkBufferCreateInfo *, const void *, VkBuffer *);
typedef void (*PFN_vkDestroyBuffer)(VkDevice, VkBuffer, const void *);
typedef void (*PFN_vkGetBufferMemoryRequirements)(VkDevice, VkBuffer, VkMemoryRequirements *);
typedef VkResult (*PFN_vkAllocateMemory)(VkDevice, const VkMemoryAllocateInfo *, const void *, VkDeviceMemory *);
typedef void (*PFN_vkFreeMemory)(VkDevice, VkDeviceMemory, const void *);
typedef VkResult (*PFN_vkBindImageMemory)(VkDevice, VkImage, VkDeviceMemory, VkDeviceSize);
typedef VkResult (*PFN_vkBindBufferMemory)(VkDevice, VkBuffer, VkDeviceMemory, VkDeviceSize);
typedef void (*PFN_vkGetImageSubresourceLayout)(VkDevice, VkImage, const VkImageSubresource *, VkSubresourceLayout *);
typedef VkResult (*PFN_vkMapMemory)(VkDevice, VkDeviceMemory, VkDeviceSize, VkDeviceSize, VkFlags, void **);
typedef void (*PFN_vkUnmapMemory)(VkDevice, VkDeviceMemory);
typedef VkResult (*PFN_vkFlushMappedMemoryRanges)(VkDevice, u32, const VkMappedMemoryRange *);
typedef VkResult (*PFN_vkInvalidateMappedMemoryRanges)(VkDevice, u32, const VkMappedMemoryRange *);
typedef VkResult (*PFN_vkBeginCommandBuffer)(VkCommandBuffer, const VkCommandBufferBeginInfo *);
typedef VkResult (*PFN_vkEndCommandBuffer)(VkCommandBuffer);
typedef VkResult (*PFN_vkResetCommandBuffer)(VkCommandBuffer, VkFlags);
typedef void (*PFN_vkCmdPipelineBarrier)(VkCommandBuffer, VkFlags, VkFlags, VkFlags, u32, const void *, u32, const void *, u32,
					 const VkImageMemoryBarrier *);
typedef void (*PFN_vkCmdClearColorImage)(VkCommandBuffer, VkImage, u32, const VkClearColorValue *, u32, const VkImageSubresourceRange *);
typedef VkResult (*PFN_vkCreateImageView)(VkDevice, const VkImageViewCreateInfo *, const void *, VkImageView *);
typedef void (*PFN_vkDestroyImageView)(VkDevice, VkImageView, const void *);
typedef VkResult (*PFN_vkCreateShaderModule)(VkDevice, const VkShaderModuleCreateInfo *, const void *, VkShaderModule *);
typedef void (*PFN_vkDestroyShaderModule)(VkDevice, VkShaderModule, const void *);
typedef VkResult (*PFN_vkCreateRenderPass)(VkDevice, const VkRenderPassCreateInfo *, const void *, VkRenderPass *);
typedef void (*PFN_vkDestroyRenderPass)(VkDevice, VkRenderPass, const void *);
typedef VkResult (*PFN_vkCreateFramebuffer)(VkDevice, const VkFramebufferCreateInfo *, const void *, VkFramebuffer *);
typedef void (*PFN_vkDestroyFramebuffer)(VkDevice, VkFramebuffer, const void *);
typedef VkResult (*PFN_vkCreatePipelineLayout)(VkDevice, const VkPipelineLayoutCreateInfo *, const void *, VkPipelineLayout *);
typedef void (*PFN_vkDestroyPipelineLayout)(VkDevice, VkPipelineLayout, const void *);
typedef VkResult (*PFN_vkCreateGraphicsPipelines)(VkDevice, u64, u32, const VkGraphicsPipelineCreateInfo *, const void *, VkPipeline *);
typedef void (*PFN_vkDestroyPipeline)(VkDevice, VkPipeline, const void *);
typedef void (*PFN_vkCmdBeginRenderPass)(VkCommandBuffer, const VkRenderPassBeginInfo *, u32);
typedef void (*PFN_vkCmdEndRenderPass)(VkCommandBuffer);
typedef void (*PFN_vkCmdBindPipeline)(VkCommandBuffer, u32, VkPipeline);
typedef void (*PFN_vkCmdBindVertexBuffers)(VkCommandBuffer, u32, u32, const VkBuffer *, const VkDeviceSize *);
typedef void (*PFN_vkCmdBindIndexBuffer)(VkCommandBuffer, VkBuffer, VkDeviceSize, VkIndexType);
typedef void (*PFN_vkCmdSetViewport)(VkCommandBuffer, u32, u32, const VkViewport *);
typedef void (*PFN_vkCmdSetScissor)(VkCommandBuffer, u32, u32, const VkRect2D *);
typedef void (*PFN_vkCmdDraw)(VkCommandBuffer, u32, u32, u32, u32);
typedef void (*PFN_vkCmdDrawIndexed)(VkCommandBuffer, u32, u32, u32, int32_t, u32);
typedef VkResult (*PFN_vkQueueSubmit)(VkQueue, u32, const VkSubmitInfo *, VkFence);
typedef VkResult (*PFN_vkCreateSwapchainKHR)(VkDevice, const VkSwapchainCreateInfoKHR *, const void *, VkSwapchainKHR *);
typedef void (*PFN_vkDestroySwapchainKHR)(VkDevice, VkSwapchainKHR, const void *);
typedef VkResult (*PFN_vkGetSwapchainImagesKHR)(VkDevice, VkSwapchainKHR, u32 *, VkImage *);
typedef VkResult (*PFN_vkAcquireNextImageKHR)(VkDevice, VkSwapchainKHR, u64, u64, VkFence, u32 *);
typedef VkResult (*PFN_vkQueuePresentKHR)(VkQueue, const VkPresentInfoKHR *);

enum {
	GFX_VULKAN_MAX_SWAPCHAIN_IMAGES = 8,
};

typedef struct gfx_vulkan_frame_s {
	VkImage image;
	VkImageView *view;
	VkFramebuffer *framebuffer;
	u32 image_index;
	u32 old_layout;
	u32 final_layout;
	int active;
	int surface;
} gfx_vulkan_frame_t;

typedef struct gfx_vulkan_s {
	void *lib;
	gfx_target_t *target;
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice device;
	VkQueue queue;
	u32 queue_family;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	VkFence fence;
	VkClearColorValue clear_color;
	gfx_vulkan_frame_t frame;
	int surface_enabled;
	int swapchain_enabled;
	PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
	PFN_vkGetDeviceProcAddr GetDeviceProcAddr;
	PFN_vkDestroyInstance DestroyInstance;
	PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties GetPhysicalDeviceQueueFamilyProperties;
	PFN_vkGetPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties;
	PFN_vkGetPhysicalDeviceFormatProperties GetPhysicalDeviceFormatProperties;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR GetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR GetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkCreateDevice CreateDevice;
	PFN_vkDestroyDevice DestroyDevice;
	PFN_vkDeviceWaitIdle DeviceWaitIdle;
	PFN_vkGetDeviceQueue GetDeviceQueue;
	PFN_vkCreateCommandPool CreateCommandPool;
	PFN_vkDestroyCommandPool DestroyCommandPool;
	PFN_vkAllocateCommandBuffers AllocateCommandBuffers;
	PFN_vkFreeCommandBuffers FreeCommandBuffers;
	PFN_vkCreateFence CreateFence;
	PFN_vkDestroyFence DestroyFence;
	PFN_vkResetFences ResetFences;
	PFN_vkWaitForFences WaitForFences;
	PFN_vkCreateImage CreateImage;
	PFN_vkDestroyImage DestroyImage;
	PFN_vkGetImageMemoryRequirements GetImageMemoryRequirements;
	PFN_vkCreateBuffer CreateBuffer;
	PFN_vkDestroyBuffer DestroyBuffer;
	PFN_vkGetBufferMemoryRequirements GetBufferMemoryRequirements;
	PFN_vkAllocateMemory AllocateMemory;
	PFN_vkFreeMemory FreeMemory;
	PFN_vkBindImageMemory BindImageMemory;
	PFN_vkBindBufferMemory BindBufferMemory;
	PFN_vkGetImageSubresourceLayout GetImageSubresourceLayout;
	PFN_vkMapMemory MapMemory;
	PFN_vkUnmapMemory UnmapMemory;
	PFN_vkFlushMappedMemoryRanges FlushMappedMemoryRanges;
	PFN_vkInvalidateMappedMemoryRanges InvalidateMappedMemoryRanges;
	PFN_vkBeginCommandBuffer BeginCommandBuffer;
	PFN_vkEndCommandBuffer EndCommandBuffer;
	PFN_vkResetCommandBuffer ResetCommandBuffer;
	PFN_vkCmdPipelineBarrier CmdPipelineBarrier;
	PFN_vkCmdClearColorImage CmdClearColorImage;
	PFN_vkCreateImageView CreateImageView;
	PFN_vkDestroyImageView DestroyImageView;
	PFN_vkCreateShaderModule CreateShaderModule;
	PFN_vkDestroyShaderModule DestroyShaderModule;
	PFN_vkCreateRenderPass CreateRenderPass;
	PFN_vkDestroyRenderPass DestroyRenderPass;
	PFN_vkCreateFramebuffer CreateFramebuffer;
	PFN_vkDestroyFramebuffer DestroyFramebuffer;
	PFN_vkCreatePipelineLayout CreatePipelineLayout;
	PFN_vkDestroyPipelineLayout DestroyPipelineLayout;
	PFN_vkCreateGraphicsPipelines CreateGraphicsPipelines;
	PFN_vkDestroyPipeline DestroyPipeline;
	PFN_vkCmdBeginRenderPass CmdBeginRenderPass;
	PFN_vkCmdEndRenderPass CmdEndRenderPass;
	PFN_vkCmdBindPipeline CmdBindPipeline;
	PFN_vkCmdBindVertexBuffers CmdBindVertexBuffers;
	PFN_vkCmdBindIndexBuffer CmdBindIndexBuffer;
	PFN_vkCmdSetViewport CmdSetViewport;
	PFN_vkCmdSetScissor CmdSetScissor;
	PFN_vkCmdDraw CmdDraw;
	PFN_vkCmdDrawIndexed CmdDrawIndexed;
	PFN_vkQueueSubmit QueueSubmit;
	PFN_vkCreateSwapchainKHR CreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR DestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR GetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR AcquireNextImageKHR;
	PFN_vkQueuePresentKHR QueuePresentKHR;
} gfx_vulkan_t;

typedef struct gfx_vulkan_render_pass_s {
	VkRenderPass render_pass;
} gfx_vulkan_render_pass_t;

typedef struct gfx_vulkan_memory_target_s {
	VkImage image;
	VkDeviceMemory memory;
	VkDeviceSize memory_size;
	int memory_coherent;
	VkSubresourceLayout layout;
	VkImageView image_view;
} gfx_vulkan_memory_target_t;

typedef struct gfx_vulkan_swapchain_s {
	VkSwapchainKHR swapchain;
	VkImage swapchain_images[GFX_VULKAN_MAX_SWAPCHAIN_IMAGES];
	VkImageView swapchain_image_views[GFX_VULKAN_MAX_SWAPCHAIN_IMAGES];
	u32 swapchain_image_layouts[GFX_VULKAN_MAX_SWAPCHAIN_IMAGES];
	u32 swapchain_image_count;
	u32 swapchain_image_index;
	int swapchain_acquired;
} gfx_vulkan_swapchain_t;

typedef struct gfx_vulkan_framebuffer_s {
	VkFramebuffer framebuffer;
	VkFramebuffer swapchain_framebuffers[GFX_VULKAN_MAX_SWAPCHAIN_IMAGES];
} gfx_vulkan_framebuffer_t;

typedef struct gfx_vulkan_buffer_s {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize memory_size;
	int memory_coherent;
	gfx_buffer_type_t type;
} gfx_vulkan_buffer_t;

typedef struct gfx_vulkan_shader_s {
	VkShaderModule module;
} gfx_vulkan_shader_t;

typedef struct gfx_vulkan_pipeline_s {
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;
} gfx_vulkan_pipeline_t;

static int load_lib_symbol(gfx_t *gfx, void **sym, strv_t name)
{
	gfx_vulkan_t *vulkan = gfx->data;
	if (proc_dlsym(gfx->proc, vulkan->lib, name, sym)) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to load Vulkan symbol: %.*s", name.len, name.data);
		return 1;
	}

	return 0;
}

static int load_instance_symbol(gfx_vulkan_t *vulkan, void **sym, const char *name)
{
	union {
		PFN_vkVoidFunction fn;
		void *ptr;
	} symbol = {.fn = vulkan->GetInstanceProcAddr(vulkan->instance, name)};

	*sym = symbol.ptr;
	if (*sym == NULL) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to load Vulkan symbol: %s", name);
		return 1;
	}

	return 0;
}

static int load_device_symbol(gfx_vulkan_t *vulkan, void **sym, const char *name)
{
	union {
		PFN_vkVoidFunction fn;
		void *ptr;
	} symbol = {.fn = vulkan->GetDeviceProcAddr(vulkan->device, name)};

	*sym = symbol.ptr;
	if (*sym == NULL) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to load Vulkan symbol: %s", name);
		return 1;
	}

	return 0;
}

static void *symbol_ptr(PFN_vkVoidFunction fn)
{
	union {
		PFN_vkVoidFunction fn;
		void *ptr;
	} symbol = {.fn = fn};

	return symbol.ptr;
}

#define LOAD_VK_LIB(_gfx, _vulkan, _name) load_lib_symbol((_gfx), (void **)&(_vulkan)->_name, STRV("vk" #_name))
#define LOAD_VK_INST(_vulkan, _name)	  load_instance_symbol((_vulkan), (void **)&(_vulkan)->_name, "vk" #_name)
#define LOAD_VK_DEV(_vulkan, _name)	  load_device_symbol((_vulkan), (void **)&(_vulkan)->_name, "vk" #_name)

static int vk_ok(VkResult result)
{
	return result == VK_SUCCESS;
}

static int vk_swapchain_ok(VkResult result)
{
	return result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR;
}

static int vk_swapchain_needs_recreate(VkResult result)
{
	return result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR;
}

static int extension_enabled(const char *const *extensions, u32 count, const char *name)
{
	for (u32 i = 0; i < count; i++) {
		if (strv_eq(strv_cstr(extensions[i]), strv_cstr(name))) {
			return 1;
		}
	}

	return 0;
}

static void gfx_vulkan_swapchain_data_free(gfx_vulkan_t *vulkan, gfx_vulkan_swapchain_t *swapchain)
{
	for (u32 i = 0; i < swapchain->swapchain_image_count; i++) {
		if (swapchain->swapchain_image_views[i] != 0) {
			vulkan->DestroyImageView(vulkan->device, swapchain->swapchain_image_views[i], NULL);
		}
	}
	if (swapchain->swapchain != 0) {
		if (vulkan->DeviceWaitIdle != NULL) {
			vulkan->DeviceWaitIdle(vulkan->device);
		}
		vulkan->DestroySwapchainKHR(vulkan->device, swapchain->swapchain, NULL);
	}
	swapchain->swapchain		 = 0;
	swapchain->swapchain_image_count = 0;
	swapchain->swapchain_image_index = 0;
	swapchain->swapchain_acquired	 = 0;
	mem_set(swapchain->swapchain_images, 0, sizeof(swapchain->swapchain_images));
	mem_set(swapchain->swapchain_image_views, 0, sizeof(swapchain->swapchain_image_views));
	mem_set(swapchain->swapchain_image_layouts, 0, sizeof(swapchain->swapchain_image_layouts));
}

static void gfx_vulkan_draw_target_free(gfx_vulkan_t *vulkan, gfx_vulkan_memory_target_t *target)
{
	if (target->image_view != 0) {
		vulkan->DestroyImageView(vulkan->device, target->image_view, NULL);
		target->image_view = 0;
	}
}

static void gfx_vulkan_swapchain_draw_targets_free(gfx_vulkan_t *vulkan, gfx_vulkan_swapchain_t *swapchain)
{
	for (u32 i = 0; i < swapchain->swapchain_image_count; i++) {
		if (swapchain->swapchain_image_views[i] != 0) {
			vulkan->DestroyImageView(vulkan->device, swapchain->swapchain_image_views[i], NULL);
			swapchain->swapchain_image_views[i] = 0;
		}
	}
}

static void gfx_vulkan_draw_resources_free(gfx_vulkan_t *vulkan)
{
	vulkan->frame = (gfx_vulkan_frame_t){0};
	if (vulkan->target == NULL || vulkan->target->driver_data == NULL) {
		return;
	}
	if (vulkan->target->type == GFX_TARGET_MEMORY) {
		gfx_vulkan_draw_target_free(vulkan, vulkan->target->driver_data);
	} else if (vulkan->target->type == GFX_TARGET_SWAPCHAIN) {
		gfx_vulkan_swapchain_draw_targets_free(vulkan, vulkan->target->driver_data);
	}
}

static void gfx_vulkan_draw_free(gfx_vulkan_t *vulkan)
{
	gfx_vulkan_draw_resources_free(vulkan);
}

static void gfx_vulkan_memory_target_free(gfx_vulkan_t *vulkan, gfx_vulkan_memory_target_t *target)
{
	if (target == NULL) {
		return; // LCOV_EXCL_LINE
	}

	gfx_vulkan_draw_target_free(vulkan, target);
	if (target->image != 0) {
		vulkan->DestroyImage(vulkan->device, target->image, NULL);
		target->image = 0;
	}
	if (target->memory != 0) {
		vulkan->FreeMemory(vulkan->device, target->memory, NULL);
		target->memory = 0;
	}
	target->memory_size	= 0;
	target->memory_coherent = 0;
	target->layout		= (VkSubresourceLayout){0};
}

static void gfx_vulkan_device_free(gfx_vulkan_t *vulkan)
{
	gfx_vulkan_draw_free(vulkan);
	if (vulkan->device != 0) {
		if (vulkan->command_buffer != 0) {
			vulkan->FreeCommandBuffers(vulkan->device, vulkan->command_pool, 1, &vulkan->command_buffer);
			vulkan->command_buffer = 0;
		}
		if (vulkan->fence != 0) {
			vulkan->DestroyFence(vulkan->device, vulkan->fence, NULL);
			vulkan->fence = 0;
		}
		if (vulkan->command_pool != 0) {
			vulkan->DestroyCommandPool(vulkan->device, vulkan->command_pool, NULL);
			vulkan->command_pool = 0;
		}
		vulkan->DestroyDevice(vulkan->device, NULL);
		vulkan->device = 0;
	}
}

static int gfx_vulkan_init_free(gfx_t *gfx, gfx_vulkan_t *vulkan)
{
	gfx_vulkan_device_free(vulkan);
	if (vulkan->instance != 0 && vulkan->DestroyInstance != NULL) {
		vulkan->DestroyInstance(vulkan->instance, NULL);
	}
	if (vulkan->lib != NULL) {
		proc_dlclose(gfx->proc, vulkan->lib);
	}
	alloc_free(&gfx->alloc, vulkan, sizeof(gfx_vulkan_t));
	gfx->data = NULL;
	return 1;
}

static int gfx_vulkan_pick_device(gfx_vulkan_t *vulkan)
{
	u32 device_count = 0;
	if (!vk_ok(vulkan->EnumeratePhysicalDevices(vulkan->instance, &device_count, NULL)) || device_count == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan physical devices");
		return 1;
	}

	VkPhysicalDevice devices[16] = {0};
	if (device_count > sizeof(devices) / sizeof(devices[0])) {
		device_count = sizeof(devices) / sizeof(devices[0]);
	}
	if (!vk_ok(vulkan->EnumeratePhysicalDevices(vulkan->instance, &device_count, devices))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan physical devices");
		return 1;
	}

	for (u32 i = 0; i < device_count; i++) {
		u32 queue_count = 0;
		vulkan->GetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_count, NULL);
		if (queue_count == 0) {
			continue;
		}

		VkQueueFamilyProperties queues[32] = {0};
		if (queue_count > sizeof(queues) / sizeof(queues[0])) {
			queue_count = sizeof(queues) / sizeof(queues[0]);
		}
		vulkan->GetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_count, queues);
		for (u32 q = 0; q < queue_count; q++) {
			if (queues[q].queueCount != 0 && (queues[q].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				VkFormatProperties props = {0};
				vulkan->GetPhysicalDeviceFormatProperties(devices[i], VK_FORMAT_R8G8B8A8_UNORM, &props);
				if ((props.linearTilingFeatures &
				     (VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)) !=
				    (VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)) {
					continue;
				}
				vulkan->physical_device = devices[i];
				vulkan->queue_family	= q;
				return 0;
			}
		}
	}

	log_error("cgfx", "gfx_vulkan", NULL, "failed to find a Vulkan graphics queue");
	return 1;
}

static int gfx_vulkan_create_device(gfx_t *gfx, const gfx_plan_t *plan)
{
	gfx_vulkan_t *vulkan = gfx->data;

	float priority		      = 1.0f;
	VkDeviceQueueCreateInfo queue = {
		.sType		  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = vulkan->queue_family,
		.queueCount	  = 1,
		.pQueuePriorities = &priority,
	};
	static const char *const swapchain_extensions[] = {
		"VK_KHR_swapchain",
	};
	const char *swapchain_extension	   = swapchain_extensions[0];
	u32 plan_extension_count	   = plan != NULL ? plan->device_extension_count : 0;
	const char *const *plan_extensions = plan != NULL ? plan->device_extensions : NULL;
	int add_swapchain = vulkan->surface_enabled && !extension_enabled(plan_extensions, plan_extension_count, swapchain_extension);
	u32 device_extension_count	     = plan_extension_count + (add_swapchain ? 1u : 0u);
	const char *const *device_extensions = plan_extensions;
	const char **owned_extensions	     = NULL;
	if (add_swapchain && plan_extension_count == 0) {
		device_extensions = swapchain_extensions;
	} else if (add_swapchain) {
		owned_extensions = alloc_alloc(&gfx->alloc, sizeof(char *) * device_extension_count);
		if (owned_extensions == NULL) {
			return 1;
		}
		for (u32 i = 0; i < plan_extension_count; i++) {
			owned_extensions[i] = plan_extensions[i];
		}
		owned_extensions[plan_extension_count] = swapchain_extension;
		device_extensions		       = owned_extensions;
	}
	VkDeviceCreateInfo create = {
		.sType			 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount	 = 1,
		.pQueueCreateInfos	 = &queue,
		.enabledExtensionCount	 = device_extension_count,
		.ppEnabledExtensionNames = device_extensions,
	};
	int create_failed = !vk_ok(vulkan->CreateDevice(vulkan->physical_device, &create, NULL, &vulkan->device));
	if (owned_extensions != NULL) {
		alloc_free(&gfx->alloc, owned_extensions, sizeof(char *) * device_extension_count);
	}
	if (create_failed) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan device");
		return 1;
	}

	if (LOAD_VK_DEV(vulkan, DeviceWaitIdle) || LOAD_VK_DEV(vulkan, GetDeviceQueue) || LOAD_VK_DEV(vulkan, CreateCommandPool) ||
	    LOAD_VK_DEV(vulkan, DestroyCommandPool) || LOAD_VK_DEV(vulkan, AllocateCommandBuffers) ||
	    LOAD_VK_DEV(vulkan, FreeCommandBuffers) || LOAD_VK_DEV(vulkan, CreateFence) || LOAD_VK_DEV(vulkan, DestroyFence) ||
	    LOAD_VK_DEV(vulkan, ResetFences) || LOAD_VK_DEV(vulkan, WaitForFences) || LOAD_VK_DEV(vulkan, CreateImage) ||
	    LOAD_VK_DEV(vulkan, DestroyImage) || LOAD_VK_DEV(vulkan, GetImageMemoryRequirements) || LOAD_VK_DEV(vulkan, CreateBuffer) ||
	    LOAD_VK_DEV(vulkan, DestroyBuffer) || LOAD_VK_DEV(vulkan, GetBufferMemoryRequirements) || LOAD_VK_DEV(vulkan, AllocateMemory) ||
	    LOAD_VK_DEV(vulkan, FreeMemory) || LOAD_VK_DEV(vulkan, BindImageMemory) || LOAD_VK_DEV(vulkan, BindBufferMemory) ||
	    LOAD_VK_DEV(vulkan, GetImageSubresourceLayout) || LOAD_VK_DEV(vulkan, MapMemory) || LOAD_VK_DEV(vulkan, UnmapMemory) ||
	    LOAD_VK_DEV(vulkan, FlushMappedMemoryRanges) || LOAD_VK_DEV(vulkan, InvalidateMappedMemoryRanges) ||
	    LOAD_VK_DEV(vulkan, BeginCommandBuffer) || LOAD_VK_DEV(vulkan, EndCommandBuffer) || LOAD_VK_DEV(vulkan, ResetCommandBuffer) ||
	    LOAD_VK_DEV(vulkan, CmdPipelineBarrier) || LOAD_VK_DEV(vulkan, CmdClearColorImage) || LOAD_VK_DEV(vulkan, CreateImageView) ||
	    LOAD_VK_DEV(vulkan, DestroyImageView) || LOAD_VK_DEV(vulkan, CreateShaderModule) || LOAD_VK_DEV(vulkan, DestroyShaderModule) ||
	    LOAD_VK_DEV(vulkan, CreateRenderPass) || LOAD_VK_DEV(vulkan, DestroyRenderPass) || LOAD_VK_DEV(vulkan, CreateFramebuffer) ||
	    LOAD_VK_DEV(vulkan, DestroyFramebuffer) || LOAD_VK_DEV(vulkan, CreatePipelineLayout) ||
	    LOAD_VK_DEV(vulkan, DestroyPipelineLayout) || LOAD_VK_DEV(vulkan, CreateGraphicsPipelines) ||
	    LOAD_VK_DEV(vulkan, DestroyPipeline) || LOAD_VK_DEV(vulkan, CmdBeginRenderPass) || LOAD_VK_DEV(vulkan, CmdEndRenderPass) ||
	    LOAD_VK_DEV(vulkan, CmdBindPipeline) || LOAD_VK_DEV(vulkan, CmdBindVertexBuffers) || LOAD_VK_DEV(vulkan, CmdBindIndexBuffer) ||
	    LOAD_VK_DEV(vulkan, CmdSetViewport) || LOAD_VK_DEV(vulkan, CmdSetScissor) || LOAD_VK_DEV(vulkan, CmdDraw) ||
	    LOAD_VK_DEV(vulkan, CmdDrawIndexed) || LOAD_VK_DEV(vulkan, QueueSubmit)) {
		return 1;
	}
	if (vulkan->swapchain_enabled && (LOAD_VK_DEV(vulkan, CreateSwapchainKHR) || LOAD_VK_DEV(vulkan, DestroySwapchainKHR) ||
					  LOAD_VK_DEV(vulkan, GetSwapchainImagesKHR) || LOAD_VK_DEV(vulkan, AcquireNextImageKHR) ||
					  LOAD_VK_DEV(vulkan, QueuePresentKHR))) {
		return 1;
	}

	vulkan->GetDeviceQueue(vulkan->device, vulkan->queue_family, 0, &vulkan->queue);
	if (vulkan->queue == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to get Vulkan queue");
		return 1;
	}

	VkCommandPoolCreateInfo pool = {
		.sType		  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags		  = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = vulkan->queue_family,
	};
	if (!vk_ok(vulkan->CreateCommandPool(vulkan->device, &pool, NULL, &vulkan->command_pool))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan command pool");
		return 1;
	}

	VkCommandBufferAllocateInfo buffer = {
		.sType		    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool	    = vulkan->command_pool,
		.level		    = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};
	if (!vk_ok(vulkan->AllocateCommandBuffers(vulkan->device, &buffer, &vulkan->command_buffer))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to allocate Vulkan command buffer");
		return 1;
	}

	VkFenceCreateInfo fence = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};
	if (!vk_ok(vulkan->CreateFence(vulkan->device, &fence, NULL, &vulkan->fence))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan fence");
		return 1;
	}

	return 0;
}

static int gfx_vulkan_init(gfx_t *gfx, const gfx_config_t *config)
{
	if (gfx == NULL || config == NULL || gfx->proc == NULL || gfx->alloc.alloc == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = alloc_alloc(&gfx->alloc, sizeof(gfx_vulkan_t));
	if (vulkan == NULL) {
		return 1;
	}
	*vulkan = (gfx_vulkan_t){
		.clear_color = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}},
	};
	if (config->plan != NULL) {
		vulkan->surface_enabled =
			extension_enabled(config->plan->instance_extensions, config->plan->instance_extension_count, "VK_KHR_surface");
		vulkan->swapchain_enabled =
			vulkan->surface_enabled ||
			extension_enabled(config->plan->device_extensions, config->plan->device_extension_count, "VK_KHR_swapchain");
	}
	gfx->data = vulkan;

	if (proc_dlopen(gfx->proc, STRV("vulkan-1.dll"), &vulkan->lib) && proc_dlopen(gfx->proc, STRV("libvulkan.so.1"), &vulkan->lib) &&
	    proc_dlopen(gfx->proc, STRV("libvulkan.so"), &vulkan->lib)) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to load Vulkan library");
		return gfx_vulkan_init_free(gfx, vulkan);
	}

	if (LOAD_VK_LIB(gfx, vulkan, GetInstanceProcAddr)) {
		return gfx_vulkan_init_free(gfx, vulkan);
	}

	PFN_vkCreateInstance create_instance = (PFN_vkCreateInstance)vulkan->GetInstanceProcAddr(0, "vkCreateInstance");
	if (create_instance == NULL) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to load Vulkan symbol: vkCreateInstance");
		return gfx_vulkan_init_free(gfx, vulkan);
	}

	VkApplicationInfo app = {
		.sType		    = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName   = "cgfx",
		.applicationVersion = 1,
		.pEngineName	    = "cgfx",
		.engineVersion	    = 1,
		.apiVersion	    = VK_API_VERSION_1_0,
	};
	VkInstanceCreateInfo create = {
		.sType			 = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo	 = &app,
		.enabledExtensionCount	 = config->plan != NULL ? config->plan->instance_extension_count : 0,
		.ppEnabledExtensionNames = config->plan != NULL ? config->plan->instance_extensions : NULL,
	};
	if (!vk_ok(create_instance(&create, NULL, &vulkan->instance))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan instance");
		return gfx_vulkan_init_free(gfx, vulkan);
	}

	if (LOAD_VK_INST(vulkan, DestroyInstance) || LOAD_VK_INST(vulkan, EnumeratePhysicalDevices) ||
	    LOAD_VK_INST(vulkan, GetPhysicalDeviceQueueFamilyProperties) || LOAD_VK_INST(vulkan, GetPhysicalDeviceMemoryProperties) ||
	    LOAD_VK_INST(vulkan, GetPhysicalDeviceFormatProperties) || LOAD_VK_INST(vulkan, CreateDevice) ||
	    LOAD_VK_INST(vulkan, DestroyDevice) || LOAD_VK_INST(vulkan, GetDeviceProcAddr)) {
		return gfx_vulkan_init_free(gfx, vulkan);
	}
	if (vulkan->surface_enabled &&
	    (LOAD_VK_INST(vulkan, GetPhysicalDeviceSurfaceSupportKHR) || LOAD_VK_INST(vulkan, GetPhysicalDeviceSurfaceCapabilitiesKHR) ||
	     LOAD_VK_INST(vulkan, GetPhysicalDeviceSurfaceFormatsKHR))) {
		return gfx_vulkan_init_free(gfx, vulkan);
	}

	if (gfx_vulkan_pick_device(vulkan) || gfx_vulkan_create_device(gfx, config->plan)) {
		return gfx_vulkan_init_free(gfx, vulkan);
	}

	return 0;
}

static int gfx_vulkan_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = gfx->data;
	gfx_vulkan_device_free(vulkan);
	if (vulkan->instance != 0 && vulkan->DestroyInstance != NULL) {
		vulkan->DestroyInstance(vulkan->instance, NULL);
	}
	if (vulkan->lib != NULL) {
		proc_dlclose(gfx->proc, vulkan->lib);
	}
	alloc_free(&gfx->alloc, vulkan, sizeof(gfx_vulkan_t));
	gfx->data = NULL;
	return 0;
}

static int gfx_vulkan_native(gfx_t *gfx, gfx_native_t *native)
{
	if (gfx == NULL || gfx->data == NULL || native == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = gfx->data;

	*native = (gfx_native_t){
		.api		 = GFX_API_VULKAN,
		.instance	 = vulkan->instance,
		.physical_device = vulkan->physical_device,
		.device		 = vulkan->device,
	};
	return 0;
}

static int gfx_vulkan_proc(gfx_t *gfx, strv_t name, void **proc)
{
	if (gfx == NULL || gfx->data == NULL || proc == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = gfx->data;
	if (proc_dlsym(gfx->proc, vulkan->lib, name, proc) == 0) {
		return 0;
	}

	str_t proc_name = strn(name.data, name.len, name.len + 1);
	if (proc_name.data == NULL) {
		*proc = NULL;
		return 1;
	}

	*proc = NULL;
	if (vulkan->GetInstanceProcAddr != NULL && vulkan->instance != 0) {
		*proc = symbol_ptr(vulkan->GetInstanceProcAddr(vulkan->instance, proc_name.data));
	}
	if (*proc == NULL && vulkan->GetDeviceProcAddr != NULL && vulkan->device != 0) {
		*proc = symbol_ptr(vulkan->GetDeviceProcAddr(vulkan->device, proc_name.data));
	}
	str_free(&proc_name);
	return *proc == NULL;
}

static void gfx_vulkan_render_pass_free(gfx_render_pass_t *render_pass)
{
	if (render_pass == NULL || render_pass->gfx == NULL || render_pass->gfx->data == NULL || render_pass->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan			 = render_pass->gfx->data;
	gfx_vulkan_render_pass_t *vk_render_pass = render_pass->data;
	if (vk_render_pass->render_pass != 0) {
		vulkan->DestroyRenderPass(vulkan->device, vk_render_pass->render_pass, NULL);
		vk_render_pass->render_pass = 0;
	}
	alloc_free(&render_pass->gfx->alloc, vk_render_pass, sizeof(gfx_vulkan_render_pass_t));
	render_pass->data = NULL;
}

static u32 gfx_vulkan_format(gfx_format_t format)
{
	switch (format) {
	case GFX_FORMAT_RGBA8_UNORM:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case GFX_FORMAT_BGRA8_UNORM:
		return VK_FORMAT_B8G8R8A8_UNORM;
	case GFX_FORMAT_RGBA8_SRGB:
		return VK_FORMAT_R8G8B8A8_SRGB;
	case GFX_FORMAT_BGRA8_SRGB:
		return VK_FORMAT_B8G8R8A8_SRGB;
	default:
		return 0;
	}
}

static u32 gfx_vulkan_load_op(gfx_load_op_t load)
{
	switch (load) {
	case GFX_LOAD_CLEAR:
		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	case GFX_LOAD_LOAD:
		return VK_ATTACHMENT_LOAD_OP_LOAD;
	default:
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}
}

static u32 gfx_vulkan_store_op(gfx_store_op_t store)
{
	return store == GFX_STORE_STORE ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

static int gfx_vulkan_render_pass_init(gfx_render_pass_t *render_pass, const gfx_render_pass_config_t *config)
{
	if (render_pass == NULL || render_pass->gfx == NULL || render_pass->gfx->data == NULL || config == NULL ||
	    gfx_vulkan_format(config->color_format) == 0) {
		return 1;
	}

	gfx_vulkan_render_pass_t *vk_render_pass = alloc_alloc(&render_pass->gfx->alloc, sizeof(gfx_vulkan_render_pass_t));
	if (vk_render_pass == NULL) {
		return 1;
	}
	*vk_render_pass	  = (gfx_vulkan_render_pass_t){0};
	render_pass->data = vk_render_pass;

	gfx_vulkan_t *vulkan = render_pass->gfx->data;

	VkAttachmentDescription attachment = {
		.format		= gfx_vulkan_format(config->color_format),
		.samples	= VK_SAMPLE_COUNT_1_BIT,
		.loadOp		= gfx_vulkan_load_op(config->load),
		.storeOp	= gfx_vulkan_store_op(config->store),
		.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.finalLayout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	VkAttachmentReference color = {
		.attachment = 0,
		.layout	    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	VkSubpassDescription subpass = {
		.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments    = &color,
	};
	VkRenderPassCreateInfo create = {
		.sType		 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments	 = &attachment,
		.subpassCount	 = 1,
		.pSubpasses	 = &subpass,
	};

	if (!vk_ok(vulkan->CreateRenderPass(vulkan->device, &create, NULL, &vk_render_pass->render_pass))) {
		gfx_vulkan_render_pass_free(render_pass);
		return 1;
	}

	return 0;
}

static int target_valid(const gfx_target_t *target)
{
	if (target == NULL || target->width == 0 || target->height == 0) {
		return 0;
	}
	if (target->type == GFX_TARGET_MEMORY) {
		return target->format == GFX_FORMAT_RGBA8_UNORM && target->data != NULL && target->stride >= (size_t)target->width * 4;
	}
	if (target->type == GFX_TARGET_SWAPCHAIN) {
		return target->swapchain != NULL && target->format == target->swapchain->format &&
		       target->width == target->swapchain->width && target->height == target->swapchain->height &&
		       gfx_vulkan_format(target->format) != 0 && target->swapchain->surface != NULL &&
		       target->swapchain->surface->api == GFX_API_VULKAN && target->swapchain->surface->handle != 0;
	}

	return 0;
}

static int memory_type_find(gfx_vulkan_t *vulkan, u32 bits, u32 *index, int *coherent)
{
	VkPhysicalDeviceMemoryProperties props = {0};
	vulkan->GetPhysicalDeviceMemoryProperties(vulkan->physical_device, &props);
	for (u32 i = 0; i < props.memoryTypeCount; i++) {
		VkFlags flags = props.memoryTypes[i].propertyFlags;
		if ((bits & (1u << i)) && (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
			*index	  = i;
			*coherent = (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
			return 0;
		}
	}

	return 1;
}

static u32 clamp_u32(u32 value, u32 min, u32 max)
{
	if (value < min) {
		return min;
	}
	if (max != 0 && value > max) {
		return max;
	}
	return value;
}

static VkFlags composite_alpha_choose(VkFlags supported)
{
	if (supported & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
		return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	}
	for (u32 i = 0; i < sizeof(VkFlags) * 8; i++) {
		VkFlags bit = 1u << i;
		if (supported & bit) {
			return bit;
		}
	}
	return 0;
}

static int gfx_vulkan_surface_format_supported(const VkSurfaceFormatKHR *formats, u32 count, u32 vk_format, gfx_format_t gfx_format,
					       VkSurfaceFormatKHR *format, gfx_format_t *target_format)
{
	for (u32 i = 0; i < count; i++) {
		if (formats[i].format == vk_format && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			*format	       = formats[i];
			*target_format = gfx_format;
			return 1;
		}
	}

	return 0;
}

static int gfx_vulkan_surface_format_choose(const VkSurfaceFormatKHR *formats, u32 count, gfx_format_t requested,
					    VkSurfaceFormatKHR *format, gfx_format_t *target_format)
{
	if (count == 1 && formats[0].format == 0) {
		u32 vk_format = gfx_vulkan_format(requested);

		*format = (VkSurfaceFormatKHR){
			.format	    = vk_format != 0 ? vk_format : VK_FORMAT_B8G8R8A8_UNORM,
			.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		};
		*target_format = vk_format != 0 ? requested : GFX_FORMAT_BGRA8_UNORM;
		return 0;
	}

	static const gfx_format_t preferred[] = {
		GFX_FORMAT_RGBA8_UNORM,
		GFX_FORMAT_BGRA8_UNORM,
		GFX_FORMAT_RGBA8_SRGB,
		GFX_FORMAT_BGRA8_SRGB,
	};

	if (gfx_vulkan_surface_format_supported(formats, count, gfx_vulkan_format(requested), requested, format, target_format)) {
		return 0;
	}
	for (u32 i = 0; i < sizeof(preferred) / sizeof(preferred[0]); i++) {
		if (gfx_vulkan_surface_format_supported(
			    formats, count, gfx_vulkan_format(preferred[i]), preferred[i], format, target_format)) {
			return 0;
		}
	}

	return 1;
}

static int gfx_vulkan_surface_format(gfx_vulkan_t *vulkan, VkSurfaceKHR surface, gfx_format_t requested, VkSurfaceFormatKHR *format,
				     gfx_format_t *target_format)
{
	u32 count = 0;
	if (!vk_ok(vulkan->GetPhysicalDeviceSurfaceFormatsKHR(vulkan->physical_device, surface, &count, NULL)) || count == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan surface formats");
		return 1;
	}

	VkSurfaceFormatKHR formats[16] = {0};
	if (count > sizeof(formats) / sizeof(formats[0])) {
		count = sizeof(formats) / sizeof(formats[0]);
	}
	if (!vk_ok(vulkan->GetPhysicalDeviceSurfaceFormatsKHR(vulkan->physical_device, surface, &count, formats))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan surface formats");
		return 1;
	}

	if (gfx_vulkan_surface_format_choose(formats, count, requested, format, target_format)) {
		log_error("cgfx", "gfx_vulkan", NULL, "no compatible Vulkan surface format is available");
		return 1;
	}
	return 0;
}

static void gfx_vulkan_target_free(gfx_target_t *target)
{
	if (target == NULL || target->gfx == NULL || target->gfx->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan = target->gfx->data;
	if (target->driver_data != NULL) {
		if (target->type == GFX_TARGET_SWAPCHAIN) {
			target->driver_data = NULL;
		} else {
			gfx_vulkan_memory_target_t *vk_target = target->driver_data;
			gfx_vulkan_memory_target_free(vulkan, vk_target);
			alloc_free(&target->gfx->alloc, vk_target, sizeof(gfx_vulkan_memory_target_t));
			target->driver_data = NULL;
		}
	}
	if (vulkan->target == target) {
		vulkan->target = NULL;
	}
}

static int gfx_vulkan_swapchain_init(gfx_swapchain_t *swapchain, const gfx_swapchain_config_t *config)
{
	(void)config;

	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->data == NULL || swapchain->surface == NULL ||
	    swapchain->surface->api != GFX_API_VULKAN || swapchain->surface->handle == 0 || gfx_vulkan_format(swapchain->format) == 0 ||
	    swapchain->width == 0 || swapchain->height == 0) {
		return 1;
	}

	gfx_vulkan_t *vulkan = swapchain->gfx->data;
	if (!vulkan->surface_enabled || !vulkan->swapchain_enabled) {
		log_error("cgfx", "gfx_vulkan", NULL, "Vulkan surface target requires WSI and swapchain support");
		return 1;
	}
	gfx_vulkan_swapchain_t *vk_swapchain = swapchain->data;
	if (vk_swapchain == NULL) {
		vk_swapchain = alloc_alloc(&swapchain->gfx->alloc, sizeof(gfx_vulkan_swapchain_t));
		if (vk_swapchain == NULL) {
			return 1;
		}
		*vk_swapchain	= (gfx_vulkan_swapchain_t){0};
		swapchain->data = vk_swapchain;
	}

	VkSurfaceKHR surface = (VkSurfaceKHR)swapchain->surface->handle;
	VkBool32 supported   = 0;
	if (!vk_ok(vulkan->GetPhysicalDeviceSurfaceSupportKHR(vulkan->physical_device, vulkan->queue_family, surface, &supported)) ||
	    !supported) {
		log_error("cgfx", "gfx_vulkan", NULL, "Vulkan queue does not support presentation to surface");
		return 1;
	}

	VkSurfaceCapabilitiesKHR caps = {0};
	if (!vk_ok(vulkan->GetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan->physical_device, surface, &caps))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to query Vulkan surface capabilities");
		return 1;
	}
	if ((caps.supportedUsageFlags & (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)) !=
	    (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)) {
		log_error("cgfx", "gfx_vulkan", NULL, "Vulkan surface does not support required image usage");
		return 1;
	}

	VkSurfaceFormatKHR format  = {0};
	gfx_format_t target_format = GFX_FORMAT_NONE;
	if (gfx_vulkan_surface_format(vulkan, surface, swapchain->format, &format, &target_format)) {
		return 1;
	}

	u32 image_count = caps.minImageCount + 1;
	if (caps.maxImageCount != 0 && image_count > caps.maxImageCount) {
		image_count = caps.maxImageCount;
	}

	VkExtent2D extent = {
		.width	= swapchain->width,
		.height = swapchain->height,
	};
	if (caps.currentExtent.width != ~0u) {
		extent = caps.currentExtent;
	} else {
		extent.width  = clamp_u32(extent.width, caps.minImageExtent.width, caps.maxImageExtent.width);
		extent.height = clamp_u32(extent.height, caps.minImageExtent.height, caps.maxImageExtent.height);
	}

	VkSwapchainCreateInfoKHR create = {
		.sType		  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface	  = surface,
		.minImageCount	  = image_count,
		.imageFormat	  = format.format,
		.imageColorSpace  = format.colorSpace,
		.imageExtent	  = extent,
		.imageArrayLayers = 1,
		.imageUsage	  = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform	  = caps.currentTransform,
		.compositeAlpha	  = composite_alpha_choose(caps.supportedCompositeAlpha),
		.presentMode	  = VK_PRESENT_MODE_FIFO_KHR,
		.clipped	  = 1,
		.oldSwapchain	  = vk_swapchain->swapchain,
	};
	if (create.compositeAlpha == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "Vulkan surface has no supported composite alpha mode");
		return 1;
	}
	VkSwapchainKHR new_swapchain = 0;
	if (!vk_ok(vulkan->CreateSwapchainKHR(vulkan->device, &create, NULL, &new_swapchain))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan swapchain");
		return 1;
	}

	u32 swapchain_image_count = 0;
	if (!vk_ok(vulkan->GetSwapchainImagesKHR(vulkan->device, new_swapchain, &swapchain_image_count, NULL)) ||
	    swapchain_image_count == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan swapchain images");
		vulkan->DestroySwapchainKHR(vulkan->device, new_swapchain, NULL);
		return 1;
	}
	if (swapchain_image_count > sizeof(vk_swapchain->swapchain_images) / sizeof(vk_swapchain->swapchain_images[0])) {
		swapchain_image_count = sizeof(vk_swapchain->swapchain_images) / sizeof(vk_swapchain->swapchain_images[0]);
	}
	VkImage swapchain_images[GFX_VULKAN_MAX_SWAPCHAIN_IMAGES] = {0};
	if (!vk_ok(vulkan->GetSwapchainImagesKHR(vulkan->device, new_swapchain, &swapchain_image_count, swapchain_images))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan swapchain images");
		vulkan->DestroySwapchainKHR(vulkan->device, new_swapchain, NULL);
		return 1;
	}

	VkSwapchainKHR old_swapchain = vk_swapchain->swapchain;
	if (old_swapchain != 0) {
		if (vulkan->DeviceWaitIdle != NULL) {
			vulkan->DeviceWaitIdle(vulkan->device);
		}
		gfx_vulkan_swapchain_draw_targets_free(vulkan, vk_swapchain);
		vulkan->DestroySwapchainKHR(vulkan->device, old_swapchain, NULL);
	}
	vk_swapchain->swapchain = new_swapchain;
	mem_set(vk_swapchain->swapchain_images, 0, sizeof(vk_swapchain->swapchain_images));
	mem_set(vk_swapchain->swapchain_image_layouts, 0, sizeof(vk_swapchain->swapchain_image_layouts));
	for (u32 i = 0; i < swapchain_image_count; i++) {
		vk_swapchain->swapchain_images[i] = swapchain_images[i];
	}
	vk_swapchain->swapchain_image_count = swapchain_image_count;
	vk_swapchain->swapchain_image_index = 0;
	vk_swapchain->swapchain_acquired    = 0;
	swapchain->format		    = target_format;
	swapchain->width		    = (u16)extent.width;
	swapchain->height		    = (u16)extent.height;
	return 0;
}

static void gfx_vulkan_swapchain_free(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->data == NULL || swapchain->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan		     = swapchain->gfx->data;
	gfx_vulkan_swapchain_t *vk_swapchain = swapchain->data;
	gfx_vulkan_swapchain_data_free(vulkan, vk_swapchain);
	alloc_free(&swapchain->gfx->alloc, vk_swapchain, sizeof(gfx_vulkan_swapchain_t));
	swapchain->data = NULL;
}

static int gfx_vulkan_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->data == NULL || width == 0 || height == 0) {
		return 1;
	}

	swapchain->width  = width;
	swapchain->height = height;
	return gfx_vulkan_swapchain_init(swapchain, &(gfx_swapchain_config_t){0});
}

static int gfx_vulkan_memory_target_init(gfx_target_t *target)
{
	gfx_vulkan_t *vulkan		      = target->gfx->data;
	gfx_vulkan_memory_target_t *vk_target = alloc_alloc(&target->gfx->alloc, sizeof(gfx_vulkan_memory_target_t));
	if (vk_target == NULL) {
		return 1;
	}
	*vk_target	    = (gfx_vulkan_memory_target_t){0};
	target->driver_data = vk_target;

	VkImageCreateInfo image = {
		.sType	       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType     = VK_IMAGE_TYPE_2D,
		.format	       = VK_FORMAT_R8G8B8A8_UNORM,
		.extent	       = {.width = target->width, .height = target->height, .depth = 1},
		.mipLevels     = 1,
		.arrayLayers   = 1,
		.samples       = VK_SAMPLE_COUNT_1_BIT,
		.tiling	       = VK_IMAGE_TILING_LINEAR,
		.usage	       = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};
	if (!vk_ok(vulkan->CreateImage(vulkan->device, &image, NULL, &vk_target->image))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan image");
		return 1;
	}

	VkMemoryRequirements req = {0};
	vulkan->GetImageMemoryRequirements(vulkan->device, vk_target->image, &req);

	u32 memory_type = 0;
	if (memory_type_find(vulkan, req.memoryTypeBits, &memory_type, &vk_target->memory_coherent)) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to find host visible Vulkan memory");
		gfx_vulkan_target_free(target);
		return 1;
	}

	VkMemoryAllocateInfo memory = {
		.sType		 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize	 = req.size,
		.memoryTypeIndex = memory_type,
	};
	if (!vk_ok(vulkan->AllocateMemory(vulkan->device, &memory, NULL, &vk_target->memory))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to allocate Vulkan memory");
		gfx_vulkan_target_free(target);
		return 1;
	}
	vk_target->memory_size = req.size;

	if (!vk_ok(vulkan->BindImageMemory(vulkan->device, vk_target->image, vk_target->memory, 0))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to bind Vulkan image memory");
		gfx_vulkan_target_free(target);
		return 1;
	}

	VkImageSubresource subresource = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
	};
	vulkan->GetImageSubresourceLayout(vulkan->device, vk_target->image, &subresource, &vk_target->layout);
	if (vk_target->layout.rowPitch < (VkDeviceSize)target->width * 4) {
		log_error("cgfx", "gfx_vulkan", NULL, "invalid Vulkan image row pitch");
		gfx_vulkan_target_free(target);
		return 1;
	}

	return 0;
}

static int gfx_vulkan_target_init(gfx_target_t *target)
{
	if (target == NULL || target->gfx == NULL || target->gfx->data == NULL || !target_valid(target)) {
		return 1;
	}

	if (target->type == GFX_TARGET_MEMORY) {
		return gfx_vulkan_memory_target_init(target);
	}

	target->driver_data = target->swapchain->data;
	return target->driver_data == NULL;
}

static int gfx_vulkan_create_image_view(gfx_vulkan_t *vulkan, const gfx_target_t *target, VkImage image, VkImageView *view)
{
	VkImageViewCreateInfo create = {
		.sType	  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image	  = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format	  = gfx_vulkan_format(target->format),
		.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.levelCount = 1,
				.layerCount = 1,
			},
	};
	return !vk_ok(vulkan->CreateImageView(vulkan->device, &create, NULL, view));
}

static int gfx_vulkan_copy_memory(gfx_vulkan_t *vulkan, const gfx_memory_readback_config_t *config)
{
	gfx_vulkan_memory_target_t *target = vulkan->target != NULL ? vulkan->target->driver_data : NULL;
	if (target == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	if (!target->memory_coherent) {
		VkMappedMemoryRange range = {
			.sType	= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.memory = target->memory,
			.offset = 0,
			.size	= target->memory_size,
		};
		if (!vk_ok(vulkan->InvalidateMappedMemoryRanges(vulkan->device, 1, &range))) {
			return 1;
		}
	}

	void *mapped = NULL;
	if (!vk_ok(vulkan->MapMemory(vulkan->device, target->memory, 0, target->memory_size, 0, &mapped))) {
		return 1;
	}

	u8 *src = (u8 *)mapped + target->layout.offset;
	for (u16 y = 0; y < vulkan->target->height; y++) {
		u8 *dst = (u8 *)config->data + (size_t)y * config->stride;
		mem_copy(dst, config->stride, src + (VkDeviceSize)y * target->layout.rowPitch, (size_t)vulkan->target->width * 4);
	}

	vulkan->UnmapMemory(vulkan->device, target->memory);
	return 0;
}

static int gfx_vulkan_target_read(gfx_target_t *target, const gfx_memory_readback_config_t *config)
{
	if (target == NULL || target->gfx == NULL || target->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan		      = target->gfx->data;
	gfx_vulkan_memory_target_t *vk_target = target->driver_data;
	if (vulkan->target != target || target->type != GFX_TARGET_MEMORY || vk_target == NULL || vk_target->image == 0 ||
	    vk_target->memory == 0 || vulkan->frame.active) {
		return 1;
	}

	return gfx_vulkan_copy_memory(vulkan, config);
}

static int gfx_vulkan_swapchain_recreate(gfx_swapchain_t *swapchain)
{
	if (gfx_vulkan_swapchain_init(swapchain, &(gfx_swapchain_config_t){0})) {
		return 1;
	}
	swapchain->generation++;
	return 0;
}

static int gfx_vulkan_swapchain_refresh(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->data == NULL || swapchain->data == NULL ||
	    swapchain->surface == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_vulkan_t *vulkan	      = swapchain->gfx->data;
	VkSurfaceKHR surface	      = (VkSurfaceKHR)swapchain->surface->handle;
	VkSurfaceCapabilitiesKHR caps = {0};
	if (!vk_ok(vulkan->GetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan->physical_device, surface, &caps))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to query Vulkan surface capabilities");
		return 1;
	}

	if (caps.currentExtent.width == ~0u ||
	    (caps.currentExtent.width == swapchain->width && caps.currentExtent.height == swapchain->height)) {
		return 0;
	}
	if (caps.currentExtent.width == 0 || caps.currentExtent.height == 0 || caps.currentExtent.width > 0xffffu ||
	    caps.currentExtent.height > 0xffffu) {
		return 1;
	}

	return gfx_vulkan_swapchain_recreate(swapchain);
}

static int gfx_vulkan_acquire_swapchain(gfx_vulkan_t *vulkan)
{
	gfx_vulkan_swapchain_t *swapchain = vulkan->target != NULL ? vulkan->target->driver_data : NULL;
	if (swapchain == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	if (swapchain->swapchain_acquired) {
		return 0;
	}

	int acquired = 0;
	for (u32 attempt = 0; attempt < 2; attempt++) {
		if (!vk_ok(vulkan->ResetFences(vulkan->device, 1, &vulkan->fence))) {
			return 1;
		}
		VkResult result = vulkan->AcquireNextImageKHR(
			vulkan->device, swapchain->swapchain, ~0ull, 0, vulkan->fence, &swapchain->swapchain_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			if (vulkan->target == NULL || vulkan->target->type != GFX_TARGET_SWAPCHAIN ||
			    gfx_vulkan_swapchain_recreate(vulkan->target->swapchain)) {
				return 1;
			}
			continue;
		}
		if (!vk_swapchain_ok(result) || !vk_ok(vulkan->WaitForFences(vulkan->device, 1, &vulkan->fence, 1, ~0ull))) {
			return 1;
		}
		acquired = 1;
		break;
	}
	if (!acquired) {
		return 1;
	}
	if (swapchain->swapchain_image_index >= swapchain->swapchain_image_count) {
		return 1;
	}

	swapchain->swapchain_acquired = 1;
	return 0;
}

static int gfx_vulkan_swapchain_present(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan		     = swapchain->gfx->data;
	gfx_vulkan_swapchain_t *vk_swapchain = swapchain->data;
	if (vk_swapchain == NULL || vk_swapchain->swapchain == 0 || !vk_swapchain->swapchain_acquired) {
		return 1;
	}

	VkPresentInfoKHR present = {
		.sType		= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1,
		.pSwapchains	= &vk_swapchain->swapchain,
		.pImageIndices	= &vk_swapchain->swapchain_image_index,
	};
	VkResult result			 = vulkan->QueuePresentKHR(vulkan->queue, &present);
	vk_swapchain->swapchain_acquired = 0;
	if (vk_swapchain_needs_recreate(result)) {
		return gfx_vulkan_swapchain_recreate(swapchain);
	}
	if (!vk_ok(result)) {
		return 1;
	}
	return 0;
}

static void gfx_vulkan_framebuffer_free(gfx_framebuffer_t *framebuffer)
{
	if (framebuffer == NULL || framebuffer->gfx == NULL || framebuffer->gfx->data == NULL || framebuffer->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan			 = framebuffer->gfx->data;
	gfx_vulkan_framebuffer_t *vk_framebuffer = framebuffer->data;
	if (vk_framebuffer->framebuffer != 0) {
		vulkan->DestroyFramebuffer(vulkan->device, vk_framebuffer->framebuffer, NULL);
		vk_framebuffer->framebuffer = 0;
	}
	for (u32 i = 0; i < sizeof(vk_framebuffer->swapchain_framebuffers) / sizeof(vk_framebuffer->swapchain_framebuffers[0]); i++) {
		if (vk_framebuffer->swapchain_framebuffers[i] != 0) {
			vulkan->DestroyFramebuffer(vulkan->device, vk_framebuffer->swapchain_framebuffers[i], NULL);
			vk_framebuffer->swapchain_framebuffers[i] = 0;
		}
	}
	alloc_free(&framebuffer->gfx->alloc, vk_framebuffer, sizeof(gfx_vulkan_framebuffer_t));
	framebuffer->data = NULL;
}

static int gfx_vulkan_create_framebuffer(gfx_vulkan_t *vulkan, const gfx_framebuffer_t *framebuffer, VkImageView view,
					 VkFramebuffer *vk_framebuffer)
{
	if (framebuffer == NULL || framebuffer->target == NULL || framebuffer->render_pass == NULL ||
	    framebuffer->render_pass->data == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_vulkan_render_pass_t *vk_render_pass = framebuffer->render_pass->data;

	VkFramebufferCreateInfo create = {
		.sType		 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass	 = vk_render_pass->render_pass,
		.attachmentCount = 1,
		.pAttachments	 = &view,
		.width		 = framebuffer->target->width,
		.height		 = framebuffer->target->height,
		.layers		 = 1,
	};
	return !vk_ok(vulkan->CreateFramebuffer(vulkan->device, &create, NULL, vk_framebuffer));
}

static int gfx_vulkan_framebuffer_init(gfx_framebuffer_t *framebuffer)
{
	if (framebuffer == NULL || framebuffer->gfx == NULL || framebuffer->gfx->data == NULL || framebuffer->target == NULL ||
	    framebuffer->target->driver_data == NULL || framebuffer->render_pass == NULL || framebuffer->render_pass->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan			 = framebuffer->gfx->data;
	gfx_vulkan_framebuffer_t *vk_framebuffer = alloc_alloc(&framebuffer->gfx->alloc, sizeof(gfx_vulkan_framebuffer_t));
	if (vk_framebuffer == NULL) {
		return 1;
	}
	*vk_framebuffer	  = (gfx_vulkan_framebuffer_t){0};
	framebuffer->data = vk_framebuffer;

	switch (framebuffer->target->type) {
	case GFX_TARGET_MEMORY: {
		gfx_vulkan_memory_target_t *target = framebuffer->target->driver_data;
		if (target->image == 0) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		if (target->image_view == 0 &&
		    gfx_vulkan_create_image_view(vulkan, framebuffer->target, target->image, &target->image_view)) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		if (gfx_vulkan_create_framebuffer(vulkan, framebuffer, target->image_view, &vk_framebuffer->framebuffer)) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		break;
	}
	case GFX_TARGET_SWAPCHAIN: {
		gfx_vulkan_swapchain_t *swapchain = framebuffer->target->driver_data;
		if (swapchain->swapchain_image_count == 0 ||
		    swapchain->swapchain_image_count >
			    sizeof(vk_framebuffer->swapchain_framebuffers) / sizeof(vk_framebuffer->swapchain_framebuffers[0])) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		for (u32 i = 0; i < swapchain->swapchain_image_count; i++) {
			if (swapchain->swapchain_images[i] == 0) {
				gfx_vulkan_framebuffer_free(framebuffer);
				return 1;
			}
			if (swapchain->swapchain_image_views[i] == 0 &&
			    gfx_vulkan_create_image_view(
				    vulkan, framebuffer->target, swapchain->swapchain_images[i], &swapchain->swapchain_image_views[i])) {
				gfx_vulkan_framebuffer_free(framebuffer);
				return 1;
			}
			if (gfx_vulkan_create_framebuffer(
				    vulkan, framebuffer, swapchain->swapchain_image_views[i], &vk_framebuffer->swapchain_framebuffers[i])) {
				gfx_vulkan_framebuffer_free(framebuffer);
				return 1;
			}
		}
		break;
	}
	default:
		gfx_vulkan_framebuffer_free(framebuffer);
		return 1;
	}

	return 0;
}

static int gfx_vulkan_frame_prepare(gfx_vulkan_t *vulkan, gfx_framebuffer_t *framebuffer)
{
	if (vulkan->frame.active || framebuffer == NULL || framebuffer->data == NULL) {
		return 1;
	}

	vulkan->frame = (gfx_vulkan_frame_t){0};
	if (vulkan->target == NULL || vulkan->target->driver_data == NULL) {
		return 1;
	}
	gfx_vulkan_framebuffer_t *vk_framebuffer = framebuffer->data;
	switch (vulkan->target->type) {
	case GFX_TARGET_MEMORY: {
		gfx_vulkan_memory_target_t *target = vulkan->target->driver_data;
		if (target->image == 0 || target->image_view == 0 || vk_framebuffer->framebuffer == 0) {
			return 1;
		}
		vulkan->frame.image	   = target->image;
		vulkan->frame.view	   = &target->image_view;
		vulkan->frame.framebuffer  = &vk_framebuffer->framebuffer;
		vulkan->frame.old_layout   = VK_IMAGE_LAYOUT_UNDEFINED;
		vulkan->frame.final_layout = VK_IMAGE_LAYOUT_GENERAL;
		break;
	}
	case GFX_TARGET_SWAPCHAIN: {
		gfx_vulkan_swapchain_t *swapchain = vulkan->target->driver_data;
		if (gfx_vulkan_acquire_swapchain(vulkan)) {
			return 1;
		}
		u32 i = swapchain->swapchain_image_index;
		if (swapchain->swapchain_images[i] == 0 || swapchain->swapchain_image_views[i] == 0 ||
		    vk_framebuffer->swapchain_framebuffers[i] == 0) {
			return 1;
		}
		vulkan->frame.image	  = swapchain->swapchain_images[i];
		vulkan->frame.view	  = &swapchain->swapchain_image_views[i];
		vulkan->frame.framebuffer = &vk_framebuffer->swapchain_framebuffers[i];
		vulkan->frame.image_index = i;
		vulkan->frame.old_layout =
			swapchain->swapchain_image_layouts[i] != 0 ? swapchain->swapchain_image_layouts[i] : VK_IMAGE_LAYOUT_UNDEFINED;
		vulkan->frame.final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		vulkan->frame.surface	   = 1;
		break;
	}
	default:
		return 1;
	}

	return 0;
}

static int gfx_vulkan_frame_begin_commands(gfx_vulkan_t *vulkan)
{
	VkCommandBufferBeginInfo begin = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	if (!vk_ok(vulkan->ResetFences(vulkan->device, 1, &vulkan->fence)) ||
	    !vk_ok(vulkan->ResetCommandBuffer(vulkan->command_buffer, 0)) ||
	    !vk_ok(vulkan->BeginCommandBuffer(vulkan->command_buffer, &begin))) {
		return 1;
	}

	return 0;
}

static int gfx_vulkan_frame_begin_render_pass(gfx_vulkan_t *vulkan, const gfx_render_pass_t *render_pass)
{
	if (vulkan->frame.image == 0 || vulkan->frame.framebuffer == NULL || *vulkan->frame.framebuffer == 0 || render_pass == NULL ||
	    render_pass->data == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_vulkan_render_pass_t *vk_render_pass = render_pass->data;

	VkImageSubresourceRange range = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.levelCount = 1,
		.layerCount = 1,
	};
	VkImageMemoryBarrier to_color = {
		.sType		     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.dstAccessMask	     = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout	     = vulkan->frame.old_layout,
		.newLayout	     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image		     = vulkan->frame.image,
		.subresourceRange    = range,
	};
	vulkan->CmdPipelineBarrier(vulkan->command_buffer,
				   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				   0,
				   0,
				   NULL,
				   0,
				   NULL,
				   1,
				   &to_color);
	VkRenderPassBeginInfo render = {
		.sType	     = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass  = vk_render_pass->render_pass,
		.framebuffer = *vulkan->frame.framebuffer,
		.renderArea =
			{
				.extent = {.width = vulkan->target->width, .height = vulkan->target->height},
			},
		.clearValueCount = render_pass->load == GFX_LOAD_CLEAR ? 1 : 0,
		.pClearValues	 = render_pass->load == GFX_LOAD_CLEAR ? &vulkan->clear_color : NULL,
	};

	vulkan->CmdBeginRenderPass(vulkan->command_buffer, &render, VK_SUBPASS_CONTENTS_INLINE);

	return 0;
}

static int gfx_vulkan_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || framebuffer == NULL || framebuffer->target == NULL ||
	    framebuffer->render_pass == NULL || framebuffer->render_pass->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan	       = frame->gfx->data;
	vulkan->clear_color.float32[0] = frame->pass.clear.r;
	vulkan->clear_color.float32[1] = frame->pass.clear.g;
	vulkan->clear_color.float32[2] = frame->pass.clear.b;
	vulkan->clear_color.float32[3] = frame->pass.clear.a;
	vulkan->target		       = framebuffer->target;
	if (gfx_vulkan_frame_prepare(vulkan, framebuffer) || gfx_vulkan_frame_begin_commands(vulkan) ||
	    gfx_vulkan_frame_begin_render_pass(vulkan, framebuffer->render_pass)) {
		vulkan->frame = (gfx_vulkan_frame_t){0};
		return 1;
	}

	vulkan->frame.active = 1;
	return 0;
}

typedef struct gfx_vulkan_vertex_2d_s {
	float x;
	float y;
	float r;
	float g;
	float b;
	float a;
} gfx_vulkan_vertex_2d_t;

static void gfx_vulkan_buffer_free(gfx_buffer_t *buffer)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || buffer->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan	       = buffer->gfx->data;
	gfx_vulkan_buffer_t *vk_buffer = buffer->data;

	if (vk_buffer->buffer != 0) {
		vulkan->DestroyBuffer(vulkan->device, vk_buffer->buffer, NULL);
		vk_buffer->buffer = 0;
	}
	if (vk_buffer->memory != 0) {
		vulkan->FreeMemory(vulkan->device, vk_buffer->memory, NULL);
		vk_buffer->memory = 0;
	}
	vk_buffer->memory_size	   = 0;
	vk_buffer->memory_coherent = 0;
	alloc_free(&buffer->gfx->alloc, vk_buffer, sizeof(gfx_vulkan_buffer_t));
	buffer->data = NULL;
}

static int gfx_vulkan_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = buffer->gfx->data;

	VkFlags usage;
	switch (config->type) {
	case GFX_BUFFER_VERTEX: {
		usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		break;
	}
	case GFX_BUFFER_INDEX: {
		usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		break;
	}
	default: {
		log_error("cgfx", "gfx_vulkan", NULL, "unsupported buffer type: %d", config->type);
		return 1;
	}
	}

	gfx_vulkan_buffer_t *vk_buffer = alloc_alloc(&buffer->gfx->alloc, sizeof(gfx_vulkan_buffer_t));
	if (vk_buffer == NULL) {
		return 1;
	}
	*vk_buffer = (gfx_vulkan_buffer_t){
		.type = config->type,
	};
	buffer->data = vk_buffer;

	VkBufferCreateInfo create = {
		.sType	     = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size	     = sizeof(gfx_vulkan_vertex_2d_t) * 3,
		.usage	     = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	if (!vk_ok(vulkan->CreateBuffer(vulkan->device, &create, NULL, &vk_buffer->buffer))) {
		gfx_vulkan_buffer_free(buffer);
		return 1;
	}

	VkMemoryRequirements req = {0};
	vulkan->GetBufferMemoryRequirements(vulkan->device, vk_buffer->buffer, &req);

	u32 memory_type = 0;
	if (memory_type_find(vulkan, req.memoryTypeBits, &memory_type, &vk_buffer->memory_coherent)) {
		gfx_vulkan_buffer_free(buffer);
		return 1;
	}

	VkMemoryAllocateInfo memory = {
		.sType		 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize	 = req.size,
		.memoryTypeIndex = memory_type,
	};
	if (!vk_ok(vulkan->AllocateMemory(vulkan->device, &memory, NULL, &vk_buffer->memory))) {
		gfx_vulkan_buffer_free(buffer);
		return 1;
	}
	vk_buffer->memory_size = req.size;

	if (!vk_ok(vulkan->BindBufferMemory(vulkan->device, vk_buffer->buffer, vk_buffer->memory, 0))) {
		gfx_vulkan_buffer_free(buffer);
		return 1;
	}

	return 0;
}

static int gfx_vulkan_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	(void)size;

	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan	       = buffer->gfx->data;
	gfx_vulkan_buffer_t *vk_buffer = buffer->data;

	void *mapped = NULL;
	if (!vk_ok(vulkan->MapMemory(vulkan->device, vk_buffer->memory, 0, size, 0, &mapped))) {
		return 1;
	}
	mem_copy(mapped, size, data, size);
	if (!vk_buffer->memory_coherent) {
		VkMappedMemoryRange range = {
			.sType	= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.memory = vk_buffer->memory,
			.offset = 0,
			.size	= size,
		};
		if (!vk_ok(vulkan->FlushMappedMemoryRanges(vulkan->device, 1, &range))) {
			vulkan->UnmapMemory(vulkan->device, vk_buffer->memory);
			return 1;
		}
	}
	vulkan->UnmapMemory(vulkan->device, vk_buffer->memory);
	return 0;
}

static int gfx_vulkan_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	(void)frame;

	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || buffer->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan	       = buffer->gfx->data;
	gfx_vulkan_buffer_t *vk_buffer = buffer->data;

	VkDeviceSize offset = 0;

	switch (buffer->type) {
	case GFX_BUFFER_VERTEX: {
		vulkan->CmdBindVertexBuffers(vulkan->command_buffer, 0, 1, &vk_buffer->buffer, &offset);
		break;
	}
	case GFX_BUFFER_INDEX: {
		vulkan->CmdBindIndexBuffer(vulkan->command_buffer, vk_buffer->buffer, 0, VK_INDEX_TYPE_UINT32);
		break;
	}
	default: {
		log_error("cgfx", "gfx_vulkan", NULL, "unsupported buffer type: %d", buffer->type);
		return 1;
	}
	}

	return 0;
}

static void gfx_vulkan_shader_free(gfx_shader_t *shader)
{
	if (shader == NULL || shader->gfx == NULL || shader->gfx->data == NULL || shader->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan	       = shader->gfx->data;
	gfx_vulkan_shader_t *vk_shader = shader->data;
	if (vk_shader->module != 0) {
		vulkan->DestroyShaderModule(vulkan->device, vk_shader->module, NULL);
		vk_shader->module = 0;
	}
	alloc_free(&shader->gfx->alloc, vk_shader, sizeof(gfx_vulkan_shader_t));
	shader->data = NULL;
}

static int gfx_vulkan_shader_init(gfx_shader_t *shader, const gfx_shader_config_t *config)
{
	if (shader == NULL || shader->gfx == NULL || shader->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = shader->gfx->data;

	gfx_shader_code_t shader_code = {0};
	if (gfx_shader_compiler_transpile(config->compiler, config->source, config->stage, GFX_SHADER_LANGUAGE_SPIRV, &shader_code)) {
		gfx_shader_code_free(&shader_code);
		return 1;
	}

	gfx_vulkan_shader_t *vk_shader = alloc_alloc(&shader->gfx->alloc, sizeof(gfx_vulkan_shader_t));
	if (vk_shader == NULL) {
		gfx_shader_code_free(&shader_code);
		return 1;
	}
	*vk_shader   = (gfx_vulkan_shader_t){0};
	shader->data = vk_shader;

	VkShaderModuleCreateInfo shader_create = {
		.sType	  = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = shader_code.code.used,
		.pCode	  = shader_code.code.data,
	};

	if (!vk_ok(vulkan->CreateShaderModule(vulkan->device, &shader_create, NULL, &vk_shader->module))) {
		gfx_shader_code_free(&shader_code);
		gfx_vulkan_shader_free(shader);
		return 1;
	}

	gfx_shader_code_free(&shader_code);

	return 0;
}

static void gfx_vulkan_pipeline_free(gfx_pipeline_t *pipeline)
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL || pipeline->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan		   = pipeline->gfx->data;
	gfx_vulkan_pipeline_t *vk_pipeline = pipeline->data;
	if (vk_pipeline->pipeline != 0) {
		vulkan->DestroyPipeline(vulkan->device, vk_pipeline->pipeline, NULL);
		vk_pipeline->pipeline = 0;
	}
	if (vk_pipeline->pipeline_layout != 0) {
		vulkan->DestroyPipelineLayout(vulkan->device, vk_pipeline->pipeline_layout, NULL);
		vk_pipeline->pipeline_layout = 0;
	}
	alloc_free(&pipeline->gfx->alloc, vk_pipeline, sizeof(gfx_vulkan_pipeline_t));
	pipeline->data = NULL;
}

static int gfx_vulkan_pipeline_init(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config)
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL || config == NULL || config->vs.data == NULL ||
	    config->fs.data == NULL || config->input_layout == NULL || config->input_layout_size == 0 ||
	    config->input_layout_size % sizeof(gfx_layout_t) != 0 || config->render_pass == NULL ||
	    config->render_pass->gfx != pipeline->gfx || config->render_pass->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan			 = pipeline->gfx->data;
	gfx_vulkan_render_pass_t *vk_render_pass = config->render_pass->data;

	gfx_vulkan_pipeline_t *vk_pipeline = alloc_alloc(&pipeline->gfx->alloc, sizeof(gfx_vulkan_pipeline_t));
	if (vk_pipeline == NULL) {
		return 1;
	}
	*vk_pipeline   = (gfx_vulkan_pipeline_t){0};
	pipeline->data = vk_pipeline;

	VkPipelineLayoutCreateInfo layout = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	};
	if (!vk_ok(vulkan->CreatePipelineLayout(vulkan->device, &layout, NULL, &vk_pipeline->pipeline_layout))) {
		gfx_vulkan_pipeline_free(pipeline);
		return 1;
	}

	gfx_vulkan_shader_t *vk_vs = config->vs.data;
	gfx_vulkan_shader_t *vk_fs = config->fs.data;

	VkPipelineShaderStageCreateInfo stages[2] = {
		{
			.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage	= VK_SHADER_STAGE_VERTEX_BIT,
			.module = vk_vs->module,
			.pName	= "main",
		},
		{
			.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage	= VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = vk_fs->module,
			.pName	= "main",
		},
	};

	size_t layout_cnt = config->input_layout_size / sizeof(gfx_layout_t);

	VkVertexInputAttributeDescription *attributes =
		alloc_alloc(&pipeline->gfx->alloc, layout_cnt * sizeof(VkVertexInputAttributeDescription));
	if (attributes == NULL) {
		gfx_vulkan_pipeline_free(pipeline);
		return 1;
	}

	size_t offset = 0;
	for (size_t i = 0; i < layout_cnt; i++) {
		attributes[i].location = config->input_layout[i].index;
		attributes[i].binding  = 0;

		if (config->input_layout[i].type == GFX_VALUE_FLOAT32 && config->input_layout[i].count == 2) {
			attributes[i].format = VK_FORMAT_R32G32_SFLOAT;
		} else if (config->input_layout[i].type == GFX_VALUE_FLOAT32 && config->input_layout[i].count == 4) {
			attributes[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		} else {
			log_error("cgfx",
				  "gfx_vulkan",
				  NULL,
				  "unsupported input layout: %d x %d",
				  config->input_layout[i].count,
				  config->input_layout[i].type);
			alloc_free(&pipeline->gfx->alloc, attributes, layout_cnt * sizeof(VkVertexInputAttributeDescription));
			gfx_vulkan_pipeline_free(pipeline);
			return 1;
		}

		attributes[i].offset = offset;

		offset += sizeof(float) * config->input_layout[i].count;
	}

	VkVertexInputBindingDescription binding = {
		.binding   = 0,
		.stride	   = offset,
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
	};

	VkPipelineVertexInputStateCreateInfo vertex_input = {
		.sType				 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount	 = 1,
		.pVertexBindingDescriptions	 = &binding,
		.vertexAttributeDescriptionCount = layout_cnt,
		.pVertexAttributeDescriptions	 = attributes,
	};
	VkPipelineInputAssemblyStateCreateInfo assembly = {
		.sType	  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};
	VkPipelineViewportStateCreateInfo viewport = {
		.sType	       = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount  = 1,
	};
	VkPipelineRasterizationStateCreateInfo raster = {
		.sType	     = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode    = VK_CULL_MODE_NONE,
		.frontFace   = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.lineWidth   = 1.0f,
	};
	VkPipelineMultisampleStateCreateInfo multisample = {
		.sType		      = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	};
	VkPipelineColorBlendAttachmentState blend_attachment = {
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp	     = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp	     = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	};
	VkPipelineColorBlendStateCreateInfo blend = {
		.sType		 = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments	 = &blend_attachment,
	};
	u32 dynamic_states[2]			 = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo dynamic = {
		.sType		   = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = 2,
		.pDynamicStates	   = dynamic_states,
	};
	VkGraphicsPipelineCreateInfo create = {
		.sType		     = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount	     = 2,
		.pStages	     = stages,
		.pVertexInputState   = &vertex_input,
		.pInputAssemblyState = &assembly,
		.pViewportState	     = &viewport,
		.pRasterizationState = &raster,
		.pMultisampleState   = &multisample,
		.pColorBlendState    = &blend,
		.pDynamicState	     = &dynamic,
		.layout		     = vk_pipeline->pipeline_layout,
		.renderPass	     = vk_render_pass->render_pass,
	};
	if (!vk_ok(vulkan->CreateGraphicsPipelines(vulkan->device, 0, 1, &create, NULL, &vk_pipeline->pipeline))) {
		alloc_free(&pipeline->gfx->alloc, attributes, layout_cnt * sizeof(VkVertexInputAttributeDescription));
		gfx_vulkan_pipeline_free(pipeline);
		return 1;
	}

	alloc_free(&pipeline->gfx->alloc, attributes, layout_cnt * sizeof(VkVertexInputAttributeDescription));

	return 0;
}

static int gfx_vulkan_pipeline_bind(gfx_frame_t *frame, const gfx_pipeline_t *pipeline)
{
	(void)frame;

	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->gfx->data == NULL || pipeline->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan		   = pipeline->gfx->data;
	gfx_vulkan_pipeline_t *vk_pipeline = pipeline->data;

	vulkan->CmdBindPipeline(vulkan->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline->pipeline);

	return 0;
}

static int gfx_vulkan_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = frame->gfx->data;
	if (!vulkan->frame.active) {
		return 1;
	}

	VkViewport viewport = {
		.x	  = frame->pass.viewport.x,
		.y	  = frame->pass.viewport.y,
		.width	  = frame->pass.viewport.width,
		.height	  = frame->pass.viewport.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	VkRect2D scissor = {
		.offset = {.x = frame->pass.viewport.x, .y = frame->pass.viewport.y},
		.extent =
			{
				.width	= frame->pass.viewport.width,
				.height = frame->pass.viewport.height,
			},
	};

	vulkan->CmdSetViewport(vulkan->command_buffer, 0, 1, &viewport);
	vulkan->CmdSetScissor(vulkan->command_buffer, 0, 1, &scissor);
	vulkan->CmdDraw(vulkan->command_buffer, vertex_count, 1, first_vertex, 0);

	return 0;
}

static int gfx_vulkan_draw_indexed(gfx_frame_t *frame, u32 index_count)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = frame->gfx->data;
	if (!vulkan->frame.active) {
		return 1;
	}

	VkViewport viewport = {
		.x	  = frame->pass.viewport.x,
		.y	  = frame->pass.viewport.y,
		.width	  = frame->pass.viewport.width,
		.height	  = frame->pass.viewport.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	VkRect2D scissor = {
		.offset = {.x = frame->pass.viewport.x, .y = frame->pass.viewport.y},
		.extent =
			{
				.width	= frame->pass.viewport.width,
				.height = frame->pass.viewport.height,
			},
	};

	vulkan->CmdSetViewport(vulkan->command_buffer, 0, 1, &viewport);
	vulkan->CmdSetScissor(vulkan->command_buffer, 0, 1, &scissor);
	vulkan->CmdDrawIndexed(vulkan->command_buffer, index_count, 1, 0, 0, 0);

	return 0;
}

static int gfx_vulkan_frame_end_render_pass(gfx_vulkan_t *vulkan)
{
	if (!vulkan->frame.active) {
		return 1; // LCOV_EXCL_LINE
	}

	VkImageSubresourceRange range = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.levelCount = 1,
		.layerCount = 1,
	};
	VkImageMemoryBarrier from_color = {
		.sType		     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask	     = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout	     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout	     = vulkan->frame.final_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image		     = vulkan->frame.image,
		.subresourceRange    = range,
	};

	vulkan->CmdEndRenderPass(vulkan->command_buffer);
	vulkan->CmdPipelineBarrier(vulkan->command_buffer,
				   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				   vulkan->frame.final_layout == VK_IMAGE_LAYOUT_GENERAL ? VK_PIPELINE_STAGE_HOST_BIT
											 : VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				   0,
				   0,
				   NULL,
				   0,
				   NULL,
				   1,
				   &from_color);
	if (!vk_ok(vulkan->EndCommandBuffer(vulkan->command_buffer))) {
		return 1;
	}

	return 0;
}

static int gfx_vulkan_frame_submit(gfx_vulkan_t *vulkan)
{
	VkSubmitInfo submit = {
		.sType		    = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers    = &vulkan->command_buffer,
	};
	if (!vk_ok(vulkan->QueueSubmit(vulkan->queue, 1, &submit, vulkan->fence)) ||
	    !vk_ok(vulkan->WaitForFences(vulkan->device, 1, &vulkan->fence, 1, ~0ull))) {
		return 1;
	}

	return 0;
}

static int gfx_vulkan_frame_finish(gfx_vulkan_t *vulkan)
{
	if (!vulkan->frame.active) {
		return 1; // LCOV_EXCL_LINE
	}
	if (!vulkan->frame.surface) {
		return 0;
	}
	gfx_vulkan_swapchain_t *swapchain = vulkan->target != NULL ? vulkan->target->driver_data : NULL;
	if (swapchain == NULL) {
		return 1;
	}

	swapchain->swapchain_image_layouts[vulkan->frame.image_index] = vulkan->frame.final_layout;
	return 0;
}

static int gfx_vulkan_end(gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = frame->gfx->data;
	if (!vulkan->frame.active) {
		return 1;
	}

	if (gfx_vulkan_frame_end_render_pass(vulkan) || gfx_vulkan_frame_submit(vulkan) || gfx_vulkan_frame_finish(vulkan)) {
		vulkan->frame = (gfx_vulkan_frame_t){0};
		return 1;
	}

	vulkan->frame = (gfx_vulkan_frame_t){0};
	return 0;
}

static gfx_driver_t gfx_vulkan = {
	.name			= "vulkan",
	.api			= GFX_API_VULKAN,
	.init			= gfx_vulkan_init,
	.free			= gfx_vulkan_free,
	.native			= gfx_vulkan_native,
	.proc			= gfx_vulkan_proc,
	.render_pass_init	= gfx_vulkan_render_pass_init,
	.render_pass_free	= gfx_vulkan_render_pass_free,
	.swapchain_init		= gfx_vulkan_swapchain_init,
	.swapchain_free		= gfx_vulkan_swapchain_free,
	.swapchain_resize	= gfx_vulkan_swapchain_resize,
	.swapchain_refresh	= gfx_vulkan_swapchain_refresh,
	.swapchain_present	= gfx_vulkan_swapchain_present,
	.target_init		= gfx_vulkan_target_init,
	.target_free		= gfx_vulkan_target_free,
	.target_read		= gfx_vulkan_target_read,
	.framebuffer_init	= gfx_vulkan_framebuffer_init,
	.framebuffer_free	= gfx_vulkan_framebuffer_free,
	.framebuffer_pass_begin = gfx_vulkan_framebuffer_pass_begin,
	.buffer_init		= gfx_vulkan_buffer_init,
	.buffer_free		= gfx_vulkan_buffer_free,
	.buffer_set_data	= gfx_vulkan_buffer_set_data,
	.buffer_bind		= gfx_vulkan_buffer_bind,
	.shader_init		= gfx_vulkan_shader_init,
	.shader_free		= gfx_vulkan_shader_free,
	.pipeline_init		= gfx_vulkan_pipeline_init,
	.pipeline_free		= gfx_vulkan_pipeline_free,
	.pipeline_bind		= gfx_vulkan_pipeline_bind,
	.draw			= gfx_vulkan_draw,
	.draw_indexed		= gfx_vulkan_draw_indexed,
	.end			= gfx_vulkan_end,
};

GFX_DRIVER(gfx_vulkan, &gfx_vulkan);
