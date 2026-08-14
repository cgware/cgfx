#include "gfx_driver.h"

#include "log.h"
#include "mem.h"
#include "vulkan.h"

enum {
	GFX_VULKAN_MAX_UNIFORM_BINDINGS = 16,
	GFX_VULKAN_MAX_DESCRIPTOR_SETS	= 64,
};

typedef struct gfx_vulkan_buffer_resource_s {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize size;
	VkDeviceSize memory_size;
	int memory_coherent;
} gfx_vulkan_buffer_resource_t;

typedef struct gfx_vulkan_frame_s {
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
} gfx_vulkan_frame_t;

typedef struct gfx_vulkan_s {
	void *lib;
	gfx_image_t *image;
	gfx_swapchain_t *swapchain;
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice device;
	VkQueue queue;
	u32 queue_family;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	VkFence fence;
	VkClearValue clear_color;
	float clear_depth;
	gfx_vulkan_frame_t frame;
	int surface_enabled;
	int swapchain_enabled;
	int fill_mode_non_solid;
	PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
	PFN_vkGetDeviceProcAddr GetDeviceProcAddr;
	PFN_vkDestroyInstance DestroyInstance;
	PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties GetPhysicalDeviceQueueFamilyProperties;
	PFN_vkGetPhysicalDeviceFeatures GetPhysicalDeviceFeatures;
	PFN_vkGetPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties;
	PFN_vkGetPhysicalDeviceFormatProperties GetPhysicalDeviceFormatProperties;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR GetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR GetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR GetPhysicalDeviceSurfacePresentModesKHR;
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
	PFN_vkCmdCopyImageToBuffer CmdCopyImageToBuffer;
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
	PFN_vkCreateDescriptorSetLayout CreateDescriptorSetLayout;
	PFN_vkDestroyDescriptorSetLayout DestroyDescriptorSetLayout;
	PFN_vkCreateDescriptorPool CreateDescriptorPool;
	PFN_vkDestroyDescriptorPool DestroyDescriptorPool;
	PFN_vkAllocateDescriptorSets AllocateDescriptorSets;
	PFN_vkUpdateDescriptorSets UpdateDescriptorSets;
	PFN_vkCreateGraphicsPipelines CreateGraphicsPipelines;
	PFN_vkDestroyPipeline DestroyPipeline;
	PFN_vkCmdBeginRenderPass CmdBeginRenderPass;
	PFN_vkCmdEndRenderPass CmdEndRenderPass;
	PFN_vkCmdBindPipeline CmdBindPipeline;
	PFN_vkCmdBindVertexBuffers CmdBindVertexBuffers;
	PFN_vkCmdBindIndexBuffer CmdBindIndexBuffer;
	PFN_vkCmdBindDescriptorSets CmdBindDescriptorSets;
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
	int depth;
} gfx_vulkan_render_pass_t;

typedef struct gfx_vulkan_memory_target_s {
	VkImage image;
	VkDeviceMemory image_memory;
	gfx_vulkan_buffer_resource_t readback;
	VkImageView image_view;
} gfx_vulkan_memory_target_t;

typedef struct gfx_vulkan_swapchain_image_s {
	VkImage image;
	VkImageView view;
	VkImageLayout layout;
} gfx_vulkan_swapchain_image_t;

typedef struct gfx_vulkan_swapchain_s {
	VkSwapchainKHR swapchain;
	u32 image_count;
	u32 image_index;
	int acquired;
} gfx_vulkan_swapchain_t;

typedef struct gfx_vulkan_framebuffer_s {
	VkFramebuffer framebuffer;
	VkFramebuffer *swapchain_framebuffers;
	VkImage depth_image;
	VkDeviceMemory depth_memory;
	VkImageView depth_view;
	VkImageLayout depth_layout;
	u32 swapchain_framebuffer_count;
} gfx_vulkan_framebuffer_t;

typedef struct gfx_vulkan_buffer_s {
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDeviceSize size;
	VkDeviceSize memory_size;
	int memory_coherent;
	gfx_buffer_type_t type;
} gfx_vulkan_buffer_t;

typedef struct gfx_vulkan_shader_s {
	VkShaderModule module;
} gfx_vulkan_shader_t;

typedef struct gfx_vulkan_pipeline_s {
	VkDescriptorSetLayout descriptor_set_layout;
	VkDescriptorPool descriptor_pool;
	VkDescriptorSet descriptor_sets[GFX_VULKAN_MAX_DESCRIPTOR_SETS];
	u32 descriptor_set_index;
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

static int gfx_vulkan_acquire_swapchain(gfx_vulkan_t *vulkan, gfx_swapchain_t *swapchain);

static int extension_enabled(const char *const *extensions, u32 count, const char *name)
{
	for (u32 i = 0; i < count; i++) {
		if (strv_eq(strv_cstr(extensions[i]), strv_cstr(name))) {
			return 1;
		}
	}

	return 0;
}

static void gfx_vulkan_swapchain_images_free(gfx_vulkan_t *vulkan, gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL) {
		return; // LCOV_EXCL_LINE
	}

	for (u32 i = 0; i < swapchain->image_capacity; i++) {
		gfx_vulkan_swapchain_image_t *image = swapchain->images[i].driver_data;
		if (image == NULL) {
			continue;
		}
		if (image->view != 0) {
			vulkan->DestroyImageView(vulkan->device, image->view, NULL);
		}
		alloc_free(&swapchain->gfx->alloc, image, sizeof(*image));
		swapchain->images[i].driver_data = NULL;
	}
}

static int gfx_vulkan_swapchain_images_desc(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->images == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	for (u32 i = 0; i < swapchain->image_capacity; i++) {
		void *driver_data = swapchain->images[i].driver_data;
		if (i >= swapchain->image_count) {
			swapchain->images[i] = (gfx_image_t){0};
			continue;
		}
		swapchain->images[i] = (gfx_image_t){
			.gfx	     = swapchain->gfx,
			.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
			.format	     = swapchain->format,
			.driver_data = driver_data,
			.swapchain   = swapchain,
			.width	     = swapchain->width,
			.height	     = swapchain->height,
			.usage	     = swapchain->usage | GFX_IMAGE_USAGE_PRESENT,
			.index	     = i,
			.generation  = swapchain->generation,
		};
	}
	return 0;
}

static void gfx_vulkan_swapchain_data_free(gfx_vulkan_t *vulkan, gfx_swapchain_t *swapchain)
{
	gfx_vulkan_swapchain_t *vk_swapchain = swapchain != NULL ? swapchain->data : NULL;
	if (vk_swapchain == NULL) {
		return; // LCOV_EXCL_LINE
	}

	gfx_vulkan_swapchain_images_free(vulkan, swapchain);
	if (vk_swapchain->swapchain != 0) {
		if (vulkan->DeviceWaitIdle != NULL) {
			vulkan->DeviceWaitIdle(vulkan->device);
		}
		vulkan->DestroySwapchainKHR(vulkan->device, vk_swapchain->swapchain, NULL);
	}
	vk_swapchain->swapchain	  = 0;
	vk_swapchain->image_count = 0;
	vk_swapchain->image_index = 0;
	vk_swapchain->acquired	  = 0;
}

static void gfx_vulkan_draw_target_free(gfx_vulkan_t *vulkan, gfx_vulkan_memory_target_t *target)
{
	if (target->image_view != 0) {
		vulkan->DestroyImageView(vulkan->device, target->image_view, NULL);
		target->image_view = 0;
	}
}

static void gfx_vulkan_swapchain_draw_targets_free(gfx_vulkan_t *vulkan, gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL) {
		return;
	}

	for (u32 i = 0; i < swapchain->image_count; i++) {
		gfx_vulkan_swapchain_image_t *image = swapchain->images[i].driver_data;
		if (image != NULL && image->view != 0) {
			vulkan->DestroyImageView(vulkan->device, image->view, NULL);
			image->view = 0;
		}
	}
}

