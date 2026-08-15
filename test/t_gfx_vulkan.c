#include "gfx_driver.h"

#include "log.h"
#include "mem.h"
#include "test.h"
#include "vulkan.h"

typedef void (*t_gfx_vulkan_symbol_t)(void);

enum {
	T_GFX_VULKAN_MAX_UNIFORM_BINDINGS = 16,
	T_GFX_VULKAN_MAX_DESCRIPTOR_SETS  = 64,
};

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
static int t_vk_copy_image_to_buffer_calls;
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
static int t_vk_create_semaphore_calls;
static int t_vk_destroy_semaphore_calls;
static int t_vk_create_xlib_surface_calls;
static int t_vk_get_surface_support_calls;
static int t_vk_get_surface_capabilities_calls;
static int t_vk_get_surface_formats_calls;
static int t_vk_get_surface_present_modes_calls;
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
static u32 t_vk_render_pass_attachment_count;
static u32 t_vk_render_pass_depth_format;
static u32 t_vk_render_pass_depth_load;
static u32 t_vk_render_pass_depth_store;
static u32 t_vk_framebuffer_attachment_count;
static int t_vk_create_descriptor_set_layout_calls;
static int t_vk_destroy_descriptor_set_layout_calls;
static int t_vk_create_descriptor_pool_calls;
static int t_vk_destroy_descriptor_pool_calls;
static int t_vk_allocate_descriptor_sets_calls;
static int t_vk_update_descriptor_sets_calls;
static int t_vk_bind_descriptor_sets_calls;
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
static VkFlags t_vk_optimal_features;
static VkFlags t_vk_memory_flags;
static VkBool32 t_vk_fill_mode_non_solid;
static u32 t_vk_memory_type_bits;
static VkDeviceSize t_vk_row_pitch;
static u8 t_vk_memory[256];
static float t_vk_clear_color[4];
static u32 t_vk_clear_layout;
static VkFramebuffer t_vk_begin_render_pass_framebuffer;
static float t_vk_begin_render_pass_depth;
static VkImage t_vk_destroyed_image;
static VkDeviceMemory t_vk_freed_memory;
static VkMappedMemoryRange t_vk_invalidate_range;
static VkMappedMemoryRange t_vk_flush_range;
static VkImageMemoryBarrier t_vk_last_barrier;
static VkImageMemoryBarrier t_vk_barriers[2];
static VkDeviceSize t_vk_buffer_size;
static VkFlags t_vk_buffer_usage;
static VkBuffer t_vk_bound_vertex_buffer;
static VkBuffer t_vk_bound_index_buffer;
static u32 t_vk_bound_index_type;
static u32 t_vk_descriptor_binding_count;
static u32 t_vk_descriptor_pool_size_count;
static u32 t_vk_descriptor_pool_descriptor_count;
static u32 t_vk_descriptor_set_count;
static u32 t_vk_descriptor_write_binding;
static VkDescriptorSet t_vk_descriptor_write_set;
static VkBuffer t_vk_descriptor_write_buffer;
static VkDeviceSize t_vk_descriptor_write_range;
static VkPipelineLayout t_vk_bound_descriptor_layout;
static VkDescriptorSet t_vk_bound_descriptor_set;
static u32 t_vk_draw_vertex_count;
static u32 t_vk_draw_instance_count;
static u32 t_vk_draw_index_count;
static int t_vk_vertex_first_x;
static int t_vk_vertex_last_y;
static gfx_image_t t_gfx_vulkan_image;

static gfx_image_t *t_gfx_vulkan_image_init_image_memory(gfx_image_t *target, gfx_t *gfx, const gfx_image_memory_config_t *config)
{
	(void)t_gfx_vulkan_image;
	return gfx_image_init_memory(target, gfx, config);
}

static gfx_image_t *t_gfx_vulkan_image_init_image_swapchain(gfx_image_t *target, gfx_swapchain_t *swapchain)
{
	if (target == NULL || swapchain == NULL || swapchain->images == NULL) {
		return NULL;
	}
	*target = swapchain->images[0];
	return target;
}

static int t_gfx_vulkan_swapchain_present(gfx_swapchain_t *swapchain)
{
	gfx_swapchain_image_t image = {0};
	return gfx_swapchain_acquire(swapchain, &image) || gfx_swapchain_present(swapchain, &image);
}
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
static int t_vk_create_semaphore_ret;
static int t_vk_create_image_ret;
static int t_vk_allocate_memory_ret;
static int t_vk_allocate_memory_fail_at;
static int t_vk_bind_image_memory_ret;
static int t_vk_create_buffer_ret;
static int t_vk_bind_buffer_memory_ret;
static int t_vk_create_image_view_ret;
static int t_vk_create_shader_module_ret;
static int t_vk_create_render_pass_ret;
static int t_vk_create_framebuffer_ret;
static int t_vk_create_descriptor_set_layout_ret;
static int t_vk_create_descriptor_pool_ret;
static int t_vk_allocate_descriptor_sets_ret;
static int t_vk_create_pipeline_layout_ret;
static int t_vk_create_graphics_pipelines_ret;
static int t_vk_map_memory_ret;
static int t_vk_flush_mapped_memory_ranges_ret;
static int t_vk_invalidate_mapped_memory_ranges_ret;
static int t_vk_reset_fences_ret;
static int t_vk_wait_for_fences_ret;
static VkFence t_vk_wait_for_fences_fence;
static VkFence t_vk_wait_for_fences_fail_fence;
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
static int t_vk_surface_present_modes_count_ret;
static int t_vk_surface_present_modes_ret;
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
static VkPresentModeKHR t_vk_surface_present_modes[20];
static u32 t_vk_surface_present_mode_count;
static VkImage t_vk_swapchain_images[20];
static u32 t_vk_present_image_index;
static gfx_shader_compiler_t t_gfx_vulkan_compiler;
static int t_gfx_vulkan_compiler_initialized;

typedef struct t_gfx_vulkan_memory_target_data_s {
	VkImage image;
	VkDeviceMemory image_memory;
	struct {
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDeviceSize size;
		VkDeviceSize memory_size;
		void *mapped;
		int memory_coherent;
	} readback;
	VkImageView image_view;
} t_gfx_vulkan_memory_target_data_t;

typedef struct t_gfx_vulkan_swapchain_image_data_s {
	VkImage image;
	VkImageView view;
	VkImageLayout layout;
	VkFence in_flight;
} t_gfx_vulkan_swapchain_image_data_t;

typedef struct t_gfx_vulkan_swapchain_data_s {
	VkSwapchainKHR swapchain;
	u32 image_count;
	u32 image_index;
	u32 present_frame;
	int acquired;
	int present_pending;
} t_gfx_vulkan_swapchain_data_t;

typedef struct t_gfx_vulkan_render_pass_data_s {
	VkRenderPass render_pass;
	int depth;
} t_gfx_vulkan_render_pass_data_t;

typedef struct t_gfx_vulkan_frame_sync_s {
	VkCommandBuffer command_buffer;
	VkFence fence;
	VkSemaphore image_available;
	VkSemaphore render_finished;
} t_gfx_vulkan_frame_sync_t;

typedef struct t_gfx_vulkan_frame_s {
	VkImage image;
	VkImage depth_image;
	VkImageView *view;
	VkFramebuffer *framebuffer;
	VkImageLayout *depth_layout;
	u32 image_index;
	VkImageLayout old_layout;
	VkImageLayout final_layout;
	int active;
	int surface;
} t_gfx_vulkan_frame_t;

typedef struct t_gfx_vulkan_data_head_s {
	void *lib;
	gfx_image_t *image;
	gfx_swapchain_t *swapchain;
} t_gfx_vulkan_data_head_t;

typedef struct t_gfx_vulkan_data_s {
	void *lib;
	gfx_image_t *image;
	gfx_swapchain_t *swapchain;
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice device;
	VkQueue queue;
	u32 queue_family;
	VkCommandPool command_pool;
	t_gfx_vulkan_frame_sync_t frames[3];
	u32 frame_index;
	u32 active_frame;
	int frame_sync_active;
	VkClearValue clear_color;
	float clear_depth;
	t_gfx_vulkan_frame_t frame;
} t_gfx_vulkan_data_t;

typedef struct t_gfx_vulkan_framebuffer_data_s {
	VkFramebuffer framebuffer;
	VkFramebuffer *swapchain_framebuffers;
	VkImage depth_image;
	VkDeviceMemory depth_memory;
	VkImageView depth_view;
	VkImageLayout depth_layout;
	u32 swapchain_framebuffer_count;
} t_gfx_vulkan_framebuffer_data_t;

typedef struct t_gfx_vulkan_buffer_data_s {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize size;
	VkDeviceSize memory_size;
	void *mapped;
	int memory_coherent;
	gfx_buffer_type_t type;
} t_gfx_vulkan_buffer_data_t;

typedef struct t_gfx_vulkan_shader_data_s {
	VkShaderModule module;
} t_gfx_vulkan_shader_data_t;

typedef struct t_gfx_vulkan_descriptor_binding_s {
	VkBuffer buffer;
	VkDeviceSize range;
} t_gfx_vulkan_descriptor_binding_t;

typedef struct t_gfx_vulkan_pipeline_data_s {
	VkDescriptorSetLayout descriptor_set_layout;
	VkDescriptorPool descriptor_pool;
	VkDescriptorSet descriptor_sets[3];
	t_gfx_vulkan_descriptor_binding_t descriptor_bindings[3][T_GFX_VULKAN_MAX_UNIFORM_BINDINGS];
	u32 descriptor_set_index;
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

static void t_vkGetPhysicalDeviceFeatures(VkPhysicalDevice device, VkPhysicalDeviceFeatures *features)
{
	(void)device;
	*features = (VkPhysicalDeviceFeatures){
		.fillModeNonSolid = t_vk_fill_mode_non_solid,
	};
}

static void t_vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice device, u32 format, VkFormatProperties *props)
{
	(void)device;
	(void)format;
	props->optimalTilingFeatures = t_vk_optimal_features;
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
	t_vk_allocate_command_buffers_calls++;
	const VkCommandBufferAllocateInfo *info = alloc_info;
	u32 count				= info != NULL ? info->commandBufferCount : 1;
	for (u32 i = 0; i < count; i++) {
		buffer[i] = 5 + i;
	}
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

static int t_vkCreateSemaphore(VkDevice device, const void *create, const void *alloc, VkSemaphore *semaphore)
{
	(void)device;
	(void)create;
	(void)alloc;
	t_vk_create_semaphore_calls++;
	*semaphore = 20 + t_vk_create_semaphore_calls;
	return t_vk_create_semaphore_ret;
}

static void t_vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const void *alloc)
{
	(void)device;
	(void)semaphore;
	(void)alloc;
	t_vk_destroy_semaphore_calls++;
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
	(void)all;
	(void)timeout;
	t_vk_wait_for_fences_calls++;
	t_vk_wait_for_fences_fence = count > 0 ? fences[0] : 0;
	if (count > 0 && t_vk_wait_for_fences_fail_fence != 0 && fences[0] == t_vk_wait_for_fences_fail_fence) {
		return 1;
	}
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
	(void)alloc;
	const VkBufferCreateInfo *info = create;
	t_vk_create_buffer_calls++;
	t_vk_buffer_size  = info->size;
	t_vk_buffer_usage = info->usage;
	*buffer		  = 12;
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
	if (t_vk_allocate_memory_fail_at != 0 && t_vk_allocate_memory_calls == t_vk_allocate_memory_fail_at) {
		return 1;
	}
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
		if (t_vk_pipeline_barrier_calls <= 2) {
			t_vk_barriers[t_vk_pipeline_barrier_calls - 1] = image_barriers[0];
		}
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

static void t_vkCmdCopyImageToBuffer(VkCommandBuffer command_buffer, VkImage image, VkImageLayout layout, VkBuffer buffer, u32 region_count,
				     const VkBufferImageCopy *regions)
{
	(void)command_buffer;
	(void)image;
	(void)layout;
	(void)buffer;
	(void)region_count;
	(void)regions;
	t_vk_copy_image_to_buffer_calls++;
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
	(void)alloc;
	const VkRenderPassCreateInfo *info = create;
	t_vk_create_render_pass_calls++;
	t_vk_render_pass_attachment_count = info != NULL ? info->attachmentCount : 0;
	if (info != NULL && info->attachmentCount > 1 && info->pAttachments != NULL) {
		t_vk_render_pass_depth_format = info->pAttachments[1].format;
		t_vk_render_pass_depth_load   = info->pAttachments[1].loadOp;
		t_vk_render_pass_depth_store  = info->pAttachments[1].storeOp;
	}
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
	(void)alloc;
	const VkFramebufferCreateInfo *info = create;
	t_vk_create_framebuffer_calls++;
	t_vk_framebuffer_attachment_count = info != NULL ? info->attachmentCount : 0;
	*framebuffer			  = 31 + (VkFramebuffer)t_vk_create_framebuffer_calls;
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

static int t_vkCreateDescriptorSetLayout(VkDevice device, const void *create, const void *alloc, VkDescriptorSetLayout *layout)
{
	(void)device;
	(void)alloc;
	const VkDescriptorSetLayoutCreateInfo *info = create;
	t_vk_create_descriptor_set_layout_calls++;
	t_vk_descriptor_binding_count = info != NULL ? info->bindingCount : 0;
	if (t_vk_create_descriptor_set_layout_ret != VK_SUCCESS) {
		return t_vk_create_descriptor_set_layout_ret;
	}
	*layout = 42;
	return 0;
}

static void t_vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout, const void *alloc)
{
	(void)device;
	(void)layout;
	(void)alloc;
	t_vk_destroy_descriptor_set_layout_calls++;
}

static int t_vkCreateDescriptorPool(VkDevice device, const void *create, const void *alloc, VkDescriptorPool *pool)
{
	(void)device;
	(void)alloc;
	const VkDescriptorPoolCreateInfo *info = create;
	t_vk_create_descriptor_pool_calls++;
	t_vk_descriptor_pool_size_count = info != NULL ? info->poolSizeCount : 0;
	if (info != NULL && info->poolSizeCount > 0 && info->pPoolSizes != NULL) {
		t_vk_descriptor_pool_descriptor_count = info->pPoolSizes[0].descriptorCount;
	}
	if (t_vk_create_descriptor_pool_ret != VK_SUCCESS) {
		return t_vk_create_descriptor_pool_ret;
	}
	*pool = 43;
	return 0;
}

static void t_vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool pool, const void *alloc)
{
	(void)device;
	(void)pool;
	(void)alloc;
	t_vk_destroy_descriptor_pool_calls++;
}

