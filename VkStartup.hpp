#ifndef VK_STARTUP_HPP
#define VK_STARTUP_HPP

#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "VkDefines.hpp"

struct VulkanInitParams
{
    GLFWwindow *m_Window;
    uint32_t m_uWindowWidth;
    uint32_t m_uWindowHeight;

    std::vector<const char *> m_vInstanceExtensions;
    std::vector<const char *> m_vInstanceLayers;
    std::vector<const char *> m_vDeviceExtensions;

    uint32_t m_uRequestedSwapchainImageCount;
    VkPresentModeKHR m_VkPresentMode;
};

void vulkanInit(const VulkanInitParams& initParams, VulkanResources& vulkanResources);

void vulkanDestroy(VulkanResources& vulkanResources);

#endif // VK_STARTUP_HPP