static void gfx_vulkan_draw_resources_free(gfx_vulkan_t *vulkan)
{
	vulkan->frame = (gfx_vulkan_frame_t){0};
	if (vulkan->image == NULL || vulkan->image->driver_data == NULL) {
		return;
	}
	if (vulkan->image->origin == GFX_IMAGE_ORIGIN_MEMORY) {
		gfx_vulkan_draw_target_free(vulkan, vulkan->image->driver_data);
	} else if (vulkan->image->origin == GFX_IMAGE_ORIGIN_SURFACE) {
		gfx_vulkan_swapchain_draw_targets_free(vulkan, vulkan->image->swapchain);
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
	if (target->image_memory != 0) {
		vulkan->FreeMemory(vulkan->device, target->image_memory, NULL);
		target->image_memory = 0;
	}
	if (target->readback.buffer != 0) {
		vulkan->DestroyBuffer(vulkan->device, target->readback.buffer, NULL);
		target->readback.buffer = 0;
	}
	if (target->readback.memory != 0) {
		vulkan->FreeMemory(vulkan->device, target->readback.memory, NULL);
		target->readback.memory = 0;
	}
	target->readback = (gfx_vulkan_buffer_resource_t){0};
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
	VkPhysicalDeviceFeatures features = {
		.fillModeNonSolid = vulkan->fill_mode_non_solid,
	};
	VkDeviceCreateInfo create = {
		.sType			 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount	 = 1,
		.pQueueCreateInfos	 = &queue,
		.enabledExtensionCount	 = device_extension_count,
		.ppEnabledExtensionNames = device_extensions,
		.pEnabledFeatures	 = &features,
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
	    LOAD_VK_DEV(vulkan, CmdPipelineBarrier) || LOAD_VK_DEV(vulkan, CmdClearColorImage) ||
	    LOAD_VK_DEV(vulkan, CmdCopyImageToBuffer) || LOAD_VK_DEV(vulkan, CreateImageView) || LOAD_VK_DEV(vulkan, DestroyImageView) ||
	    LOAD_VK_DEV(vulkan, CreateShaderModule) || LOAD_VK_DEV(vulkan, DestroyShaderModule) || LOAD_VK_DEV(vulkan, CreateRenderPass) ||
	    LOAD_VK_DEV(vulkan, DestroyRenderPass) || LOAD_VK_DEV(vulkan, CreateFramebuffer) || LOAD_VK_DEV(vulkan, DestroyFramebuffer) ||
	    LOAD_VK_DEV(vulkan, CreatePipelineLayout) || LOAD_VK_DEV(vulkan, DestroyPipelineLayout) ||
	    LOAD_VK_DEV(vulkan, CreateDescriptorSetLayout) || LOAD_VK_DEV(vulkan, DestroyDescriptorSetLayout) ||
	    LOAD_VK_DEV(vulkan, CreateDescriptorPool) || LOAD_VK_DEV(vulkan, DestroyDescriptorPool) ||
	    LOAD_VK_DEV(vulkan, AllocateDescriptorSets) || LOAD_VK_DEV(vulkan, UpdateDescriptorSets) ||
	    LOAD_VK_DEV(vulkan, CreateGraphicsPipelines) || LOAD_VK_DEV(vulkan, DestroyPipeline) ||
	    LOAD_VK_DEV(vulkan, CmdBeginRenderPass) || LOAD_VK_DEV(vulkan, CmdEndRenderPass) || LOAD_VK_DEV(vulkan, CmdBindPipeline) ||
	    LOAD_VK_DEV(vulkan, CmdBindVertexBuffers) || LOAD_VK_DEV(vulkan, CmdBindIndexBuffer) ||
	    LOAD_VK_DEV(vulkan, CmdBindDescriptorSets) || LOAD_VK_DEV(vulkan, CmdSetViewport) || LOAD_VK_DEV(vulkan, CmdSetScissor) ||
	    LOAD_VK_DEV(vulkan, CmdDraw) || LOAD_VK_DEV(vulkan, CmdDrawIndexed) || LOAD_VK_DEV(vulkan, QueueSubmit)) {
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
		.clear_color = {.color = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}}},
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
	    LOAD_VK_INST(vulkan, GetPhysicalDeviceQueueFamilyProperties) || LOAD_VK_INST(vulkan, GetPhysicalDeviceFeatures) ||
	    LOAD_VK_INST(vulkan, GetPhysicalDeviceMemoryProperties) || LOAD_VK_INST(vulkan, GetPhysicalDeviceFormatProperties) ||
	    LOAD_VK_INST(vulkan, CreateDevice) || LOAD_VK_INST(vulkan, DestroyDevice) || LOAD_VK_INST(vulkan, GetDeviceProcAddr)) {
		return gfx_vulkan_init_free(gfx, vulkan);
	}
	if (vulkan->surface_enabled &&
	    (LOAD_VK_INST(vulkan, GetPhysicalDeviceSurfaceSupportKHR) || LOAD_VK_INST(vulkan, GetPhysicalDeviceSurfaceCapabilitiesKHR) ||
	     LOAD_VK_INST(vulkan, GetPhysicalDeviceSurfaceFormatsKHR) || LOAD_VK_INST(vulkan, GetPhysicalDeviceSurfacePresentModesKHR))) {
		return gfx_vulkan_init_free(gfx, vulkan);
	}

	if (gfx_vulkan_pick_device(vulkan)) {
		return gfx_vulkan_init_free(gfx, vulkan);
	}
	VkPhysicalDeviceFeatures features = {0};
	vulkan->GetPhysicalDeviceFeatures(vulkan->physical_device, &features);
	vulkan->fill_mode_non_solid = features.fillModeNonSolid != 0;
	if (gfx_vulkan_create_device(gfx, config->plan)) {
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
	case GFX_FORMAT_D32_FLOAT:
		return VK_FORMAT_D32_SFLOAT;
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
	    gfx_vulkan_format(config->color_format) == 0 ||
	    (config->depth_format != GFX_FORMAT_NONE && gfx_vulkan_format(config->depth_format) == 0)) {
		return 1;
	}

	gfx_vulkan_render_pass_t *vk_render_pass = alloc_alloc(&render_pass->gfx->alloc, sizeof(gfx_vulkan_render_pass_t));
	if (vk_render_pass == NULL) {
		return 1;
	}
	*vk_render_pass	  = (gfx_vulkan_render_pass_t){0};
	render_pass->data = vk_render_pass;

	gfx_vulkan_t *vulkan = render_pass->gfx->data;

	VkAttachmentDescription attachments[2] = {
		{
			.format		= gfx_vulkan_format(config->color_format),
			.samples	= VK_SAMPLE_COUNT_1_BIT,
			.loadOp		= gfx_vulkan_load_op(config->load),
			.storeOp	= gfx_vulkan_store_op(config->store),
			.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.finalLayout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		},
	};
	if (config->depth_format != GFX_FORMAT_NONE) {
		attachments[1] = (VkAttachmentDescription){
			.format		= gfx_vulkan_format(config->depth_format),
			.samples	= VK_SAMPLE_COUNT_1_BIT,
			.loadOp		= gfx_vulkan_load_op(config->depth_load),
			.storeOp	= gfx_vulkan_store_op(config->depth_store),
			.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout	= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.finalLayout	= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		};
		vk_render_pass->depth = 1;
	}
	VkAttachmentReference color = {
		.attachment = 0,
		.layout	    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	VkAttachmentReference depth = {
		.attachment = 1,
		.layout	    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};
	VkSubpassDescription subpass = {
		.pipelineBindPoint	 = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount	 = 1,
		.pColorAttachments	 = &color,
		.pDepthStencilAttachment = config->depth_format != GFX_FORMAT_NONE ? &depth : NULL,
	};
	VkRenderPassCreateInfo create = {
		.sType		 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = config->depth_format != GFX_FORMAT_NONE ? 2 : 1,
		.pAttachments	 = attachments,
		.subpassCount	 = 1,
		.pSubpasses	 = &subpass,
	};

	if (!vk_ok(vulkan->CreateRenderPass(vulkan->device, &create, NULL, &vk_render_pass->render_pass))) {
		gfx_vulkan_render_pass_free(render_pass);
		return 1;
	}

	return 0;
}