static int t_vkAllocateDescriptorSets(VkDevice device, const void *alloc, VkDescriptorSet *sets)
{
	(void)device;
	const VkDescriptorSetAllocateInfo *info = alloc;
	t_vk_allocate_descriptor_sets_calls++;
	t_vk_descriptor_set_count = info != NULL ? info->descriptorSetCount : 0;
	if (t_vk_allocate_descriptor_sets_ret != VK_SUCCESS) {
		return t_vk_allocate_descriptor_sets_ret;
	}
	for (u32 i = 0; i < t_vk_descriptor_set_count; i++) {
		sets[i] = 44 + i;
	}
	return 0;
}

static void t_vkUpdateDescriptorSets(VkDevice device, u32 write_count, const void *writes, u32 copy_count, const void *copies)
{
	(void)device;
	(void)copy_count;
	(void)copies;
	const VkWriteDescriptorSet *write = writes;
	t_vk_update_descriptor_sets_calls++;
	if (write_count > 0 && write != NULL && write->pBufferInfo != NULL) {
		t_vk_descriptor_write_set     = write->dstSet;
		t_vk_descriptor_write_binding = write->dstBinding;
		t_vk_descriptor_write_buffer  = write->pBufferInfo->buffer;
		t_vk_descriptor_write_range   = write->pBufferInfo->range;
	}
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
		const VkClearColorValue *clear = &info->pClearValues[0].color;
		t_vk_clear_color[0]	       = clear->float32[0];
		t_vk_clear_color[1]	       = clear->float32[1];
		t_vk_clear_color[2]	       = clear->float32[2];
		t_vk_clear_color[3]	       = clear->float32[3];
		if (info->clearValueCount > 1) {
			t_vk_begin_render_pass_depth = info->pClearValues[1].depthStencil.depth;
		}
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

static void t_vkCmdBindDescriptorSets(VkCommandBuffer buffer, u32 bind_point, VkPipelineLayout layout, u32 first_set, u32 count,
				      const VkDescriptorSet *sets, u32 dynamic_count, const u32 *dynamic_offsets)
{
	(void)buffer;
	(void)bind_point;
	(void)first_set;
	(void)dynamic_count;
	(void)dynamic_offsets;
	t_vk_bind_descriptor_sets_calls++;
	t_vk_bound_descriptor_layout = layout;
	if (count > 0 && sets != NULL) {
		t_vk_bound_descriptor_set = sets[0];
	}
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

static int t_vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice device, VkSurfaceKHR surface, u32 *count, VkPresentModeKHR *modes)
{
	(void)device;
	t_vk_get_surface_present_modes_calls++;
	t_vk_surface = surface;
	if (modes == NULL) {
		*count = t_vk_surface_present_mode_count;
		return t_vk_surface_present_modes_count_ret;
	}
	for (u32 i = 0; i < *count; i++) {
		modes[i] = t_vk_surface_present_modes[i];
	}
	return t_vk_surface_present_modes_ret;
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
	t_vk_copy_image_to_buffer_calls		   = 0;
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
	t_vk_create_semaphore_calls		   = 0;
	t_vk_destroy_semaphore_calls		   = 0;
	t_vk_create_xlib_surface_calls		   = 0;
	t_vk_get_surface_support_calls		   = 0;
	t_vk_get_surface_capabilities_calls	   = 0;
	t_vk_get_surface_formats_calls		   = 0;
	t_vk_get_surface_present_modes_calls	   = 0;
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
	t_vk_render_pass_attachment_count	   = 0;
	t_vk_render_pass_depth_format		   = 0;
	t_vk_render_pass_depth_load		   = 0;
	t_vk_render_pass_depth_store		   = 0;
	t_vk_framebuffer_attachment_count	   = 0;
	t_vk_create_descriptor_set_layout_calls	   = 0;
	t_vk_destroy_descriptor_set_layout_calls   = 0;
	t_vk_create_descriptor_pool_calls	   = 0;
	t_vk_destroy_descriptor_pool_calls	   = 0;
	t_vk_allocate_descriptor_sets_calls	   = 0;
	t_vk_update_descriptor_sets_calls	   = 0;
	t_vk_bind_descriptor_sets_calls		   = 0;
	t_vk_create_pipeline_layout_calls	   = 0;
	t_vk_destroy_pipeline_layout_calls	   = 0;
	t_vk_create_graphics_pipelines_calls	   = 0;
	t_vk_destroy_pipeline_calls		   = 0;
	t_vk_begin_render_pass_calls		   = 0;
	t_vk_begin_render_pass_clear_value_count   = 0;
	t_vk_begin_render_pass_depth		   = 0.0f;
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
	t_vk_optimal_features			   = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	t_vk_memory_flags			   = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	t_vk_fill_mode_non_solid		   = 1;
	t_vk_memory_type_bits			   = 1;
	t_vk_row_pitch				   = 8;
	t_vk_destroyed_image			   = 0;
	t_vk_freed_memory			   = 0;
	t_vk_invalidate_range			   = (VkMappedMemoryRange){0};
	t_vk_flush_range			   = (VkMappedMemoryRange){0};
	t_vk_last_barrier			   = (VkImageMemoryBarrier){0};
	t_vk_barriers[0]			   = (VkImageMemoryBarrier){0};
	t_vk_barriers[1]			   = (VkImageMemoryBarrier){0};
	t_vk_buffer_size			   = 0;
	t_vk_buffer_usage			   = 0;
	t_vk_begin_render_pass_framebuffer	   = 0;
	t_vk_bound_vertex_buffer		   = 0;
	t_vk_bound_index_buffer			   = 0;
	t_vk_bound_index_type			   = 0;
	t_vk_descriptor_binding_count		   = 0;
	t_vk_descriptor_pool_size_count		   = 0;
	t_vk_descriptor_pool_descriptor_count	   = 0;
	t_vk_descriptor_set_count		   = 0;
	t_vk_descriptor_write_binding		   = 0;
	t_vk_descriptor_write_set		   = 0;
	t_vk_descriptor_write_buffer		   = 0;
	t_vk_descriptor_write_range		   = 0;
	t_vk_bound_descriptor_layout		   = 0;
	t_vk_bound_descriptor_set		   = 0;
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
	t_vk_create_semaphore_ret		   = VK_SUCCESS;
	t_vk_create_image_ret			   = VK_SUCCESS;
	t_vk_create_buffer_ret			   = VK_SUCCESS;
	t_vk_allocate_memory_ret		   = VK_SUCCESS;
	t_vk_allocate_memory_fail_at		   = 0;
	t_vk_bind_image_memory_ret		   = VK_SUCCESS;
	t_vk_bind_buffer_memory_ret		   = VK_SUCCESS;
	t_vk_create_image_view_ret		   = VK_SUCCESS;
	t_vk_create_shader_module_ret		   = VK_SUCCESS;
	t_vk_create_render_pass_ret		   = VK_SUCCESS;
	t_vk_create_framebuffer_ret		   = VK_SUCCESS;
	t_vk_create_descriptor_set_layout_ret	   = VK_SUCCESS;
	t_vk_create_descriptor_pool_ret		   = VK_SUCCESS;
	t_vk_allocate_descriptor_sets_ret	   = VK_SUCCESS;
	t_vk_create_pipeline_layout_ret		   = VK_SUCCESS;
	t_vk_create_graphics_pipelines_ret	   = VK_SUCCESS;
	t_vk_map_memory_ret			   = VK_SUCCESS;
	t_vk_flush_mapped_memory_ranges_ret	   = VK_SUCCESS;
	t_vk_invalidate_mapped_memory_ranges_ret   = VK_SUCCESS;
	t_vk_reset_fences_ret			   = VK_SUCCESS;
	t_vk_wait_for_fences_ret		   = VK_SUCCESS;
	t_vk_wait_for_fences_fence		   = 0;
	t_vk_wait_for_fences_fail_fence		   = 0;
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
	t_vk_surface_formats_count_ret	     = VK_SUCCESS;
	t_vk_surface_formats_ret	     = VK_SUCCESS;
	t_vk_surface_present_modes_count_ret = VK_SUCCESS;
	t_vk_surface_present_modes_ret	     = VK_SUCCESS;
	t_vk_swapchain_image_count	     = 2;
	t_vk_create_swapchain_ret	     = VK_SUCCESS;
	t_vk_get_swapchain_images_count_ret  = VK_SUCCESS;
	t_vk_get_swapchain_images_ret	     = VK_SUCCESS;
	t_vk_acquire_next_image_ret	     = VK_SUCCESS;
	t_vk_acquire_next_image_index	     = 1;
	t_vk_queue_present_ret		     = VK_SUCCESS;
	t_gfx_vulkan_alloc_count	     = 0;
	t_gfx_vulkan_alloc_fail_at	     = 0;
	t_vk_instance_extension_count	     = 0;
	t_vk_instance_extensions	     = NULL;
	t_vk_application_api_version	     = 0;
	t_vk_device_extension_count	     = 0;
	t_vk_device_extensions		     = NULL;
	for (u32 i = 0; i < sizeof(t_vk_device_extension_storage) / sizeof(t_vk_device_extension_storage[0]); i++) {
		t_vk_device_extension_storage[i] = NULL;
	}
	t_vk_surface	     = 0;
	t_gfx_vulkan_surface = (gfx_surface_t){
		.api	= GFX_API_VULKAN,
		.handle = 0x44,
	};
	t_vk_swapchain			= 9;
	t_vk_swapchain_create		= (VkSwapchainCreateInfoKHR){0};
	t_vk_surface_format_count	= 1;
	t_vk_surface_present_mode_count = 1;

	t_vk_surface_formats[0] = (VkSurfaceFormatKHR){
		.format	    = VK_FORMAT_R8G8B8A8_UNORM,
		.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
	};
	for (u32 i = 1; i < sizeof(t_vk_surface_formats) / sizeof(t_vk_surface_formats[0]); i++) {
		t_vk_surface_formats[i] = (VkSurfaceFormatKHR){0};
	}
	t_vk_surface_present_modes[0] = VK_PRESENT_MODE_FIFO_KHR;
	for (u32 i = 1; i < sizeof(t_vk_surface_present_modes) / sizeof(t_vk_surface_present_modes[0]); i++) {
		t_vk_surface_present_modes[i] = VK_PRESENT_MODE_MAX_ENUM_KHR;
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
	if (t_strcmp(name, "vkCreateSemaphore") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateSemaphore);
	}
	if (t_strcmp(name, "vkDestroySemaphore") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroySemaphore);
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
	if (t_strcmp(name, "vkCmdCopyImageToBuffer") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdCopyImageToBuffer);
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
	if (t_strcmp(name, "vkCreateDescriptorSetLayout") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateDescriptorSetLayout);
	}
	if (t_strcmp(name, "vkDestroyDescriptorSetLayout") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyDescriptorSetLayout);
	}
	if (t_strcmp(name, "vkCreateDescriptorPool") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCreateDescriptorPool);
	}
	if (t_strcmp(name, "vkDestroyDescriptorPool") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkDestroyDescriptorPool);
	}
	if (t_strcmp(name, "vkAllocateDescriptorSets") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkAllocateDescriptorSets);
	}
	if (t_strcmp(name, "vkUpdateDescriptorSets") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkUpdateDescriptorSets);
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
	if (t_strcmp(name, "vkCmdBindDescriptorSets") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkCmdBindDescriptorSets);
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
	if (t_strcmp(name, "vkGetPhysicalDeviceFeatures") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetPhysicalDeviceFeatures);
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
	if (t_strcmp(name, "vkGetPhysicalDeviceSurfacePresentModesKHR") == 0) {
		return t_gfx_vulkan_symbol((t_gfx_vulkan_symbol_t)t_vkGetPhysicalDeviceSurfacePresentModesKHR);
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

static gfx_image_t *t_gfx_vulkan_init_swapchain_target(gfx_t *gfx, gfx_swapchain_t *swapchain, gfx_image_t *target, u16 width, u16 height)
{
	static gfx_image_t images[32];
	mem_set(images, 0, sizeof(images));
	gfx_swapchain_config_t swapchain_config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &t_gfx_vulkan_surface,
		.width		 = width,
		.height		 = height,
		.images		 = images,
		.min_image_count = 2,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};
	if (gfx_swapchain_init(swapchain, gfx, &swapchain_config) != swapchain) {
		return NULL;
	}
	if (t_gfx_vulkan_image_init_image_swapchain(target, swapchain) != target) {
		gfx_swapchain_free(swapchain);
		return NULL;
	}
	return target;
}

