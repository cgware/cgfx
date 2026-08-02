#include "gfx_driver.h"

#include "log.h"
#include "mem.h"
#include "test.h"

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

enum {
	VK_SUCCESS				      = 0,
	VK_SUBOPTIMAL_KHR			      = 1000001003,
	VK_ERROR_OUT_OF_DATE_KHR		      = -1000001004,
	VK_QUEUE_GRAPHICS_BIT			      = 0x00000001,
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT	      = 0x00000002,
	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	      = 0x00000004,
	VK_FORMAT_FEATURE_TRANSFER_DST_BIT	      = 0x00004000,
	VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT	      = 0x00000080,
	VK_IMAGE_LAYOUT_GENERAL			      = 1,
	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL      = 2,
	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR		      = 1000001002,
	VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE	      = 6,
	VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER	      = 44,
	VK_IMAGE_ASPECT_COLOR_BIT		      = 0x00000001,
	VK_ACCESS_TRANSFER_WRITE_BIT		      = 0x00001000,
	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT	      = 0x00000100,
	VK_ACCESS_HOST_READ_BIT			      = 0x00002000,
	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT	      = 0x00000001,
	VK_PIPELINE_STAGE_TRANSFER_BIT		      = 0x00001000,
	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
	VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT	      = 0x00002000,
	VK_PIPELINE_STAGE_HOST_BIT		      = 0x00004000,
	VK_API_VERSION_1_0			      = 1u << 22,
	VK_FORMAT_R8G8B8A8_UNORM		      = 37,
	VK_FORMAT_R8G8B8A8_SRGB			      = 43,
	VK_FORMAT_B8G8R8A8_UNORM		      = 44,
	VK_FORMAT_B8G8R8A8_SRGB			      = 50,
	VK_IMAGE_USAGE_TRANSFER_DST_BIT		      = 0x00000002,
	VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT	      = 0x00000010,
	VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR	      = 0x00000001,
	VK_COLOR_SPACE_SRGB_NONLINEAR_KHR	      = 0,
};

typedef struct VkExtent2D_s {
	u32 width;
	u32 height;
} VkExtent2D;

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
	u32 presentMode;
	VkBool32 clipped;
	VkSwapchainKHR oldSwapchain;
} VkSwapchainCreateInfoKHR;

typedef struct VkPresentInfoKHR_s {
	u32 sType;
	const void *pNext;
	u32 waitSemaphoreCount;
	const void *pWaitSemaphores;
	u32 swapchainCount;
	const VkSwapchainKHR *pSwapchains;
	const u32 *pImageIndices;
	int *pResults;
} VkPresentInfoKHR;

typedef struct VkInstanceCreateInfo_s {
	u32 sType;
	const void *pNext;
	VkFlags flags;
	const struct VkApplicationInfo_s *pApplicationInfo;
	u32 enabledLayerCount;
	const char *const *ppEnabledLayerNames;
	u32 enabledExtensionCount;
	const char *const *ppEnabledExtensionNames;
} VkInstanceCreateInfo;

typedef struct VkApplicationInfo_s {
	u32 sType;
	const void *pNext;
	const char *pApplicationName;
	u32 applicationVersion;
	const char *pEngineName;
	u32 engineVersion;
	u32 apiVersion;
} VkApplicationInfo;

typedef struct VkMemoryRequirements_s {
	VkDeviceSize size;
	VkDeviceSize alignment;
	u32 memoryTypeBits;
} VkMemoryRequirements;

typedef union VkClearColorValue_u {
	float float32[4];
	int int32[4];
	u32 uint32[4];
} VkClearColorValue;

typedef struct VkOffset2D_s {
	int x;
	int y;
} VkOffset2D;

typedef struct VkRect2D_s {
	VkOffset2D offset;
	VkExtent2D extent;
} VkRect2D;

typedef struct VkRenderPassBeginInfo_s {
	u32 sType;
	const void *pNext;
	VkRenderPass renderPass;
	VkFramebuffer framebuffer;
	VkRect2D renderArea;
	u32 clearValueCount;
	const void *pClearValues;
} VkRenderPassBeginInfo;

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

typedef void (*t_gfx_vulkan_symbol_t)(void);

static int t_vk_create_instance_calls;
static int t_vk_destroy_instance_calls;
static int t_vk_enumerate_physical_devices_calls;
static int t_vk_create_device_calls;
static int t_vk_device_wait_idle_calls;
static int t_vk_destroy_device_calls;
static int t_vk_create_image_calls;
static int t_vk_destroy_image_calls;
static int t_vk_create_buffer_calls;
static int t_vk_destroy_buffer_calls;
static int t_vk_allocate_memory_calls;
static int t_vk_free_memory_calls;
static int t_vk_bind_image_memory_calls;
static int t_vk_bind_buffer_memory_calls;
static int t_vk_clear_color_image_calls;
static int t_vk_flush_mapped_memory_ranges_calls;
static int t_vk_queue_submit_calls;
static int t_vk_wait_for_fences_calls;
static int t_vk_invalidate_mapped_memory_ranges_calls;
static int t_vk_map_memory_calls;
static int t_vk_unmap_memory_calls;
static int t_vk_pipeline_barrier_calls;
static int t_vk_get_device_queue_calls;
static int t_vk_reset_fences_calls;
static int t_vk_reset_command_buffer_calls;
static int t_vk_begin_command_buffer_calls;
static int t_vk_end_command_buffer_calls;
static int t_vk_create_command_pool_calls;
static int t_vk_destroy_command_pool_calls;
static int t_vk_allocate_command_buffers_calls;
static int t_vk_free_command_buffers_calls;
static int t_vk_create_fence_calls;
static int t_vk_destroy_fence_calls;
static int t_vk_create_xlib_surface_calls;
static int t_vk_get_surface_support_calls;
static int t_vk_get_surface_capabilities_calls;
static int t_vk_get_surface_formats_calls;
static int t_vk_create_swapchain_calls;
static int t_vk_destroy_swapchain_calls;
static int t_vk_get_swapchain_images_calls;
static int t_vk_acquire_next_image_calls;
static int t_vk_queue_present_calls;
static int t_vk_create_image_view_calls;
static int t_vk_destroy_image_view_calls;
static int t_vk_create_shader_module_calls;
static int t_vk_destroy_shader_module_calls;
static int t_vk_create_render_pass_calls;
static int t_vk_destroy_render_pass_calls;
static int t_vk_create_framebuffer_calls;
static int t_vk_destroy_framebuffer_calls;
static int t_vk_create_pipeline_layout_calls;
static int t_vk_destroy_pipeline_layout_calls;
static int t_vk_create_graphics_pipelines_calls;
static int t_vk_destroy_pipeline_calls;
static int t_vk_begin_render_pass_calls;
static u32 t_vk_begin_render_pass_clear_value_count;
static int t_vk_end_render_pass_calls;
static int t_vk_bind_pipeline_calls;
static int t_vk_bind_vertex_buffers_calls;
static int t_vk_bind_index_buffer_calls;
static int t_vk_set_viewport_calls;
static int t_vk_set_scissor_calls;
static int t_vk_draw_calls;
static int t_vk_draw_indexed_calls;
static int t_vk_physical_device_count;
static int t_vk_queue_count;
static VkFlags t_vk_linear_features;
static VkFlags t_vk_memory_flags;
static u32 t_vk_memory_type_bits;
static VkDeviceSize t_vk_row_pitch;
static u8 t_vk_memory[256];
static float t_vk_clear_color[4];
static u32 t_vk_clear_layout;
static VkFramebuffer t_vk_begin_render_pass_framebuffer;
static VkImage t_vk_destroyed_image;
static VkDeviceMemory t_vk_freed_memory;
static VkMappedMemoryRange t_vk_invalidate_range;
static VkMappedMemoryRange t_vk_flush_range;
static VkImageMemoryBarrier t_vk_last_barrier;
static VkBuffer t_vk_bound_vertex_buffer;
static VkBuffer t_vk_bound_index_buffer;
static u32 t_vk_bound_index_type;
static u32 t_vk_draw_vertex_count;
static u32 t_vk_draw_instance_count;
static u32 t_vk_draw_index_count;
static int t_vk_vertex_first_x;
static int t_vk_vertex_last_y;
static int t_vk_missing_device_symbol;
static int t_vk_missing_instance_symbol;
static const char *t_vk_missing_device_symbol_name;
static const char *t_vk_missing_instance_symbol_name;
static int t_vk_missing_create_instance;
static int t_vk_create_instance_ret;
static int t_vk_enumerate_physical_devices_count_ret;
static int t_vk_enumerate_physical_devices_ret;
static int t_vk_create_device_ret;
static int t_vk_get_device_queue_null;
static int t_vk_create_command_pool_ret;
static int t_vk_allocate_command_buffers_ret;
static int t_vk_create_fence_ret;
static int t_vk_create_image_ret;
static int t_vk_allocate_memory_ret;
static int t_vk_bind_image_memory_ret;
static int t_vk_create_buffer_ret;
static int t_vk_bind_buffer_memory_ret;
static int t_vk_create_image_view_ret;
static int t_vk_create_shader_module_ret;
static int t_vk_create_render_pass_ret;
static int t_vk_create_framebuffer_ret;
static int t_vk_create_pipeline_layout_ret;
static int t_vk_create_graphics_pipelines_ret;
static int t_vk_map_memory_ret;
static int t_vk_flush_mapped_memory_ranges_ret;
static int t_vk_invalidate_mapped_memory_ranges_ret;
static int t_vk_reset_fences_ret;
static int t_vk_wait_for_fences_ret;
static int t_vk_reset_command_buffer_ret;
static int t_vk_begin_command_buffer_ret;
static int t_vk_end_command_buffer_ret;
static int t_vk_queue_submit_ret;
static int t_vk_surface_support_ret;
static int t_vk_surface_supported;
static int t_vk_surface_capabilities_ret;
static int t_vk_surface_capabilities_fail_at;
static VkSurfaceCapabilitiesKHR t_vk_surface_capabilities;
static int t_vk_surface_formats_count_ret;
static int t_vk_surface_formats_ret;
static u32 t_vk_swapchain_image_count;
static int t_vk_create_swapchain_ret;
static int t_vk_get_swapchain_images_count_ret;
static int t_vk_get_swapchain_images_ret;
static int t_vk_acquire_next_image_ret;
static u32 t_vk_acquire_next_image_index;
static int t_vk_queue_present_ret;
static u32 t_vk_instance_extension_count;
static const char *const *t_vk_instance_extensions;
static u32 t_vk_application_api_version;
static u32 t_vk_device_extension_count;
static const char *const *t_vk_device_extensions;
static const char *t_vk_device_extension_storage[8];
static VkSurfaceKHR t_vk_surface;
static gfx_surface_t t_gfx_vulkan_surface;
static VkSwapchainKHR t_vk_swapchain;
static VkSwapchainCreateInfoKHR t_vk_swapchain_create;
static VkSurfaceFormatKHR t_vk_surface_formats[20];
static u32 t_vk_surface_format_count;
static VkImage t_vk_swapchain_images[20];
static u32 t_vk_present_image_index;
static gfx_shader_compiler_t t_gfx_vulkan_compiler;
static int t_gfx_vulkan_compiler_initialized;

typedef struct t_gfx_vulkan_memory_target_data_s {
	VkImage image;
	VkDeviceMemory memory;
	VkDeviceSize memory_size;
	int memory_coherent;
	VkSubresourceLayout layout;
	VkImageView image_view;
} t_gfx_vulkan_memory_target_data_t;

typedef struct t_gfx_vulkan_swapchain_data_s {
	VkSwapchainKHR swapchain;
	VkImage swapchain_images[8];
	VkImageView swapchain_image_views[8];
	u32 swapchain_image_layouts[8];
	u32 swapchain_image_count;
	u32 swapchain_image_index;
	int swapchain_acquired;
} t_gfx_vulkan_swapchain_data_t;

typedef struct t_gfx_vulkan_render_pass_data_s {
	VkRenderPass render_pass;
} t_gfx_vulkan_render_pass_data_t;

typedef struct t_gfx_vulkan_data_head_s {
	void *lib;
	gfx_target_t *target;
} t_gfx_vulkan_data_head_t;

typedef struct t_gfx_vulkan_framebuffer_data_s {
	VkFramebuffer framebuffer;
	VkFramebuffer swapchain_framebuffers[8];
} t_gfx_vulkan_framebuffer_data_t;

typedef struct t_gfx_vulkan_buffer_data_s {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize memory_size;
	int memory_coherent;
} t_gfx_vulkan_buffer_data_t;

typedef struct t_gfx_vulkan_shader_data_s {
	VkShaderModule module;
} t_gfx_vulkan_shader_data_t;

typedef struct t_gfx_vulkan_pipeline_data_s {
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;
} t_gfx_vulkan_pipeline_data_t;