static int image_valid(const gfx_image_t *image)
{
	if (image == NULL || image->width == 0 || image->height == 0) {
		return 0;
	}
	if (image->origin == GFX_IMAGE_ORIGIN_MEMORY) {
		return image->format == GFX_FORMAT_RGBA8_UNORM && image->data != NULL && image->stride >= (size_t)image->width * 4;
	}
	if (image->origin == GFX_IMAGE_ORIGIN_SURFACE) {
		return image->swapchain != NULL && image->format == image->swapchain->format && image->width == image->swapchain->width &&
		       image->height == image->swapchain->height && gfx_vulkan_format(image->format) != 0 &&
		       image->swapchain->surface != NULL && image->swapchain->surface->api == GFX_API_VULKAN &&
		       image->swapchain->surface->handle != 0;
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

static int gfx_vulkan_present_mode_supported(const VkPresentModeKHR *modes, u32 count, VkPresentModeKHR mode)
{
	for (u32 i = 0; i < count; i++) {
		if (modes[i] == mode) {
			return 1;
		}
	}

	return 0;
}

static VkPresentModeKHR gfx_vulkan_present_mode_choose(const VkPresentModeKHR *modes, u32 count, gfx_present_mode_t requested,
						       gfx_present_mode_t *actual)
{
	switch (requested) {
	case GFX_PRESENT_MODE_IMMEDIATE:
		if (gfx_vulkan_present_mode_supported(modes, count, VK_PRESENT_MODE_IMMEDIATE_KHR)) {
			*actual = GFX_PRESENT_MODE_IMMEDIATE;
			return VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
		break;
	case GFX_PRESENT_MODE_MAILBOX:
		if (gfx_vulkan_present_mode_supported(modes, count, VK_PRESENT_MODE_MAILBOX_KHR)) {
			*actual = GFX_PRESENT_MODE_MAILBOX;
			return VK_PRESENT_MODE_MAILBOX_KHR;
		}
		break;
	case GFX_PRESENT_MODE_DEFAULT:
	case GFX_PRESENT_MODE_VSYNC:
		break;
	}

	*actual = GFX_PRESENT_MODE_VSYNC;
	return VK_PRESENT_MODE_FIFO_KHR;
}

static int gfx_vulkan_present_mode(gfx_vulkan_t *vulkan, VkSurfaceKHR surface, gfx_present_mode_t requested, VkPresentModeKHR *mode,
				   gfx_present_mode_t *actual)
{
	u32 count = 0;
	if (!vk_ok(vulkan->GetPhysicalDeviceSurfacePresentModesKHR(vulkan->physical_device, surface, &count, NULL)) || count == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan present modes");
		return 1;
	}

	VkPresentModeKHR modes[16] = {0};
	if (count > sizeof(modes) / sizeof(modes[0])) {
		count = sizeof(modes) / sizeof(modes[0]);
	}
	if (!vk_ok(vulkan->GetPhysicalDeviceSurfacePresentModesKHR(vulkan->physical_device, surface, &count, modes))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan present modes");
		return 1;
	}

	*mode = gfx_vulkan_present_mode_choose(modes, count, requested, actual);
	return 0;
}

static void gfx_vulkan_image_free(gfx_image_t *image)
{
	if (image == NULL || image->gfx == NULL || image->gfx->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan = image->gfx->data;
	if (image->driver_data != NULL) {
		if (image->origin == GFX_IMAGE_ORIGIN_SURFACE) {
			image->driver_data = NULL;
		} else {
			gfx_vulkan_memory_target_t *vk_target = image->driver_data;
			gfx_vulkan_memory_target_free(vulkan, vk_target);
			alloc_free(&image->gfx->alloc, vk_target, sizeof(gfx_vulkan_memory_target_t));
			image->driver_data = NULL;
		}
	}
	if (vulkan->image == image) {
		vulkan->image	  = NULL;
		vulkan->swapchain = NULL;
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
	VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
	if (gfx_vulkan_present_mode(vulkan, surface, swapchain->present_mode, &present_mode, &swapchain->actual_present_mode)) {
		return 1;
	}

	u32 image_count = swapchain->min_image_count;
	if (image_count < caps.minImageCount) {
		image_count = caps.minImageCount;
	}
	if ((swapchain->max_image_count != 0 && image_count > swapchain->max_image_count) ||
	    (caps.maxImageCount != 0 && image_count > caps.maxImageCount) || image_count > swapchain->image_capacity) {
		return 1;
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
		.presentMode	  = present_mode,
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

	u32 native_image_count = 0;
	if (!vk_ok(vulkan->GetSwapchainImagesKHR(vulkan->device, new_swapchain, &native_image_count, NULL)) || native_image_count == 0) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan swapchain images");
		vulkan->DestroySwapchainKHR(vulkan->device, new_swapchain, NULL);
		return 1;
	}
	if (native_image_count > swapchain->image_capacity) {
		vulkan->DestroySwapchainKHR(vulkan->device, new_swapchain, NULL);
		return 1;
	}
	u32 native_image_capacity = native_image_count;
	VkImage *native_images	  = alloc_alloc(&swapchain->gfx->alloc, native_image_capacity * sizeof(*native_images));
	if (native_images == NULL) {
		vulkan->DestroySwapchainKHR(vulkan->device, new_swapchain, NULL);
		return 1;
	}
	if (!vk_ok(vulkan->GetSwapchainImagesKHR(vulkan->device, new_swapchain, &native_image_count, native_images))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to enumerate Vulkan swapchain images");
		alloc_free(&swapchain->gfx->alloc, native_images, native_image_capacity * sizeof(*native_images));
		vulkan->DestroySwapchainKHR(vulkan->device, new_swapchain, NULL);
		return 1;
	}
	if (native_image_count > native_image_capacity || native_image_count > swapchain->image_capacity ||
	    (swapchain->max_image_count != 0 && native_image_count > swapchain->max_image_count)) {
		alloc_free(&swapchain->gfx->alloc, native_images, native_image_capacity * sizeof(*native_images));
		vulkan->DestroySwapchainKHR(vulkan->device, new_swapchain, NULL);
		return 1;
	}

	gfx_vulkan_swapchain_image_t **images = alloc_alloc(&swapchain->gfx->alloc, native_image_count * sizeof(images[0]));
	if (images == NULL) {
		alloc_free(&swapchain->gfx->alloc, native_images, native_image_capacity * sizeof(*native_images));
		vulkan->DestroySwapchainKHR(vulkan->device, new_swapchain, NULL);
		return 1;
	}
	mem_set(images, 0, native_image_count * sizeof(images[0]));
	for (u32 i = 0; i < native_image_count; i++) {
		images[i] = alloc_alloc(&swapchain->gfx->alloc, sizeof(*images[i]));
		if (images[i] == NULL) {
			for (u32 j = 0; j < i; j++) {
				alloc_free(&swapchain->gfx->alloc, images[j], sizeof(*images[j]));
			}
			alloc_free(&swapchain->gfx->alloc, images, native_image_count * sizeof(images[0]));
			alloc_free(&swapchain->gfx->alloc, native_images, native_image_capacity * sizeof(*native_images));
			vulkan->DestroySwapchainKHR(vulkan->device, new_swapchain, NULL);
			return 1;
		}
		*images[i] = (gfx_vulkan_swapchain_image_t){.image = native_images[i]};
	}
	alloc_free(&swapchain->gfx->alloc, native_images, native_image_capacity * sizeof(*native_images));

	VkSwapchainKHR old_swapchain = vk_swapchain->swapchain;
	if (old_swapchain != 0) {
		if (vulkan->DeviceWaitIdle != NULL) {
			vulkan->DeviceWaitIdle(vulkan->device);
		}
		gfx_vulkan_swapchain_images_free(vulkan, swapchain);
		vulkan->DestroySwapchainKHR(vulkan->device, old_swapchain, NULL);
	}
	vk_swapchain->swapchain = new_swapchain;
	for (u32 i = 0; i < native_image_count; i++) {
		swapchain->images[i].driver_data = images[i];
	}
	alloc_free(&swapchain->gfx->alloc, images, native_image_count * sizeof(images[0]));
	vk_swapchain->image_count = native_image_count;
	vk_swapchain->image_index = 0;
	vk_swapchain->acquired	  = 0;
	swapchain->format	  = target_format;
	swapchain->width	  = (u16)extent.width;
	swapchain->height	  = (u16)extent.height;
	swapchain->image_count	  = native_image_count;
	return 0;
}

static void gfx_vulkan_swapchain_free(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->data == NULL || swapchain->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan		     = swapchain->gfx->data;
	gfx_vulkan_swapchain_t *vk_swapchain = swapchain->data;
	if (vulkan->swapchain == swapchain ||
	    (vulkan->image != NULL && vulkan->image->origin == GFX_IMAGE_ORIGIN_SURFACE && vulkan->image->swapchain == swapchain)) {
		vulkan->frame	  = (gfx_vulkan_frame_t){0};
		vulkan->image	  = NULL;
		vulkan->swapchain = NULL;
	}
	gfx_vulkan_swapchain_data_free(vulkan, swapchain);
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

static int gfx_vulkan_memory_image_init(gfx_image_t *image)
{
	gfx_vulkan_t *vulkan		      = image->gfx->data;
	gfx_vulkan_memory_target_t *vk_target = alloc_alloc(&image->gfx->alloc, sizeof(gfx_vulkan_memory_target_t));
	if (vk_target == NULL) {
		return 1;
	}
	*vk_target	   = (gfx_vulkan_memory_target_t){0};
	image->driver_data = vk_target;

	VkImageCreateInfo create = {
		.sType	       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType     = VK_IMAGE_TYPE_2D,
		.format	       = VK_FORMAT_R8G8B8A8_UNORM,
		.extent	       = {.width = image->width, .height = image->height, .depth = 1},
		.mipLevels     = 1,
		.arrayLayers   = 1,
		.samples       = VK_SAMPLE_COUNT_1_BIT,
		.tiling	       = VK_IMAGE_TILING_OPTIMAL,
		.usage	       = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};
	if (!vk_ok(vulkan->CreateImage(vulkan->device, &create, NULL, &vk_target->image))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan image");
		return 1;
	}

	VkMemoryRequirements req = {0};
	vulkan->GetImageMemoryRequirements(vulkan->device, vk_target->image, &req);

	u32 memory_type				      = 0;
	VkPhysicalDeviceMemoryProperties memory_props = {0};
	vulkan->GetPhysicalDeviceMemoryProperties(vulkan->physical_device, &memory_props);
	int image_memory_found = 0;
	for (u32 i = 0; i < memory_props.memoryTypeCount; i++) {
		if ((req.memoryTypeBits & (1u << i)) && (memory_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
			memory_type	   = i;
			image_memory_found = 1;
			break;
		}
	}
	if (!image_memory_found) {
		for (u32 i = 0; i < memory_props.memoryTypeCount; i++) {
			if (req.memoryTypeBits & (1u << i)) {
				memory_type	   = i;
				image_memory_found = 1;
				break;
			}
		}
	}
	if (!image_memory_found) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to find Vulkan image memory");
		gfx_vulkan_image_free(image);
		return 1;
	}

	VkMemoryAllocateInfo memory = {
		.sType		 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize	 = req.size,
		.memoryTypeIndex = memory_type,
	};
	if (!vk_ok(vulkan->AllocateMemory(vulkan->device, &memory, NULL, &vk_target->image_memory))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to allocate Vulkan image memory");
		gfx_vulkan_image_free(image);
		return 1;
	}

	if (!vk_ok(vulkan->BindImageMemory(vulkan->device, vk_target->image, vk_target->image_memory, 0))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to bind Vulkan image memory");
		gfx_vulkan_image_free(image);
		return 1;
	}

	vk_target->readback.size  = (VkDeviceSize)image->width * image->height * 4;
	VkBufferCreateInfo buffer = {
		.sType	     = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size	     = vk_target->readback.size,
		.usage	     = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	if (!vk_ok(vulkan->CreateBuffer(vulkan->device, &buffer, NULL, &vk_target->readback.buffer))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to create Vulkan readback buffer");
		gfx_vulkan_image_free(image);
		return 1;
	}
	vulkan->GetBufferMemoryRequirements(vulkan->device, vk_target->readback.buffer, &req);
	if (memory_type_find(vulkan, req.memoryTypeBits, &memory_type, &vk_target->readback.memory_coherent)) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to find host visible Vulkan readback memory");
		gfx_vulkan_image_free(image);
		return 1;
	}
	memory.allocationSize  = req.size;
	memory.memoryTypeIndex = memory_type;
	if (!vk_ok(vulkan->AllocateMemory(vulkan->device, &memory, NULL, &vk_target->readback.memory))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to allocate Vulkan readback memory");
		gfx_vulkan_image_free(image);
		return 1;
	}
	vk_target->readback.memory_size = req.size;
	if (!vk_ok(vulkan->BindBufferMemory(vulkan->device, vk_target->readback.buffer, vk_target->readback.memory, 0))) {
		log_error("cgfx", "gfx_vulkan", NULL, "failed to bind Vulkan readback memory");
		gfx_vulkan_image_free(image);
		return 1;
	}

	return 0;
}

static int gfx_vulkan_image_init(gfx_image_t *image)
{
	if (image == NULL || image->gfx == NULL || image->gfx->data == NULL || !image_valid(image)) {
		return 1;
	}

	if (image->origin == GFX_IMAGE_ORIGIN_MEMORY) {
		return gfx_vulkan_memory_image_init(image);
	}

	return image->driver_data == NULL;
}

static int gfx_vulkan_create_image_view(gfx_vulkan_t *vulkan, const gfx_image_t *image, VkImage vk_image, VkImageView *view)
{
	VkImageViewCreateInfo create = {
		.sType	  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image	  = vk_image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format	  = gfx_vulkan_format(image->format),
		.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.levelCount = 1,
				.layerCount = 1,
			},
	};
	return !vk_ok(vulkan->CreateImageView(vulkan->device, &create, NULL, view));
}

static int gfx_vulkan_create_depth_view(gfx_vulkan_t *vulkan, VkImage image, VkImageView *view)
{
	VkImageViewCreateInfo create = {
		.sType	  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image	  = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format	  = VK_FORMAT_D32_SFLOAT,
		.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
				.levelCount = 1,
				.layerCount = 1,
			},
	};
	return !vk_ok(vulkan->CreateImageView(vulkan->device, &create, NULL, view));
}

static int gfx_vulkan_create_depth_attachment(gfx_vulkan_t *vulkan, gfx_framebuffer_t *framebuffer,
					      gfx_vulkan_framebuffer_t *vk_framebuffer)
{
	VkImageCreateInfo create = {
		.sType	       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType     = VK_IMAGE_TYPE_2D,
		.format	       = VK_FORMAT_D32_SFLOAT,
		.extent	       = {.width = framebuffer->width, .height = framebuffer->height, .depth = 1},
		.mipLevels     = 1,
		.arrayLayers   = 1,
		.samples       = VK_SAMPLE_COUNT_1_BIT,
		.tiling	       = VK_IMAGE_TILING_OPTIMAL,
		.usage	       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		.sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};
	if (!vk_ok(vulkan->CreateImage(vulkan->device, &create, NULL, &vk_framebuffer->depth_image))) {
		return 1;
	}

	VkMemoryRequirements req = {0};
	vulkan->GetImageMemoryRequirements(vulkan->device, vk_framebuffer->depth_image, &req);
	u32 memory_type				      = 0;
	VkPhysicalDeviceMemoryProperties memory_props = {0};
	vulkan->GetPhysicalDeviceMemoryProperties(vulkan->physical_device, &memory_props);
	int found = 0;
	for (u32 i = 0; i < memory_props.memoryTypeCount; i++) {
		if ((req.memoryTypeBits & (1u << i)) && (memory_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
			memory_type = i;
			found	    = 1;
			break;
		}
	}
	if (!found) {
		for (u32 i = 0; i < memory_props.memoryTypeCount; i++) {
			if (req.memoryTypeBits & (1u << i)) {
				memory_type = i;
				found	    = 1;
				break;
			}
		}
	}
	if (!found) {
		return 1;
	}

	VkMemoryAllocateInfo memory = {
		.sType		 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize	 = req.size,
		.memoryTypeIndex = memory_type,
	};
	if (!vk_ok(vulkan->AllocateMemory(vulkan->device, &memory, NULL, &vk_framebuffer->depth_memory)) ||
	    !vk_ok(vulkan->BindImageMemory(vulkan->device, vk_framebuffer->depth_image, vk_framebuffer->depth_memory, 0)) ||
	    gfx_vulkan_create_depth_view(vulkan, vk_framebuffer->depth_image, &vk_framebuffer->depth_view)) {
		return 1;
	}
	return 0;
}

static int gfx_vulkan_copy_memory(gfx_vulkan_t *vulkan, const gfx_memory_readback_config_t *config)
{
	gfx_vulkan_memory_target_t *target = vulkan->image != NULL ? vulkan->image->driver_data : NULL;
	if (target == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	void *mapped = NULL;
	if (!vk_ok(vulkan->MapMemory(vulkan->device, target->readback.memory, 0, target->readback.memory_size, 0, &mapped))) {
		return 1;
	}
	if (!target->readback.memory_coherent) {
		VkMappedMemoryRange range = {
			.sType	= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.memory = target->readback.memory,
			.offset = 0,
			.size	= target->readback.memory_size,
		};
		if (!vk_ok(vulkan->InvalidateMappedMemoryRanges(vulkan->device, 1, &range))) {
			vulkan->UnmapMemory(vulkan->device, target->readback.memory);
			return 1;
		}
	}

	u8 *src = mapped;
	for (u16 y = 0; y < vulkan->image->height; y++) {
		u8 *dst = (u8 *)config->data + (size_t)y * config->stride;
		mem_copy(dst, config->stride, src + (size_t)y * vulkan->image->width * 4, (size_t)vulkan->image->width * 4);
	}

	vulkan->UnmapMemory(vulkan->device, target->readback.memory);
	return 0;
}

static int gfx_vulkan_image_read(gfx_image_t *image, const gfx_memory_readback_config_t *config)
{
	if (image == NULL || image->gfx == NULL || image->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan		      = image->gfx->data;
	gfx_vulkan_memory_target_t *vk_target = image->driver_data;
	if (vulkan->image != image || image->origin != GFX_IMAGE_ORIGIN_MEMORY || vk_target == NULL || vk_target->image == 0 ||
	    vk_target->readback.buffer == 0 || vk_target->readback.memory == 0 || vulkan->frame.active) {
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
	if (gfx_vulkan_swapchain_images_desc(swapchain)) {
		return 1; // LCOV_EXCL_LINE
	}
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

static int gfx_vulkan_swapchain_acquire(gfx_swapchain_t *swapchain, gfx_swapchain_image_t *image)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->data == NULL || image == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan = swapchain->gfx->data;
	if (gfx_vulkan_acquire_swapchain(vulkan, swapchain)) {
		return 1;
	}

	gfx_vulkan_swapchain_t *vk_swapchain = swapchain->data;
	if (vk_swapchain == NULL || vk_swapchain->image_index >= swapchain->image_count) {
		return 1;
	}

	u32 index			       = vk_swapchain->image_index;
	gfx_vulkan_swapchain_image_t *vk_image = swapchain->images[index].driver_data;
	if (vk_image == NULL) {
		return 1;
	}
	swapchain->images[index] = (gfx_image_t){
		.gfx	     = swapchain->gfx,
		.origin	     = GFX_IMAGE_ORIGIN_SURFACE,
		.format	     = swapchain->format,
		.driver_data = vk_image,
		.swapchain   = swapchain,
		.width	     = swapchain->width,
		.height	     = swapchain->height,
		.usage	     = swapchain->usage | GFX_IMAGE_USAGE_PRESENT,
		.index	     = index,
		.generation  = swapchain->generation,
	};
	*image = (gfx_swapchain_image_t){
		.image	    = &swapchain->images[index],
		.index	    = index,
		.generation = swapchain->generation,
	};
	return 0;
}

static int gfx_vulkan_acquire_swapchain(gfx_vulkan_t *vulkan, gfx_swapchain_t *public_swapchain)
{
	gfx_vulkan_swapchain_t *swapchain = public_swapchain != NULL ? public_swapchain->data : NULL;
	if (swapchain == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	if (swapchain->acquired) {
		return 0;
	}

	int acquired = 0;
	for (u32 attempt = 0; attempt < 2; attempt++) {
		if (!vk_ok(vulkan->ResetFences(vulkan->device, 1, &vulkan->fence))) {
			return 1;
		}
		VkResult result =
			vulkan->AcquireNextImageKHR(vulkan->device, swapchain->swapchain, ~0ull, 0, vulkan->fence, &swapchain->image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			if (gfx_vulkan_swapchain_recreate(public_swapchain)) {
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
	if (swapchain->image_index >= swapchain->image_count) {
		return 1;
	}

	swapchain->acquired = 1;
	return 0;
}

static int gfx_vulkan_swapchain_present(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->gfx == NULL || swapchain->gfx->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan		     = swapchain->gfx->data;
	gfx_vulkan_swapchain_t *vk_swapchain = swapchain->data;
	if (vk_swapchain == NULL || vk_swapchain->swapchain == 0 || !vk_swapchain->acquired) {
		return 1;
	}

	VkPresentInfoKHR present = {
		.sType		= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1,
		.pSwapchains	= &vk_swapchain->swapchain,
		.pImageIndices	= &vk_swapchain->image_index,
	};
	VkResult result	       = vulkan->QueuePresentKHR(vulkan->queue, &present);
	vk_swapchain->acquired = 0;
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
	for (u32 i = 0; i < vk_framebuffer->swapchain_framebuffer_count; i++) {
		if (vk_framebuffer->swapchain_framebuffers[i] != 0) {
			vulkan->DestroyFramebuffer(vulkan->device, vk_framebuffer->swapchain_framebuffers[i], NULL);
			vk_framebuffer->swapchain_framebuffers[i] = 0;
		}
	}
	if (vk_framebuffer->depth_view != 0) {
		vulkan->DestroyImageView(vulkan->device, vk_framebuffer->depth_view, NULL);
		vk_framebuffer->depth_view = 0;
	}
	if (vk_framebuffer->depth_image != 0) {
		vulkan->DestroyImage(vulkan->device, vk_framebuffer->depth_image, NULL);
		vk_framebuffer->depth_image = 0;
	}
	if (vk_framebuffer->depth_memory != 0) {
		vulkan->FreeMemory(vulkan->device, vk_framebuffer->depth_memory, NULL);
		vk_framebuffer->depth_memory = 0;
	}
	if (vk_framebuffer->swapchain_framebuffers != NULL) {
		alloc_free(&framebuffer->gfx->alloc,
			   vk_framebuffer->swapchain_framebuffers,
			   vk_framebuffer->swapchain_framebuffer_count * sizeof(vk_framebuffer->swapchain_framebuffers[0]));
	}
	alloc_free(&framebuffer->gfx->alloc, vk_framebuffer, sizeof(gfx_vulkan_framebuffer_t));
	framebuffer->data = NULL;
}

static int gfx_vulkan_create_framebuffer(gfx_vulkan_t *vulkan, const gfx_framebuffer_t *framebuffer, VkImageView view,
					 VkFramebuffer *vk_framebuffer)
{
	if (framebuffer == NULL || framebuffer->image == NULL || framebuffer->render_pass == NULL ||
	    framebuffer->render_pass->data == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_vulkan_render_pass_t *vk_render_pass = framebuffer->render_pass->data;

	VkImageView attachments[2]     = {view};
	u32 attachment_count	       = 1;
	gfx_vulkan_framebuffer_t *data = framebuffer->data;
	if (framebuffer->render_pass->depth_format != GFX_FORMAT_NONE) {
		if (data == NULL || data->depth_view == 0) {
			return 1; // LCOV_EXCL_LINE
		}
		attachments[attachment_count++] = data->depth_view;
	}

	VkFramebufferCreateInfo create = {
		.sType		 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass	 = vk_render_pass->render_pass,
		.attachmentCount = attachment_count,
		.pAttachments	 = attachments,
		.width		 = framebuffer->image->width,
		.height		 = framebuffer->image->height,
		.layers		 = 1,
	};
	return !vk_ok(vulkan->CreateFramebuffer(vulkan->device, &create, NULL, vk_framebuffer));
}

static int gfx_vulkan_framebuffer_init(gfx_framebuffer_t *framebuffer)
{
	if (framebuffer == NULL || framebuffer->gfx == NULL || framebuffer->gfx->data == NULL || framebuffer->image == NULL ||
	    framebuffer->image->driver_data == NULL || framebuffer->render_pass == NULL || framebuffer->render_pass->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan			 = framebuffer->gfx->data;
	gfx_vulkan_framebuffer_t *vk_framebuffer = alloc_alloc(&framebuffer->gfx->alloc, sizeof(gfx_vulkan_framebuffer_t));
	if (vk_framebuffer == NULL) {
		return 1;
	}
	*vk_framebuffer	  = (gfx_vulkan_framebuffer_t){0};
	framebuffer->data = vk_framebuffer;
	if (framebuffer->render_pass->depth_format != GFX_FORMAT_NONE &&
	    gfx_vulkan_create_depth_attachment(vulkan, framebuffer, vk_framebuffer)) {
		gfx_vulkan_framebuffer_free(framebuffer);
		return 1;
	}

	switch (framebuffer->image->origin) {
	case GFX_IMAGE_ORIGIN_MEMORY: {
		gfx_vulkan_memory_target_t *target = framebuffer->image->driver_data;
		if (target->image == 0) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		if (target->image_view == 0 &&
		    gfx_vulkan_create_image_view(vulkan, framebuffer->image, target->image, &target->image_view)) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		if (gfx_vulkan_create_framebuffer(vulkan, framebuffer, target->image_view, &vk_framebuffer->framebuffer)) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		break;
	}
	case GFX_IMAGE_ORIGIN_SURFACE: {
		gfx_vulkan_swapchain_t *swapchain = framebuffer->image->swapchain != NULL ? framebuffer->image->swapchain->data : NULL;
		if (swapchain == NULL) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		if (swapchain->image_count == 0) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		vk_framebuffer->swapchain_framebuffers =
			alloc_alloc(&framebuffer->gfx->alloc, swapchain->image_count * sizeof(vk_framebuffer->swapchain_framebuffers[0]));
		if (vk_framebuffer->swapchain_framebuffers == NULL) {
			gfx_vulkan_framebuffer_free(framebuffer);
			return 1;
		}
		vk_framebuffer->swapchain_framebuffer_count = swapchain->image_count;
		mem_set(vk_framebuffer->swapchain_framebuffers,
			0,
			vk_framebuffer->swapchain_framebuffer_count * sizeof(vk_framebuffer->swapchain_framebuffers[0]));
		for (u32 i = 0; i < swapchain->image_count; i++) {
			gfx_vulkan_swapchain_image_t *image = framebuffer->image->swapchain->images[i].driver_data;
			if (image == NULL || image->image == 0) {
				gfx_vulkan_framebuffer_free(framebuffer);
				return 1;
			}
			if (image->view == 0 && gfx_vulkan_create_image_view(vulkan, framebuffer->image, image->image, &image->view)) {
				gfx_vulkan_framebuffer_free(framebuffer);
				return 1;
			}
			if (gfx_vulkan_create_framebuffer(vulkan, framebuffer, image->view, &vk_framebuffer->swapchain_framebuffers[i])) {
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
	if (vulkan->image == NULL || vulkan->image->driver_data == NULL) {
		return 1;
	}
	gfx_vulkan_framebuffer_t *vk_framebuffer = framebuffer->data;
	switch (vulkan->image->origin) {
	case GFX_IMAGE_ORIGIN_MEMORY: {
		gfx_vulkan_memory_target_t *target = vulkan->image->driver_data;
		if (target->image == 0 || target->image_view == 0 || vk_framebuffer->framebuffer == 0) {
			return 1;
		}
		vulkan->frame.image	   = target->image;
		vulkan->frame.depth_image  = vk_framebuffer->depth_image;
		vulkan->frame.view	   = &target->image_view;
		vulkan->frame.framebuffer  = &vk_framebuffer->framebuffer;
		vulkan->frame.depth_layout = &vk_framebuffer->depth_layout;
		vulkan->frame.old_layout   = VK_IMAGE_LAYOUT_UNDEFINED;
		vulkan->frame.final_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		break;
	}
	case GFX_IMAGE_ORIGIN_SURFACE: {
		if (vulkan->swapchain == NULL) {
			return 1;
		}
		gfx_vulkan_swapchain_t *swapchain = vulkan->swapchain->data;
		if (gfx_vulkan_acquire_swapchain(vulkan, vulkan->swapchain)) {
			return 1;
		}
		u32 i				    = swapchain->image_index;
		gfx_vulkan_swapchain_image_t *image = vulkan->swapchain->images[i].driver_data;
		if (image == NULL || image->image == 0 || image->view == 0 || vk_framebuffer->swapchain_framebuffers == NULL ||
		    i >= vk_framebuffer->swapchain_framebuffer_count || vk_framebuffer->swapchain_framebuffers[i] == 0) {
			return 1;
		}
		vulkan->frame.image	   = image->image;
		vulkan->frame.depth_image  = vk_framebuffer->depth_image;
		vulkan->frame.view	   = &image->view;
		vulkan->frame.framebuffer  = &vk_framebuffer->swapchain_framebuffers[i];
		vulkan->frame.depth_layout = &vk_framebuffer->depth_layout;
		vulkan->frame.image_index  = i;
		vulkan->frame.old_layout   = image->layout != 0 ? image->layout : VK_IMAGE_LAYOUT_UNDEFINED;
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
	if (vulkan->frame.depth_image != 0) {
		VkImageSubresourceRange depth_range = {
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
			.levelCount = 1,
			.layerCount = 1,
		};
		VkImageMemoryBarrier to_depth = {
			.sType	       = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.oldLayout = vulkan->frame.depth_layout != NULL && *vulkan->frame.depth_layout != 0 ? *vulkan->frame.depth_layout
													    : VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image		     = vulkan->frame.depth_image,
			.subresourceRange    = depth_range,
		};
		vulkan->CmdPipelineBarrier(vulkan->command_buffer,
					   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					   0,
					   0,
					   NULL,
					   0,
					   NULL,
					   1,
					   &to_depth);
		if (vulkan->frame.depth_layout != NULL) {
			*vulkan->frame.depth_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
	}
	VkClearValue clears[2] = {
		vulkan->clear_color,
		{.depthStencil = {.depth = vulkan->clear_depth}},
	};
	VkRenderPassBeginInfo render = {
		.sType	     = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass  = vk_render_pass->render_pass,
		.framebuffer = *vulkan->frame.framebuffer,
		.renderArea =
			{
				.extent = {.width = vulkan->image->width, .height = vulkan->image->height},
			},
		.clearValueCount = render_pass->depth_format != GFX_FORMAT_NONE ? 2
				   : render_pass->load == GFX_LOAD_CLEAR	? 1
										: 0,
		.pClearValues	 = render_pass->depth_format != GFX_FORMAT_NONE || render_pass->load == GFX_LOAD_CLEAR ? clears : NULL,
	};

	vulkan->CmdBeginRenderPass(vulkan->command_buffer, &render, VK_SUBPASS_CONTENTS_INLINE);

	return 0;
}

static int gfx_vulkan_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || framebuffer == NULL || framebuffer->image == NULL ||
	    framebuffer->render_pass == NULL || framebuffer->render_pass->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan		     = frame->gfx->data;
	vulkan->clear_color.color.float32[0] = frame->pass.clear.r;
	vulkan->clear_color.color.float32[1] = frame->pass.clear.g;
	vulkan->clear_color.color.float32[2] = frame->pass.clear.b;
	vulkan->clear_color.color.float32[3] = frame->pass.clear.a;
	vulkan->clear_depth		     = frame->pass.clear_depth;
	vulkan->image			     = framebuffer->image;
	vulkan->swapchain = framebuffer->image->origin == GFX_IMAGE_ORIGIN_SURFACE ? framebuffer->image->swapchain : NULL;
	if (gfx_vulkan_frame_prepare(vulkan, framebuffer) || gfx_vulkan_frame_begin_commands(vulkan) ||
	    gfx_vulkan_frame_begin_render_pass(vulkan, framebuffer->render_pass)) {
		vulkan->frame = (gfx_vulkan_frame_t){0};
		return 1;
	}

	vulkan->frame.active = 1;
	return 0;
}

static void gfx_vulkan_buffer_resource_free(gfx_vulkan_t *vulkan, gfx_vulkan_buffer_resource_t *resource)
{
	if (vulkan == NULL || resource == NULL) {
		return; // LCOV_EXCL_LINE
	}

	if (resource->buffer != 0) {
		vulkan->DestroyBuffer(vulkan->device, resource->buffer, NULL);
		resource->buffer = 0;
	}
	if (resource->memory != 0) {
		vulkan->FreeMemory(vulkan->device, resource->memory, NULL);
		resource->memory = 0;
	}
	resource->size		  = 0;
	resource->memory_size	  = 0;
	resource->memory_coherent = 0;
}

static void gfx_vulkan_buffer_resources_free(gfx_vulkan_t *vulkan, gfx_vulkan_buffer_t *vk_buffer)
{
	if (vulkan == NULL || vk_buffer == NULL) {
		return; // LCOV_EXCL_LINE
	}

	if (vk_buffer->buffer != 0) {
		vulkan->DestroyBuffer(vulkan->device, vk_buffer->buffer, NULL);
		vk_buffer->buffer = 0;
	}
	if (vk_buffer->memory != 0) {
		vulkan->FreeMemory(vulkan->device, vk_buffer->memory, NULL);
		vk_buffer->memory = 0;
	}
	vk_buffer->size		   = 0;
	vk_buffer->memory_size	   = 0;
	vk_buffer->memory_coherent = 0;
}

static void gfx_vulkan_buffer_free(gfx_buffer_t *buffer)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || buffer->data == NULL) {
		return;
	}

	gfx_vulkan_t *vulkan	       = buffer->gfx->data;
	gfx_vulkan_buffer_t *vk_buffer = buffer->data;

	gfx_vulkan_buffer_resources_free(vulkan, vk_buffer);
	alloc_free(&buffer->gfx->alloc, vk_buffer, sizeof(gfx_vulkan_buffer_t));
	buffer->data = NULL;
}

static int gfx_vulkan_buffer_usage(gfx_buffer_type_t type, VkFlags *usage)
{
	if (usage == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	switch (type) {
	case GFX_BUFFER_VERTEX: {
		*usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		break;
	}
	case GFX_BUFFER_INDEX: {
		*usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		break;
	}
	case GFX_BUFFER_UNIFORM: {
		*usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		break;
	}
	default: {
		return 1;
	}
	}
	return 0;
}

static int gfx_vulkan_buffer_create_resource(gfx_buffer_t *buffer, gfx_vulkan_buffer_t *vk_buffer, VkDeviceSize size,
					     gfx_vulkan_buffer_resource_t *resource)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || vk_buffer == NULL || resource == NULL || size == 0) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_vulkan_t *vulkan = buffer->gfx->data;

	VkFlags usage = 0;
	if (gfx_vulkan_buffer_usage(vk_buffer->type, &usage)) {
		log_error("cgfx", "gfx_vulkan", NULL, "unsupported buffer type: %d", vk_buffer->type);
		return 1;
	}

	*resource		  = (gfx_vulkan_buffer_resource_t){0};
	VkBufferCreateInfo create = {
		.sType	     = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size	     = size,
		.usage	     = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	if (!vk_ok(vulkan->CreateBuffer(vulkan->device, &create, NULL, &resource->buffer))) {
		return 1;
	}

	VkMemoryRequirements req = {0};
	vulkan->GetBufferMemoryRequirements(vulkan->device, resource->buffer, &req);

	u32 memory_type = 0;
	if (memory_type_find(vulkan, req.memoryTypeBits, &memory_type, &resource->memory_coherent)) {
		gfx_vulkan_buffer_resource_free(vulkan, resource);
		return 1;
	}

	VkMemoryAllocateInfo memory = {
		.sType		 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize	 = req.size,
		.memoryTypeIndex = memory_type,
	};
	if (!vk_ok(vulkan->AllocateMemory(vulkan->device, &memory, NULL, &resource->memory))) {
		gfx_vulkan_buffer_resource_free(vulkan, resource);
		return 1;
	}
	resource->size	      = size;
	resource->memory_size = req.size;

	if (!vk_ok(vulkan->BindBufferMemory(vulkan->device, resource->buffer, resource->memory, 0))) {
		gfx_vulkan_buffer_resource_free(vulkan, resource);
		return 1;
	}

	return 0;
}

static void gfx_vulkan_buffer_use_resource(gfx_buffer_t *buffer, gfx_vulkan_buffer_t *vk_buffer,
					   const gfx_vulkan_buffer_resource_t *resource)
{
	vk_buffer->buffer	   = resource->buffer;
	vk_buffer->memory	   = resource->memory;
	vk_buffer->size		   = resource->size;
	vk_buffer->memory_size	   = resource->memory_size;
	vk_buffer->memory_coherent = resource->memory_coherent;
	buffer->size		   = (size_t)resource->size;
}

static int gfx_vulkan_buffer_allocate(gfx_buffer_t *buffer, gfx_vulkan_buffer_t *vk_buffer, VkDeviceSize size)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || vk_buffer == NULL || size == 0) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_vulkan_t *vulkan		  = buffer->gfx->data;
	gfx_vulkan_buffer_resource_t next = {0};
	if (gfx_vulkan_buffer_create_resource(buffer, vk_buffer, size, &next)) {
		return 1;
	}

	gfx_vulkan_buffer_resources_free(vulkan, vk_buffer);
	gfx_vulkan_buffer_use_resource(buffer, vk_buffer, &next);

	return 0;
}

static int gfx_vulkan_buffer_upload(gfx_buffer_t *buffer, gfx_vulkan_buffer_t *vk_buffer, const void *data, size_t size)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || vk_buffer == NULL || vk_buffer->memory == 0 ||
	    data == NULL || size == 0) {
		return 1;
	}

	gfx_vulkan_t *vulkan = buffer->gfx->data;

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

static int gfx_vulkan_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || config == NULL) {
		return 1;
	}

	VkFlags usage = 0;
	if (gfx_vulkan_buffer_usage(config->type, &usage)) {
		log_error("cgfx", "gfx_vulkan", NULL, "unsupported buffer type: %d", config->type);
		return 1;
	}

	gfx_vulkan_buffer_t *vk_buffer = alloc_alloc(&buffer->gfx->alloc, sizeof(gfx_vulkan_buffer_t));
	if (vk_buffer == NULL) {
		return 1;
	}
	*vk_buffer = (gfx_vulkan_buffer_t){
		.type = config->type,
	};
	buffer->data = vk_buffer;

	if (config->size != 0 && gfx_vulkan_buffer_allocate(buffer, vk_buffer, (VkDeviceSize)config->size)) {
		gfx_vulkan_buffer_free(buffer);
		return 1;
	}
	if (config->data != NULL && gfx_vulkan_buffer_upload(buffer, vk_buffer, config->data, config->size)) {
		gfx_vulkan_buffer_free(buffer);
		return 1;
	}

	return 0;
}

static int gfx_vulkan_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || buffer->data == NULL || data == NULL || size == 0 ||
	    buffer->usage == GFX_BUFFER_USAGE_STATIC) {
		return 1;
	}

	gfx_vulkan_buffer_t *vk_buffer = buffer->data;

	if (size > vk_buffer->size && gfx_vulkan_buffer_allocate(buffer, vk_buffer, (VkDeviceSize)size)) {
		return 1;
	}

	return gfx_vulkan_buffer_upload(buffer, vk_buffer, data, size);
}

static int gfx_vulkan_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	(void)frame;

	if (buffer == NULL || buffer->gfx == NULL || buffer->gfx->data == NULL || buffer->data == NULL) {
		return 1;
	}

	gfx_vulkan_t *vulkan	       = buffer->gfx->data;
	gfx_vulkan_buffer_t *vk_buffer = buffer->data;
	if (vk_buffer->buffer == 0) {
		return 1;
	}

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
	case GFX_BUFFER_UNIFORM: {
		break;
	}
	default: {
		log_error("cgfx", "gfx_vulkan", NULL, "unsupported buffer type: %d", buffer->type);
		return 1;
	}
	}

	return 0;
}

static int gfx_vulkan_bind_resources(gfx_frame_t *frame, const gfx_resource_binding_t *bindings, u32 binding_count)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || frame->pipeline == NULL || frame->pipeline->data == NULL ||
	    (bindings == NULL && binding_count != 0)) {
		return 1;
	}

	gfx_vulkan_pipeline_t *vk_pipeline = frame->pipeline->data;
	if (vk_pipeline == NULL || vk_pipeline->descriptor_sets[0] == 0 || vk_pipeline->pipeline_layout == 0) {
		return 1;
	}
	if (vk_pipeline->descriptor_set_index >= GFX_VULKAN_MAX_DESCRIPTOR_SETS) {
		log_error("cgfx", "gfx_vulkan", NULL, "too many uniform binds in one pipeline bind");
		return 1;
	}

	gfx_vulkan_t *vulkan	       = frame->gfx->data;
	VkDescriptorSet descriptor_set = vk_pipeline->descriptor_sets[vk_pipeline->descriptor_set_index];

	VkDescriptorBufferInfo buffer_infos[GFX_VULKAN_MAX_UNIFORM_BINDINGS] = {0};
	VkWriteDescriptorSet writes[GFX_VULKAN_MAX_UNIFORM_BINDINGS]	     = {0};
	u32 write_count							     = 0;
	for (u32 i = 0; i < binding_count; i++) {
		const gfx_resource_binding_t *binding = &bindings[i];
		if (binding->type != GFX_RESOURCE_UNIFORM_BUFFER || binding->buffer == NULL ||
		    binding->binding >= GFX_VULKAN_MAX_UNIFORM_BINDINGS || write_count >= GFX_VULKAN_MAX_UNIFORM_BINDINGS ||
		    binding->buffer->gfx != frame->gfx || binding->buffer->type != GFX_BUFFER_UNIFORM || binding->buffer->data == NULL) {
			return 1;
		}
		gfx_vulkan_buffer_t *vk_buffer = binding->buffer->data;
		if (vk_buffer->buffer == 0 || vk_buffer->size == 0) {
			return 1;
		}

		buffer_infos[write_count] = (VkDescriptorBufferInfo){
			.buffer = vk_buffer->buffer,
			.offset = 0,
			.range	= vk_buffer->size,
		};
		writes[write_count] = (VkWriteDescriptorSet){
			.sType		 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet		 = descriptor_set,
			.dstBinding	 = binding->binding,
			.descriptorCount = 1,
			.descriptorType	 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo	 = &buffer_infos[write_count],
		};
		write_count++;
	}
	vulkan->UpdateDescriptorSets(vulkan->device, write_count, writes, 0, NULL);
	vulkan->CmdBindDescriptorSets(
		vulkan->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline->pipeline_layout, 0, 1, &descriptor_set, 0, NULL);
	vk_pipeline->descriptor_set_index++;
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
	if (vk_pipeline->descriptor_pool != 0) {
		vulkan->DestroyDescriptorPool(vulkan->device, vk_pipeline->descriptor_pool, NULL);
		vk_pipeline->descriptor_pool = 0;
	}
	if (vk_pipeline->descriptor_set_layout != 0) {
		vulkan->DestroyDescriptorSetLayout(vulkan->device, vk_pipeline->descriptor_set_layout, NULL);
		vk_pipeline->descriptor_set_layout = 0;
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
	if (config->raster.fill == GFX_FILL_WIREFRAME && !vulkan->fill_mode_non_solid) {
		log_error("cgfx", "gfx_vulkan", NULL, "Vulkan wireframe pipeline requires fillModeNonSolid support");
		return 1;
	}

	gfx_vulkan_pipeline_t *vk_pipeline = alloc_alloc(&pipeline->gfx->alloc, sizeof(gfx_vulkan_pipeline_t));
	if (vk_pipeline == NULL) {
		return 1;
	}
	*vk_pipeline   = (gfx_vulkan_pipeline_t){0};
	pipeline->data = vk_pipeline;

	VkDescriptorSetLayoutBinding bindings[GFX_VULKAN_MAX_UNIFORM_BINDINGS] = {0};
	for (u32 i = 0; i < GFX_VULKAN_MAX_UNIFORM_BINDINGS; i++) {
		bindings[i] = (VkDescriptorSetLayoutBinding){
			.binding	 = i,
			.descriptorType	 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags	 = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		};
	}
	VkDescriptorSetLayoutCreateInfo set_layout = {
		.sType	      = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = GFX_VULKAN_MAX_UNIFORM_BINDINGS,
		.pBindings    = bindings,
	};
	if (!vk_ok(vulkan->CreateDescriptorSetLayout(vulkan->device, &set_layout, NULL, &vk_pipeline->descriptor_set_layout))) {
		gfx_vulkan_pipeline_free(pipeline);
		return 1;
	}

	VkDescriptorPoolSize pool_size = {
		.type		 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = GFX_VULKAN_MAX_DESCRIPTOR_SETS * GFX_VULKAN_MAX_UNIFORM_BINDINGS,
	};
	VkDescriptorPoolCreateInfo pool = {
		.sType	       = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets       = GFX_VULKAN_MAX_DESCRIPTOR_SETS,
		.poolSizeCount = 1,
		.pPoolSizes    = &pool_size,
	};
	if (!vk_ok(vulkan->CreateDescriptorPool(vulkan->device, &pool, NULL, &vk_pipeline->descriptor_pool))) {
		gfx_vulkan_pipeline_free(pipeline);
		return 1;
	}

	VkDescriptorSetLayout set_layouts[GFX_VULKAN_MAX_DESCRIPTOR_SETS] = {0};
	for (u32 i = 0; i < GFX_VULKAN_MAX_DESCRIPTOR_SETS; i++) {
		set_layouts[i] = vk_pipeline->descriptor_set_layout;
	}
	VkDescriptorSetAllocateInfo set_alloc = {
		.sType		    = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool	    = vk_pipeline->descriptor_pool,
		.descriptorSetCount = GFX_VULKAN_MAX_DESCRIPTOR_SETS,
		.pSetLayouts	    = set_layouts,
	};
	if (!vk_ok(vulkan->AllocateDescriptorSets(vulkan->device, &set_alloc, vk_pipeline->descriptor_sets))) {
		gfx_vulkan_pipeline_free(pipeline);
		return 1;
	}

	VkPipelineLayoutCreateInfo layout = {
		.sType		= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1,
		.pSetLayouts	= &vk_pipeline->descriptor_set_layout,
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
	if (layout_cnt > U32_MAX) {
		log_error("cgfx", "gfx_vulkan", NULL, "too many input layout elements: %zu", layout_cnt);
		gfx_vulkan_pipeline_free(pipeline);
		return 1;
	}
	u32 attribute_count = (u32)layout_cnt;

	VkVertexInputAttributeDescription *attributes =
		alloc_alloc(&pipeline->gfx->alloc, layout_cnt * sizeof(VkVertexInputAttributeDescription));
	if (attributes == NULL) {
		gfx_vulkan_pipeline_free(pipeline);
		return 1;
	}

	u32 offset = 0;
	for (size_t i = 0; i < layout_cnt; i++) {
		attributes[i].location = config->input_layout[i].index;
		attributes[i].binding  = 0;

		u64 size = (u64)sizeof(float) * config->input_layout[i].count;
		if (size > U32_MAX || offset > U32_MAX - (u32)size) {
			log_error("cgfx", "gfx_vulkan", NULL, "input layout stride is too large");
			alloc_free(&pipeline->gfx->alloc, attributes, layout_cnt * sizeof(VkVertexInputAttributeDescription));
			gfx_vulkan_pipeline_free(pipeline);
			return 1;
		}

		if (config->input_layout[i].type == GFX_VALUE_FLOAT32 && config->input_layout[i].count == 2) {
			attributes[i].format = VK_FORMAT_R32G32_SFLOAT;
		} else if (config->input_layout[i].type == GFX_VALUE_FLOAT32 && config->input_layout[i].count == 3) {
			attributes[i].format = VK_FORMAT_R32G32B32_SFLOAT;
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
		offset += (u32)size;
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
		.vertexAttributeDescriptionCount = attribute_count,
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
		.polygonMode = config->raster.fill == GFX_FILL_WIREFRAME ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL,
		.cullMode    = config->raster.cull == GFX_CULL_NONE    ? VK_CULL_MODE_NONE
			       : config->raster.cull == GFX_CULL_FRONT ? VK_CULL_MODE_FRONT_BIT
								       : VK_CULL_MODE_BACK_BIT,
		.frontFace = config->raster.front_face == GFX_WINDING_CLOCKWISE ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.lineWidth = 1.0f,
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
	VkPipelineDepthStencilStateCreateInfo depth = {
		.sType		  = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable  = config->depth.test,
		.depthWriteEnable = config->depth.write,
		.depthCompareOp	  = VK_COMPARE_OP_LESS,
	};
	VkDynamicState dynamic_states[2]	 = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
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
		.pDepthStencilState  = config->render_pass->depth_format != GFX_FORMAT_NONE ? &depth : NULL,
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

	vk_pipeline->descriptor_set_index = 0;
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
		.dstAccessMask	     = vulkan->frame.surface ? 0 : VK_ACCESS_TRANSFER_READ_BIT,
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
				   vulkan->frame.surface ? VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT : VK_PIPELINE_STAGE_TRANSFER_BIT,
				   0,
				   0,
				   NULL,
				   0,
				   NULL,
				   1,
				   &from_color);
	if (!vulkan->frame.surface) {
		gfx_vulkan_memory_target_t *target = vulkan->image != NULL ? vulkan->image->driver_data : NULL;
		if (target == NULL || target->readback.buffer == 0) {
			return 1;
		}
		VkBufferImageCopy copy = {
			.imageSubresource =
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.layerCount = 1,
				},
			.imageExtent =
				{
					.width	= vulkan->image->width,
					.height = vulkan->image->height,
					.depth	= 1,
				},
		};
		vulkan->CmdCopyImageToBuffer(vulkan->command_buffer,
					     vulkan->frame.image,
					     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					     target->readback.buffer,
					     1,
					     &copy);
		VkBufferMemoryBarrier to_host = {
			.sType		     = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			.srcAccessMask	     = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask	     = VK_ACCESS_HOST_READ_BIT,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.buffer		     = target->readback.buffer,
			.size		     = ~0ull,
		};
		vulkan->CmdPipelineBarrier(vulkan->command_buffer,
					   VK_PIPELINE_STAGE_TRANSFER_BIT,
					   VK_PIPELINE_STAGE_HOST_BIT,
					   0,
					   0,
					   NULL,
					   1,
					   &to_host,
					   0,
					   NULL);
	}
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
	if (vulkan->swapchain == NULL || vulkan->frame.image_index >= vulkan->swapchain->image_count) {
		return 1;
	}

	gfx_vulkan_swapchain_image_t *image = vulkan->swapchain->images[vulkan->frame.image_index].driver_data;
	if (image == NULL) {
		return 1;
	}

	image->layout = vulkan->frame.final_layout;
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
	.swapchain_acquire	= gfx_vulkan_swapchain_acquire,
	.swapchain_present	= gfx_vulkan_swapchain_present,
	.image_init		= gfx_vulkan_image_init,
	.image_free		= gfx_vulkan_image_free,
	.image_read		= gfx_vulkan_image_read,
	.framebuffer_init	= gfx_vulkan_framebuffer_init,
	.framebuffer_free	= gfx_vulkan_framebuffer_free,
	.framebuffer_pass_begin = gfx_vulkan_framebuffer_pass_begin,
	.buffer_init		= gfx_vulkan_buffer_init,
	.buffer_free		= gfx_vulkan_buffer_free,
	.buffer_set_data	= gfx_vulkan_buffer_set_data,
	.buffer_bind		= gfx_vulkan_buffer_bind,
	.bind_resources		= gfx_vulkan_bind_resources,
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