static gfx_swapchain_t *t_gfx_vulkan_init_swapchain(gfx_t *gfx, gfx_swapchain_t *swapchain, gfx_present_mode_t present_mode)
{
	static gfx_image_t images[32];
	mem_set(images, 0, sizeof(images));
	gfx_swapchain_config_t swapchain_config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &t_gfx_vulkan_surface,
		.width		 = 640,
		.height		 = 480,
		.present_mode	 = present_mode,
		.images		 = images,
		.min_image_count = 2,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};
	return gfx_swapchain_init(swapchain, gfx, &swapchain_config);
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

TEST(gfx_vulkan_init_does_not_require_memory_target_format)
{
	START;

	t_vkReset();
	t_vk_optimal_features = 0;
	gfx_t gfx	      = {0};
	proc_t proc	      = {0};

	EXPECT_EQ(t_gfx_vulkan_init_gfx_current(&gfx, &proc), 0);

	gfx_free(&gfx);
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

TEST(gfx_vulkan_free_swapchain_target_without_swapchain_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t target_data     = {.swapchain = 9, .image_count = 1};
	gfx_image_t target			      = {.gfx = &gfx, .origin = GFX_IMAGE_ORIGIN_SURFACE, .driver_data = &target_data};
	((t_gfx_vulkan_data_head_t *)gfx.data)->image = &target;

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_free_swapchain_target_destroys_cached_image_views_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t target_data      = {.swapchain = 9, .image_count = 1};
	t_gfx_vulkan_swapchain_image_data_t image_data = {.image = 10, .view = 20};
	gfx_image_t images[1]			       = {{.driver_data = &image_data}};

	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.surface	= &t_gfx_vulkan_surface,
		.images		= images,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
	};
	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
		.swapchain   = &swapchain,
		.driver_data = &target_data,
	};
	((t_gfx_vulkan_data_head_t *)gfx.data)->image = &target;

	gfx_free(&gfx);
	EXPECT_EQ(t_vk_destroy_image_view_calls, 1);
	EXPECT_EQ(image_data.view, 0);
	proc_free(&proc);
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
	gfx_image_t target = {
		.origin = GFX_IMAGE_ORIGIN_NONE,
	};

	EXPECT_EQ(gfx.drv->image_init(&target), 1);

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
	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_UNKNOWN, .usage = GFX_BUFFER_USAGE_DYNAMIC}));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_rejects_invalid_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->buffer_init(NULL, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->buffer_init(&buffer, &(gfx_buffer_config_t){.type = GFX_BUFFER_UNKNOWN, .usage = GFX_BUFFER_USAGE_DYNAMIC}), 1);
	log_set_quiet(0, 0);
	EXPECT_NULL(buffer.data);
	gfx_vertex_2d_t vertex = {0};
	EXPECT_EQ(gfx.drv->buffer_init(&buffer,
				       &(gfx_buffer_config_t){
					       .type  = GFX_BUFFER_VERTEX,
					       .usage = GFX_BUFFER_USAGE_DYNAMIC,
					       .data  = &vertex,
				       }),
		  1);
	EXPECT_NULL(buffer.data);

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

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}));

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_static_create_buffer_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_buffer_ret	    = 1;
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_buffer_t buffer	    = {0};

	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_VERTEX,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(vertices),
		.data  = vertices,
	};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &buffer_config));
	EXPECT_NULL(buffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_static_upload_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_map_memory_ret	    = 1;
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_buffer_t buffer	    = {0};

	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_VERTEX,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(vertices),
		.data  = vertices,
	};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &buffer_config));
	EXPECT_NULL(buffer.data);
	EXPECT_EQ(t_vk_create_buffer_calls, 1);
	EXPECT_EQ(t_vk_destroy_buffer_calls, 1);
	EXPECT_EQ(t_vk_free_memory_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_static_uploads_data)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 1.0f},
		{.x = 0.0f, .y = 0.0f, .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f},
		{.y = 2.0f},
	};
	gfx_buffer_t buffer = {0};

	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_VERTEX,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(vertices),
		.data  = vertices,
	};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &buffer_config), &buffer);
	EXPECT_EQ(t_vk_create_buffer_calls, 1);
	EXPECT_EQ(t_vk_buffer_size, sizeof(vertices));
	EXPECT_EQ(t_vk_map_memory_calls, 1);
	EXPECT_EQ(t_vk_unmap_memory_calls, 0);

	gfx_buffer_free(&buffer);
	EXPECT_EQ(t_vk_unmap_memory_calls, 1);
	EXPECT_EQ(t_vk_vertex_first_x, 1);
	EXPECT_EQ(t_vk_vertex_last_y, 2);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_init_uniform_buffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	float data[4]	    = {1.0f, 2.0f, 3.0f, 4.0f};
	gfx_buffer_t buffer = {0};

	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_UNIFORM,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(data),
		.data  = data,
	};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &buffer_config), &buffer);
	EXPECT_EQ(t_vk_create_buffer_calls, 1);
	EXPECT_EQ(t_vk_buffer_usage, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	EXPECT_EQ(t_vk_buffer_size, sizeof(data));

	gfx_buffer_free(&buffer);
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

TEST(gfx_vulkan_buffer_set_data_create_buffer_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_buffer_ret	    = 1;
	gfx_buffer_t buffer	    = {0};
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_set_data_memory_type_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_type_bits	    = 0;
	gfx_buffer_t buffer	    = {0};
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_set_data_allocate_memory_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_allocate_memory_ret    = 1;
	gfx_buffer_t buffer	    = {0};
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_set_data_bind_memory_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_bind_buffer_memory_ret = 1;
	gfx_buffer_t buffer	    = {0};
	gfx_vertex_2d_t vertices[3] = {0};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	gfx_buffer_free(&buffer);
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
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	u32 indices[3] = {0, 1, 2};
	EXPECT_EQ(gfx_buffer_set_data(&buffer, indices, sizeof(indices)), 0);
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

TEST(gfx_vulkan_buffer_bind_rejects_empty_storage)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_buffer_data_t driver_buffer = {
		.type = GFX_BUFFER_VERTEX,
	};
	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.type = GFX_BUFFER_VERTEX,
		.data = &driver_buffer,
	};

	EXPECT_EQ(gfx.drv->buffer_bind(&(gfx_frame_t){.gfx = &gfx, .active = 1}, &buffer), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_uniform_buffer_bind_skips_vertex_input_bind)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_buffer_data_t driver_buffer = {
		.buffer = 77,
		.size	= 64,
		.type	= GFX_BUFFER_UNIFORM,
	};
	gfx_pipeline_t pipeline = {.gfx = &gfx, .data = &(t_gfx_vulkan_pipeline_data_t){.pipeline_layout = 1, .descriptor_sets = {2}}};
	gfx_buffer_t buffer	= {.gfx = &gfx, .type = GFX_BUFFER_UNIFORM, .data = &driver_buffer};
	gfx_frame_t frame	= {.gfx = &gfx, .pipeline = &pipeline, .active = 1};

	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(t_vk_bind_vertex_buffers_calls, 0);
	EXPECT_EQ(t_vk_bind_index_buffer_calls, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_bind_resources_updates_descriptor)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_buffer_data_t driver_buffer = {
		.buffer = 77,
		.size	= 64,
		.type	= GFX_BUFFER_UNIFORM,
	};
	t_gfx_vulkan_pipeline_data_t driver_pipeline = {
		.pipeline_layout = 33,
		.descriptor_sets = {44},
	};
	gfx_pipeline_t pipeline		  = {.gfx = &gfx, .data = &driver_pipeline};
	gfx_buffer_t buffer		  = {.gfx = &gfx, .type = GFX_BUFFER_UNIFORM, .data = &driver_buffer};
	gfx_resource_binding_t bindings[] = {{.binding = 3, .type = GFX_RESOURCE_UNIFORM_BUFFER, .buffer = &buffer}};
	gfx_frame_t frame		  = {.gfx = &gfx, .pipeline = &pipeline, .active = 1};

	EXPECT_EQ(gfx.drv->bind_resources(&frame, bindings, 1), 0);
	EXPECT_EQ(t_vk_update_descriptor_sets_calls, 1);
	EXPECT_EQ(t_vk_descriptor_write_set, 44);
	EXPECT_EQ(t_vk_descriptor_write_binding, 3);
	EXPECT_EQ(t_vk_descriptor_write_buffer, 77);
	EXPECT_EQ(t_vk_descriptor_write_range, 64);
	EXPECT_EQ(t_vk_bind_descriptor_sets_calls, 1);
	EXPECT_EQ(t_vk_bound_descriptor_layout, 33);
	EXPECT_EQ(t_vk_bound_descriptor_set, 44);

	EXPECT_EQ(gfx.drv->bind_resources(&frame, bindings, 1), 0);
	EXPECT_EQ(t_vk_update_descriptor_sets_calls, 1);
	EXPECT_EQ(t_vk_bind_descriptor_sets_calls, 2);
	EXPECT_EQ(t_vk_bound_descriptor_set, 44);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_bind_resources_rejects_invalid_args)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_buffer_data_t driver_buffer = {
		.buffer = 77,
		.size	= 64,
		.type	= GFX_BUFFER_UNIFORM,
	};
	t_gfx_vulkan_pipeline_data_t driver_pipeline = {
		.pipeline_layout = 33,
		.descriptor_sets = {44},
	};
	gfx_pipeline_t pipeline		      = {.gfx = &gfx, .data = &driver_pipeline};
	gfx_buffer_t buffer		      = {.gfx = &gfx, .type = GFX_BUFFER_UNIFORM, .data = &driver_buffer};
	gfx_buffer_t vertex		      = {.gfx = &gfx, .type = GFX_BUFFER_VERTEX, .data = &driver_buffer};
	gfx_resource_binding_t bindings[]     = {{.binding = 0, .type = GFX_RESOURCE_UNIFORM_BUFFER, .buffer = &buffer}};
	gfx_resource_binding_t high_binding   = {.binding = 16, .type = GFX_RESOURCE_UNIFORM_BUFFER, .buffer = &buffer};
	gfx_resource_binding_t vertex_binding = {.binding = 0, .type = GFX_RESOURCE_UNIFORM_BUFFER, .buffer = &vertex};
	gfx_frame_t frame		      = {.gfx = &gfx, .pipeline = &pipeline, .active = 1};

	EXPECT_EQ(gfx.drv->bind_resources(NULL, bindings, 1), 1);
	EXPECT_EQ(gfx.drv->bind_resources(&(gfx_frame_t){.gfx = &gfx}, bindings, 1), 1);
	EXPECT_EQ(gfx.drv->bind_resources(&frame, NULL, 1), 1);
	EXPECT_EQ(gfx.drv->bind_resources(&frame,
					  &(gfx_resource_binding_t){.binding = 0,
								    .type    = GFX_RESOURCE_UNIFORM_BUFFER,
								    .buffer  = &(gfx_buffer_t){.gfx = &gfx}},
					  1),
		  1);
	EXPECT_EQ(gfx.drv->bind_resources(&frame, &high_binding, 1), 1);
	EXPECT_EQ(gfx.drv->bind_resources(&frame, &vertex_binding, 1), 1);
	driver_pipeline.descriptor_set_index = 64;
	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->bind_resources(&frame, bindings, 1), 1);
	log_set_quiet(0, 0);
	driver_pipeline.descriptor_set_index = 0;
	driver_buffer.buffer		     = 0;
	EXPECT_EQ(gfx.drv->bind_resources(&frame, bindings, 1), 1);
	driver_buffer.buffer		   = 77;
	driver_pipeline.descriptor_sets[0] = 0;
	EXPECT_EQ(gfx.drv->bind_resources(&frame, bindings, 1), 1);
	driver_pipeline.descriptor_sets[0] = 44;
	driver_pipeline.pipeline_layout	   = 0;
	EXPECT_EQ(gfx.drv->bind_resources(&frame, bindings, 1), 1);

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
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
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
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
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
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx_vertex_2d_t vertices[3]	    = {0};
	t_vk_flush_mapped_memory_ranges_ret = 1;

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);
	EXPECT_EQ(t_vk_unmap_memory_calls, 0);

	gfx_buffer_free(&buffer);
	EXPECT_EQ(t_vk_unmap_memory_calls, 1);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_set_data_grows_buffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx_vertex_2d_t triangle[3]  = {0};
	gfx_vertex_2d_t rectangle[4] = {0};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, triangle, sizeof(triangle)), 0);
	EXPECT_EQ(t_vk_create_buffer_calls, 1);
	EXPECT_EQ(t_vk_buffer_size, sizeof(triangle));

	EXPECT_EQ(gfx_buffer_set_data(&buffer, rectangle, sizeof(rectangle)), 0);
	EXPECT_EQ(t_vk_create_buffer_calls, 2);
	EXPECT_EQ(t_vk_destroy_buffer_calls, 1);
	EXPECT_EQ(t_vk_free_memory_calls, 1);
	EXPECT_EQ(t_vk_buffer_size, sizeof(rectangle));

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_set_data_rejects_unknown_type_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_buffer_data_t driver_buffer = {
		.type = GFX_BUFFER_UNKNOWN,
	};
	gfx_vertex_2d_t vertices[3] = {0};

	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.type = GFX_BUFFER_UNKNOWN,
		.data = &driver_buffer,
	};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(t_vk_create_buffer_calls, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_buffer_set_data_rejects_empty_storage_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_buffer_data_t driver_buffer = {
		.memory	     = 8,
		.size	     = sizeof(gfx_vertex_2d_t),
		.memory_size = sizeof(gfx_vertex_2d_t),
		.type	     = GFX_BUFFER_VERTEX,
	};
	gfx_vertex_2d_t vertex = {0};

	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.type = GFX_BUFFER_VERTEX,
		.data = &driver_buffer,
	};

	EXPECT_EQ(gfx.drv->buffer_set_data(&buffer, &vertex, sizeof(vertex)), 1);
	EXPECT_EQ(t_vk_map_memory_calls, 0);

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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 2,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
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
	EXPECT_EQ(gfx_buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(gfx_draw(&frame, 3, 0), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 0);
	EXPECT_EQ(t_vk_begin_render_pass_calls, 1);
	EXPECT_EQ(t_vk_begin_render_pass_clear_value_count, 1);
	EXPECT_EQ(t_vk_bind_pipeline_calls, 1);
	EXPECT_EQ(t_vk_bind_vertex_buffers_calls, 1);
	EXPECT_EQ(t_vk_set_viewport_calls, 1);
	EXPECT_EQ(t_vk_set_scissor_calls, 1);
	EXPECT_EQ(t_vk_draw_calls, 1);
	EXPECT_EQ(t_vk_end_render_pass_calls, 1);
	EXPECT_EQ(t_vk_copy_image_to_buffer_calls, 1);
	EXPECT_EQ(t_vk_queue_submit_calls, 1);
	EXPECT_EQ(t_vk_invalidate_mapped_memory_ranges_calls, 0);
	EXPECT_EQ(t_vk_map_memory_calls, 2);
	EXPECT_EQ(t_vk_unmap_memory_calls, 1);

	gfx_buffer_free(&buffer);
	EXPECT_EQ(t_vk_unmap_memory_calls, 2);
	gfx_pipeline_free(&pipeline);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 2,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	EXPECT_PTR(
		gfx_buffer_init(&vertex_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&vertex_buffer);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 1.0f},
		{.x = 0.0f, .y = 0.0f, .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f},
		{.y = 2.0f},
	};
	EXPECT_EQ(gfx_buffer_set_data(&vertex_buffer, vertices, sizeof(vertices)), 0);
	gfx_buffer_t index_buffer = {0};
	EXPECT_PTR(
		gfx_buffer_init(&index_buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		&index_buffer);
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
	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 0);
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_image_t *image			    = &swapchain.images[0];
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = image->format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, image, &render_pass), &framebuffer);
	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(t_gfx_vulkan_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_vk_acquire_next_image_calls, 1);
	EXPECT_EQ(t_vk_queue_present_calls, 1);
	EXPECT_EQ(t_vk_present_image_index, 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_free_detaches_active_draw_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain(&gfx, &swapchain, GFX_PRESENT_MODE_IMMEDIATE), &swapchain);
	gfx_image_t *images	      = swapchain.images;
	gfx_render_pass_t render_pass = {0};
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = images[0].format,
						.load	      = GFX_LOAD_LOAD,
						.store	      = GFX_STORE_STORE,
					}),
		   &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &images[0], &render_pass), &framebuffer);
	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_swapchain_free(&swapchain);
	t_gfx_vulkan_data_head_t *vulkan = gfx.data;
	EXPECT_NULL(vulkan->image);
	EXPECT_NULL(vulkan->swapchain);

	int destroyed					   = t_vk_destroy_image_view_calls;
	t_gfx_vulkan_swapchain_image_data_t poisoned_image = {.view = (VkImageView)0xfdfdfe55};
	gfx_image_t poisoned_images[1]			   = {{.driver_data = &poisoned_image}};
	gfx_swapchain_t poisoned_swapchain		   = {.images = poisoned_images, .image_count = 1};
	images[0]					   = (gfx_image_t){
							 .origin      = GFX_IMAGE_ORIGIN_SURFACE,
							 .driver_data = &poisoned_image,
							 .swapchain   = &poisoned_swapchain,
	 };
	gfx_free(&gfx);
	EXPECT_EQ(t_vk_destroy_image_view_calls, destroyed);

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
	gfx_image_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_image_t *image			    = &swapchain.images[0];
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = image->format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, image, &render_pass), &framebuffer);

	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	t_vk_queue_present_ret	 = VK_ERROR_OUT_OF_DATE_KHR;
	t_vk_swapchain_images[0] = 20;
	t_vk_swapchain_images[1] = 21;
	EXPECT_EQ(t_gfx_vulkan_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_vk_destroy_framebuffer_calls, 0);
	EXPECT_EQ(t_vk_create_framebuffer_calls, 2);

	t_vk_queue_present_ret = VK_SUCCESS;
	frame		       = (gfx_frame_t){0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(t_vk_destroy_framebuffer_calls, 2);
	EXPECT_EQ(t_vk_create_framebuffer_calls, 4);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(t_gfx_vulkan_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_vk_acquire_next_image_calls, 2);
	EXPECT_EQ(t_vk_queue_present_calls, 2);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_image_t *image			    = &swapchain.images[0];
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = image->format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, image, &render_pass), &framebuffer);
	t_vk_surface_capabilities.currentExtent = (VkExtent2D){.width = 320, .height = 240};
	gfx_frame_t frame			= {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(image->width, 320);
	EXPECT_EQ(image->height, 240);
	EXPECT_EQ(swapchain.width, 320);
	EXPECT_EQ(swapchain.height, 240);
	EXPECT_EQ(framebuffer.width, 320);
	EXPECT_EQ(framebuffer.height, 240);
	EXPECT_EQ(t_vk_destroy_framebuffer_calls, 2);
	EXPECT_EQ(t_vk_create_framebuffer_calls, 4);
	EXPECT_EQ(gfx_end(&frame), 0);
	EXPECT_EQ(t_gfx_vulkan_swapchain_present(&swapchain), 0);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(gfx_end(&frame), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_pass_begin_missing_frame_sync_resource)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_gfx_vulkan_data_t *vulkan	 = gfx.data;
	VkCommandBuffer command_buffer	 = vulkan->frames[0].command_buffer;
	vulkan->frames[0].command_buffer = 0;
	gfx_frame_t frame		 = {0};

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 2, .height = 1}}), 1);

	vulkan->frames[0].command_buffer = command_buffer;
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	gfx_image_free(&target);
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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	gfx_image_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_end_submit_missing_frame_sync_resource)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	t_gfx_vulkan_data_t *vulkan		   = gfx.data;
	VkFence fence				   = vulkan->frames[vulkan->active_frame].fence;
	vulkan->frames[vulkan->active_frame].fence = 0;

	EXPECT_EQ(gfx_end(&frame), 1);

	vulkan->frames[vulkan->active_frame].fence = fence;
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_end_submit_wait_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	t_vk_wait_for_fences_ret = 1;

	EXPECT_EQ(gfx_end(&frame), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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

	EXPECT_EQ(gfx_end(&frame), 0);

	target.driver_data = target_data;
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_end_memory_requires_readback_buffer_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_memory_target_data_t target_data = {
		.image	    = 10,
		.image_view = 20,
	};
	t_gfx_vulkan_framebuffer_data_t framebuffer_data = {.framebuffer = 30};
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = GFX_IMAGE_ORIGIN_MEMORY,
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
		.image	     = &target,
		.render_pass = &render_pass,
		.data	     = &framebuffer_data,
		.width	     = 2,
		.height	     = 1,
	};
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 0);
	EXPECT_EQ(gfx.drv->end(&frame), 1);

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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	gfx_image_free(&target);
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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_image_free(&target);
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

