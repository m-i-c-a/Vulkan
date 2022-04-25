#ifndef VK_FRAME_HPP
#define VK_FRAME_HPP

#include <array>

#include <vulkan/vulkan.h>

class VulkanResources;

class VkFrame
{
private:
    static VulkanResources* m_pVkResources; 

    enum
    {
        ATTACHMENT_COLOR = 0
    };

    enum
    {
        COMMMAND_BUFFER_RENDER = 0,
    };

    void resetCommandPools();
    void transitionAttachmentsStartOfFrame();
    void transitionAttachmentsEndOfFrame();

public:
    static void setResources(VulkanResources* resources) { m_pVkResources = resources; }

    VkFrame();
    // ~VkFrame();

    void init();
    void cleanup();

    void setColorAttachment(VkImage image);
    void cull();
    VkCommandBuffer render();

    std::array<VkCommandPool, 1> m_VkCommandPools;
    std::array<VkCommandBuffer, 1> m_VkCommandBuffers;

    std::array<VkImage, 1> m_VkImageAttachments;

    VkSemaphore m_VkAcquireCompleteSemaphore;
    VkSemaphore m_VkRenderCompleteSemaphore;
    VkFence m_VkCommandBufferIsExecutableFence;
};

#endif // VK_FRAME_HPP