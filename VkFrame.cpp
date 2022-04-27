#include "VkFrame.hpp"
#include "VkRuntime.hpp"
#include "VkDefines.hpp"

VulkanResources* VkFrame::m_pVkResources = nullptr; 

VkFrame::VkFrame()
{
    for (uint32_t i = 0; i < m_VkCommandPools.size(); ++i)
    {
        m_VkCommandPools[i] = VK_NULL_HANDLE;
        m_VkCommandBuffers[i] = VK_NULL_HANDLE;
    }

    m_VkAcquireCompleteSemaphore = VK_NULL_HANDLE;
    m_VkRenderCompleteSemaphore = VK_NULL_HANDLE;
    m_VkCommandBufferIsExecutableFence = VK_NULL_HANDLE;
}

void VkFrame::init()
{
    for (uint32_t i = 0; i < m_VkCommandPools.size(); ++i)
    {
        m_VkCommandPools[i] = createCommandPool(m_pVkResources->m_VkDevice, m_pVkResources->m_uGraphicsQueueFamilyIndex);
        m_VkCommandBuffers[i] = createCommandBuffer(m_pVkResources->m_VkDevice, m_VkCommandPools[i]);
    }

    m_VkAcquireCompleteSemaphore = createSemaphore(m_pVkResources->m_VkDevice);
    m_VkRenderCompleteSemaphore = createSemaphore(m_pVkResources->m_VkDevice);
    m_VkCommandBufferIsExecutableFence = createFence(m_pVkResources->m_VkDevice, true);
}

void VkFrame::cleanup()
{
    for (VkCommandPool commandPool : m_VkCommandPools)
        vkDestroyCommandPool(m_pVkResources->m_VkDevice, commandPool, nullptr);

    vkDestroySemaphore(m_pVkResources->m_VkDevice, m_VkAcquireCompleteSemaphore, nullptr);
    vkDestroySemaphore(m_pVkResources->m_VkDevice, m_VkRenderCompleteSemaphore, nullptr);
    vkDestroyFence(m_pVkResources->m_VkDevice, m_VkCommandBufferIsExecutableFence, nullptr);
}

void VkFrame::setColorAttachment(VkImage image)
{
    m_VkImageAttachments[ATTACHMENT_COLOR] = image;
}

void VkFrame::resetCommandPools()
{
    for (VkCommandPool commandPool : m_VkCommandPools)
        vkResetCommandPool(m_pVkResources->m_VkDevice, commandPool, 0x0);
}

void VkFrame::transitionAttachmentsStartOfFrame()
{
    static VkImageMemoryBarrier2KHR imageMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
        .pNext = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR,
        .srcAccessMask = VK_ACCESS_2_NONE_KHR,
        .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = m_pVkResources->m_uGraphicsQueueFamilyIndex,
        .dstQueueFamilyIndex = m_pVkResources->m_uGraphicsQueueFamilyIndex,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0u,
            .levelCount = 1u,
            .baseArrayLayer = 0u,
            .layerCount = 1u}};

    static VkDependencyInfoKHR dependencyInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
        .pNext = nullptr,
        .dependencyFlags = 0x0,
        .memoryBarrierCount = 0u,
        .pMemoryBarriers = nullptr,
        .bufferMemoryBarrierCount = 0u,
        .pBufferMemoryBarriers = nullptr,
        .imageMemoryBarrierCount = 1u};

    imageMemoryBarrier.image = m_VkImageAttachments[ATTACHMENT_COLOR];
    dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

    m_pVkResources->vkCmdPipelineBarrier2KHR(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], &dependencyInfo);
}

void VkFrame::transitionAttachmentsEndOfFrame()
{
    static VkImageMemoryBarrier2KHR imageMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
        .pNext = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR,
        .dstStageMask = VK_PIPELINE_STAGE_2_NONE_KHR,
        .dstAccessMask = VK_ACCESS_2_NONE_KHR,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = m_pVkResources->m_uGraphicsQueueFamilyIndex,
        .dstQueueFamilyIndex = m_pVkResources->m_uGraphicsQueueFamilyIndex,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0u,
            .levelCount = 1u,
            .baseArrayLayer = 0u,
            .layerCount = 1u}};

    static VkDependencyInfoKHR dependencyInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
        .pNext = nullptr,
        .dependencyFlags = 0x0,
        .memoryBarrierCount = 0u,
        .pMemoryBarriers = nullptr,
        .bufferMemoryBarrierCount = 0u,
        .pBufferMemoryBarriers = nullptr,
        .imageMemoryBarrierCount = 1u};

    imageMemoryBarrier.image = m_VkImageAttachments[ATTACHMENT_COLOR];
    dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

    m_pVkResources->vkCmdPipelineBarrier2KHR(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], &dependencyInfo);
}



VkCommandBuffer VkFrame::render(const RenderManager& renderer)
{
    resetCommandPools();

    static const VkCommandBufferBeginInfo commandBufferBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};

    VK_CHECK(vkBeginCommandBuffer(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], &commandBufferBeginInfo));

    transitionAttachmentsStartOfFrame();

    static VkClearValue clearColor{
        .color = {0.14f, 0.68f, 0.23f, 1.0f}};

    const VkRenderingAttachmentInfoKHR colorAttachmentInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .pNext = nullptr,
        .imageView = m_pVkResources->m_VkSwapchainImageViews[m_pVkResources->m_uSwapchainImageIdx],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clearColor};

    const VkRect2D renderArea{
        .offset = {0u, 0u},
        .extent = m_pVkResources->m_VkSwapchainExtent};

    const VkRenderingInfoKHR renderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .pNext = nullptr,
        .flags = 0x0,
        .renderArea = renderArea,
        .layerCount = 1u,
        .viewMask = 0,
        .colorAttachmentCount = 1u,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
    };

    m_pVkResources->vkCmdBeginRenderingKHR(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], &renderingInfo);

    renderer.render(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER]);

    // {
    //     vkCmdBindPipeline(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    //     for (const Model& model : models)
    //     {
    //         VkDeviceSize pOffsets = 0;
    //         vkCmdBindVertexBuffers(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], 0u, 1u, model.m_pPrototype->m_VertexBuffer.getBufferPointer(), &pOffsets);
    //         vkCmdBindIndexBuffer(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], model.m_pPrototype->m_IndexBuffer.getBuffer(), 0u, VK_INDEX_TYPE_UINT32);

    //         for (const Renderable& renderable : model.m_pPrototype->m_Renderables)
    //         {
    //             vkCmdDrawIndexed(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], renderable.indexCount, 1u, 0u, renderable.vertexOffset, 0u);
    //         }
    //     }
    // }


    m_pVkResources->vkCmdEndRenderingKHR(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER]);

    transitionAttachmentsEndOfFrame();

    VK_CHECK(vkEndCommandBuffer(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER]));

    return m_VkCommandBuffers[COMMMAND_BUFFER_RENDER];
}