TEST(gfx_vulkan_end_surface_requires_swapchain_image_data)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};
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
	void *driver_data					    = swapchain.images[t_vk_acquire_next_image_index].driver_data;
	swapchain.images[t_vk_acquire_next_image_index].driver_data = NULL;

	EXPECT_EQ(gfx_end(&frame), 1);

	swapchain.images[t_vk_acquire_next_image_index].driver_data = driver_data;
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_end_surface_rejects_invalid_public_image_index)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};
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
	u32 image_count	      = swapchain.image_count;
	swapchain.image_count = 0;

	EXPECT_EQ(gfx_end(&frame), 1);

	swapchain.image_count = image_count;
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
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

TEST(gfx_vulkan_swapchain_acquire_invalid_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_image_t image = {0};

	EXPECT_EQ(gfx.drv->swapchain_acquire(NULL, &image), 1);
	EXPECT_EQ(gfx.drv->swapchain_acquire(&(gfx_swapchain_t){.gfx = &gfx}, NULL), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_acquire_without_bound_target_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_acquire_next_image_ret		     = 1;
	t_gfx_vulkan_swapchain_data_t swapchain_data = {.swapchain = 9, .image_count = 1};
	gfx_image_t images[1]			     = {{.driver_data = &(t_gfx_vulkan_swapchain_image_data_t){.image = 10}}};

	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.format		= GFX_FORMAT_RGBA8,
		.surface	= &t_gfx_vulkan_surface,
		.images		= images,
		.width		= 640,
		.height		= 480,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
		.data		= &swapchain_data,
	};
	gfx_swapchain_image_t image = {0};

	EXPECT_EQ(gfx.drv->swapchain_acquire(&swapchain, &image), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_acquire_bound_target_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_acquire_next_image_ret		     = 1;
	t_gfx_vulkan_swapchain_data_t swapchain_data = {.swapchain = 9, .image_count = 1};
	gfx_image_t images[1]			     = {{.driver_data = &(t_gfx_vulkan_swapchain_image_data_t){.image = 10}}};

	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.format		= GFX_FORMAT_RGBA8,
		.surface	= &t_gfx_vulkan_surface,
		.images		= images,
		.width		= 640,
		.height		= 480,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
		.data		= &swapchain_data,
	};
	gfx_image_t target = {
		.gfx	   = &gfx,
		.origin	   = GFX_IMAGE_ORIGIN_SURFACE,
		.swapchain = &swapchain,
	};
	((t_gfx_vulkan_data_head_t *)gfx.data)->image = &target;
	gfx_swapchain_image_t image		      = {0};

	EXPECT_EQ(gfx.drv->swapchain_acquire(&swapchain, &image), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_acquire_rejects_invalid_index_or_image_data_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t swapchain_data = {.swapchain = 9, .image_count = 1};
	gfx_image_t images[1]			     = {0};

	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.format		= GFX_FORMAT_RGBA8,
		.surface	= &t_gfx_vulkan_surface,
		.images		= images,
		.width		= 640,
		.height		= 480,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
		.data		= &swapchain_data,
	};
	gfx_swapchain_image_t image = {0};

	t_vk_acquire_next_image_index = 1;
	EXPECT_EQ(gfx.drv->swapchain_acquire(&swapchain, &image), 1);

	t_vk_acquire_next_image_index = 0;
	EXPECT_EQ(gfx.drv->swapchain_acquire(&swapchain, &image), 1);

	swapchain_data.acquired	      = 1;
	swapchain_data.image_index    = 0;
	t_vk_acquire_next_image_index = 0;
	EXPECT_EQ(gfx.drv->swapchain_acquire(&swapchain, &image), 1);

	swapchain_data.acquired	      = 1;
	swapchain_data.image_index    = 1;
	t_vk_acquire_next_image_index = 0;
	EXPECT_EQ(gfx.drv->swapchain_acquire(&swapchain, &image), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_acquire_rejects_missing_public_swapchain_data_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_acquire_next_image_index		  = 0;
	t_gfx_vulkan_swapchain_data_t target_data = {.swapchain = 9, .image_count = 1};
	gfx_image_t images[1]			  = {{.driver_data = &(t_gfx_vulkan_swapchain_image_data_t){.image = 10}}};

	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.format		= GFX_FORMAT_RGBA8,
		.surface	= &t_gfx_vulkan_surface,
		.images		= images,
		.width		= 640,
		.height		= 480,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
	};
	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
		.swapchain   = &swapchain,
		.driver_data = &target_data,
	};
	((t_gfx_vulkan_data_head_t *)gfx.data)->image = &target;
	gfx_swapchain_image_t image		      = {0};

	EXPECT_EQ(gfx.drv->swapchain_acquire(&swapchain, &image), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_acquire_rejects_missing_image_available_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t swapchain_data   = {.swapchain = 9, .image_count = 1};
	t_gfx_vulkan_swapchain_image_data_t image_data = {.image = 10};
	gfx_image_t images[1]			       = {{.driver_data = &image_data}};

	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.format		= GFX_FORMAT_RGBA8,
		.surface	= &t_gfx_vulkan_surface,
		.images		= images,
		.width		= 640,
		.height		= 480,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
		.data		= &swapchain_data,
	};
	t_gfx_vulkan_data_t *vulkan	  = gfx.data;
	VkSemaphore image_available	  = vulkan->frames[0].image_available;
	vulkan->frames[0].image_available = 0;
	gfx_swapchain_image_t image	  = {0};

	EXPECT_EQ(gfx.drv->swapchain_acquire(&swapchain, &image), 1);

	vulkan->frames[0].image_available = image_available;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_acquire_rejects_in_flight_fence_wait_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t swapchain_data   = {.swapchain = 9, .image_count = 1};
	t_gfx_vulkan_swapchain_image_data_t image_data = {.image = 10, .in_flight = 77};
	gfx_image_t images[1]			       = {{.driver_data = &image_data}};

	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.format		= GFX_FORMAT_RGBA8,
		.surface	= &t_gfx_vulkan_surface,
		.images		= images,
		.width		= 640,
		.height		= 480,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
		.data		= &swapchain_data,
	};
	t_vk_acquire_next_image_index	= 0;
	t_vk_wait_for_fences_fail_fence = image_data.in_flight;
	gfx_swapchain_image_t image	= {0};

	EXPECT_EQ(gfx.drv->swapchain_acquire(&swapchain, &image), 1);
	EXPECT_EQ(t_vk_wait_for_fences_fence, image_data.in_flight);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_present_requires_backend_acquire_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t swapchain_data = {.swapchain = 9, .image_count = 1};

	gfx_swapchain_t swapchain = {
		.gfx	     = &gfx,
		.format	     = GFX_FORMAT_RGBA8,
		.surface     = &t_gfx_vulkan_surface,
		.width	     = 640,
		.height	     = 480,
		.image_count = 1,
		.data	     = &swapchain_data,
	};

	EXPECT_EQ(gfx.drv->swapchain_present(&swapchain), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_present_rejects_missing_render_finished_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t swapchain_data = {
		.swapchain	 = 9,
		.image_count	 = 1,
		.present_frame	 = 0,
		.acquired	 = 1,
		.present_pending = 1,
	};
	gfx_swapchain_t swapchain = {
		.gfx	     = &gfx,
		.format	     = GFX_FORMAT_RGBA8,
		.surface     = &t_gfx_vulkan_surface,
		.width	     = 640,
		.height	     = 480,
		.image_count = 1,
		.data	     = &swapchain_data,
	};
	t_gfx_vulkan_data_t *vulkan	  = gfx.data;
	VkSemaphore render_finished	  = vulkan->frames[0].render_finished;
	vulkan->frames[0].render_finished = 0;

	EXPECT_EQ(gfx.drv->swapchain_present(&swapchain), 1);

	vulkan->frames[0].render_finished = render_finished;
	gfx_free(&gfx);
	proc_free(&proc);
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

TEST(gfx_vulkan_image_init_rejects_zero_size_direct)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target = {
		.gfx	= &gfx,
		.origin = GFX_IMAGE_ORIGIN_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 0,
		.height = 1,
		.stride = 8,
	};

	EXPECT_EQ(gfx.drv->image_init(&target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_image_init_rejects_unknown_type_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target = {
		.gfx	= &gfx,
		.origin = (gfx_image_origin_t)99,
		.format = GFX_FORMAT_RGBA8,
		.width	= 1,
		.height = 1,
	};

	EXPECT_EQ(gfx.drv->image_init(&target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx.alloc		  = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};

	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_validates_surface_metadata_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {
		.gfx	 = &gfx,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_gfx_vulkan_surface,
		.width	 = 640,
		.height	 = 480,
	};
	gfx_image_t target = {
		.gfx	   = &gfx,
		.origin	   = GFX_IMAGE_ORIGIN_SURFACE,
		.format	   = GFX_FORMAT_RGBA8,
		.swapchain = &swapchain,
		.width	   = 640,
		.height	   = 480,
	};

	EXPECT_EQ(gfx.drv->image_init(&target), 1);
	target.driver_data = &swapchain;
	EXPECT_EQ(gfx.drv->image_init(&target), 0);
	target.driver_data = NULL;
	target.height	   = 479;
	EXPECT_EQ(gfx.drv->image_init(&target), 1);
	target.height	  = 480;
	swapchain.surface = &(gfx_surface_t){.api = GFX_API_SOFTWARE, .handle = t_gfx_vulkan_surface.handle};
	EXPECT_EQ(gfx.drv->image_init(&target), 1);
	swapchain.surface = &(gfx_surface_t){.api = GFX_API_VULKAN};
	EXPECT_EQ(gfx.drv->image_init(&target), 1);

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

TEST(gfx_vulkan_swapchain_init_uses_immediate_present_mode)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_present_mode_count = 2;
	t_vk_surface_present_modes[0]	= VK_PRESENT_MODE_FIFO_KHR;
	t_vk_surface_present_modes[1]	= VK_PRESENT_MODE_IMMEDIATE_KHR;
	gfx_swapchain_t swapchain	= {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain(&gfx, &swapchain, GFX_PRESENT_MODE_IMMEDIATE), &swapchain);
	EXPECT_EQ(t_vk_swapchain_create.presentMode, VK_PRESENT_MODE_IMMEDIATE_KHR);
	EXPECT_EQ(swapchain.actual_present_mode, GFX_PRESENT_MODE_IMMEDIATE);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_init_uses_mailbox_present_mode)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_present_mode_count = 2;
	t_vk_surface_present_modes[0]	= VK_PRESENT_MODE_FIFO_KHR;
	t_vk_surface_present_modes[1]	= VK_PRESENT_MODE_MAILBOX_KHR;
	gfx_swapchain_t swapchain	= {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain(&gfx, &swapchain, GFX_PRESENT_MODE_MAILBOX), &swapchain);
	EXPECT_EQ(t_vk_swapchain_create.presentMode, VK_PRESENT_MODE_MAILBOX_KHR);
	EXPECT_EQ(swapchain.actual_present_mode, GFX_PRESENT_MODE_MAILBOX);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_init_falls_back_to_vsync_present_mode)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain(&gfx, &swapchain, GFX_PRESENT_MODE_IMMEDIATE), &swapchain);
	EXPECT_EQ(t_vk_swapchain_create.presentMode, VK_PRESENT_MODE_FIFO_KHR);
	EXPECT_EQ(swapchain.actual_present_mode, GFX_PRESENT_MODE_VSYNC);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_init_mailbox_falls_back_to_vsync_present_mode)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain(&gfx, &swapchain, GFX_PRESENT_MODE_MAILBOX), &swapchain);
	EXPECT_EQ(t_vk_swapchain_create.presentMode, VK_PRESENT_MODE_FIFO_KHR);
	EXPECT_EQ(swapchain.actual_present_mode, GFX_PRESENT_MODE_VSYNC);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_init_rejects_invalid_present_mode)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};

	EXPECT_NULL(t_gfx_vulkan_init_swapchain(&gfx, &swapchain, (gfx_present_mode_t)99));
	EXPECT_EQ(t_vk_create_swapchain_calls, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_init_limits_present_modes)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_present_mode_count = sizeof(t_vk_surface_present_modes) / sizeof(t_vk_surface_present_modes[0]);
	for (u32 i = 0; i < t_vk_surface_present_mode_count; i++) {
		t_vk_surface_present_modes[i] = VK_PRESENT_MODE_FIFO_KHR;
	}
	t_vk_surface_present_modes[15] = VK_PRESENT_MODE_MAILBOX_KHR;
	gfx_swapchain_t swapchain      = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain(&gfx, &swapchain, GFX_PRESENT_MODE_MAILBOX), &swapchain);
	EXPECT_EQ(t_vk_swapchain_create.presentMode, VK_PRESENT_MODE_MAILBOX_KHR);
	EXPECT_EQ(swapchain.actual_present_mode, GFX_PRESENT_MODE_MAILBOX);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_init_present_mode_count_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_present_modes_count_ret = 1;
	gfx_swapchain_t swapchain	     = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain(&gfx, &swapchain, GFX_PRESENT_MODE_DEFAULT));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_swapchain_init_present_mode_list_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_present_modes_ret = 1;
	gfx_swapchain_t swapchain      = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain(&gfx, &swapchain, GFX_PRESENT_MODE_DEFAULT));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_without_wsi)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_surface_support_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_supported	  = 0;
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_capabilities_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities_ret = 1;
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t target	      = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_usage_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	gfx_swapchain_t swapchain		      = {0};
	gfx_image_t target			      = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_format_count_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats_count_ret = 1;
	gfx_swapchain_t swapchain      = {0};
	gfx_image_t target	       = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_format_list_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats_ret  = 1;
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_limits_surface_formats)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_format_count = 20;
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);

	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_undefined_format)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats[0].format = 0;
	gfx_swapchain_t swapchain      = {0};
	gfx_image_t target	       = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);

	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_prefers_supported_format)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats[0].format = VK_FORMAT_B8G8R8A8_UNORM;
	gfx_swapchain_t swapchain      = {0};
	gfx_image_t target	       = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	EXPECT_EQ(target.format, GFX_FORMAT_BGRA8_UNORM);

	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_rejects_unsupported_format)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_formats[0].format = 999;
	gfx_swapchain_t swapchain      = {0};
	gfx_image_t target	       = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_clamps_extent_and_image_count)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.maxImageCount		= 2;
	t_vk_surface_capabilities.minImageExtent.width	= 10;
	t_vk_surface_capabilities.minImageExtent.height = 20;
	t_vk_surface_capabilities.maxImageExtent.width	= 100;
	t_vk_surface_capabilities.maxImageExtent.height = 200;
	gfx_swapchain_t swapchain			= {0};
	gfx_image_t target				= {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 1, 300), &target);
	EXPECT_EQ(t_vk_swapchain_create.minImageCount, 2);
	EXPECT_EQ(target.width, 10);
	EXPECT_EQ(target.height, 200);

	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_raises_min_image_count_to_surface_min)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.minImageCount = 3;
	t_vk_swapchain_image_count		= 3;
	t_vk_swapchain_images[2]		= 12;
	gfx_swapchain_t swapchain		= {0};
	gfx_image_t images[3]			= {0};

	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &t_gfx_vulkan_surface,
		.width		 = 640,
		.height		 = 480,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};

	EXPECT_PTR(gfx_swapchain_init(&swapchain, &gfx, &config), &swapchain);
	EXPECT_EQ(t_vk_swapchain_create.minImageCount, 3);
	EXPECT_EQ(swapchain.image_count, 3);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_rejects_surface_min_above_user_capacity)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.minImageCount = 3;
	gfx_swapchain_t swapchain		= {0};
	gfx_image_t images[2]			= {0};

	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &t_gfx_vulkan_surface,
		.width		 = 640,
		.height		 = 480,
		.images		 = images,
		.min_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};

	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &config));

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_uses_current_extent)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.currentExtent = (VkExtent2D){.width = 320, .height = 240};
	gfx_swapchain_t swapchain		= {0};
	gfx_image_t target			= {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	EXPECT_EQ(target.width, 320);
	EXPECT_EQ(target.height, 240);

	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_rejects_native_image_count_above_capacity)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_swapchain_image_count    = 2;
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t image	      = {0};
	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &t_gfx_vulkan_surface,
		.width		 = 640,
		.height		 = 480,
		.images		 = &image,
		.min_image_count = 1,
		.image_capacity	 = 1,
	};

	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &config));
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_rejects_native_image_count_above_user_max)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_swapchain_image_count    = 2;
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t images[2]	      = {0};
	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &t_gfx_vulkan_surface,
		.width		 = 640,
		.height		 = 480,
		.images		 = images,
		.min_image_count = 1,
		.max_image_count = 1,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};

	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &config));
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_chooses_nonopaque_alpha)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.supportedCompositeAlpha = 2;
	gfx_swapchain_t swapchain			  = {0};
	gfx_image_t target				  = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	EXPECT_EQ(t_vk_swapchain_create.compositeAlpha, 2);

	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_native_image_array_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_alloc_count      = 0;
	t_gfx_vulkan_alloc_fail_at    = 2;
	gfx.alloc		      = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t images[2]	      = {0};
	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &t_gfx_vulkan_surface,
		.width		 = 640,
		.height		 = 480,
		.images		 = images,
		.min_image_count = 2,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};

	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &config));
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);

	gfx.alloc = ALLOC_STD;
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_image_pointer_array_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_alloc_count      = 0;
	t_gfx_vulkan_alloc_fail_at    = 3;
	gfx.alloc		      = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t images[2]	      = {0};
	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &t_gfx_vulkan_surface,
		.width		 = 640,
		.height		 = 480,
		.images		 = images,
		.min_image_count = 2,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};

	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &config));
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);

	gfx.alloc = ALLOC_STD;
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_image_payload_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_alloc_count      = 0;
	t_gfx_vulkan_alloc_fail_at    = 5;
	gfx.alloc		      = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t images[2]	      = {0};
	gfx_swapchain_config_t config = {
		.format		 = GFX_FORMAT_RGBA8,
		.surface	 = &t_gfx_vulkan_surface,
		.width		 = 640,
		.height		 = 480,
		.images		 = images,
		.min_image_count = 2,
		.image_capacity	 = sizeof(images) / sizeof(images[0]),
	};

	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &config));
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);
	EXPECT_NULL(images[0].driver_data);

	gfx.alloc = ALLOC_STD;
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_rejects_missing_alpha)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_surface_capabilities.supportedCompositeAlpha = 0;
	gfx_swapchain_t swapchain			  = {0};
	gfx_image_t target				  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_create_swapchain_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_create_swapchain_ret = 1;
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_image_swapchain_count_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_get_swapchain_images_count_ret = 1;
	gfx_swapchain_t swapchain	    = {0};
	gfx_image_t target		    = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480));
	log_set_quiet(0, 0);
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_limits_swapchain_images)
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
	gfx_image_t target	  = {0};

	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	EXPECT_EQ(((t_gfx_vulkan_swapchain_data_t *)swapchain.data)->image_count, 20);

	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_surface_image_init_image_swapchain_list_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_vk_get_swapchain_images_ret = 1;
	gfx_swapchain_t swapchain     = {0};
	gfx_image_t target	      = {0};

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
	gfx_image_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	t_vk_swapchain = 19;

	EXPECT_EQ(gfx_swapchain_resize(&swapchain, 320, 240), 0);
	EXPECT_EQ(t_vk_device_wait_idle_calls, 1);
	EXPECT_EQ(t_vk_destroy_swapchain_calls, 1);

	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);

	gfx_swapchain_image_t image = {
		.image	    = &swapchain.images[0],
		.index	    = 0,
		.generation = swapchain.images[0].generation,
	};
	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 1);

	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_swapchain_image_t image = {0};
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 0);
	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	t_vk_queue_present_ret = VK_ERROR_OUT_OF_DATE_KHR;

	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 0);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_swapchain_image_t image = {0};
	EXPECT_EQ(gfx_swapchain_acquire(&swapchain, &image), 0);
	gfx_frame_t frame = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){.viewport = {.width = 640, .height = 480}}), 0);
	EXPECT_EQ(gfx_end(&frame), 0);
	t_vk_queue_present_ret = 1;

	EXPECT_EQ(gfx_swapchain_present(&swapchain, &image), 1);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_alloc_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx.alloc	   = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_image_t target = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_NULL(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_create_image_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_image_ret = 1;
	gfx_image_t target    = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_image_memory_type_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_type_bits = 0;
	gfx_image_t target    = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_allocate_memory_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_allocate_memory_ret = 1;
	gfx_image_t target	 = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_bind_memory_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_bind_image_memory_ret = 1;
	gfx_image_t target	   = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_uses_device_local_memory)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_flags =
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	gfx_image_t target = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
	EXPECT_EQ(t_vk_allocate_memory_calls, 2);

	gfx_image_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_readback_buffer_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_buffer_ret = 1;
	gfx_image_t target     = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_readback_memory_type_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_flags  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	gfx_image_t target = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);
	EXPECT_NULL(target.driver_data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_readback_allocate_memory_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_allocate_memory_fail_at = 2;
	gfx_image_t target	     = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);
	EXPECT_NULL(target.driver_data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_memory_image_init_readback_bind_memory_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_bind_buffer_memory_ret = 1;
	gfx_image_t target	    = {0};

	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config));
	log_set_quiet(0, 0);
	EXPECT_NULL(target.driver_data);

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