static const gfx_layout_t t_gfx_vulkan_input_layout[] = {
	{.index = 0, .semantic = "POSITION", .count = 2, .type = GFX_VALUE_FLOAT32},
	{.index = 1, .semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
};

static void *t_gfx_vulkan_alloc_fail(alloc_t *alloc, size_t size)
{
	(void)alloc;
	(void)size;
	return NULL;
}

static int t_gfx_vulkan_alloc_count;
static int t_gfx_vulkan_alloc_fail_at;

static void *t_gfx_vulkan_alloc_fail_n(alloc_t *alloc, size_t size)
{
	t_gfx_vulkan_alloc_count++;
	if (t_gfx_vulkan_alloc_count == t_gfx_vulkan_alloc_fail_at) {
		return NULL;
	}
	return alloc_alloc_std(alloc, size);
}

static void *t_gfx_vulkan_symbol(t_gfx_vulkan_symbol_t fn)
{
	union {
		t_gfx_vulkan_symbol_t fn;
		void *ptr;
	} symbol = {.fn = fn};

	return symbol.ptr;
}

static t_gfx_vulkan_symbol_t t_gfx_vulkan_fn(void *ptr)
{
	union {
		void *ptr;
		t_gfx_vulkan_symbol_t fn;
	} symbol = {.ptr = ptr};

	return symbol.fn;
}

static int t_vkCreateInstance(const void *create, const void *alloc, VkInstance *instance)
{
	(void)alloc;
	const VkInstanceCreateInfo *info = create;
	t_vk_create_instance_calls++;
	t_vk_instance_extension_count = info->enabledExtensionCount;
	t_vk_instance_extensions      = info->ppEnabledExtensionNames;
	t_vk_application_api_version  = info->pApplicationInfo->apiVersion;
	*instance		      = 1;
	return t_vk_create_instance_ret;
}

static void t_vkDestroyInstance(VkInstance instance, const void *alloc)
{
	(void)instance;
	(void)alloc;
	t_vk_destroy_instance_calls++;
}

static int t_vkEnumeratePhysicalDevices(VkInstance instance, u32 *count, VkPhysicalDevice *devices)
{
	(void)instance;
	t_vk_enumerate_physical_devices_calls++;
	if (devices == NULL) {
		*count = (u32)t_vk_physical_device_count;
		return t_vk_enumerate_physical_devices_count_ret;
	}
	for (u32 i = 0; i < *count; i++) {
		devices[i] = (VkPhysicalDevice)(i + 1);
	}
	return t_vk_enumerate_physical_devices_ret;
}

static void t_vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device, u32 *count, VkQueueFamilyProperties *queues)
{
	(void)device;
	if (queues == NULL) {
		*count = (u32)t_vk_queue_count;
		return;
	}
	queues[0].queueCount = 1;
	queues[0].queueFlags = VK_QUEUE_GRAPHICS_BIT;
}

static void t_vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice device, VkPhysicalDeviceMemoryProperties *props)
{
	(void)device;
	*props				    = (VkPhysicalDeviceMemoryProperties){0};
	props->memoryTypeCount		    = 1;
	props->memoryTypes[0].propertyFlags = t_vk_memory_flags;
}

static void t_vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice device, u32 format, VkFormatProperties *props)
{
	(void)device;
	(void)format;
	props->linearTilingFeatures = t_vk_linear_features;
}

static int t_vkCreateDevice(VkPhysicalDevice physical_device, const void *create, const void *alloc, VkDevice *device)
{
	(void)physical_device;
	(void)alloc;
	typedef struct VkDeviceCreateInfo_s {
		u32 sType;
		const void *pNext;
		VkFlags flags;
		u32 queueCreateInfoCount;
		const void *pQueueCreateInfos;
		u32 enabledLayerCount;
		const char *const *ppEnabledLayerNames;
		u32 enabledExtensionCount;
		const char *const *ppEnabledExtensionNames;
		const void *pEnabledFeatures;
	} VkDeviceCreateInfo;
	const VkDeviceCreateInfo *info = create;
	t_vk_create_device_calls++;
	t_vk_device_extension_count = info->enabledExtensionCount;
	for (u32 i = 0;
	     i < info->enabledExtensionCount && i < sizeof(t_vk_device_extension_storage) / sizeof(t_vk_device_extension_storage[0]);
	     i++) {
		t_vk_device_extension_storage[i] = info->ppEnabledExtensionNames[i];
	}
	t_vk_device_extensions = t_vk_device_extension_storage;
	*device		       = 2;
	return t_vk_create_device_ret;
}

static int t_vkDeviceWaitIdle(VkDevice device)
{
	(void)device;
	t_vk_device_wait_idle_calls++;
	return VK_SUCCESS;
}

static void t_vkDestroyDevice(VkDevice device, const void *alloc)
{
	(void)device;
	(void)alloc;
	t_vk_destroy_device_calls++;
}

static void t_vkGetDeviceQueue(VkDevice device, u32 queue_family, u32 queue_index, VkQueue *queue)
{
	(void)device;
	(void)queue_family;
	(void)queue_index;
	t_vk_get_device_queue_calls++;
	*queue = t_vk_get_device_queue_null ? 0 : 3;
}

static int t_vkCreateCommandPool(VkDevice device, const void *create, const void *alloc, VkCommandPool *pool)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_command_pool_calls++;
	*pool = 4;
	return t_vk_create_command_pool_ret;
}

static void t_vkDestroyCommandPool(VkDevice device, VkCommandPool pool, const void *alloc)
{
	(void)device;
	(void)pool;
	(void)alloc;
	t_vk_destroy_command_pool_calls++;
}

static int t_vkAllocateCommandBuffers(VkDevice device, const void *alloc_info, VkCommandBuffer *buffer)
{
	(void)device;
	(void)alloc_info;
	t_vk_allocate_command_buffers_calls++;
	*buffer = 5;
	return t_vk_allocate_command_buffers_ret;
}

static void t_vkFreeCommandBuffers(VkDevice device, VkCommandPool pool, u32 count, const VkCommandBuffer *buffers)
{
	(void)device;
	(void)pool;
	(void)count;
	(void)buffers;
	t_vk_free_command_buffers_calls++;
}

static int t_vkCreateFence(VkDevice device, const void *create, const void *alloc, VkFence *fence)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_fence_calls++;
	*fence = 6;
	return t_vk_create_fence_ret;
}

static void t_vkDestroyFence(VkDevice device, VkFence fence, const void *alloc)
{
	(void)device;
	(void)fence;
	(void)alloc;
	t_vk_destroy_fence_calls++;
}

static int t_vkResetFences(VkDevice device, u32 count, const VkFence *fences)
{
	(void)device;
	(void)count;
	(void)fences;
	t_vk_reset_fences_calls++;
	return t_vk_reset_fences_ret;
}

static int t_vkWaitForFences(VkDevice device, u32 count, const VkFence *fences, VkBool32 all, u64 timeout)
{
	(void)device;
	(void)count;
	(void)fences;
	(void)all;
	(void)timeout;
	t_vk_wait_for_fences_calls++;
	return t_vk_wait_for_fences_ret;
}

static int t_vkCreateImage(VkDevice device, const void *create, const void *alloc, VkImage *image)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_image_calls++;
	*image = 7;
	return t_vk_create_image_ret;
}

static void t_vkDestroyImage(VkDevice device, VkImage image, const void *alloc)
{
	(void)device;
	(void)alloc;
	t_vk_destroy_image_calls++;
	t_vk_destroyed_image = image;
}

static void t_vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements *req)
{
	(void)device;
	(void)image;
	req->size	    = sizeof(t_vk_memory);
	req->alignment	    = 1;
	req->memoryTypeBits = t_vk_memory_type_bits;
}

static int t_vkCreateBuffer(VkDevice device, const void *create, const void *alloc, VkBuffer *buffer)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_buffer_calls++;
	*buffer = 12;
	return t_vk_create_buffer_ret;
}

static void t_vkDestroyBuffer(VkDevice device, VkBuffer buffer, const void *alloc)
{
	(void)device;
	(void)buffer;
	(void)alloc;
	t_vk_destroy_buffer_calls++;
}

static void t_vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements *req)
{
	(void)device;
	(void)buffer;
	req->size	    = sizeof(t_vk_memory);
	req->alignment	    = 1;
	req->memoryTypeBits = t_vk_memory_type_bits;
}

static int t_vkAllocateMemory(VkDevice device, const void *alloc_info, const void *alloc, VkDeviceMemory *memory)
{
	(void)device;
	(void)alloc_info;
	(void)alloc;
	t_vk_allocate_memory_calls++;
	*memory = 8;
	return t_vk_allocate_memory_ret;
}

static void t_vkFreeMemory(VkDevice device, VkDeviceMemory memory, const void *alloc)
{
	(void)device;
	(void)alloc;
	t_vk_free_memory_calls++;
	t_vk_freed_memory = memory;
}

static int t_vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize offset)
{
	(void)device;
	(void)image;
	(void)memory;
	(void)offset;
	t_vk_bind_image_memory_calls++;
	return t_vk_bind_image_memory_ret;
}

static int t_vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize offset)
{
	(void)device;
	(void)buffer;
	(void)memory;
	(void)offset;
	t_vk_bind_buffer_memory_calls++;
	return t_vk_bind_buffer_memory_ret;
}

static void t_vkGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource *subresource,
					  VkSubresourceLayout *layout)
{
	(void)device;
	(void)image;
	(void)subresource;
	*layout = (VkSubresourceLayout){
		.offset	  = 4,
		.size	  = sizeof(t_vk_memory) - 4,
		.rowPitch = t_vk_row_pitch,
	};
}

static int t_vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkFlags flags, void **data)
{
	(void)device;
	(void)memory;
	(void)offset;
	(void)size;
	(void)flags;
	t_vk_map_memory_calls++;
	*data = t_vk_memory;
	return t_vk_map_memory_ret;
}

static void t_vkUnmapMemory(VkDevice device, VkDeviceMemory memory)
{
	(void)device;
	(void)memory;
	const float *vertices = (const float *)t_vk_memory;
	t_vk_vertex_first_x   = (int)vertices[0];
	t_vk_vertex_last_y    = (int)vertices[13];
	t_vk_unmap_memory_calls++;
}

static int t_vkFlushMappedMemoryRanges(VkDevice device, u32 count, const VkMappedMemoryRange *ranges)
{
	(void)device;
	t_vk_flush_mapped_memory_ranges_calls++;
	if (count > 0) {
		t_vk_flush_range = ranges[0];
	}
	return t_vk_flush_mapped_memory_ranges_ret;
}

static int t_vkInvalidateMappedMemoryRanges(VkDevice device, u32 count, const VkMappedMemoryRange *ranges)
{
	(void)device;
	t_vk_invalidate_mapped_memory_ranges_calls++;
	if (count > 0) {
		t_vk_invalidate_range = ranges[0];
	}
	return t_vk_invalidate_mapped_memory_ranges_ret;
}

static int t_vkBeginCommandBuffer(VkCommandBuffer buffer, const void *begin)
{
	(void)buffer;
	(void)begin;
	t_vk_begin_command_buffer_calls++;
	return t_vk_begin_command_buffer_ret;
}

static int t_vkEndCommandBuffer(VkCommandBuffer buffer)
{
	(void)buffer;
	t_vk_end_command_buffer_calls++;
	return t_vk_end_command_buffer_ret;
}

static int t_vkResetCommandBuffer(VkCommandBuffer buffer, VkFlags flags)
{
	(void)buffer;
	(void)flags;
	t_vk_reset_command_buffer_calls++;
	return t_vk_reset_command_buffer_ret;
}

static void t_vkCmdPipelineBarrier(VkCommandBuffer buffer, VkFlags src_stage, VkFlags dst_stage, VkFlags deps, u32 memory_count,
				   const void *memory_barriers, u32 buffer_count, const void *buffer_barriers, u32 image_count,
				   const VkImageMemoryBarrier *image_barriers)
{
	(void)buffer;
	(void)src_stage;
	(void)dst_stage;
	(void)deps;
	(void)memory_count;
	(void)memory_barriers;
	(void)buffer_count;
	(void)buffer_barriers;
	t_vk_pipeline_barrier_calls++;
	if (image_count > 0) {
		t_vk_last_barrier = image_barriers[0];
	}
}

static void t_vkCmdClearColorImage(VkCommandBuffer buffer, VkImage image, u32 layout, const VkClearColorValue *color, u32 range_count,
				   const VkImageSubresourceRange *ranges)
{
	(void)buffer;
	(void)image;
	(void)range_count;
	(void)ranges;
	t_vk_clear_color_image_calls++;
	t_vk_clear_layout   = layout;
	t_vk_clear_color[0] = color->float32[0];
	t_vk_clear_color[1] = color->float32[1];
	t_vk_clear_color[2] = color->float32[2];
	t_vk_clear_color[3] = color->float32[3];
}

static int t_vkCreateImageView(VkDevice device, const void *create, const void *alloc, VkImageView *view)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_image_view_calls++;
	*view = 13 + (VkImageView)t_vk_create_image_view_calls;
	return t_vk_create_image_view_ret;
}

static void t_vkDestroyImageView(VkDevice device, VkImageView view, const void *alloc)
{
	(void)device;
	(void)view;
	(void)alloc;
	t_vk_destroy_image_view_calls++;
}

static int t_vkCreateShaderModule(VkDevice device, const void *create, const void *alloc, VkShaderModule *shader)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_shader_module_calls++;
	*shader = 20 + (VkShaderModule)t_vk_create_shader_module_calls;
	return t_vk_create_shader_module_ret;
}

static void t_vkDestroyShaderModule(VkDevice device, VkShaderModule shader, const void *alloc)
{
	(void)device;
	(void)shader;
	(void)alloc;
	t_vk_destroy_shader_module_calls++;
}

static int t_vkCreateRenderPass(VkDevice device, const void *create, const void *alloc, VkRenderPass *render_pass)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_render_pass_calls++;
	*render_pass = 30;
	return t_vk_create_render_pass_ret;
}

