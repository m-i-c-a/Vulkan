#include "VkRuntime.hpp"
#include "VkDefines.hpp"

/**
 * @brief Create a Command Pool object
 * 
 * @param device 
 * @param graphicsQueueFamilyIndex 
 * @return VkCommandPool 
 * 
 * CommandPools should be created without RESET_COMMAND_BUFFER_BIT and be reset manually via vkResetCommandPool
 *  for performance. See (https://arm-software.github.io/vulkan_best_practice_for_mobile_developers/samples/performance/command_buffer_usage/command_buffer_usage_tutorial.html)
 */
VkCommandPool createCommandPool(VkDevice device, uint32_t graphicsQueueFamilyIndex)
{
    const VkCommandPoolCreateInfo commandPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0x0,
        .queueFamilyIndex = graphicsQueueFamilyIndex};

    VkCommandPool commandPool;
    VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));
    return commandPool;
}

VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool pool)
{
    const VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer));
    return commandBuffer;
}

VkFence createFence(VkDevice device, bool signaled)
{
    const VkFenceCreateInfo fenceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = (signaled) ? VK_FENCE_CREATE_SIGNALED_BIT : 0u,
    };

    VkFence fence;
    VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));
    return fence;
}

VkSemaphore createSemaphore(VkDevice device)
{
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkSemaphore semaphore;
    VK_CHECK(vkCreateSemaphore(device, &createInfo, nullptr, &semaphore));
    return semaphore;
}