TEST(gfx_vulkan_image_free_null_target_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	drv->image_free(NULL);

	END;
}

TEST(gfx_vulkan_image_read_null_target_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_vulkan_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->image_read(NULL, &(gfx_memory_readback_config_t){0}), 1);

	END;
}

TEST(gfx_vulkan_image_read_requires_bound_target)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);

	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 1);

	gfx_image_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_image_read_noncoherent_memory)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_flags			       = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
	((t_gfx_vulkan_data_head_t *)gfx.data)->image = &target;

	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 0);
	EXPECT_EQ(t_vk_invalidate_mapped_memory_ranges_calls, 1);

	gfx_image_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_image_read_invalidate_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_memory_flags			       = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
	((t_gfx_vulkan_data_head_t *)gfx.data)->image = &target;
	t_vk_invalidate_mapped_memory_ranges_ret      = 1;

	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 1);

	gfx_image_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_image_read_map_failure)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
	((t_gfx_vulkan_data_head_t *)gfx.data)->image = &target;
	t_vk_map_memory_ret			      = 1;

	EXPECT_EQ(gfx_image_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 8}), 1);

	gfx_image_free(&target);
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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
		.image	     = &target,
		.render_pass = &render_pass,
		.width	     = target.width,
		.height	     = target.height,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	t_gfx_vulkan_swapchain_image_data_t *image_data = swapchain.images[0].driver_data;
	image_data->image				= 0;
	gfx_render_pass_t render_pass			= {0};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));

	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	t_gfx_vulkan_swapchain_data_t *target_data  = swapchain.data;
	target_data->image_count		    = 0;
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
	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_init_surface_requires_swapchain_data_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t target_data	 = {0};
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_swapchain_t swapchain = {
		.gfx	 = &gfx,
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_gfx_vulkan_surface,
		.width	 = 640,
		.height	 = 480,
	};
	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
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
		.image	     = &target,
		.render_pass = &render_pass,
		.width	     = 640,
		.height	     = 480,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);
	EXPECT_NULL(framebuffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_init_surface_framebuffer_array_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_image_t target	  = {0};
	EXPECT_PTR(t_gfx_vulkan_init_swapchain_target(&gfx, &swapchain, &target, 640, 480), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_gfx_vulkan_alloc_count      = 0;
	t_gfx_vulkan_alloc_fail_at    = 2;
	gfx.alloc		      = (alloc_t){.alloc = t_gfx_vulkan_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_framebuffer_t framebuffer = {0};

	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	EXPECT_NULL(framebuffer.data);

	gfx.alloc = ALLOC_STD;
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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
	gfx_image_t target	  = {0};
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
	gfx_image_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_depth_pass_begin)
{
	START;

	u8 pixels[8] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass		    = {0};
	gfx_render_pass_config_t render_pass_config = {
		.color_format = target.format,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
		.depth_format = GFX_FORMAT_D32_FLOAT,
		.depth_load   = GFX_LOAD_CLEAR,
		.depth_store  = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	EXPECT_EQ(t_vk_render_pass_attachment_count, 2);
	EXPECT_EQ(t_vk_render_pass_depth_format, VK_FORMAT_D32_SFLOAT);
	EXPECT_EQ(t_vk_render_pass_depth_load, VK_ATTACHMENT_LOAD_OP_CLEAR);
	EXPECT_EQ(t_vk_render_pass_depth_store, VK_ATTACHMENT_STORE_OP_STORE);
	t_gfx_vulkan_render_pass_data_t *pass_data = render_pass.data;
	EXPECT_EQ(pass_data->depth, 1);

	t_vk_memory_flags	      = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(t_vk_framebuffer_attachment_count, 2);
	t_gfx_vulkan_framebuffer_data_t *framebuffer_data = framebuffer.data;
	EXPECT_NOT_NULL(framebuffer_data);
	EXPECT_NE(framebuffer_data->depth_image, 0);
	EXPECT_NE(framebuffer_data->depth_memory, 0);
	EXPECT_NE(framebuffer_data->depth_view, 0);

	t_vk_pipeline_barrier_calls = 0;
	t_vk_barriers[0]	    = (VkImageMemoryBarrier){0};
	t_vk_barriers[1]	    = (VkImageMemoryBarrier){0};
	gfx_frame_t frame	    = {0};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer,
					     &frame,
					     &(gfx_pass_config_t){
						     .clear	  = {.r = 0.1f, .g = 0.2f, .b = 0.3f, .a = 0.4f},
						     .clear_depth = 0.25f,
						     .viewport	  = {.width = 2, .height = 1},
					     }),
		  0);
	EXPECT_EQ(t_vk_pipeline_barrier_calls, 2);
	EXPECT_EQ(t_vk_barriers[1].image, framebuffer_data->depth_image);
	EXPECT_EQ(t_vk_barriers[1].newLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	EXPECT_EQ(framebuffer_data->depth_layout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	EXPECT_EQ(t_vk_begin_render_pass_clear_value_count, 2);
	EXPECT_EQ(t_vk_begin_render_pass_depth, 0.25f);

	EXPECT_EQ(gfx_end(&frame), 0);
	gfx_framebuffer_free(&framebuffer);
	EXPECT_EQ(t_vk_destroy_image_view_calls > 0, 1);
	EXPECT_EQ(t_vk_destroy_image_calls > 0, 1);
	EXPECT_EQ(t_vk_free_memory_calls > 0, 1);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_depth_memory_type_fallback)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass = {0};
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = target.format,
						.depth_format = GFX_FORMAT_D32_FLOAT,
					}),
		   &render_pass);

	t_vk_memory_flags	      = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_framebuffer_depth_attachment_failures)
{
	START;

	u8 pixels[4] = {0};
	gfx_t gfx    = {0};
	proc_t proc  = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_t render_pass = {0};
	EXPECT_PTR(gfx_render_pass_init(&render_pass,
					&gfx,
					&(gfx_render_pass_config_t){
						.color_format = target.format,
						.depth_format = GFX_FORMAT_D32_FLOAT,
					}),
		   &render_pass);

	gfx_framebuffer_t framebuffer = {0};
	t_vk_create_image_ret	      = 1;
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	t_vk_create_image_ret = VK_SUCCESS;

	t_vk_memory_type_bits = 0;
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	t_vk_memory_type_bits = 1;

	t_vk_allocate_memory_ret = 1;
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	t_vk_allocate_memory_ret = VK_SUCCESS;

	t_vk_bind_image_memory_ret = 1;
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	t_vk_bind_image_memory_ret = VK_SUCCESS;

	t_vk_create_image_view_ret = 1;
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	t_vk_create_image_view_ret = VK_SUCCESS;

	gfx_render_pass_free(&render_pass);
	gfx_image_free(&target);
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

	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = (gfx_image_origin_t)99,
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
		.image	     = &target,
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

	gfx_image_t target = {
		.gfx	= &gfx,
		.origin = GFX_IMAGE_ORIGIN_MEMORY,
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
		.image	     = &target,
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
	gfx_image_t target = {
		.gfx	   = &gfx,
		.origin	   = GFX_IMAGE_ORIGIN_SURFACE,
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
		.image	     = &target,
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

TEST(gfx_vulkan_framebuffer_pass_begin_surface_requires_swapchain_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t target_data	 = {0};
	t_gfx_vulkan_framebuffer_data_t framebuffer_data = {0};
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
		.format	     = GFX_FORMAT_RGBA8,
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
		.image	     = &target,
		.render_pass = &render_pass,
		.data	     = &framebuffer_data,
		.width	     = 640,
		.height	     = 480,
	};
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

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
	t_gfx_vulkan_memory_target_data_t target_data = {
		.readback = {.buffer = 12, .memory = 8, .memory_size = 8},
	};
	t_gfx_vulkan_framebuffer_data_t framebuffer_data = {.framebuffer = 30};
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};

	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = GFX_IMAGE_ORIGIN_MEMORY,
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
		.image	     = &target,
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
		.swapchain   = 9,
		.image_count = 2,
		.image_index = 1,
		.acquired    = 1,
	};
	t_gfx_vulkan_swapchain_image_data_t image_data = {
		.image = 11,
		.view  = 21,
	};
	t_gfx_vulkan_framebuffer_data_t framebuffer_data = {0};
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};
	gfx_image_t images[2]				 = {0};
	images[1].driver_data				 = &image_data;

	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.format		= GFX_FORMAT_RGBA8,
		.surface	= &t_gfx_vulkan_surface,
		.images		= images,
		.width		= 640,
		.height		= 480,
		.image_count	= 2,
		.image_capacity = sizeof(images) / sizeof(images[0]),
	};
	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
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
		.image	     = &target,
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