static void t_vkDestroyRenderPass(VkDevice device, VkRenderPass render_pass, const void *alloc)
{
	(void)device;
	(void)render_pass;
	(void)alloc;
	t_vk_destroy_render_pass_calls++;
}

static int t_vkCreateFramebuffer(VkDevice device, const void *create, const void *alloc, VkFramebuffer *framebuffer)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_framebuffer_calls++;
	*framebuffer = 31 + (VkFramebuffer)t_vk_create_framebuffer_calls;
	return t_vk_create_framebuffer_ret;
}

static void t_vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const void *alloc)
{
	(void)device;
	(void)framebuffer;
	(void)alloc;
	t_vk_destroy_framebuffer_calls++;
}

static int t_vkCreatePipelineLayout(VkDevice device, const void *create, const void *alloc, VkPipelineLayout *layout)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_pipeline_layout_calls++;
	*layout = 40;
	return t_vk_create_pipeline_layout_ret;
}

static void t_vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout layout, const void *alloc)
{
	(void)device;
	(void)layout;
	(void)alloc;
	t_vk_destroy_pipeline_layout_calls++;
}

static int t_vkCreateGraphicsPipelines(VkDevice device, u64 cache, u32 count, const void *create, const void *alloc, VkPipeline *pipeline)
{
	(void)device;
	(void)cache;
	(void)count;
	(void)create;
	(void)alloc;
	t_vk_create_graphics_pipelines_calls++;
	*pipeline = 41;
	return t_vk_create_graphics_pipelines_ret;
}

static void t_vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const void *alloc)
{
	(void)device;
	(void)pipeline;
	(void)alloc;
	t_vk_destroy_pipeline_calls++;
}

static void t_vkCmdBeginRenderPass(VkCommandBuffer buffer, const void *begin, u32 contents)
{
	(void)buffer;
	(void)contents;
	t_vk_begin_render_pass_calls++;
	const VkRenderPassBeginInfo *info	 = begin;
	t_vk_begin_render_pass_framebuffer	 = info != NULL ? info->framebuffer : 0;
	t_vk_begin_render_pass_clear_value_count = info != NULL ? info->clearValueCount : 0;
	if (info != NULL && info->clearValueCount > 0 && info->pClearValues != NULL) {
		const VkClearColorValue *clear = info->pClearValues;
		t_vk_clear_color[0]	       = clear->float32[0];
		t_vk_clear_color[1]	       = clear->float32[1];
		t_vk_clear_color[2]	       = clear->float32[2];
		t_vk_clear_color[3]	       = clear->float32[3];
	}
}

static void t_vkCmdEndRenderPass(VkCommandBuffer buffer)
{
	(void)buffer;
	t_vk_end_render_pass_calls++;
}

static void t_vkCmdBindPipeline(VkCommandBuffer buffer, u32 bind_point, VkPipeline pipeline)
{
	(void)buffer;
	(void)bind_point;
	(void)pipeline;
	t_vk_bind_pipeline_calls++;
}

static void t_vkCmdBindVertexBuffers(VkCommandBuffer buffer, u32 first, u32 count, const VkBuffer *buffers, const VkDeviceSize *offsets)
{
	(void)buffer;
	(void)first;
	(void)count;
	(void)offsets;
	t_vk_bind_vertex_buffers_calls++;
	t_vk_bound_vertex_buffer = buffers[0];
}

static void t_vkCmdBindIndexBuffer(VkCommandBuffer buffer, VkBuffer index_buffer, VkDeviceSize offset, u32 index_type)
{
	(void)buffer;
	(void)offset;
	t_vk_bind_index_buffer_calls++;
	t_vk_bound_index_buffer = index_buffer;
	t_vk_bound_index_type	= index_type;
}

static void t_vkCmdSetViewport(VkCommandBuffer buffer, u32 first, u32 count, const void *viewports)
{
	(void)buffer;
	(void)first;
	(void)count;
	(void)viewports;
	t_vk_set_viewport_calls++;
}

static void t_vkCmdSetScissor(VkCommandBuffer buffer, u32 first, u32 count, const void *scissors)
{
	(void)buffer;
	(void)first;
	(void)count;
	(void)scissors;
	t_vk_set_scissor_calls++;
}

static void t_vkCmdDraw(VkCommandBuffer buffer, u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance)
{
	(void)buffer;
	(void)first_vertex;
	(void)first_instance;
	t_vk_draw_calls++;
	t_vk_draw_vertex_count	 = vertex_count;
	t_vk_draw_instance_count = instance_count;
}

static void t_vkCmdDrawIndexed(VkCommandBuffer buffer, u32 index_count, u32 instance_count, u32 first_index, int vertex_offset,
			       u32 first_instance)
{
	(void)buffer;
	(void)first_index;
	(void)vertex_offset;
	(void)first_instance;
	t_vk_draw_indexed_calls++;
	t_vk_draw_index_count	 = index_count;
	t_vk_draw_instance_count = instance_count;
}

static int t_vkQueueSubmit(VkQueue queue, u32 count, const void *submits, VkFence fence)
{
	(void)queue;
	(void)count;
	(void)submits;
	(void)fence;
	t_vk_queue_submit_calls++;
	return t_vk_queue_submit_ret;
}

static int t_vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice device, u32 queue_family, VkSurfaceKHR surface, VkBool32 *supported)
{
	(void)device;
	(void)queue_family;
	t_vk_get_surface_support_calls++;
	t_vk_surface = surface;
	*supported   = (VkBool32)t_vk_surface_supported;
	return t_vk_surface_support_ret;
}

static int t_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice device, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR *caps)
{
	(void)device;
	t_vk_get_surface_capabilities_calls++;
	t_vk_surface = surface;
	*caps	     = t_vk_surface_capabilities;
	if (t_vk_surface_capabilities_fail_at == t_vk_get_surface_capabilities_calls) {
		return 1;
	}
	return t_vk_surface_capabilities_ret;
}

static int t_vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice device, VkSurfaceKHR surface, u32 *count, VkSurfaceFormatKHR *formats)
{
	(void)device;
	t_vk_get_surface_formats_calls++;
	t_vk_surface = surface;
	if (formats == NULL) {
		*count = t_vk_surface_format_count;
		return t_vk_surface_formats_count_ret;
	}
	for (u32 i = 0; i < *count; i++) {
		formats[i] = t_vk_surface_formats[i];
	}
	return t_vk_surface_formats_ret;
}

static int t_vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR *create, const void *alloc, VkSwapchainKHR *swapchain)
{
	(void)device;
	(void)alloc;
	t_vk_create_swapchain_calls++;
	t_vk_swapchain_create = *create;
	*swapchain	      = t_vk_swapchain;
	return t_vk_create_swapchain_ret;
}

static void t_vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const void *alloc)
{
	(void)device;
	(void)alloc;
	t_vk_destroy_swapchain_calls++;
	t_vk_swapchain = swapchain;
}

static int t_vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, u32 *count, VkImage *images)
{
	(void)device;
	t_vk_get_swapchain_images_calls++;
	t_vk_swapchain = swapchain;
	if (images == NULL) {
		*count = t_vk_swapchain_image_count;
		return t_vk_get_swapchain_images_count_ret;
	}
	for (u32 i = 0; i < *count; i++) {
		images[i] = t_vk_swapchain_images[i];
	}
	return t_vk_get_swapchain_images_ret;
}

static int t_vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, u64 timeout, u64 semaphore, VkFence fence, u32 *image_index)
{
	(void)device;
	(void)timeout;
	(void)semaphore;
	(void)fence;
	t_vk_acquire_next_image_calls++;
	t_vk_swapchain = swapchain;
	*image_index   = t_vk_acquire_next_image_index;
	return t_vk_acquire_next_image_ret;
}

static int t_vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR *present)
{
	(void)queue;
	t_vk_queue_present_calls++;
	t_vk_swapchain		 = present->pSwapchains[0];
	t_vk_present_image_index = present->pImageIndices[0];
	return t_vk_queue_present_ret;
}

static void t_vkCreateXlibSurfaceKHR(void)
{
	t_vk_create_xlib_surface_calls++;
}

static void t_vkReset(void)
{
	t_vk_create_instance_calls		   = 0;
	t_vk_destroy_instance_calls		   = 0;
	t_vk_enumerate_physical_devices_calls	   = 0;
	t_vk_create_device_calls		   = 0;
	t_vk_device_wait_idle_calls		   = 0;
	t_vk_destroy_device_calls		   = 0;
	t_vk_create_image_calls			   = 0;
	t_vk_destroy_image_calls		   = 0;
	t_vk_create_buffer_calls		   = 0;
	t_vk_destroy_buffer_calls		   = 0;
	t_vk_allocate_memory_calls		   = 0;
	t_vk_free_memory_calls			   = 0;
	t_vk_bind_image_memory_calls		   = 0;
	t_vk_bind_buffer_memory_calls		   = 0;
	t_vk_clear_color_image_calls		   = 0;
	t_vk_flush_mapped_memory_ranges_calls	   = 0;
	t_vk_queue_submit_calls			   = 0;
	t_vk_wait_for_fences_calls		   = 0;
	t_vk_invalidate_mapped_memory_ranges_calls = 0;
	t_vk_map_memory_calls			   = 0;
	t_vk_unmap_memory_calls			   = 0;
	t_vk_pipeline_barrier_calls		   = 0;
	t_vk_get_device_queue_calls		   = 0;
	t_vk_reset_fences_calls			   = 0;
	t_vk_reset_command_buffer_calls		   = 0;
	t_vk_begin_command_buffer_calls		   = 0;
	t_vk_end_command_buffer_calls		   = 0;
	t_vk_create_command_pool_calls		   = 0;
	t_vk_destroy_command_pool_calls		   = 0;
	t_vk_allocate_command_buffers_calls	   = 0;
	t_vk_free_command_buffers_calls		   = 0;
	t_vk_create_fence_calls			   = 0;
	t_vk_destroy_fence_calls		   = 0;
	t_vk_create_xlib_surface_calls		   = 0;
	t_vk_get_surface_support_calls		   = 0;
	t_vk_get_surface_capabilities_calls	   = 0;
	t_vk_get_surface_formats_calls		   = 0;
	t_vk_create_swapchain_calls		   = 0;
	t_vk_destroy_swapchain_calls		   = 0;
	t_vk_get_swapchain_images_calls		   = 0;
	t_vk_acquire_next_image_calls		   = 0;
	t_vk_queue_present_calls		   = 0;
	t_vk_create_image_view_calls		   = 0;
	t_vk_destroy_image_view_calls		   = 0;
	t_vk_create_shader_module_calls		   = 0;
	t_vk_destroy_shader_module_calls	   = 0;
	t_vk_create_render_pass_calls		   = 0;
	t_vk_destroy_render_pass_calls		   = 0;
	t_vk_create_framebuffer_calls		   = 0;
	t_vk_destroy_framebuffer_calls		   = 0;
	t_vk_create_pipeline_layout_calls	   = 0;
	t_vk_destroy_pipeline_layout_calls	   = 0;
	t_vk_create_graphics_pipelines_calls	   = 0;
	t_vk_destroy_pipeline_calls		   = 0;
	t_vk_begin_render_pass_calls		   = 0;
	t_vk_begin_render_pass_clear_value_count   = 0;
	t_vk_end_render_pass_calls		   = 0;
	t_vk_bind_pipeline_calls		   = 0;
	t_vk_bind_vertex_buffers_calls		   = 0;
	t_vk_bind_index_buffer_calls		   = 0;
	t_vk_set_viewport_calls			   = 0;
	t_vk_set_scissor_calls			   = 0;
	t_vk_draw_calls				   = 0;
	t_vk_draw_indexed_calls			   = 0;
	t_vk_physical_device_count		   = 1;
	t_vk_queue_count			   = 1;
	t_vk_linear_features			   = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	t_vk_memory_flags			   = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	t_vk_memory_type_bits			   = 1;
	t_vk_row_pitch				   = 8;
	t_vk_destroyed_image			   = 0;
	t_vk_freed_memory			   = 0;
	t_vk_invalidate_range			   = (VkMappedMemoryRange){0};
	t_vk_flush_range			   = (VkMappedMemoryRange){0};
	t_vk_last_barrier			   = (VkImageMemoryBarrier){0};
	t_vk_begin_render_pass_framebuffer	   = 0;
	t_vk_bound_vertex_buffer		   = 0;
	t_vk_bound_index_buffer			   = 0;
	t_vk_bound_index_type			   = 0;
	t_vk_draw_vertex_count			   = 0;
	t_vk_draw_instance_count		   = 0;
	t_vk_draw_index_count			   = 0;
	t_vk_vertex_first_x			   = 0;
	t_vk_vertex_last_y			   = 0;
	t_vk_clear_layout			   = 0;
	t_vk_missing_device_symbol		   = 0;
	t_vk_missing_instance_symbol		   = 0;
	t_vk_missing_device_symbol_name		   = NULL;
	t_vk_missing_instance_symbol_name	   = NULL;
	t_vk_missing_create_instance		   = 0;
	t_vk_create_instance_ret		   = VK_SUCCESS;
	t_vk_enumerate_physical_devices_count_ret  = VK_SUCCESS;
	t_vk_enumerate_physical_devices_ret	   = VK_SUCCESS;
	t_vk_create_device_ret			   = VK_SUCCESS;
	t_vk_get_device_queue_null		   = 0;
	t_vk_create_command_pool_ret		   = VK_SUCCESS;
	t_vk_allocate_command_buffers_ret	   = VK_SUCCESS;
	t_vk_create_fence_ret			   = VK_SUCCESS;
	t_vk_create_image_ret			   = VK_SUCCESS;
	t_vk_create_buffer_ret			   = VK_SUCCESS;
	t_vk_allocate_memory_ret		   = VK_SUCCESS;
	t_vk_bind_image_memory_ret		   = VK_SUCCESS;
	t_vk_bind_buffer_memory_ret		   = VK_SUCCESS;
	t_vk_create_image_view_ret		   = VK_SUCCESS;
	t_vk_create_shader_module_ret		   = VK_SUCCESS;
	t_vk_create_render_pass_ret		   = VK_SUCCESS;
	t_vk_create_framebuffer_ret		   = VK_SUCCESS;
	t_vk_create_pipeline_layout_ret		   = VK_SUCCESS;
	t_vk_create_graphics_pipelines_ret	   = VK_SUCCESS;
	t_vk_map_memory_ret			   = VK_SUCCESS;
	t_vk_flush_mapped_memory_ranges_ret	   = VK_SUCCESS;
	t_vk_invalidate_mapped_memory_ranges_ret   = VK_SUCCESS;
	t_vk_reset_fences_ret			   = VK_SUCCESS;
	t_vk_wait_for_fences_ret		   = VK_SUCCESS;
	t_vk_reset_command_buffer_ret		   = VK_SUCCESS;
	t_vk_begin_command_buffer_ret		   = VK_SUCCESS;
	t_vk_end_command_buffer_ret		   = VK_SUCCESS;
	t_vk_queue_submit_ret			   = VK_SUCCESS;
	t_vk_surface_support_ret		   = VK_SUCCESS;
	t_vk_surface_supported			   = 1;
	t_vk_surface_capabilities_ret		   = VK_SUCCESS;
	t_vk_surface_capabilities_fail_at	   = 0;

	t_vk_surface_capabilities = (VkSurfaceCapabilitiesKHR){
		.minImageCount		 = 1,
		.maxImageCount		 = 3,
		.currentExtent		 = {.width = ~0u, .height = ~0u},
		.minImageExtent		 = {.width = 1, .height = 1},
		.maxImageExtent		 = {.width = 4096, .height = 4096},
		.currentTransform	 = 1,
		.supportedCompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.supportedUsageFlags	 = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	};
	t_vk_surface_formats_count_ret	    = VK_SUCCESS;
	t_vk_surface_formats_ret	    = VK_SUCCESS;
	t_vk_swapchain_image_count	    = 2;
	t_vk_create_swapchain_ret	    = VK_SUCCESS;
	t_vk_get_swapchain_images_count_ret = VK_SUCCESS;
	t_vk_get_swapchain_images_ret	    = VK_SUCCESS;
	t_vk_acquire_next_image_ret	    = VK_SUCCESS;
	t_vk_acquire_next_image_index	    = 1;
	t_vk_queue_present_ret		    = VK_SUCCESS;
	t_gfx_vulkan_alloc_count	    = 0;
	t_gfx_vulkan_alloc_fail_at	    = 0;
	t_vk_instance_extension_count	    = 0;
	t_vk_instance_extensions	    = NULL;
	t_vk_application_api_version	    = 0;
	t_vk_device_extension_count	    = 0;
	t_vk_device_extensions		    = NULL;
	for (u32 i = 0; i < sizeof(t_vk_device_extension_storage) / sizeof(t_vk_device_extension_storage[0]); i++) {
		t_vk_device_extension_storage[i] = NULL;
	}
	t_vk_surface	     = 0;
	t_gfx_vulkan_surface = (gfx_surface_t){
		.api	= GFX_API_VULKAN,
		.handle = 0x44,
	};
	t_vk_swapchain		  = 9;
	t_vk_swapchain_create	  = (VkSwapchainCreateInfoKHR){0};
	t_vk_surface_format_count = 1;

	t_vk_surface_formats[0] = (VkSurfaceFormatKHR){
		.format	    = VK_FORMAT_R8G8B8A8_UNORM,
		.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
	};
	for (u32 i = 1; i < sizeof(t_vk_surface_formats) / sizeof(t_vk_surface_formats[0]); i++) {
		t_vk_surface_formats[i] = (VkSurfaceFormatKHR){0};
	}
	t_vk_swapchain_images[0] = 10;
	t_vk_swapchain_images[1] = 11;
	t_vk_present_image_index = 0;
	mem_set(t_vk_memory, 0, sizeof(t_vk_memory));
}

