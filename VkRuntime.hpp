#ifndef VK_RUNTIME_HPP
#define VK_RUNTIME_HPP

#include <vulkan/vulkan.h>

VkCommandPool createCommandPool(VkDevice device, uint32_t graphicsQueueFamilyIndex);

VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool pool);

VkFence createFence(VkDevice device, bool signaled = false);

VkSemaphore createSemaphore(VkDevice device);

#endif // VK_RUNTIME_HPP