TEST(gfx_vulkan_framebuffer_pass_begin_surface_requires_swapchain_framebuffer_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_surface_gfx(&gfx, &proc), 0);
	t_gfx_vulkan_swapchain_data_t swapchain_data = {
		.swapchain   = 9,
		.image_count = 1,
		.image_index = 0,
		.acquired    = 1,
	};
	t_gfx_vulkan_swapchain_image_data_t image_data = {
		.image = 11,
		.view  = 21,
	};
	t_gfx_vulkan_framebuffer_data_t framebuffer_data = {
		.swapchain_framebuffers	     = &(VkFramebuffer){0},
		.swapchain_framebuffer_count = 1,
	};
	t_gfx_vulkan_render_pass_data_t render_pass_data = {0};
	gfx_image_t images[1]				 = {{.driver_data = &image_data}};

	gfx_swapchain_t swapchain = {
		.gfx		= &gfx,
		.format		= GFX_FORMAT_RGBA8,
		.surface	= &t_gfx_vulkan_surface,
		.images		= images,
		.width		= 640,
		.height		= 480,
		.image_count	= 1,
		.image_capacity = sizeof(images) / sizeof(images[0]),
		.data		= &swapchain_data,
	};
	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
		.format	     = GFX_FORMAT_RGBA8,
		.swapchain   = &swapchain,
		.driver_data = &swapchain_data,
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
		.image	     = &target,
		.render_pass = &render_pass,
		.data	     = &framebuffer_data,
		.width	     = 640,
		.height	     = 480,
	};
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

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

	gfx_image_t target = {
		.gfx	     = &gfx,
		.origin	     = (gfx_image_origin_t)99,
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
		.image	     = &target,
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
	gfx_image_t target			       = {0};
	gfx_image_memory_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 2,
		.height = 1,
		.stride = 8,
	};
	EXPECT_PTR(t_gfx_vulkan_image_init_image_memory(&target, &gfx, &memory_target_config), &target);
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
	gfx_image_free(&target);
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