static void *t_vkGetDeviceProcAddr(VkDevice device, const char *name)
{
	(void)device;
	if (t_vk_missing_device_symbol && t_strcmp(name, "vkCreateImage") == 0) {
		return NULL;
	}
	if (t_vk_missing_device_symbol_name != NULL && t_strcmp(name, t_vk_missing_device_symbol_name) == 0) {
		return NULL;
	}
	if (t_strcmp(name, "vkGetDeviceQueue") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetDeviceQueue);
	}
	if (t_strcmp(name, "vkDeviceWaitIdle") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDeviceWaitIdle);
	}
	if (t_strcmp(name, "vkCreateCommandPool") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateCommandPool);
	}
	if (t_strcmp(name, "vkDestroyCommandPool") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyCommandPool);
	}
	if (t_strcmp(name, "vkAllocateCommandBuffers") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkAllocateCommandBuffers);
	}
	if (t_strcmp(name, "vkFreeCommandBuffers") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkFreeCommandBuffers);
	}
	if (t_strcmp(name, "vkCreateFence") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateFence);
	}
	if (t_strcmp(name, "vkDestroyFence") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyFence);
	}
	if (t_strcmp(name, "vkResetFences") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkResetFences);
	}
	if (t_strcmp(name, "vkWaitForFences") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkWaitForFences);
	}
	if (t_strcmp(name, "vkCreateImage") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateImage);
	}
	if (t_strcmp(name, "vkDestroyImage") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyImage);
	}
	if (t_strcmp(name, "vkGetImageMemoryRequirements") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetImageMemoryRequirements);
	}
	if (t_strcmp(name, "vkCreateBuffer") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateBuffer);
	}
	if (t_strcmp(name, "vkDestroyBuffer") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyBuffer);
	}
	if (t_strcmp(name, "vkGetBufferMemoryRequirements") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetBufferMemoryRequirements);
	}
	if (t_strcmp(name, "vkAllocateMemory") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkAllocateMemory);
	}
	if (t_strcmp(name, "vkFreeMemory") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkFreeMemory);
	}
	if (t_strcmp(name, "vkBindImageMemory") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkBindImageMemory);
	}
	if (t_strcmp(name, "vkBindBufferMemory") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkBindBufferMemory);
	}
	if (t_strcmp(name, "vkGetImageSubresourceLayout") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetImageSubresourceLayout);
	}
	if (t_strcmp(name, "vkMapMemory") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkMapMemory);
	}
	if (t_strcmp(name, "vkUnmapMemory") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkUnmapMemory);
	}
	if (t_strcmp(name, "vkFlushMappedMemoryRanges") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkFlushMappedMemoryRanges);
	}
	if (t_strcmp(name, "vkInvalidateMappedMemoryRanges") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkInvalidateMappedMemoryRanges);
	}
	if (t_strcmp(name, "vkBeginCommandBuffer") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkBeginCommandBuffer);
	}
	if (t_strcmp(name, "vkEndCommandBuffer") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkEndCommandBuffer);
	}
	if (t_strcmp(name, "vkResetCommandBuffer") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkResetCommandBuffer);
	}
	if (t_strcmp(name, "vkCmdPipelineBarrier") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdPipelineBarrier);
	}
	if (t_strcmp(name, "vkCmdClearColorImage") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdClearColorImage);
	}
	if (t_strcmp(name, "vkCreateImageView") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateImageView);
	}
	if (t_strcmp(name, "vkDestroyImageView") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyImageView);
	}
	if (t_strcmp(name, "vkCreateShaderModule") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateShaderModule);
	}
	if (t_strcmp(name, "vkDestroyShaderModule") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyShaderModule);
	}
	if (t_strcmp(name, "vkCreateRenderPass") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateRenderPass);
	}
	if (t_strcmp(name, "vkDestroyRenderPass") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyRenderPass);
	}
	if (t_strcmp(name, "vkCreateFramebuffer") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateFramebuffer);
	}
	if (t_strcmp(name, "vkDestroyFramebuffer") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyFramebuffer);
	}
	if (t_strcmp(name, "vkCreatePipelineLayout") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreatePipelineLayout);
	}
	if (t_strcmp(name, "vkDestroyPipelineLayout") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyPipelineLayout);
	}
	if (t_strcmp(name, "vkCreateGraphicsPipelines") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateGraphicsPipelines);
	}
	if (t_strcmp(name, "vkDestroyPipeline") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyPipeline);
	}
	if (t_strcmp(name, "vkCmdBeginRenderPass") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdBeginRenderPass);
	}
	if (t_strcmp(name, "vkCmdEndRenderPass") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdEndRenderPass);
	}
	if (t_strcmp(name, "vkCmdBindPipeline") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdBindPipeline);
	}
	if (t_strcmp(name, "vkCmdBindVertexBuffers") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdBindVertexBuffers);
	}
	if (t_strcmp(name, "vkCmdBindIndexBuffer") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdBindIndexBuffer);
	}
	if (t_strcmp(name, "vkCmdSetViewport") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdSetViewport);
	}
	if (t_strcmp(name, "vkCmdSetScissor") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdSetScissor);
	}
	if (t_strcmp(name, "vkCmdDraw") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdDraw);
	}
	if (t_strcmp(name, "vkCmdDrawIndexed") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdDrawIndexed);
	}
	if (t_strcmp(name, "vkQueueSubmit") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkQueueSubmit);
	}
	if (t_strcmp(name, "vkCreateSwapchainKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateSwapchainKHR);
	}
	if (t_strcmp(name, "vkDestroySwapchainKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroySwapchainKHR);
	}
	if (t_strcmp(name, "vkGetSwapchainImagesKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetSwapchainImagesKHR);
	}
	if (t_strcmp(name, "vkAcquireNextImageKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkAcquireNextImageKHR);
	}
	if (t_strcmp(name, "vkQueuePresentKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkQueuePresentKHR);
	}
	return NULL;
}

static void *t_vkGetInstanceProcAddr(VkInstance instance, const char *name)
{
	(void)instance;
	if (t_vk_missing_instance_symbol && t_strcmp(name, "vkEnumeratePhysicalDevices") == 0) {
		return NULL;
	}
	if (t_vk_missing_instance_symbol_name != NULL && t_strcmp(name, t_vk_missing_instance_symbol_name) == 0) {
		return NULL;
	}
	if (t_vk_missing_create_instance && t_strcmp(name, "vkCreateInstance") == 0) {
		return NULL;
	}
	if (t_strcmp(name, "vkCreateInstance") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateInstance);
	}
	if (t_strcmp(name, "vkDestroyInstance") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyInstance);
	}
	if (t_strcmp(name, "vkEnumeratePhysicalDevices") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkEnumeratePhysicalDevices);
	}
	if (t_strcmp(name, "vkGetPhysicalDeviceQueueFamilyProperties") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetPhysicalDeviceQueueFamilyProperties);
	}
	if (t_strcmp(name, "vkGetPhysicalDeviceMemoryProperties") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetPhysicalDeviceMemoryProperties);
	}
	if (t_strcmp(name, "vkGetPhysicalDeviceFormatProperties") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetPhysicalDeviceFormatProperties);
	}
	if (t_strcmp(name, "vkGetPhysicalDeviceSurfaceSupportKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetPhysicalDeviceSurfaceSupportKHR);
	}
	if (t_strcmp(name, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
	}
	if (t_strcmp(name, "vkGetPhysicalDeviceSurfaceFormatsKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetPhysicalDeviceSurfaceFormatsKHR);
	}
	if (t_strcmp(name, "vkCreateDevice") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateDevice);
	}
	if (t_strcmp(name, "vkDestroyDevice") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyDevice);
	}
	if (t_strcmp(name, "vkGetDeviceProcAddr") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetDeviceProcAddr);
	}
	if (t_strcmp(name, "vkCreateXlibSurfaceKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateXlibSurfaceKHR);
	}
	return NULL;
}

static void t_gfx_vulkan_symbols(proc_t *proc)
{
	proc_setdlsym(proc,
		      STRV("libvulkan.so.1"),
		      STRV("vkGetInstanceProcAddr"),
		      t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetInstanceProcAddr));
	proc_setdlsym(proc,
		      STRV("libvulkan.so.1"),
		      STRV("vkEnumerateInstanceVersion"),
		      t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateInstance));
}

static void t_gfx_vulkan_fallback_symbols(proc_t *proc)
{
	proc_setdlsym(proc,
		      STRV("libvulkan.so"),
		      STRV("vkGetInstanceProcAddr"),
		      t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetInstanceProcAddr));
}

static void t_gfx_vulkan_windows_symbols(proc_t *proc)
{
	proc_setdlsym(proc,
		      STRV("vulkan-1.dll"),
		      STRV("vkGetInstanceProcAddr"),
		      t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetInstanceProcAddr));
}

static gfx_driver_t *t_gfx_vulkan_driver(void)
{
	return gfx_driver_find(STRV("vulkan"));
}

static int t_gfx_vulkan_init_gfx(gfx_t *gfx, proc_t *proc)
{
	t_vkReset();
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(proc);
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	return gfx_init(gfx, drv, &(gfx_config_t){0}, proc, ALLOC_STD) != gfx;
}

