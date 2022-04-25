#ifndef VK_DEFINES_HPP
#define VK_DEFINES_HPP

#include <assert.h>
#include <vector>

#include <vulkan/vulkan.h>

#define VK_CHECK(val)                  \
    do                                 \
    {                                  \
        if (val != VK_SUCCESS)         \
        {                              \
            assert(val == VK_SUCCESS); \
        }                              \
    } while (false)

struct VulkanResources
{
    VkInstance m_VkInstance;
    VkSurfaceKHR m_VkSurface;
    VkPhysicalDevice m_VkPhysicalDevice;
    VkDevice m_VkDevice;

    VkSwapchainKHR m_VkSwapchain;
    VkFormat m_VkSwapchainImageFormat;
    VkExtent2D m_VkSwapchainExtent;
    std::vector<VkImage> m_VkSwapchainImages;
    std::vector<VkImageView> m_VkSwapchainImageViews;
    uint32_t m_uSwapchainImageIdx;

    VkBuffer m_VkBuffers;
    VkDeviceMemory m_VkBuferDeviceMemorys;

    uint32_t m_uGraphicsQueueFamilyIndex;
    VkQueue m_VkGraphicsQueue;

    VkPhysicalDeviceMemoryProperties m_VkPhysicalDeviceMemProps;

    PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
    PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
    PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR;
    PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;
};

#endif // VK_DEFINES_HPP