TEST(gfx_vulkan_pipeline_init_descriptor_set_layout_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_descriptor_set_layout_ret	  = 1;
	t_gfx_vulkan_shader_data_t vs_data	  = {.module = 1};
	t_gfx_vulkan_shader_data_t fs_data	  = {.module = 2};
	t_gfx_vulkan_render_pass_data_t pass_data = {.render_pass = 3};
	gfx_shader_t vs				  = {.gfx = &gfx, .data = &vs_data};
	gfx_shader_t fs				  = {.gfx = &gfx, .data = &fs_data};
	gfx_render_pass_t render_pass		  = {.gfx = &gfx, .data = &pass_data};
	gfx_pipeline_t pipeline			  = {.gfx = &gfx};
	gfx_pipeline_config_t pipeline_config	  = {
		    .render_pass       = &render_pass,
		    .vs		       = vs,
		    .fs		       = fs,
		    .input_layout      = t_gfx_vulkan_input_layout,
		    .input_layout_size = sizeof(t_gfx_vulkan_input_layout),
	    };

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);
	EXPECT_NULL(pipeline.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_descriptor_pool_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_create_descriptor_pool_ret		  = 1;
	t_gfx_vulkan_shader_data_t vs_data	  = {.module = 1};
	t_gfx_vulkan_shader_data_t fs_data	  = {.module = 2};
	t_gfx_vulkan_render_pass_data_t pass_data = {.render_pass = 3};
	gfx_shader_t vs				  = {.gfx = &gfx, .data = &vs_data};
	gfx_shader_t fs				  = {.gfx = &gfx, .data = &fs_data};
	gfx_render_pass_t render_pass		  = {.gfx = &gfx, .data = &pass_data};
	gfx_pipeline_t pipeline			  = {.gfx = &gfx};
	gfx_pipeline_config_t pipeline_config	  = {
		    .render_pass       = &render_pass,
		    .vs		       = vs,
		    .fs		       = fs,
		    .input_layout      = t_gfx_vulkan_input_layout,
		    .input_layout_size = sizeof(t_gfx_vulkan_input_layout),
	    };

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);
	EXPECT_EQ(t_vk_destroy_descriptor_set_layout_calls, 1);
	EXPECT_NULL(pipeline.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_descriptor_set_alloc_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_vulkan_init_gfx(&gfx, &proc), 0);
	t_vk_allocate_descriptor_sets_ret	  = 1;
	t_gfx_vulkan_shader_data_t vs_data	  = {.module = 1};
	t_gfx_vulkan_shader_data_t fs_data	  = {.module = 2};
	t_gfx_vulkan_render_pass_data_t pass_data = {.render_pass = 3};
	gfx_shader_t vs				  = {.gfx = &gfx, .data = &vs_data};
	gfx_shader_t fs				  = {.gfx = &gfx, .data = &fs_data};
	gfx_render_pass_t render_pass		  = {.gfx = &gfx, .data = &pass_data};
	gfx_pipeline_t pipeline			  = {.gfx = &gfx};
	gfx_pipeline_config_t pipeline_config	  = {
		    .render_pass       = &render_pass,
		    .vs		       = vs,
		    .fs		       = fs,
		    .input_layout      = t_gfx_vulkan_input_layout,
		    .input_layout_size = sizeof(t_gfx_vulkan_input_layout),
	    };

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);
	EXPECT_EQ(t_vk_destroy_descriptor_pool_calls, 1);
	EXPECT_EQ(t_vk_destroy_descriptor_set_layout_calls, 1);
	EXPECT_NULL(pipeline.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_creates_descriptors_direct)
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
		{.index = 1, .semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
	};
	gfx_pipeline_t pipeline		      = {.gfx = &gfx};
	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = layout,
		.input_layout_size = sizeof(layout),
	};

	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 0);
	EXPECT_EQ(t_vk_create_descriptor_set_layout_calls, 1);
	EXPECT_EQ(t_vk_descriptor_binding_count, 16);
	EXPECT_EQ(t_vk_create_descriptor_pool_calls, 1);
	EXPECT_EQ(t_vk_descriptor_pool_size_count, 1);
	EXPECT_EQ(t_vk_descriptor_pool_descriptor_count, 48);
	EXPECT_EQ(t_vk_allocate_descriptor_sets_calls, 1);
	EXPECT_EQ(t_vk_descriptor_set_count, 3);

	gfx.drv->pipeline_free(&pipeline);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_vulkan_pipeline_init_rejects_wireframe_without_feature_direct)
{
	START;

	t_vkReset();
	t_vk_fill_mode_non_solid = 0;
	proc_t proc		 = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_vulkan_symbols(&proc);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_vulkan_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);
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
		.raster		   = {.fill = GFX_FILL_WIREFRAME},
	};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);
	log_set_quiet(0, 0);
	EXPECT_NULL(pipeline.data);
	EXPECT_EQ(t_vk_create_graphics_pipelines_calls, 0);

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

TEST(gfx_vulkan_pipeline_init_rejects_too_many_layout_elements_direct)
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
		{.index = 0, .semantic = "POSITION", .count = 2, .type = GFX_VALUE_FLOAT32},
	};
	gfx_pipeline_t pipeline = {.gfx = &gfx};

	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = layout,
		.input_layout_size = ((size_t)U32_MAX + (size_t)1) * sizeof(gfx_layout_t),
	};
	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &pipeline_config), 1);
	log_set_quiet(0, 0);
	EXPECT_NULL(pipeline.data);

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
		{.index = 0, .semantic = "POSITION", .count = 1, .type = GFX_VALUE_FLOAT32},
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