static int t_gfx_vulkan_compiler_init(void)
{
	if (t_gfx_vulkan_compiler_initialized) {
		return 0;
	}
	if (gfx_shader_compiler_init(&t_gfx_vulkan_compiler, ALLOC_STD) == NULL) {
		return 1;
	}
	t_gfx_vulkan_compiler_initialized = 1;
	return 0;
}

static void t_gfx_vulkan_compiler_free(void)
{
	if (!t_gfx_vulkan_compiler_initialized) {
		return;
	}
	gfx_shader_compiler_free(&t_gfx_vulkan_compiler);
	t_gfx_vulkan_compiler		  = (gfx_shader_compiler_t){0};
	t_gfx_vulkan_compiler_initialized = 0;
}

static int t_gfx_vulkan_shader(gfx_t *gfx, gfx_shader_t *shader, gfx_shader_stage_t stage)
{
	if (!t_gfx_vulkan_compiler_initialized) {
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
		.compiler = &t_gfx_vulkan_compiler,
		.source	  = strv_cstr(triangle_src),
		.stage	  = stage,
	};
	return gfx_shader_init(shader, gfx, &config) != shader;
}

static int t_gfx_vulkan_init_gfx_current(gfx_t *gfx, proc_t *proc)
{
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(proc);
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	return gfx_init(gfx, drv, &(gfx_config_t){0}, proc, ALLOC_STD) != gfx;
}

static int t_gfx_vulkan_init_surface_gfx(gfx_t *gfx, proc_t *proc)
{
	t_vkReset();
	static const char *const instance_extensions[] = {"VK_KHR_surface"};
	static const char *const device_extensions[]   = {"VK_KHR_swapchain"};

	gfx_plan_t plan = {
		.instance_extensions	  = instance_extensions,
		.instance_extension_count = 1,
		.device_extensions	  = device_extensions,
		.device_extension_count	  = 1,
	};
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(proc);
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	return gfx_init(gfx, drv, &(gfx_config_t){.plan = &plan}, proc, ALLOC_STD) != gfx;
}

static int t_gfx_vulkan_init_surface_gfx_current(gfx_t *gfx, proc_t *proc)
{
	static const char *const instance_extensions[] = {"VK_KHR_surface"};
	static const char *const device_extensions[]   = {"VK_KHR_swapchain"};

	gfx_plan_t plan = {
		.instance_extensions	  = instance_extensions,
		.instance_extension_count = 1,
		.device_extensions	  = device_extensions,
		.device_extension_count	  = 1,
	};
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(proc);
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	return gfx_init(gfx, drv, &(gfx_config_t){.plan = &plan}, proc, ALLOC_STD) != gfx;
}

static int t_gfx_vulkan_init_surface_gfx_without_device_extensions(gfx_t *gfx, proc_t *proc)
{
	t_vkReset();
	static const char *const instance_extensions[] = {"VK_KHR_surface"};

	gfx_plan_t plan = {
		.instance_extensions	  = instance_extensions,
		.instance_extension_count = 1,
	};
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(proc);
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	return gfx_init(gfx, drv, &(gfx_config_t){.plan = &plan}, proc, ALLOC_STD) != gfx;
}

static gfx_target_t *t_gfx_vulkan_init_swapchain_target(gfx_t *gfx, gfx_swapchain_t *swapchain, gfx_target_t *target, u16 width, u16 height)
{
	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_gfx_vulkan_surface,
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

TEST(gfx_vulkan_driver_is_registered)
{
	START;

	EXPECT_NOT_NULL(t_gfx_vulkan_driver());

	END;
}

TEST(gfx_vulkan_init_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_vulkan_init_null_proc)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(&gfx, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_vulkan_init_alloc_failure)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, (alloc_t){.alloc = t_gfx_vulkan_alloc_fail}));

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_missing_library)
{
	START;

	t_vkReset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_fallback_library)
{
	START;

	t_vkReset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_fallback_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_windows_library)
{
	START;

	t_vkReset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_windows_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_missing_instance_symbol)
{
	START;

	t_vkReset();
	t_vk_missing_instance_symbol = 1;
	proc_t proc		     = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_missing_device_symbol)
{
	START;

	t_vkReset();
	t_vk_missing_device_symbol = 1;
	proc_t proc		   = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_missing_lib_symbol)
{
	START;

	t_vkReset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	proc_setdlsym(&proc,
		      STRV("libvulkan.so.1"),
		      STRV("vkEnumerateInstanceVersion"),
		      t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateInstance));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_missing_create_instance)
{
	START;

	t_vkReset();
	t_vk_missing_create_instance = 1;
	gfx_t gfx		     = {0};
	proc_t proc		     = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_create_instance_failure)
{
	START;

	t_vkReset();
	t_vk_create_instance_ret = 1;
	gfx_t gfx		 = {0};
	proc_t proc		 = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_physical_device_count_failure)
{
	START;

	t_vkReset();
	t_vk_enumerate_physical_devices_count_ret = 1;
	gfx_t gfx				  = {0};
	proc_t proc				  = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_physical_device_list_failure)
{
	START;

	t_vkReset();
	t_vk_enumerate_physical_devices_ret = 1;
	gfx_t gfx			    = {0};
	proc_t proc			    = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_limits_physical_device_count)
{
	START;

	t_vkReset();
	t_vk_physical_device_count = 20;
	gfx_t gfx		   = {0};
	proc_t proc		   = {0};

	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_skips_device_without_queues)
{
	START;

	t_vkReset();
	t_vk_queue_count = 0;
	gfx_t gfx	 = {0};
	proc_t proc	 = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_limits_queue_count)
{
	START;

	t_vkReset();
	t_vk_queue_count = 40;
	gfx_t gfx	 = {0};
	proc_t proc	 = {0};

	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_skips_queue_without_transfer_feature)
{
	START;

	t_vkReset();
	t_vk_linear_features = 0;
	gfx_t gfx	     = {0};
	proc_t proc	     = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_adds_swapchain_to_existing_device_extensions)
{
	START;

	t_vkReset();
	static const char *const instance_extensions[] = {"VK_KHR_surface"};
	static const char *const device_extensions[]   = {"VK_EXT_test"};

	gfx_plan_t plan = {
		.instance_extensions	  = instance_extensions,
		.instance_extension_count = 1,
		.device_extensions	  = device_extensions,
		.device_extension_count	  = 1,
	};
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.plan = &plan}, &proc, ALLOC_STD), &gfx);
	EXPECT_EQ(t_vk_device_extension_count, 2);
	EXPECT_EQ(t_strcmp(t_vk_device_extensions[1], "VK_KHR_swapchain"), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_device_extension_alloc_failure)
{
	START;

	t_vkReset();
	t_gfx_vulkan_alloc_fail_at		       = 2;
	static const char *const instance_extensions[] = {"VK_KHR_surface"};
	static const char *const device_extensions[]   = {"VK_EXT_test"};

	gfx_plan_t plan = {
		.instance_extensions	  = instance_extensions,
		.instance_extension_count = 1,
		.device_extensions	  = device_extensions,
		.device_extension_count	  = 1,
	};
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	gfx_config_t config = {
		.plan = &plan,
	};
	EXPECT_NULL(gfx_init(&gfx,
			     drv,
			     &config,
			     &proc,
			     (alloc_t){.alloc = t_gfx_vulkan_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std}));

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_create_device_failure)
{
	START;

	t_vkReset();
	t_vk_create_device_ret = 1;
	gfx_t gfx	       = {0};
	proc_t proc	       = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_missing_swapchain_device_symbol)
{
	START;

	t_vkReset();
	t_vk_missing_device_symbol_name = "vkCreateSwapchainKHR";
	gfx_t gfx			= {0};
	proc_t proc			= {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_get_queue_failure)
{
	START;

	t_vkReset();
	t_vk_get_device_queue_null = 1;
	gfx_t gfx		   = {0};
	proc_t proc		   = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_create_command_pool_failure)
{
	START;

	t_vkReset();
	t_vk_create_command_pool_ret = 1;
	gfx_t gfx		     = {0};
	proc_t proc		     = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_allocate_command_buffer_failure)
{
	START;

	t_vkReset();
	t_vk_allocate_command_buffers_ret = 1;
	gfx_t gfx			  = {0};
	proc_t proc			  = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_create_fence_failure)
{
	START;

	t_vkReset();
	t_vk_create_fence_ret = 1;
	gfx_t gfx	      = {0};
	proc_t proc	      = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_success)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};

	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_uses_vulkan_1_0_api_version)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(t_vk_application_api_version, VK_API_VERSION_1_0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_uses_plan_extension_count)
{
	START;

	t_vkReset();
	static const char *const extensions[] = {"VK_KHR_surface", "VK_KHR_xlib_surface"};

	gfx_plan_t plan = {
		.instance_extensions	  = extensions,
		.instance_extension_count = 2,
	};
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.plan = &plan}, &proc, ALLOC_STD), &gfx);
	EXPECT_EQ(t_vk_instance_extension_count, 2);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_uses_plan_extensions)
{
	START;

	t_vkReset();
	static const char *const extensions[] = {"VK_KHR_surface", "VK_KHR_xlib_surface"};

	gfx_plan_t plan = {
		.instance_extensions	  = extensions,
		.instance_extension_count = 2,
	};
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.plan = &plan}, &proc, ALLOC_STD), &gfx);
	EXPECT_PTR(t_vk_instance_extensions, extensions);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_uses_plan_device_extension_count)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);

	EXPECT_EQ(t_vk_device_extension_count, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_uses_plan_device_extensions)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);

	EXPECT_EQ(t_strcmp(t_vk_device_extensions[0], "VK_KHR_swapchain"), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_surface_adds_swapchain_extension_count)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx_without_device_extensions(&gfx, &proc), 0);

	EXPECT_EQ(t_vk_device_extension_count, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_surface_adds_swapchain_extension_name)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx_without_device_extensions(&gfx, &proc), 0);

	EXPECT_EQ(t_strcmp(t_vk_device_extensions[0], "VK_KHR_swapchain"), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_creates_device)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(t_vk_create_device_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_gets_queue)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(t_vk_get_device_queue_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_proc_loads_symbol)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);

	void *sym = NULL;
	EXPECT_EQ(gfx_proc(&gfx, STRV("vkEnumerateInstanceVersion"), &sym), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_proc_sets_symbol)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);

	void *sym = NULL;
	gfx_proc(&gfx, STRV("vkEnumerateInstanceVersion"), &sym);
	EXPECT_EQ(t_gfx_vulkan_fn(sym), (t_gfx_vulkan_symbol_t)t_vkCreateInstance);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_proc_loads_instance_symbol)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);

	void *sym = NULL;
	EXPECT_EQ(gfx_proc(&gfx, STRV("vkCreateXlibSurfaceKHR"), &sym), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_native_sets_instance)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);

	gfx_native_t native = {0};
	gfx_native(&gfx, &native);

	EXPECT_EQ(native.instance, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_enables_swapchain_from_device_extension)
{
	START;

	t_vkReset();
	static const char *const device_extensions[] = {"VK_KHR_swapchain"};

	gfx_plan_t plan = {
		.device_extensions	= device_extensions,
		.device_extension_count = 1,
	};
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.plan = &plan}, &proc, ALLOC_STD), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_init_missing_surface_instance_symbol)
{
	START;

	t_vkReset();
	t_vk_missing_instance_symbol_name = "vkGetPhysicalDeviceSurfaceSupportKHR";
	gfx_t gfx			  = {0};
	proc_t proc			  = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx_current(&gfx, &proc), 1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->free(&gfx), 1);

	END;
}

TEST(gfx_vulkan_proc_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	void *sym = NULL;

	EXPECT_EQ(gfx.drv->proc(&gfx, STRV("vkCreateImage"), &sym), 1);

	END;
}

TEST(gfx_vulkan_proc_loads_device_symbol)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	void *sym = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("vkCreateImage"), &sym), 0);
	EXPECT_EQ(t_gfx_vulkan_fn(sym), (t_gfx_vulkan_symbol_t)t_vkCreateImage);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_proc_missing_symbol)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	void *sym = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("vkMissing"), &sym), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_proc_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	void *sym   = (void *)1;
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);

	log_set_quiet(0, 1);
	mem_oom(1);
	EXPECT_EQ(gfx_proc(&gfx, STRV("vkUnknown"), &sym), 1);
	mem_oom(0);
	log_set_quiet(0, 0);
	EXPECT_NULL(sym);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_native_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_native_t native = {0};

	EXPECT_EQ(gfx.drv->native(&gfx, &native), 1);

	END;
}

TEST(gfx_vulkan_set_target_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_target_t target = {
		.type = GFX_TARGET_NONE,
	};

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	END;
}

TEST(gfx_vulkan_clear_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}

TEST(gfx_vulkan_clear_color_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}

TEST(gfx_vulkan_viewport_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}
TEST(gfx_vulkan_draw_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->draw(NULL, 3, 0), 1);

	END;
}

TEST(gfx_vulkan_begin_null_frame)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}

TEST(gfx_vulkan_buffer_bind_null_frame)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->buffer_bind(NULL, NULL), 1);

	END;
}

TEST(gfx_vulkan_pipeline_bind_null_frame)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->pipeline_bind(NULL, NULL), 1);

	END;
}

