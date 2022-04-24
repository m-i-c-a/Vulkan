#ifndef STAGING_BUFFER_HPP
#define STAGING_BUFFER_HPP

#include <vulkan/vulkan.h>

class StagingBuffer
{
private:
    VkDevice* m_VkDevice;


    VkCommandPool m_VkCommandPool;
    VkCommandBuffer m_VkCommandBuffer;

public:
    void uploadData(const VkDeviceSize nBytes, const void* data)
    {
        const VkBufferCreateInfo cpuBufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = nBytes,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        VkBuffer cpuBuffer;
        vkCreateBuffer(*m_VkDevice, &cpuBufferCreateInfo, nullptr, &cpuBuffer);

        VkBuffer stagingBuffer = vkCreate
        VkDeviceMemory stagingBufferMemory = allocateBufferMemory(device, stagingBuffer, physicalDeviceMemoryProperties);
        VK_CHECK(vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0));

    }
};

#endif // STAGING_BUFFER_HPP