TEST(gfx_vulkan_pipeline_init_rejects_large_layout_stride_direct)
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
		{.index = 0, .semantic = "POSITION", .count = U32_MAX, .type = GFX_VALUE_FLOAT32},
	};
	gfx_pipeline_t pipeline = {.gfx = &gfx};

	gfx_pipeline_config_t pipeline_config = {
		.render_pass	   = &render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = layout,
		.input_layout_size = sizeof(layout),
	};
	log_set_quiet(0, 1);
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
	RUN(gfx_vulkan_init_does_not_require_memory_target_format);
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
	RUN(gfx_vulkan_free_swapchain_target_without_swapchain_direct);
	RUN(gfx_vulkan_free_swapchain_target_destroys_cached_image_views_direct);
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
	RUN(gfx_vulkan_buffer_init_rejects_invalid_direct);
	RUN(gfx_vulkan_buffer_init_alloc_failure);
	RUN(gfx_vulkan_buffer_init_static_create_buffer_failure);
	RUN(gfx_vulkan_buffer_init_static_upload_failure);
	RUN(gfx_vulkan_buffer_init_static_uploads_data);
	RUN(gfx_vulkan_buffer_init_uniform_buffer);
	RUN(gfx_vulkan_buffer_free_null_data);
	RUN(gfx_vulkan_buffer_init_null_config);
	RUN(gfx_vulkan_buffer_set_data_create_buffer_failure);
	RUN(gfx_vulkan_buffer_set_data_memory_type_failure);
	RUN(gfx_vulkan_buffer_set_data_allocate_memory_failure);
	RUN(gfx_vulkan_buffer_set_data_bind_memory_failure);
	RUN(gfx_vulkan_buffer_bind_index_uses_uint32_indices);
	RUN(gfx_vulkan_buffer_bind_rejects_unknown_type);
	RUN(gfx_vulkan_buffer_bind_rejects_empty_storage);
	RUN(gfx_vulkan_uniform_buffer_bind_skips_vertex_input_bind);
	RUN(gfx_vulkan_bind_resources_updates_descriptor);
	RUN(gfx_vulkan_bind_resources_rejects_invalid_args);
	RUN(gfx_vulkan_buffer_set_data_null_data);
	RUN(gfx_vulkan_buffer_set_data_map_failure);
	RUN(gfx_vulkan_buffer_set_data_flushes_noncoherent_memory);
	RUN(gfx_vulkan_buffer_set_data_flush_failure);
	RUN(gfx_vulkan_buffer_set_data_grows_buffer);
	RUN(gfx_vulkan_buffer_set_data_rejects_unknown_type_direct);
	RUN(gfx_vulkan_buffer_set_data_rejects_empty_storage_direct);
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
	RUN(gfx_vulkan_swapchain_free_detaches_active_draw_target);
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
	RUN(gfx_vulkan_memory_pass_begin_missing_frame_sync_resource);
	RUN(gfx_vulkan_end_command_failure);
	RUN(gfx_vulkan_end_submit_failure);
	RUN(gfx_vulkan_end_submit_missing_frame_sync_resource);
	RUN(gfx_vulkan_end_submit_wait_failure);
	RUN(gfx_vulkan_end_finish_missing_target_data);
	RUN(gfx_vulkan_end_memory_requires_readback_buffer_direct);
	RUN(gfx_vulkan_draw_create_image_view_failure);
	RUN(gfx_vulkan_draw_create_framebuffer_failure);
	RUN(gfx_vulkan_draw_surface_framebuffer_failure);
	RUN(gfx_vulkan_end_null_frame);
	RUN(gfx_vulkan_end_inactive_frame);
	RUN(gfx_vulkan_end_surface_requires_swapchain_image_data);
	RUN(gfx_vulkan_end_surface_rejects_invalid_public_image_index);
	RUN(gfx_vulkan_present_null_data);
	RUN(gfx_vulkan_swapchain_acquire_invalid_direct);
	RUN(gfx_vulkan_swapchain_acquire_without_bound_target_failure_direct);
	RUN(gfx_vulkan_swapchain_acquire_bound_target_failure_direct);
	RUN(gfx_vulkan_swapchain_acquire_rejects_invalid_index_or_image_data_direct);
	RUN(gfx_vulkan_swapchain_acquire_rejects_missing_public_swapchain_data_direct);
	RUN(gfx_vulkan_swapchain_acquire_rejects_missing_image_available_direct);
	RUN(gfx_vulkan_swapchain_acquire_rejects_in_flight_fence_wait_failure_direct);
	RUN(gfx_vulkan_swapchain_present_requires_backend_acquire_direct);
	RUN(gfx_vulkan_swapchain_present_rejects_missing_render_finished_direct);
	RUN(gfx_vulkan_render_pass_init_rejects_unknown_format);
	RUN(gfx_vulkan_render_pass_init_alloc_failure);
	RUN(gfx_vulkan_render_pass_init_create_failure);
	RUN(gfx_vulkan_render_pass_init_uses_load_load);
	RUN(gfx_vulkan_render_pass_init_uses_supported_formats);
	RUN(gfx_vulkan_render_pass_init_uses_unknown_load_as_dont_care);
	RUN(gfx_vulkan_image_init_rejects_zero_size_direct);
	RUN(gfx_vulkan_image_init_rejects_unknown_type_direct);
	RUN(gfx_vulkan_surface_image_init_alloc_failure);
	RUN(gfx_vulkan_surface_image_init_validates_surface_metadata_direct);
	RUN(gfx_vulkan_swapchain_init_rejects_invalid_direct);
	RUN(gfx_vulkan_swapchain_init_uses_immediate_present_mode);
	RUN(gfx_vulkan_swapchain_init_uses_mailbox_present_mode);
	RUN(gfx_vulkan_swapchain_init_falls_back_to_vsync_present_mode);
	RUN(gfx_vulkan_swapchain_init_mailbox_falls_back_to_vsync_present_mode);
	RUN(gfx_vulkan_swapchain_init_rejects_invalid_present_mode);
	RUN(gfx_vulkan_swapchain_init_limits_present_modes);
	RUN(gfx_vulkan_swapchain_init_present_mode_count_failure);
	RUN(gfx_vulkan_swapchain_init_present_mode_list_failure);
	RUN(gfx_vulkan_surface_image_init_without_wsi);
	RUN(gfx_vulkan_surface_image_init_surface_support_failure);
	RUN(gfx_vulkan_surface_image_init_capabilities_failure);
	RUN(gfx_vulkan_surface_image_init_usage_failure);
	RUN(gfx_vulkan_surface_image_init_format_count_failure);
	RUN(gfx_vulkan_surface_image_init_format_list_failure);
	RUN(gfx_vulkan_surface_image_init_limits_surface_formats);
	RUN(gfx_vulkan_surface_image_init_undefined_format);
	RUN(gfx_vulkan_surface_image_init_prefers_supported_format);
	RUN(gfx_vulkan_surface_image_init_rejects_unsupported_format);
	RUN(gfx_vulkan_surface_image_init_clamps_extent_and_image_count);
	RUN(gfx_vulkan_surface_image_init_raises_min_image_count_to_surface_min);
	RUN(gfx_vulkan_surface_image_init_rejects_surface_min_above_user_capacity);
	RUN(gfx_vulkan_surface_image_init_uses_current_extent);
	RUN(gfx_vulkan_surface_image_init_rejects_native_image_count_above_capacity);
	RUN(gfx_vulkan_surface_image_init_rejects_native_image_count_above_user_max);
	RUN(gfx_vulkan_surface_image_init_chooses_nonopaque_alpha);
	RUN(gfx_vulkan_surface_image_init_native_image_array_alloc_failure);
	RUN(gfx_vulkan_surface_image_init_image_pointer_array_alloc_failure);
	RUN(gfx_vulkan_surface_image_init_image_payload_alloc_failure);
	RUN(gfx_vulkan_surface_image_init_rejects_missing_alpha);
	RUN(gfx_vulkan_surface_image_init_create_swapchain_failure);
	RUN(gfx_vulkan_surface_image_init_image_swapchain_count_failure);
	RUN(gfx_vulkan_surface_image_init_limits_swapchain_images);
	RUN(gfx_vulkan_surface_image_init_image_swapchain_list_failure);
	RUN(gfx_vulkan_swapchain_resize_recreates_swapchain);
	RUN(gfx_vulkan_swapchain_present_requires_acquired_swapchain);
	RUN(gfx_vulkan_swapchain_present_recreates_on_out_of_date);
	RUN(gfx_vulkan_swapchain_present_queue_failure);
	RUN(gfx_vulkan_memory_image_init_alloc_failure);
	RUN(gfx_vulkan_memory_image_init_create_image_failure);
	RUN(gfx_vulkan_memory_image_init_image_memory_type_failure);
	RUN(gfx_vulkan_memory_image_init_allocate_memory_failure);
	RUN(gfx_vulkan_memory_image_init_bind_memory_failure);
	RUN(gfx_vulkan_memory_image_init_uses_device_local_memory);
	RUN(gfx_vulkan_memory_image_init_readback_buffer_failure);
	RUN(gfx_vulkan_memory_image_init_readback_memory_type_failure);
	RUN(gfx_vulkan_memory_image_init_readback_allocate_memory_failure);
	RUN(gfx_vulkan_memory_image_init_readback_bind_memory_failure);
	RUN(gfx_vulkan_swapchain_resize_null_swapchain_direct);
	RUN(gfx_vulkan_image_free_null_target_direct);
	RUN(gfx_vulkan_image_read_null_target_direct);
	RUN(gfx_vulkan_image_read_requires_bound_target);
	RUN(gfx_vulkan_image_read_noncoherent_memory);
	RUN(gfx_vulkan_image_read_invalidate_failure);
	RUN(gfx_vulkan_image_read_map_failure);
	RUN(gfx_vulkan_framebuffer_init_null_direct);
	RUN(gfx_vulkan_framebuffer_init_alloc_failure);
	RUN(gfx_vulkan_framebuffer_init_memory_requires_image);
	RUN(gfx_vulkan_framebuffer_init_surface_requires_swapchain_image);
	RUN(gfx_vulkan_framebuffer_init_surface_requires_swapchain_count);
	RUN(gfx_vulkan_framebuffer_init_surface_requires_swapchain_data_direct);
	RUN(gfx_vulkan_framebuffer_init_surface_framebuffer_array_alloc_failure);
	RUN(gfx_vulkan_framebuffer_init_surface_image_view_failure);
	RUN(gfx_vulkan_framebuffer_depth_pass_begin);
	RUN(gfx_vulkan_framebuffer_depth_memory_type_fallback);
	RUN(gfx_vulkan_framebuffer_depth_attachment_failures);
	RUN(gfx_vulkan_framebuffer_init_unknown_target_type_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_requires_target_data_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_surface_requires_target_data_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_surface_requires_swapchain_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_memory_requires_handles_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_surface_requires_framebuffer_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_surface_requires_swapchain_framebuffer_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_unknown_target_type_direct);
	RUN(gfx_vulkan_framebuffer_pass_begin_rejects_active_frame);
	RUN(gfx_vulkan_pipeline_init_rejects_invalid_config_direct);
	RUN(gfx_vulkan_pipeline_init_alloc_failure_direct);
	RUN(gfx_vulkan_pipeline_init_create_layout_failure_direct);
	RUN(gfx_vulkan_pipeline_init_descriptor_set_layout_failure_direct);
	RUN(gfx_vulkan_pipeline_init_descriptor_pool_failure_direct);
	RUN(gfx_vulkan_pipeline_init_descriptor_set_alloc_failure_direct);
	RUN(gfx_vulkan_pipeline_init_creates_descriptors_direct);
	RUN(gfx_vulkan_pipeline_init_rejects_wireframe_without_feature_direct);
	RUN(gfx_vulkan_pipeline_init_attribute_alloc_failure_direct);
	RUN(gfx_vulkan_pipeline_init_rejects_too_many_layout_elements_direct);
	RUN(gfx_vulkan_pipeline_init_unsupported_layout_direct);
	RUN(gfx_vulkan_pipeline_init_rejects_large_layout_stride_direct);
	RUN(gfx_vulkan_pipeline_init_create_pipeline_failure_direct);

	t_gfx_vulkan_compiler_free();

	SEND;
}