TEST(gfx_vulkan_draw_inactive_frame)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->draw(&frame, 3, 0), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_draw_indexed_null_data)
{
	START;

	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->draw_indexed(NULL, 3), 1);
	EXPECT_EQ(drv->draw_indexed(&(gfx_frame_t){.gfx = &(gfx_t){0}}, 3), 1);

	END;
}

TEST(gfx_vulkan_draw_indexed_inactive_frame)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 3), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_unsupported_type)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_UNKNOWN}));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_free_null_data)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {.gfx = &gfx};

	gfx_buffer_free(&buffer);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_null_config)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, NULL));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_create_buffer_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_buffer_ret = 1;
	gfx_buffer_t buffer    = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));
	EXPECT_NULL(buffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_memory_type_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_type_bits = 0;
	gfx_buffer_t buffer   = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));
	EXPECT_NULL(buffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_allocate_memory_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_allocate_memory_ret = 1;
	gfx_buffer_t buffer	 = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));
	EXPECT_NULL(buffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_bind_memory_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_bind_buffer_memory_ret = 1;
	gfx_buffer_t buffer	    = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}));
	EXPECT_NULL(buffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_bind_index_uses_uint32_indices)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX}), &buffer);
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = (const gfx_pipeline_t *)&buffer,
		.active	  = 1,
	};
	gfx.frame = &frame;

	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(t_vk_bind_index_buffer_calls, 1);
	EXPECT_EQ(t_vk_bound_index_buffer != 0, 1);
	EXPECT_EQ(t_vk_bound_index_type, 1);

	gfx.frame = NULL;
	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_bind_rejects_unknown_type)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_buffer_data_t driver_buffer = {.buffer = 8};

	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.type = GFX_BUFFER_UNKNOWN,
		.data = &driver_buffer,
	};
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = (const gfx_pipeline_t *)&buffer,
		.active	  = 1,
	};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_set_data_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_buffer_t buffer	    = {.gfx = &gfx};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	END;
}

TEST(gfx_vulkan_buffer_set_data_map_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	gfx_vertex_2d_t vertices[3] = {0};
	t_vk_map_memory_ret	    = 1;

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_set_data_flushes_noncoherent_memory)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_flags   = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);
	EXPECT_EQ(t_vk_flush_mapped_memory_ranges_calls, 1);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_set_data_flush_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_flags   = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	gfx_vertex_2d_t vertices[3]	    = {0};
	t_vk_flush_mapped_memory_ranges_ret = 1;

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);
	EXPECT_EQ(t_vk_unmap_memory_calls, 1);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_shader_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_shader_t shader = {.gfx = &gfx};

	gfx_shader_free(&shader);

	END;
}

TEST(gfx_vulkan_shader_init_null_config)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, NULL));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_shader_init_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){0}));
	log_set_quiet(0, 0);

	END;
}

TEST(gfx_vulkan_shader_init_transpile_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	gfx_shader_config_t shader_config = {
		.compiler = &t_gfx_vulkan_compiler,
		.source	  = STRV("not shader source\n"),
		.stage	  = GFX_SHADER_STAGE_VERTEX,
	};
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, &shader_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_shader_init_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_shader(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_shader_init_create_shader_module_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_shader_module_ret = 1;
	gfx_shader_t shader	      = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_vulkan_shader(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_pipeline_t pipeline = {.gfx = &gfx};

	gfx_pipeline_free(&pipeline);

	END;
}

TEST(gfx_vulkan_pipeline_init_null_config)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_pipeline_t pipeline = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, NULL));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_pipeline_t pipeline = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){0}));

	END;
}

TEST(gfx_vulkan_memory_target_render_flow)
{
	START;

	u8 pixels[16] = {0};
	gfx_t gfx     = {0};
	proc_t proc   = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 2,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_vulkan_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_vulkan_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_pipeline_t pipeline		      = {0};
	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = t_gfx_vulkan_input_layout,
		.input_layout_size = sizeof(t_gfx_vulkan_input_layout),
	};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &pipeline_config), &pipeline);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &buffer);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 1.0f},
		{.x = 0.0f, .y = 0.0f, .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f},
		{.y = 2.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);
	gfx_frame_t frame	      = {0};
	gfx_pass_config_t pass_config = {
		.clear	  = {.r = 0.1f, .g = 0.2f, .b = 0.3f, .a = 0.4f},
		.viewport = {.x = 1, .y = 2, .width = 3, .height = 4},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 0);
	EXPECT_EQ(t_vk_begin_render_pass_calls, 1);
	EXPECT_EQ(t_vk_begin_render_pass_clear_value_count, 1);
	EXPECT_EQ(t_vk_bind_pipeline_calls, 1);
	EXPECT_EQ(t_vk_bind_vertex_buffers_calls, 1);
	EXPECT_EQ(t_vk_set_viewport_calls, 1);
	EXPECT_EQ(t_vk_set_scissor_calls, 1);
	EXPECT_EQ(t_vk_draw_calls, 1);
	EXPECT_EQ(t_vk_end_render_pass_calls, 1);
	EXPECT_EQ(t_vk_queue_submit_calls, 1);
	EXPECT_EQ(t_vk_invalidate_mapped_memory_ranges_calls, 0);
	EXPECT_EQ(t_vk_map_memory_calls, 2);
	EXPECT_EQ(t_vk_unmap_memory_calls, 2);

	gfx_buffer_free(&buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_target_draw_indexed_flow)
{
	START;

	u8 pixels[16] = {0};
	gfx_t gfx     = {0};
	proc_t proc   = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 2,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_vulkan_shader(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_vulkan_shader(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_pipeline_t pipeline		      = {0};
	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = t_gfx_vulkan_input_layout,
		.input_layout_size = sizeof(t_gfx_vulkan_input_layout),
	};
	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &pipeline_config), &pipeline);
	gfx_buffer_t vertex_buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&vertex_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX}), &vertex_buffer);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 1.0f},
		{.x = 0.0f, .y = 0.0f, .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f},
		{.y = 2.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(vertices)), 0);
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&index_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX}), &index_buffer);
	u32 indices[3] = {0, 1, 2};
	EXPECT_EQ(gfx_buffer_set_data(&index_buffer, indices, sizeof(indices)), 0);
	gfx_frame_t frame	      = {0};
	gfx_pass_config_t pass_config = {
		.clear	  = {.r = 0.1f, .g = 0.2f, .b = 0.3f, .a = 0.4f},
		.viewport = {.x = 1, .y = 2, .width = 3, .height = 4},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &vertex_buffer), 0);
	EXPECT_EQ(gfx_buffer_bind(&frame, &index_buffer), 0);
	EXPECT_EQ(gfx_draw_indexed(&frame, 3), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 0);
	EXPECT_EQ(t_vk_bind_vertex_buffers_calls, 1);
	EXPECT_EQ(t_vk_bind_index_buffer_calls, 1);
	EXPECT_EQ(t_vk_bound_index_type, 1);
	EXPECT_EQ(t_vk_draw_calls, 0);
	EXPECT_EQ(t_vk_draw_indexed_calls, 1);
	EXPECT_EQ(t_vk_draw_index_count, 3);
	EXPECT_EQ(t_vk_draw_instance_count, 1);
	EXPECT_EQ(t_vk_end_render_pass_calls, 1);
	EXPECT_EQ(t_vk_queue_submit_calls, 1);

	gfx_buffer_free(&index_buffer);
	gfx_buffer_free(&vertex_buffer);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_present_flow)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(gfx_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_vk_acquire_next_image_calls, 1);
	EXPECT_EQ(t_vk_queue_present_calls, 1);
	EXPECT_EQ(t_vk_present_image_index, 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_present_out_of_date_refreshes_framebuffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	t_vk_queue_present_ret	 = VK_ERROR_OUT_OF_DATE_KHR;
	t_vk_swapchain_images[0] = 20;
	t_vk_swapchain_images[1] = 21;
	EXPECT_EQ(gfx_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_vk_destroy_framebuffer_calls, 0);
	EXPECT_EQ(t_vk_create_framebuffer_calls, 2);

	t_vk_queue_present_ret = VK_SUCCESS;
	frame		       = (gfx_frame_t){0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(t_vk_destroy_framebuffer_calls, 2);
	EXPECT_EQ(t_vk_create_framebuffer_calls, 4);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(gfx_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_vk_acquire_next_image_calls, 2);
	EXPECT_EQ(t_vk_queue_present_calls, 2);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_pass_begin_refresh_capabilities_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_surface_capabilities_ret = 1;
	gfx_frame_t frame	      = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 1);
	log_set_quiet(0, 0);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_pass_begin_refresh_invalid_extent)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_surface_capabilities.currentExtent = (VkExtent2D){.width = 0, .height = 480};
	gfx_frame_t frame			= {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_pass_begin_refresh_changed_extent_recreates_framebuffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_surface_capabilities.currentExtent = (VkExtent2D){.width = 320, .height = 240};
	gfx_frame_t frame			= {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(target.width, 320);
	EXPECT_EQ(target.height, 240);
	EXPECT_EQ(swapchain.width, 320);
	EXPECT_EQ(swapchain.height, 240);
	EXPECT_EQ(framebuffer.width, 320);
	EXPECT_EQ(framebuffer.height, 240);
	EXPECT_EQ(t_vk_destroy_framebuffer_calls, 2);
	EXPECT_EQ(t_vk_create_framebuffer_calls, 4);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(gfx_swapchain_present(&swapchain), 0);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_pass_begin_reset_fences_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_reset_fences_ret = 1;
	gfx_frame_t frame     = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_pass_begin_command_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_begin_command_buffer_ret = 1;
	gfx_frame_t frame	      = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 2, .height = 1}}), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_pass_begin_acquire_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_acquire_next_image_ret = 1;
	gfx_frame_t frame	    = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_pass_begin_wait_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_wait_for_fences_ret = 1;
	gfx_frame_t frame	 = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_pass_begin_acquire_out_of_date_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_acquire_next_image_ret	  = VK_ERROR_OUT_OF_DATE_KHR;
	t_vk_surface_capabilities_fail_at = t_vk_get_surface_capabilities_calls + 2;
	gfx_frame_t frame		  = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 1);
	log_set_quiet(0, 0);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_pass_begin_acquire_exhausted)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_acquire_next_image_ret = VK_ERROR_OUT_OF_DATE_KHR;
	gfx_frame_t frame	    = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_pass_begin_acquire_invalid_index)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_vk_acquire_next_image_index = 8;
	gfx_frame_t frame	      = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_end_command_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 2, .height = 1}}), 0);
	t_vk_end_command_buffer_ret = 1;

	EXPECT_EQ(gfx_end(&frame), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_end_submit_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 2, .height = 1}}), 0);
	t_vk_queue_submit_ret = 1;

	EXPECT_EQ(gfx_end(&frame), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_end_finish_missing_target_data)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	void *target_data  = target.driver_data;
	target.driver_data = NULL;

	EXPECT_EQ(gfx_end(&frame), 1);

	target.driver_data = target_data;
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_draw_create_image_view_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_vk_create_image_view_ret    = 1;
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_draw_create_framebuffer_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_vk_create_framebuffer_ret   = 1;
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_draw_surface_framebuffer_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_vk_create_framebuffer_ret   = 1;
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_end_null_frame)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->end(NULL), 1);

	END;
}

TEST(gfx_vulkan_end_inactive_frame)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->end(&frame), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_present_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_vulkan_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->swapchain_present(&(gfx_swapchain_t){0}), 1);

	END;
}

