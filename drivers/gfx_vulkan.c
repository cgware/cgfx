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
typedef u32 VkColorSpaceKHR;
typedef u32 VkPresentModeKHR;

typedef enum VkResult_e {
	VK_SUCCESS	  = 0,
	VK_SUBOPTIMAL_KHR = 1000001003,
} VkResult;

enum {
	VK_QUEUE_GRAPHICS_BIT				= 0x00000001,
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT		= 0x00000002,
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT		= 0x00000004,
	VK_IMAGE_USAGE_TRANSFER_DST_BIT			= 0x00000002,
	VK_FORMAT_FEATURE_TRANSFER_DST_BIT		= 0x00004000,
	VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT = 0x00000002,
	VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT	= 0x00000001,
	VK_IMAGE_ASPECT_COLOR_BIT			= 0x00000001,
	VK_ACCESS_TRANSFER_WRITE_BIT			= 0x00001000,
	VK_ACCESS_HOST_READ_BIT				= 0x00002000,
	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT		= 0x00000001,
	VK_PIPELINE_STAGE_TRANSFER_BIT			= 0x00001000,
	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT		= 0x00002000,
	VK_PIPELINE_STAGE_HOST_BIT			= 0x00004000,
	VK_FENCE_CREATE_SIGNALED_BIT			= 0x00000001,
	VK_STRUCTURE_TYPE_APPLICATION_INFO		= 0,
	VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO		= 1,
	VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO	= 2,
	VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO		= 3,
	VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO		= 14,
	VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO		= 5,
	VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO	= 39,
	VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO	= 40,
	VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO	= 42,
	VK_STRUCTURE_TYPE_SUBMIT_INFO			= 4,
	VK_STRUCTURE_TYPE_FENCE_CREATE_INFO		= 8,
	VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE		= 6,
	VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER		= 44,
	VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR	= 1000001000,
	VK_STRUCTURE_TYPE_PRESENT_INFO_KHR		= 1000001001,
	VK_IMAGE_TYPE_2D				= 0,
	VK_FORMAT_R8G8B8A8_UNORM			= 37,
	VK_FORMAT_R8G8B8A8_SRGB				= 43,
	VK_FORMAT_B8G8R8A8_UNORM			= 44,
	VK_FORMAT_B8G8R8A8_SRGB				= 50,
	VK_IMAGE_TILING_LINEAR				= 1,
	VK_IMAGE_LAYOUT_UNDEFINED			= 0,
	VK_IMAGE_LAYOUT_GENERAL				= 1,
	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL		= 7,
	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR			= 1000001002,
	VK_SHARING_MODE_EXCLUSIVE			= 0,
	VK_SAMPLE_COUNT_1_BIT				= 1,
	VK_COMMAND_BUFFER_LEVEL_PRIMARY			= 0,
	VK_API_VERSION_1_0				= 1u << 22,
	VK_COLOR_SPACE_SRGB_NONLINEAR_KHR		= 0,
	VK_PRESENT_MODE_FIFO_KHR			= 2,
	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR		= 0x00000001,
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

typedef union VkClearColorValue_u {
	float float32[4];
	int int32[4];
	u32 uint32[4];
} VkClearColorValue;

typedef struct VkImageSubresourceRange_s {
	VkFlags aspectMask;
	u32 baseMipLevel;
	u32 levelCount;
	u32 baseArrayLayer;
	u32 layerCount;
} VkImageSubresourceRange;

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
typedef VkResult (*PFN_vkAllocateMemory)(VkDevice, const VkMemoryAllocateInfo *, const void *, VkDeviceMemory *);
typedef void (*PFN_vkFreeMemory)(VkDevice, VkDeviceMemory, const void *);
typedef VkResult (*PFN_vkBindImageMemory)(VkDevice, VkImage, VkDeviceMemory, VkDeviceSize);
typedef void (*PFN_vkGetImageSubresourceLayout)(VkDevice, VkImage, const VkImageSubresource *, VkSubresourceLayout *);
typedef VkResult (*PFN_vkMapMemory)(VkDevice, VkDeviceMemory, VkDeviceSize, VkDeviceSize, VkFlags, void **);
typedef void (*PFN_vkUnmapMemory)(VkDevice, VkDeviceMemory);
typedef VkResult (*PFN_vkInvalidateMappedMemoryRanges)(VkDevice, u32, const VkMappedMemoryRange *);
typedef VkResult (*PFN_vkBeginCommandBuffer)(VkCommandBuffer, const VkCommandBufferBeginInfo *);
typedef VkResult (*PFN_vkEndCommandBuffer)(VkCommandBuffer);
typedef VkResult (*PFN_vkResetCommandBuffer)(VkCommandBuffer, VkFlags);
typedef void (*PFN_vkCmdPipelineBarrier)(VkCommandBuffer, VkFlags, VkFlags, VkFlags, u32, const void *, u32, const void *, u32,
					 const VkImageMemoryBarrier *);
typedef void (*PFN_vkCmdClearColorImage)(VkCommandBuffer, VkImage, u32, const VkClearColorValue *, u32, const VkImageSubresourceRange *);
typedef VkResult (*PFN_vkQueueSubmit)(VkQueue, u32, const VkSubmitInfo *, VkFence);
typedef VkResult (*PFN_vkCreateSwapchainKHR)(VkDevice, const VkSwapchainCreateInfoKHR *, const void *, VkSwapchainKHR *);
typedef void (*PFN_vkDestroySwapchainKHR)(VkDevice, VkSwapchainKHR, const void *);
typedef VkResult (*PFN_vkGetSwapchainImagesKHR)(VkDevice, VkSwapchainKHR, u32 *, VkImage *);
typedef VkResult (*PFN_vkAcquireNextImageKHR)(VkDevice, VkSwapchainKHR, u64, u64, VkFence, u32 *);
typedef VkResult (*PFN_vkQueuePresentKHR)(VkQueue, const VkPresentInfoKHR *);

typedef struct gfx_vulkan_s {
	proc_t *proc;
	void *lib;
	alloc_t alloc;
	gfx_target_t target;
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice device;
	VkQueue queue;
	u32 queue_family;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	VkFence fence;
	VkImage image;
	VkDeviceMemory memory;
	VkDeviceSize memory_size;
	int memory_coherent;
	VkSubresourceLayout layout;
	VkSwapchainKHR swapchain;
	VkImage swapchain_images[8];
	u32 swapchain_image_layouts[8];
	u32 swapchain_image_count;
	u32 swapchain_image_index;
	int swapchain_acquired;
	VkClearColorValue clear_color;
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
	PFN_vkAllocateMemory AllocateMemory;
	PFN_vkFreeMemory FreeMemory;
	PFN_vkBindImageMemory BindImageMemory;
	PFN_vkGetImageSubresourceLayout GetImageSubresourceLayout;
	PFN_vkMapMemory MapMemory;
	PFN_vkUnmapMemory UnmapMemory;
	PFN_vkInvalidateMappedMemoryRanges InvalidateMappedMemoryRanges;
	PFN_vkBeginCommandBuffer BeginCommandBuffer;
	PFN_vkEndCommandBuffer EndCommandBuffer;
	PFN_vkResetCommandBuffer ResetCommandBuffer;
	PFN_vkCmdPipelineBarrier CmdPipelineBarrier;
	PFN_vkCmdClearColorImage CmdClearColorImage;
	PFN_vkQueueSubmit QueueSubmit;
	PFN_vkCreateSwapchainKHR CreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR DestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR GetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR AcquireNextImageKHR;
	PFN_vkQueuePresentKHR QueuePresentKHR;
} gfx_vulkan_t;

static int load_lib_symbol(gfx_vulkan_t *vulkan, void **sym, strv_t name)
{
	if (proc_dlsym(vulkan->proc, vulkan->lib, name, sym)) {
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

#define LOAD_VK_LIB(_vulkan, _name)  load_lib_symbol((_vulkan), (void **)&(_vulkan)->_name, STRV("vk" #_name))
#define LOAD_VK_INST(_vulkan, _name) load_instance_symbol((_vulkan), (void **)&(_vulkan)->_name, "vk" #_name)
#define LOAD_VK_DEV(_vulkan, _name)  load_device_symbol((_vulkan), (void **)&(_vulkan)->_name, "vk" #_name)

static int vk_ok(VkResult result)
{
	return result == VK_SUCCESS;
}

static int vk_swapchain_ok(VkResult result)
{
	return result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR;
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

static void gfx_vulkan_swapchain_free(gfx_vulkan_t *vulkan)
{
	if (vulkan->swapchain != 0) {
		if (vulkan->DeviceWaitIdle != NULL) {
			vulkan->DeviceWaitIdle(vulkan->device);
		}
		vulkan->DestroySwapchainKHR(vulkan->device, vulkan->swapchain, NULL);
	}
	vulkan->swapchain	      = 0;
	vulkan->swapchain_image_count = 0;
	vulkan->swapchain_image_index = 0;
	vulkan->swapchain_acquired    = 0;
	mem_set(vulkan->swapchain_images, 0, sizeof(vulkan->swapchain_images));
	mem_set(vulkan->swapchain_image_layouts, 0, sizeof(vulkan->swapchain_image_layouts));
}

static void gfx_vulkan_target_free(gfx_vulkan_t *vulkan)
{
	if (vulkan->device == 0 && vulkan->image == 0 && vulkan->memory == 0 && vulkan->swapchain == 0) {
		vulkan->target = (gfx_target_t){0};
		return;
	}

	gfx_vulkan_swapchain_free(vulkan);
	if (vulkan->image != 0) {
		vulkan->DestroyImage(vulkan->device, vulkan->image, NULL);
		vulkan->image = 0;
	}
	if (vulkan->memory != 0) {
		vulkan->FreeMemory(vulkan->device, vulkan->memory, NULL);
		vulkan->memory = 0;
	}
	vulkan->memory_size	= 0;
	vulkan->memory_coherent = 0;
	vulkan->layout		= (VkSubresourceLayout){0};
	vulkan->target		= (gfx_target_t){0};
}

static void gfx_vulkan_device_free(gfx_vulkan_t *vulkan)
{
	gfx_vulkan_target_free(vulkan);
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
		proc_dlclose(vulkan->proc, vulkan->lib);
	}
	alloc_free(&vulkan->alloc, vulkan, sizeof(*vulkan));
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
				if ((props.linearTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT) == 0) {
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

static int gfx_vulkan_create_device(gfx_vulkan_t *vulkan, const gfx_plan_t *plan)
{
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
		owned_extensions = alloc_alloc(&vulkan->alloc, sizeof(*owned_extensions) * device_extension_count);
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
		alloc_free(&vulkan->alloc, owned_extensions, sizeof(*owned_extensions) * device_extension_count);
	}
	if (create_failed) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan device");
		return 1;
	}

	if (LOAD_VK_DEV(vulkan, DeviceWaitIdle) || LOAD_VK_DEV(vulkan, GetDeviceQueue) || LOAD_VK_DEV(vulkan, CreateCommandPool) ||
	    LOAD_VK_DEV(vulkan, DestroyCommandPool) || LOAD_VK_DEV(vulkan, AllocateCommandBuffers) ||
	    LOAD_VK_DEV(vulkan, FreeCommandBuffers) || LOAD_VK_DEV(vulkan, CreateFence) || LOAD_VK_DEV(vulkan, DestroyFence) ||
	    LOAD_VK_DEV(vulkan, ResetFences) || LOAD_VK_DEV(vulkan, WaitForFences) || LOAD_VK_DEV(vulkan, CreateImage) ||
	    LOAD_VK_DEV(vulkan, DestroyImage) || LOAD_VK_DEV(vulkan, GetImageMemoryRequirements) || LOAD_VK_DEV(vulkan, AllocateMemory) ||
	    LOAD_VK_DEV(vulkan, FreeMemory) || LOAD_VK_DEV(vulkan, BindImageMemory) || LOAD_VK_DEV(vulkan, GetImageSubresourceLayout) ||
	    LOAD_VK_DEV(vulkan, MapMemory) || LOAD_VK_DEV(vulkan, UnmapMemory) || LOAD_VK_DEV(vulkan, InvalidateMappedMemoryRanges) ||
	    LOAD_VK_DEV(vulkan, BeginCommandBuffer) || LOAD_VK_DEV(vulkan, EndCommandBuffer) || LOAD_VK_DEV(vulkan, ResetCommandBuffer) ||
	    LOAD_VK_DEV(vulkan, CmdPipelineBarrier) || LOAD_VK_DEV(vulkan, CmdClearColorImage) || LOAD_VK_DEV(vulkan, QueueSubmit)) {
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
	if (gfx == NULL || config == NULL || config->proc == NULL || config->alloc.alloc == NULL) {
		return 1;
	}

	alloc_t alloc	     = config->alloc;
	gfx_vulkan_t *vulkan = alloc_alloc(&alloc, sizeof(gfx_vulkan_t));
	if (vulkan == NULL) {
		return 1;
	}
	*vulkan = (gfx_vulkan_t){
		.proc	     = config->proc,
		.alloc	     = alloc,
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

	if (proc_dlopen(vulkan->proc, STRV("libvulkan.so.1"), &vulkan->lib) &&
	    proc_dlopen(vulkan->proc, STRV("libvulkan.so"), &vulkan->lib)) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to load libvulkan.so");
		return gfx_vulkan_init_free(gfx, vulkan);
	}

	if (LOAD_VK_LIB(vulkan, GetInstanceProcAddr)) {
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

	if (gfx_vulkan_pick_device(vulkan) || gfx_vulkan_create_device(vulkan, config->plan)) {
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
		proc_dlclose(vulkan->proc, vulkan->lib);
	}
	alloc_free(&vulkan->alloc, vulkan, sizeof(*vulkan));
	gfx->data = NULL;
	return 0;
}

static int gfx_vulkan_proc(gfx_t *gfx, strv_t name, void **proc)
{
	if (gfx == NULL || gfx->data == NULL || proc == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = gfx->data;
	if (proc_dlsym(vulkan->proc, vulkan->lib, name, proc) == 0) {
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

static int gfx_vulkan_native(gfx_t *gfx, gfx_native_t *native)
{
	if (gfx == NULL || gfx->data == NULL || native == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = gfx->data;
	*native		     = (gfx_native_t){
		.api		 = GFX_API_VULKAN,
		.instance	 = vulkan->instance,
		.physical_device = vulkan->physical_device,
		.device		 = vulkan->device,
	};
	return 0;
}

static u32 gfx_vulkan_format(gfx_format_t format);

static int target_valid(const gfx_target_t *target)
{
	if (target == NULL || target->width == 0 || target->height == 0) {
		return 0;
	}
	if (target->type == GFX_TARGET_MEMORY) {
		return target->format == GFX_FORMAT_RGBA8_UNORM && target->data != NULL && target->stride >= (size_t)target->width * 4;
	}
	if (target->type == GFX_TARGET_SURFACE) {
		return gfx_vulkan_format(target->format) != 0 && target->surface != 0;
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
		*format	      = (VkSurfaceFormatKHR){
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

static int gfx_vulkan_set_surface_target(gfx_vulkan_t *vulkan, const gfx_target_t *target)
{
	if (!vulkan->surface_enabled || !vulkan->swapchain_enabled) {
		log_error("cgfx", "gfx_vulkan", NULL, "Vulkan surface target requires WSI and swapchain support");
		return 1;
	}

	VkSurfaceKHR surface = (VkSurfaceKHR)target->surface;
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
	if ((caps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "Vulkan surface does not support transfer destination usage");
		return 1;
	}

	VkSurfaceFormatKHR format  = {0};
	gfx_format_t target_format = GFX_FORMAT_NONE;
	if (gfx_vulkan_surface_format(vulkan, surface, target->format, &format, &target_format)) {
		return 1;
	}

	u32 image_count = caps.minImageCount + 1;
	if (caps.maxImageCount != 0 && image_count > caps.maxImageCount) {
		image_count = caps.maxImageCount;
	}

	VkExtent2D extent = {
		.width	= target->width,
		.height = target->height,
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
		.imageUsage	  = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform	  = caps.currentTransform,
		.compositeAlpha	  = composite_alpha_choose(caps.supportedCompositeAlpha),
		.presentMode	  = VK_PRESENT_MODE_FIFO_KHR,
		.clipped	  = 1,
	};
	if (create.compositeAlpha == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "Vulkan surface has no supported composite alpha mode");
		return 1;
	}
	if (!vk_ok(vulkan->CreateSwapchainKHR(vulkan->device, &create, NULL, &vulkan->swapchain))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan swapchain");
		return 1;
	}

	u32 swapchain_image_count = 0;
	if (!vk_ok(vulkan->GetSwapchainImagesKHR(vulkan->device, vulkan->swapchain, &swapchain_image_count, NULL)) ||
	    swapchain_image_count == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan swapchain images");
		gfx_vulkan_swapchain_free(vulkan);
		return 1;
	}
	if (swapchain_image_count > sizeof(vulkan->swapchain_images) / sizeof(vulkan->swapchain_images[0])) {
		swapchain_image_count = sizeof(vulkan->swapchain_images) / sizeof(vulkan->swapchain_images[0]);
	}
	if (!vk_ok(vulkan->GetSwapchainImagesKHR(vulkan->device, vulkan->swapchain, &swapchain_image_count, vulkan->swapchain_images))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan swapchain images");
		gfx_vulkan_swapchain_free(vulkan);
		return 1;
	}
	vulkan->swapchain_image_count = swapchain_image_count;
	vulkan->target		      = *target;
	vulkan->target.format	      = target_format;
	return 0;
}

static int gfx_vulkan_set_memory_target(gfx_vulkan_t *vulkan, const gfx_target_t *target)
{
	VkImageCreateInfo image = {
		.sType	       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType     = VK_IMAGE_TYPE_2D,
		.format	       = VK_FORMAT_R8G8B8A8_UNORM,
		.extent	       = {.width = target->width, .height = target->height, .depth = 1},
		.mipLevels     = 1,
		.arrayLayers   = 1,
		.samples       = VK_SAMPLE_COUNT_1_BIT,
		.tiling	       = VK_IMAGE_TILING_LINEAR,
		.usage	       = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};
	if (!vk_ok(vulkan->CreateImage(vulkan->device, &image, NULL, &vulkan->image))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan image");
		return 1;
	}

	VkMemoryRequirements req = {0};
	vulkan->GetImageMemoryRequirements(vulkan->device, vulkan->image, &req);

	u32 memory_type = 0;
	if (memory_type_find(vulkan, req.memoryTypeBits, &memory_type, &vulkan->memory_coherent)) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to find host visible Vulkan memory");
		gfx_vulkan_target_free(vulkan);
		return 1;
	}

	VkMemoryAllocateInfo memory = {
		.sType		 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize	 = req.size,
		.memoryTypeIndex = memory_type,
	};
	if (!vk_ok(vulkan->AllocateMemory(vulkan->device, &memory, NULL, &vulkan->memory))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to allocate Vulkan memory");
		gfx_vulkan_target_free(vulkan);
		return 1;
	}
	vulkan->memory_size = req.size;

	if (!vk_ok(vulkan->BindImageMemory(vulkan->device, vulkan->image, vulkan->memory, 0))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to bind Vulkan image memory");
		gfx_vulkan_target_free(vulkan);
		return 1;
	}

	VkImageSubresource subresource = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
	};
	vulkan->GetImageSubresourceLayout(vulkan->device, vulkan->image, &subresource, &vulkan->layout);
	if (vulkan->layout.rowPitch < (VkDeviceSize)target->width * 4) {
		log_error("cgfx", "gfx_vulkan", NULL, "invalid Vulkan image row pitch");
		gfx_vulkan_target_free(vulkan);
		return 1;
	}

	vulkan->target = *target;
	return 0;
}

static int gfx_vulkan_set_target(gfx_t *gfx, const gfx_target_t *target)
{
	if (gfx == NULL || gfx->data == NULL || target == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = gfx->data;
	if (target->type == GFX_TARGET_NONE) {
		gfx_vulkan_target_free(vulkan);
		return 0;
	}
	if (!target_valid(target)) {
		return 1;
	}
	gfx_vulkan_target_free(vulkan);
	if (target->type == GFX_TARGET_MEMORY) {
		return gfx_vulkan_set_memory_target(vulkan, target);
	}
	return gfx_vulkan_set_surface_target(vulkan, target);
}

static int gfx_vulkan_clear_color(gfx_t *gfx, float r, float g, float b, float a)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan	       = gfx->data;
	vulkan->clear_color.float32[0] = r;
	vulkan->clear_color.float32[1] = g;
	vulkan->clear_color.float32[2] = b;
	vulkan->clear_color.float32[3] = a;
	return 0;
}

static int gfx_vulkan_copy_memory(gfx_vulkan_t *vulkan)
{
	if (!vulkan->memory_coherent) {
		VkMappedMemoryRange range = {
			.sType	= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.memory = vulkan->memory,
			.offset = 0,
			.size	= vulkan->memory_size,
		};
		if (!vk_ok(vulkan->InvalidateMappedMemoryRanges(vulkan->device, 1, &range))) {
			return 1;
		}
	}

	void *mapped = NULL;
	if (!vk_ok(vulkan->MapMemory(vulkan->device, vulkan->memory, 0, vulkan->memory_size, 0, &mapped))) {
		return 1;
	}

	u8 *src = (u8 *)mapped + vulkan->layout.offset;
	for (u16 y = 0; y < vulkan->target.height; y++) {
		u8 *dst = (u8 *)vulkan->target.data + (size_t)y * vulkan->target.stride;
		mem_copy(dst, vulkan->target.stride, src + (VkDeviceSize)y * vulkan->layout.rowPitch, (size_t)vulkan->target.width * 4);
	}

	vulkan->UnmapMemory(vulkan->device, vulkan->memory);
	return 0;
}

static int gfx_vulkan_clear_memory(gfx_vulkan_t *vulkan)
{
	VkImageSubresourceRange range = {
		.aspectMask	= VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel	= 0,
		.levelCount	= 1,
		.baseArrayLayer = 0,
		.layerCount	= 1,
	};
	VkImageMemoryBarrier to_general = {
		.sType		     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout	     = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout	     = VK_IMAGE_LAYOUT_GENERAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image		     = vulkan->image,
		.subresourceRange    = range,
	};
	VkImageMemoryBarrier to_host = {
		.sType		     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask	     = VK_ACCESS_TRANSFER_WRITE_BIT,
		.dstAccessMask	     = VK_ACCESS_HOST_READ_BIT,
		.oldLayout	     = VK_IMAGE_LAYOUT_GENERAL,
		.newLayout	     = VK_IMAGE_LAYOUT_GENERAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image		     = vulkan->image,
		.subresourceRange    = range,
	};
	VkCommandBufferBeginInfo begin = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	if (!vk_ok(vulkan->ResetFences(vulkan->device, 1, &vulkan->fence)) ||
	    !vk_ok(vulkan->ResetCommandBuffer(vulkan->command_buffer, 0)) ||
	    !vk_ok(vulkan->BeginCommandBuffer(vulkan->command_buffer, &begin))) {
		return 1;
	}

	vulkan->CmdPipelineBarrier(
		vulkan->command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &to_general);
	vulkan->CmdClearColorImage(vulkan->command_buffer, vulkan->image, VK_IMAGE_LAYOUT_GENERAL, &vulkan->clear_color, 1, &range);
	vulkan->CmdPipelineBarrier(
		vulkan->command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 0, NULL, 0, NULL, 1, &to_host);

	if (!vk_ok(vulkan->EndCommandBuffer(vulkan->command_buffer))) {
		return 1;
	}

	VkSubmitInfo submit = {
		.sType		    = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers    = &vulkan->command_buffer,
	};
	if (!vk_ok(vulkan->QueueSubmit(vulkan->queue, 1, &submit, vulkan->fence)) ||
	    !vk_ok(vulkan->WaitForFences(vulkan->device, 1, &vulkan->fence, 1, ~0ull))) {
		return 1;
	}

	return gfx_vulkan_copy_memory(vulkan);
}

static int gfx_vulkan_acquire_swapchain(gfx_vulkan_t *vulkan)
{
	if (vulkan->swapchain_acquired) {
		return 0;
	}

	if (!vk_ok(vulkan->ResetFences(vulkan->device, 1, &vulkan->fence)) ||
	    !vk_swapchain_ok(vulkan->AcquireNextImageKHR(
		    vulkan->device, vulkan->swapchain, ~0ull, 0, vulkan->fence, &vulkan->swapchain_image_index)) ||
	    !vk_ok(vulkan->WaitForFences(vulkan->device, 1, &vulkan->fence, 1, ~0ull))) {
		return 1;
	}
	if (vulkan->swapchain_image_index >= vulkan->swapchain_image_count) {
		return 1;
	}

	vulkan->swapchain_acquired = 1;
	return 0;
}

static int gfx_vulkan_clear_surface(gfx_vulkan_t *vulkan)
{
	if (gfx_vulkan_acquire_swapchain(vulkan)) {
		return 1;
	}

	VkImageSubresourceRange range = {
		.aspectMask	= VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel	= 0,
		.levelCount	= 1,
		.baseArrayLayer = 0,
		.layerCount	= 1,
	};
	VkImage image			= vulkan->swapchain_images[vulkan->swapchain_image_index];
	u32 old_layout			= vulkan->swapchain_image_layouts[vulkan->swapchain_image_index] != 0
						  ? vulkan->swapchain_image_layouts[vulkan->swapchain_image_index]
						  : VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageMemoryBarrier to_general = {
		.sType		     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.dstAccessMask	     = VK_ACCESS_TRANSFER_WRITE_BIT,
		.oldLayout	     = old_layout,
		.newLayout	     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image		     = image,
		.subresourceRange    = range,
	};
	VkImageMemoryBarrier to_present = {
		.sType		     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask	     = VK_ACCESS_TRANSFER_WRITE_BIT,
		.oldLayout	     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.newLayout	     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image		     = image,
		.subresourceRange    = range,
	};
	VkCommandBufferBeginInfo begin = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	if (!vk_ok(vulkan->ResetFences(vulkan->device, 1, &vulkan->fence)) ||
	    !vk_ok(vulkan->ResetCommandBuffer(vulkan->command_buffer, 0)) ||
	    !vk_ok(vulkan->BeginCommandBuffer(vulkan->command_buffer, &begin))) {
		return 1;
	}

	vulkan->CmdPipelineBarrier(vulkan->command_buffer,
				   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				   VK_PIPELINE_STAGE_TRANSFER_BIT,
				   0,
				   0,
				   NULL,
				   0,
				   NULL,
				   1,
				   &to_general);
	vulkan->CmdClearColorImage(vulkan->command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &vulkan->clear_color, 1, &range);
	vulkan->CmdPipelineBarrier(vulkan->command_buffer,
				   VK_PIPELINE_STAGE_TRANSFER_BIT,
				   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				   0,
				   0,
				   NULL,
				   0,
				   NULL,
				   1,
				   &to_present);
	if (!vk_ok(vulkan->EndCommandBuffer(vulkan->command_buffer))) {
		return 1;
	}

	VkSubmitInfo submit = {
		.sType		    = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers    = &vulkan->command_buffer,
	};
	if (!vk_ok(vulkan->QueueSubmit(vulkan->queue, 1, &submit, vulkan->fence)) ||
	    !vk_ok(vulkan->WaitForFences(vulkan->device, 1, &vulkan->fence, 1, ~0ull))) {
		return 1;
	}

	vulkan->swapchain_image_layouts[vulkan->swapchain_image_index] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	return 0;
}

static int gfx_vulkan_clear(gfx_t *gfx, u32 buffers)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}
	if ((buffers & GFX_CLEAR_COLOR_BUFFER) == 0) {
		return 0;
	}

	gfx_vulkan_t *vulkan = gfx->data;
	if (vulkan->target.type == GFX_TARGET_MEMORY) {
		return gfx_vulkan_clear_memory(vulkan);
	}
	if (vulkan->target.type == GFX_TARGET_SURFACE) {
		return gfx_vulkan_clear_surface(vulkan);
	}

	return 1;
}

static int gfx_vulkan_present(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = gfx->data;
	if (vulkan->target.type != GFX_TARGET_SURFACE || vulkan->swapchain == 0 || !vulkan->swapchain_acquired) {
		return 1;
	}

	VkPresentInfoKHR present = {
		.sType		= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1,
		.pSwapchains	= &vulkan->swapchain,
		.pImageIndices	= &vulkan->swapchain_image_index,
	};
	if (!vk_swapchain_ok(vulkan->QueuePresentKHR(vulkan->queue, &present))) {
		return 1;
	}

	vulkan->swapchain_acquired = 0;
	return 0;
}

static gfx_driver_t gfx_vulkan = {
	.name	     = "vulkan",
	.api	     = GFX_API_VULKAN,
	.init	     = gfx_vulkan_init,
	.free	     = gfx_vulkan_free,
	.native	     = gfx_vulkan_native,
	.proc	     = gfx_vulkan_proc,
	.set_target  = gfx_vulkan_set_target,
	.clear_color = gfx_vulkan_clear_color,
	.clear	     = gfx_vulkan_clear,
	.present     = gfx_vulkan_present,
};

GFX_DRIVER(gfx_vulkan, &gfx_vulkan);