TEST(gfx_vulkan_render_pass_init_rejects_unknown_format)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {.gfx = &gfx};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = (gfx_format_t)99,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_EQ(gfx.drv->render_pass_init(&render_pass, &render_pass_config), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_render_pass_init_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx.alloc		      = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_render_pass_t render_pass = {0};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_NULL(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_render_pass_init_create_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_render_pass_ret   = 1;
	gfx_render_pass_t render_pass = {0};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_NULL(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config));
	EXPECT_NULL(render_pass.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_render_pass_init_uses_load_load)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {0};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);

	gfx_render_pass_free(&render_pass);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_render_pass_init_uses_supported_formats)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	const gfx_format_t formats[] = {
		GFX_FORMAT_BGRA8_UNORM,
		GFX_FORMAT_RGBA8_SRGB,
		GFX_FORMAT_BGRA8_SRGB,
	};
	for (size_t i = 0; i < sizeof(formats) / sizeof(formats[0]); i++) {
		gfx_render_pass_t render_pass		    = {0};
		gfx_render_pass_config_t render_pass_config = {
			.color_format = formats[i],
			.load	      = GFX_LOAD_LOAD,
			.store	      = GFX_STORE_STORE,
		};
		EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
		gfx_render_pass_free(&render_pass);
	}

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_render_pass_init_uses_unknown_load_as_dont_care)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {.gfx = &gfx};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = (gfx_load_op_t)99,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_EQ(gfx.drv->render_pass_init(&render_pass, &render_pass_config), 0);

	gfx.drv->render_pass_free(&render_pass);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_target_init_rejects_zero_size_direct)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target = {
		.gfx	= &gfx,
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 0,
		.height = 1,
		.stride = 8,
	};

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_target_init_rejects_unknown_type_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target = {
		.gfx	= &gfx,
		.type	= (gfx_target_type_t)99,
		.format = GFX_FORMAT_RGBA8,
		.width	= 1,
		.height = 1,
	};

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx.alloc		  = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};

	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_init_rejects_invalid_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx.drv->swapchain_init(NULL, NULL), 1);
	EXPECT_EQ(gfx.drv->swapchain_init(&(gfx_swapchain_t){.gfx = &gfx}, NULL), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_without_wsi)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_surface_support_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_supported	  = 0;
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_capabilities_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities_ret = 1;
	gfx_swapchain_t swapchain     = {0};
	gfx_target_t target	      = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_usage_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	gfx_swapchain_t swapchain		      = {0};
	gfx_target_t target			      = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_format_count_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats_count_ret = 1;
	gfx_swapchain_t swapchain      = {0};
	gfx_target_t target	       = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_format_list_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats_ret  = 1;
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_limits_surface_formats)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_format_count = 20;
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_undefined_format)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats[0].format = 0;
	gfx_swapchain_t swapchain      = {0};
	gfx_target_t target	       = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_prefers_supported_format)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats[0].format = VK_FORMAT_B8G8R8A8_UNORM;
	gfx_swapchain_t swapchain      = {0};
	gfx_target_t target	       = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	EXPECT_EQ(target.format, GFX_FORMAT_BGRA8_UNORM);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_rejects_unsupported_format)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats[0].format = 999;
	gfx_swapchain_t swapchain      = {0};
	gfx_target_t target	       = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_clamps_extent_and_image_count)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.maxImageCount		= 1;
	t_vk_surface_capabilities.minImageExtent.width	= 10;
	t_vk_surface_capabilities.minImageExtent.height = 20;
	t_vk_surface_capabilities.maxImageExtent.width	= 100;
	t_vk_surface_capabilities.maxImageExtent.height = 200;
	gfx_swapchain_t swapchain			= {0};
	gfx_target_t target				= {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 1, 300), &target);
	EXPECT_EQ(t_vk_swapchain_create.minImageCount, 1);
	EXPECT_EQ(target.width, 10);
	EXPECT_EQ(target.height, 200);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_uses_current_extent)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.currentExtent = (VkExtent2D){.width = 320, .height = 240};
	gfx_swapchain_t swapchain		= {0};
	gfx_target_t target			= {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	EXPECT_EQ(target.width, 320);
	EXPECT_EQ(target.height, 240);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_chooses_nonopaque_alpha)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.supportedCompositeAlpha = 2;
	gfx_swapchain_t swapchain			  = {0};
	gfx_target_t target				  = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	EXPECT_EQ(t_vk_swapchain_create.compositeAlpha, 2);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_rejects_missing_alpha)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.supportedCompositeAlpha = 0;
	gfx_swapchain_t swapchain			  = {0};
	gfx_target_t target				  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_create_swapchain_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_create_swapchain_ret = 1;
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_swapchain_count_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_get_swapchain_images_count_ret = 1;
	gfx_swapchain_t swapchain	    = {0};
	gfx_target_t target		    = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_limits_swapchain_images)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_swapchain_image_count = 20;
	for (u32 i = 0; i < sizeof(t_vk_swapchain_images) / sizeof(t_vk_swapchain_images[0]); i++) {
		t_vk_swapchain_images[i] = 100 + i;
	}
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	EXPECT_EQ(((t_gfx_vulkan_swapchain_data_t *)target.driver_data)->swapchain_image_count, 8);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_target_init_swapchain_list_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_get_swapchain_images_ret = 1;
	gfx_swapchain_t swapchain     = {0};
	gfx_target_t target	      = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_resize_recreates_swapchain)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	t_vk_swapchain = 19;

	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 320, 240), 0);
	EXPECT_EQ(t_vk_device_wait_idle_calls, 1);
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_present_requires_acquired_swapchain)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);

	EXPECT_EQ(gfx_swapchain_present(&swapchain), 1);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_present_recreates_on_out_of_date)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	((t_gfx_vulkan_swapchain_data_t *)target.driver_data)->swapchain_acquired = 1;
	t_vk_queue_present_ret							  = VK_ERROR_OUT_OF_DATE_KHR;

	EXPECT_EQ(gfx_swapchain_present(&swapchain), 0);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_present_queue_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	((t_gfx_vulkan_swapchain_data_t *)target.driver_data)->swapchain_acquired = 1;
	t_vk_queue_present_ret							  = 1;

	EXPECT_EQ(gfx_swapchain_present(&swapchain), 1);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_target_init_alloc_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_target_t target = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_target_init_create_image_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_image_ret = 1;
	gfx_target_t target   = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_target_init_memory_type_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_type_bits = 0;
	gfx_target_t target   = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_target_init_allocate_memory_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_allocate_memory_ret = 1;
	gfx_target_t target	 = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_target_init_bind_memory_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_bind_image_memory_ret = 1;
	gfx_target_t target	   = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_target_init_row_pitch_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_row_pitch	    = 7;
	gfx_target_t target = {0};

	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_target_init_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_resize_null_swapchain_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->swapchain_resize(NULL, 1, 1), 1);

	END;
}

TEST(gfx_vulkan_target_free_null_target_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	drv->target_free(NULL);

	END;
}

TEST(gfx_vulkan_target_read_null_target_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->target_read(NULL, &(gfx_memory_readback_config_t){0}), 1);

	END;
}

TEST(gfx_vulkan_target_read_requires_bound_target)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);

	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 1);

	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_target_read_noncoherent_memory)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_flags				= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	((t_gfx_vulkan_data_head_t *)gfx.data)->target = &target;

	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 0);
	EXPECT_EQ(t_vk_invalidate_mapped_memory_ranges_calls, 1);

	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_target_read_invalidate_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_flags				= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	((t_gfx_vulkan_data_head_t *)gfx.data)->target = &target;
	t_vk_invalidate_mapped_memory_ranges_ret       = 1;

	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 1);

	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_target_read_map_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	((t_gfx_vulkan_data_head_t *)gfx.data)->target = &target;
	t_vk_map_memory_ret			       = 1;

	EXPECT_EQ(gfx_target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 1);

	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}
TEST(gfx_vulkan_framebuffer_init_null_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->framebuffer_init(NULL), 1);

	END;
}

TEST(gfx_vulkan_framebuffer_init_alloc_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx.alloc		      = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.width	     = target.width,
		.height	     = target.height,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_init_memory_requires_image)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	t_gfx_vulkan_memory_target_data_t *target_data = target.driver_data;
	target_data->image			       = 0;
	gfx_render_pass_t render_pass		       = {0};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_init_surface_requires_swapchain_image)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	t_gfx_vulkan_swapchain_data_t *target_data  = target.driver_data;
	target_data->swapchain_images[0]	    = 0;
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_init_surface_requires_swapchain_count)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	t_gfx_vulkan_swapchain_data_t *target_data  = target.driver_data;
	target_data->swapchain_image_count	    = 0;
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_init_surface_image_view_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_vk_create_image_view_ret    = 1;
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_init_unknown_target_type_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	int target_data					 = 0;
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = (gfx_target_type_t)99,
		.format	     = GFX_FORMAT_RGBA8,
		.driver_data = &target_data,
		.width	     = 1,
		.height	     = 1,
	};
	gfx_render_pass_t render_pass = {
		.gfx	      = &gfx,
		.color_format = target.format,
		.data	      = &render_pass_data,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.width	     = 1,
		.height	     = 1,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_pass_begin_requires_target_data_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	int framebuffer_data				 = 0;
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_target_t target = {
		.gfx	= &gfx,
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= &(u8[8]){0},
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	gfx_render_pass_t render_pass = {
		.gfx	      = &gfx,
		.color_format = target.format,
		.data	      = &render_pass_data,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &framebuffer_data,
		.width	     = 2,
		.height	     = 1,
	};
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_pass_begin_surface_requires_target_data_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	int framebuffer_data				 = 0;
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_swapchain_t swapchain = {
		.gfx	 = &gfx,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_gfx_vulkan_surface,
		.width	 = 640,
		.height	 = 480,
	};
	gfx_target_t target = {
		.gfx	   = &gfx,
		.type	   = GFX_TARGET_SWAPCHAIN,
		.format	   = GFX_FORMAT_RGBA8,
		.swapchain = &swapchain,
		.width	   = 640,
		.height	   = 480,
	};
	gfx_render_pass_t render_pass = {
		.gfx	      = &gfx,
		.color_format = target.format,
		.data	      = &render_pass_data,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &framebuffer_data,
		.width	     = 640,
		.height	     = 480,
	};
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_pass_begin_memory_requires_handles_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_memory_target_data_t target_data	 = {.memory = 8, .memory_size = 8};
	t_gfx_vulkan_framebuffer_data_t framebuffer_data = {.framebuffer = 30};
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_MEMORY,
		.format	     = GFX_FORMAT_RGBA8,
		.data	     = &(u8[8]){0},
		.driver_data = &target_data,
		.width	     = 2,
		.height	     = 1,
		.stride	     = 8,
	};
	gfx_render_pass_t render_pass = {
		.gfx	      = &gfx,
		.color_format = target.format,
		.data	      = &render_pass_data,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &framebuffer_data,
		.width	     = 2,
		.height	     = 1,
	};
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_pass_begin_surface_requires_framebuffer_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t target_data = {
		.swapchain		  = 9,
		.swapchain_images[1]	  = 11,
		.swapchain_image_views[1] = 21,
		.swapchain_image_count	  = 2,
		.swapchain_image_index	  = 1,
		.swapchain_acquired	  = 1,
	};
	t_gfx_vulkan_framebuffer_data_t framebuffer_data = {0};
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_swapchain_t swapchain = {
		.gfx	 = &gfx,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_gfx_vulkan_surface,
		.width	 = 640,
		.height	 = 480,
	};
	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_SWAPCHAIN,
		.format	     = GFX_FORMAT_RGBA8,
		.swapchain   = &swapchain,
		.driver_data = &target_data,
		.width	     = 640,
		.height	     = 480,
	};
	gfx_render_pass_t render_pass = {
		.gfx	      = &gfx,
		.color_format = target.format,
		.data	      = &render_pass_data,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &framebuffer_data,
		.width	     = 640,
		.height	     = 480,
	};
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_pass_begin_unknown_target_type_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_memory_target_data_t target_data	 = {0};
	t_gfx_vulkan_framebuffer_data_t framebuffer_data = {.framebuffer = 30};
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = (gfx_target_type_t)99,
		.format	     = GFX_FORMAT_RGBA8,
		.driver_data = &target_data,
		.width	     = 1,
		.height	     = 1,
	};
	gfx_render_pass_t render_pass = {
		.gfx	      = &gfx,
		.color_format = target.format,
		.data	      = &render_pass_data,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &framebuffer_data,
		.width	     = 1,
		.height	     = 1,
	};
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_pass_begin_rejects_active_frame)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_target_t target				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	gfx_frame_t frame = {.gfx = &gfx};
	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 0);
	gfx_frame_t second = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &second), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_rejects_invalid_config_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_pipeline_t pipeline = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &(gfx_pipeline_config_t){0}), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_alloc_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx.alloc = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	t_gfx_vulkan_shader_data_t vs_data	  = {.module = 1};
	t_gfx_vulkan_shader_data_t fs_data	  = {.module = 2};
	t_gfx_vulkan_render_pass_data_t pass_data = {.render_pass = 3};
	gfx_shader_t vs				  = {.gfx = &gfx, .data = &vs_data};
	gfx_shader_t fs				  = {.gfx = &gfx, .data = &fs_data};
	gfx_render_pass_t render_pass		  = {.gfx = &gfx, .data = &pass_data};
	gfx_pipeline_t pipeline			  = {.gfx = &gfx};

	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = t_gfx_vulkan_input_layout,
		.input_layout_size = sizeof(t_gfx_vulkan_input_layout),
	};
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_create_layout_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_pipeline_layout_ret		  = 1;
	t_gfx_vulkan_shader_data_t vs_data	  = {.module = 1};
	t_gfx_vulkan_shader_data_t fs_data	  = {.module = 2};
	t_gfx_vulkan_render_pass_data_t pass_data = {.render_pass = 3};
	gfx_shader_t vs				  = {.gfx = &gfx, .data = &vs_data};
	gfx_shader_t fs				  = {.gfx = &gfx, .data = &fs_data};
	gfx_render_pass_t render_pass		  = {.gfx = &gfx, .data = &pass_data};
	gfx_pipeline_t pipeline			  = {.gfx = &gfx};

	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = t_gfx_vulkan_input_layout,
		.input_layout_size = sizeof(t_gfx_vulkan_input_layout),
	};
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);
	EXPECT_NULL(pipeline.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_attribute_alloc_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_alloc_count   = 0;
	t_gfx_vulkan_alloc_fail_at = 2;
	gfx.alloc		   = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std};
	t_gfx_vulkan_shader_data_t vs_data	  = {.module = 1};
	t_gfx_vulkan_shader_data_t fs_data	  = {.module = 2};
	t_gfx_vulkan_render_pass_data_t pass_data = {.render_pass = 3};
	gfx_shader_t vs				  = {.gfx = &gfx, .data = &vs_data};
	gfx_shader_t fs				  = {.gfx = &gfx, .data = &fs_data};
	gfx_render_pass_t render_pass		  = {.gfx = &gfx, .data = &pass_data};
	gfx_pipeline_t pipeline			  = {.gfx = &gfx};

	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = t_gfx_vulkan_input_layout,
		.input_layout_size = sizeof(t_gfx_vulkan_input_layout),
	};
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);
	EXPECT_NULL(pipeline.data);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_unsupported_layout_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_shader_data_t vs_data	  = {.module = 1};
	t_gfx_vulkan_shader_data_t fs_data	  = {.module = 2};
	t_gfx_vulkan_render_pass_data_t pass_data = {.render_pass = 3};
	gfx_shader_t vs				  = {.gfx = &gfx, .data = &vs_data};
	gfx_shader_t fs				  = {.gfx = &gfx, .data = &fs_data};
	gfx_render_pass_t render_pass		  = {.gfx = &gfx, .data = &pass_data};

	const gfx_layout_t layout[] = {
		{.index = 0, .semantic = "POSITION", .count = 3, .type = GFX_VALUE_FLOAT32},
	};
	gfx_pipeline_t pipeline = {.gfx = &gfx};

	log_set_quiet(0, 1);
	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = layout,
		.input_layout_size = sizeof(layout),
	};
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);
	log_set_quiet(0, 0);
	EXPECT_NULL(pipeline.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_create_pipeline_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_graphics_pipelines_ret	  = 1;
	t_gfx_vulkan_shader_data_t vs_data	  = {.module = 1};
	t_gfx_vulkan_shader_data_t fs_data	  = {.module = 2};
	t_gfx_vulkan_render_pass_data_t pass_data = {.render_pass = 3};
	gfx_shader_t vs				  = {.gfx = &gfx, .data = &vs_data};
	gfx_shader_t fs				  = {.gfx = &gfx, .data = &fs_data};
	gfx_render_pass_t render_pass		  = {.gfx = &gfx, .data = &pass_data};
	gfx_pipeline_t pipeline			  = {.gfx = &gfx};

	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = t_gfx_vulkan_input_layout,
		.input_layout_size = sizeof(t_gfx_vulkan_input_layout),
	};
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);
	EXPECT_NULL(pipeline.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}
STEST(gfx_vulkan)
{
	SSTART;

	(void)t_gfx_vulkan_compiler_init();

	RUN(gfx_vulkan_driver_is_registered);
	RUN(gfx_vulkan_init_null_gfx);
	RUN(gfx_vulkan_init_null_proc);
	RUN(gfx_vulkan_init_alloc_failure);
	RUN(gfx_vulkan_init_missing_library);
	RUN(gfx_vulkan_init_fallback_library);
	RUN(gfx_vulkan_init_windows_library);
	RUN(gfx_vulkan_init_missing_instance_symbol);
	RUN(gfx_vulkan_init_missing_device_symbol);
	RUN(gfx_vulkan_init_missing_lib_symbol);
	RUN(gfx_vulkan_init_missing_create_instance);
	RUN(gfx_vulkan_init_create_instance_failure);
	RUN(gfx_vulkan_init_physical_device_count_failure);
	RUN(gfx_vulkan_init_physical_device_list_failure);
	RUN(gfx_vulkan_init_limits_physical_device_count);
	RUN(gfx_vulkan_init_skips_device_without_queues);
	RUN(gfx_vulkan_init_limits_queue_count);
	RUN(gfx_vulkan_init_skips_queue_without_transfer_feature);
	RUN(gfx_vulkan_init_adds_swapchain_to_existing_device_extensions);
	RUN(gfx_vulkan_init_device_extension_alloc_failure);
	RUN(gfx_vulkan_init_create_device_failure);
	RUN(gfx_vulkan_init_missing_swapchain_device_symbol);
	RUN(gfx_vulkan_init_get_queue_failure);
	RUN(gfx_vulkan_init_create_command_pool_failure);
	RUN(gfx_vulkan_init_allocate_command_buffer_failure);
	RUN(gfx_vulkan_init_create_fence_failure);
	RUN(gfx_vulkan_init_success);
	RUN(gfx_vulkan_init_uses_vulkan_1_0_api_version);
	RUN(gfx_vulkan_init_uses_plan_extension_count);
	RUN(gfx_vulkan_init_uses_plan_extensions);
	RUN(gfx_vulkan_init_uses_plan_device_extension_count);
	RUN(gfx_vulkan_init_uses_plan_device_extensions);
	RUN(gfx_vulkan_init_surface_adds_swapchain_extension_count);
	RUN(gfx_vulkan_init_surface_adds_swapchain_extension_name);
	RUN(gfx_vulkan_init_creates_device);
	RUN(gfx_vulkan_init_gets_queue);
	RUN(gfx_vulkan_proc_loads_symbol);
	RUN(gfx_vulkan_proc_sets_symbol);
	RUN(gfx_vulkan_proc_loads_instance_symbol);
	RUN(gfx_vulkan_native_sets_instance);
	RUN(gfx_vulkan_init_enables_swapchain_from_device_extension);
	RUN(gfx_vulkan_init_missing_surface_instance_symbol);
	RUN(gfx_vulkan_free_null_data);
	RUN(gfx_vulkan_proc_null_data);
	RUN(gfx_vulkan_proc_loads_device_symbol);
	RUN(gfx_vulkan_proc_missing_symbol);
	RUN(gfx_vulkan_proc_alloc_failure);
	RUN(gfx_vulkan_native_null_data);
	RUN(gfx_vulkan_set_target_null_data);
	RUN(gfx_vulkan_clear_null_data);
	RUN(gfx_vulkan_clear_color_null_data);
	RUN(gfx_vulkan_viewport_null_data);
	RUN(gfx_vulkan_draw_null_data);
	RUN(gfx_vulkan_begin_null_frame);
	RUN(gfx_vulkan_buffer_bind_null_frame);
	RUN(gfx_vulkan_pipeline_bind_null_frame);
	RUN(gfx_vulkan_draw_inactive_frame);
	RUN(gfx_vulkan_draw_indexed_null_data);
	RUN(gfx_vulkan_draw_indexed_inactive_frame);
	RUN(gfx_vulkan_buffer_init_unsupported_type);
	RUN(gfx_vulkan_buffer_init_alloc_failure);
	RUN(gfx_vulkan_buffer_free_null_data);
	RUN(gfx_vulkan_buffer_init_null_config);
	RUN(gfx_vulkan_buffer_init_create_buffer_failure);
	RUN(gfx_vulkan_buffer_init_memory_type_failure);
	RUN(gfx_vulkan_buffer_init_allocate_memory_failure);
	RUN(gfx_vulkan_buffer_init_bind_memory_failure);
	RUN(gfx_vulkan_buffer_bind_index_uses_uint32_indices);
	RUN(gfx_vulkan_buffer_bind_rejects_unknown_type);
	RUN(gfx_vulkan_buffer_set_data_null_data);
	RUN(gfx_vulkan_buffer_set_data_map_failure);
	RUN(gfx_vulkan_buffer_set_data_flushes_noncoherent_memory);
	RUN(gfx_vulkan_buffer_set_data_flush_failure);
	RUN(gfx_vulkan_shader_free_null_data);
	RUN(gfx_vulkan_shader_init_null_config);
	RUN(gfx_vulkan_shader_init_null_data);
	RUN(gfx_vulkan_shader_init_transpile_failure);
	RUN(gfx_vulkan_shader_init_alloc_failure);
	RUN(gfx_vulkan_shader_init_create_shader_module_failure);
	RUN(gfx_vulkan_pipeline_free_null_data);
	RUN(gfx_vulkan_pipeline_init_null_config);
	RUN(gfx_vulkan_pipeline_init_null_data);
	RUN(gfx_vulkan_memory_target_render_flow);
	RUN(gfx_vulkan_memory_target_draw_indexed_flow);
	RUN(gfx_vulkan_swapchain_present_flow);
	RUN(gfx_vulkan_swapchain_present_out_of_date_refreshes_framebuffer);
	RUN(gfx_vulkan_surface_pass_begin_refresh_capabilities_failure);
	RUN(gfx_vulkan_surface_pass_begin_refresh_invalid_extent);
	RUN(gfx_vulkan_surface_pass_begin_refresh_changed_extent_recreates_framebuffer);
	RUN(gfx_vulkan_surface_pass_begin_reset_fences_failure);
	RUN(gfx_vulkan_memory_pass_begin_command_failure);
	RUN(gfx_vulkan_surface_pass_begin_acquire_failure);
	RUN(gfx_vulkan_surface_pass_begin_wait_failure);
	RUN(gfx_vulkan_surface_pass_begin_acquire_out_of_date_failure);
	RUN(gfx_vulkan_surface_pass_begin_acquire_exhausted);
	RUN(gfx_vulkan_surface_pass_begin_acquire_invalid_index);
	RUN(gfx_vulkan_end_command_failure);
	RUN(gfx_vulkan_end_submit_failure);
	RUN(gfx_vulkan_end_finish_missing_target_data);
	RUN(gfx_vulkan_draw_create_image_view_failure);
	RUN(gfx_vulkan_draw_create_framebuffer_failure);
	RUN(gfx_vulkan_draw_surface_framebuffer_failure);
	RUN(gfx_vulkan_end_null_frame);
	RUN(gfx_vulkan_end_inactive_frame);
	RUN(gfx_vulkan_present_null_data);
	RUN(gfx_vulkan_render_pass_init_rejects_unknown_format);
	RUN(gfx_vulkan_render_pass_init_alloc_failure);
	RUN(gfx_vulkan_render_pass_init_create_failure);
	RUN(gfx_vulkan_render_pass_init_uses_load_load);
	RUN(gfx_vulkan_render_pass_init_uses_supported_formats);
	RUN(gfx_vulkan_render_pass_init_uses_unknown_load_as_dont_care);
	RUN(gfx_vulkan_target_init_rejects_zero_size_direct);
	RUN(gfx_vulkan_target_init_rejects_unknown_type_direct);
	RUN(gfx_vulkan_surface_target_init_alloc_failure);
	RUN(gfx_vulkan_swapchain_init_rejects_invalid_direct);
	RUN(gfx_vulkan_surface_target_init_without_wsi);
	RUN(gfx_vulkan_surface_target_init_surface_support_failure);
	RUN(gfx_vulkan_surface_target_init_capabilities_failure);
	RUN(gfx_vulkan_surface_target_init_usage_failure);
	RUN(gfx_vulkan_surface_target_init_format_count_failure);
	RUN(gfx_vulkan_surface_target_init_format_list_failure);
	RUN(gfx_vulkan_surface_target_init_limits_surface_formats);
	RUN(gfx_vulkan_surface_target_init_undefined_format);
	RUN(gfx_vulkan_surface_target_init_prefers_supported_format);
	RUN(gfx_vulkan_surface_target_init_rejects_unsupported_format);
	RUN(gfx_vulkan_surface_target_init_clamps_extent_and_image_count);
	RUN(gfx_vulkan_surface_target_init_uses_current_extent);
	RUN(gfx_vulkan_surface_target_init_chooses_nonopaque_alpha);
	RUN(gfx_vulkan_surface_target_init_rejects_missing_alpha);
	RUN(gfx_vulkan_surface_target_init_create_swapchain_failure);
	RUN(gfx_vulkan_surface_target_init_swapchain_count_failure);
	RUN(gfx_vulkan_surface_target_init_limits_swapchain_images);
	RUN(gfx_vulkan_surface_target_init_swapchain_list_failure);
	RUN(gfx_vulkan_swapchain_resize_recreates_swapchain);
	RUN(gfx_vulkan_swapchain_present_requires_acquired_swapchain);
	RUN(gfx_vulkan_swapchain_present_recreates_on_out_of_date);
	RUN(gfx_vulkan_swapchain_present_queue_failure);
	RUN(gfx_vulkan_memory_target_init_alloc_failure);
	RUN(gfx_vulkan_memory_target_init_create_image_failure);
	RUN(gfx_vulkan_memory_target_init_memory_type_failure);
	RUN(gfx_vulkan_memory_target_init_allocate_memory_failure);
	RUN(gfx_vulkan_memory_target_init_bind_memory_failure);
	RUN(gfx_vulkan_memory_target_init_row_pitch_failure);
	RUN(gfx_vulkan_swapchain_resize_null_swapchain_direct);
	RUN(gfx_vulkan_target_free_null_target_direct);
	RUN(gfx_vulkan_target_read_null_target_direct);
	RUN(gfx_vulkan_target_read_requires_bound_target);
	RUN(gfx_vulkan_target_read_noncoherent_memory);
	RUN(gfx_vulkan_target_read_invalidate_failure);
	RUN(gfx_vulkan_target_read_map_failure);
	RUN(gfx_vulkan_framebuffer_init_null_direct);
	RUN(gfx_vulkan_framebuffer_init_alloc_failure);
	RUN(gfx_vulkan_framebuffer_init_memory_requires_image);
	RUN(gfx_vulkan_framebuffer_init_surface_requires_swapchain_image);
	RUN(gfx_vulkan_framebuffer_init_surface_requires_swapchain_count);
	RUN(gfx_vulkan_framebuffer_init_surface_image_view_failure);
	RUN(gfx_vulkan_framebuffer_init_unknown_target_type_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_requires_target_data_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_surface_requires_target_data_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_memory_requires_handles_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_surface_requires_framebuffer_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_unknown_target_type_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_rejects_active_frame);
	RUN(gfx_vulkan_pipeline_init_rejects_invalid_config_direct);
	RUN(gfx_vulkan_pipeline_init_alloc_failure_direct);
	RUN(gfx_vulkan_pipeline_init_create_layout_failure_direct);
	RUN(gfx_vulkan_pipeline_init_attribute_alloc_failure_direct);
	RUN(gfx_vulkan_pipeline_init_unsupported_layout_direct);
	RUN(gfx_vulkan_pipeline_init_create_pipeline_failure_direct);

	t_gfx_vulkan_compiler_free();

	SEND;
}
