#include "Buffer.hpp"

#include <assert.h>
#include <string.h>
#include <iostream>

#define VK_CHECK(val)                  \
    do                                 \
    {                                  \
        if (val != VK_SUCCESS)         \
        {                              \
            assert(val == VK_SUCCESS); \
        }                              \
    } while (false)

#define VULKAN_1_2

namespace
{
    uint32_t getHeapIndex(const uint32_t memoryTypeIndices, const VkMemoryPropertyFlags memoryPropertyFlags, VkPhysicalDeviceMemoryProperties memoryProperties)
    {
        // Iterate over all memory types available for the device used in this example
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if (memoryTypeIndices & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
            {
                return i;
            }
        }

        assert(false && "Could not find suitable memory type!");
        return 0;
    }

    VkBuffer createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage)
    {
        const VkBufferCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        VkBuffer buffer;
        VK_CHECK(vkCreateBuffer(device, &createInfo, nullptr, &buffer));
        return buffer;
    }

    VkDeviceMemory allocateBufferMemory(VkDevice device, VkBuffer buffer, const VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties)
    {
        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(device, buffer, &memReqs);

        const VkMemoryAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memReqs.size,
            .memoryTypeIndex = getHeapIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, physicalDeviceMemoryProperties)};

        VkDeviceMemory memory;
        VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &memory));
        return memory;
    }

    void uploadStagingToDeviceLocal()
    {
    }
}

VkDevice* StagingBuffer::m_pVkDevice = nullptr;
VkQueue StagingBuffer::m_VkQueue = VK_NULL_HANDLE;
VkCommandPool StagingBuffer::m_VkCommandPool = VK_NULL_HANDLE;
VkCommandBuffer StagingBuffer::m_VkCommandBuffer = VK_NULL_HANDLE;

void StagingBuffer::init(VkDevice *device, VkQueue queue, uint32_t queueFamilyIndex)
{
    m_pVkDevice = device;
    m_VkQueue = queue;

    const VkCommandPoolCreateInfo commandPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0x0,
        .queueFamilyIndex = queueFamilyIndex};

    VK_CHECK(vkCreateCommandPool(*m_pVkDevice, &commandPoolCreateInfo, nullptr, &m_VkCommandPool));

    const VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_VkCommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VK_CHECK(vkAllocateCommandBuffers(*m_pVkDevice, &commandBufferAllocateInfo, &m_VkCommandBuffer));
}

void StagingBuffer::upload(VkBuffer srcBuffer, VkDeviceSize srcOffset, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize nbytes)
{
    static const VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    vkBeginCommandBuffer(m_VkCommandBuffer, &commandBufferBeginInfo);

    const VkBufferCopy bufferCopy {
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
        .size = nbytes
    };

    vkCmdCopyBuffer(m_VkCommandBuffer, srcBuffer, dstBuffer, 1u, &bufferCopy);

    vkEndCommandBuffer(m_VkCommandBuffer);

#ifdef VULKAN_1_3
    static const VkCommandBufferSubmitInfo commandBufferSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = m_VkCommandBuffer
    };

    static const VkSubmitInfo2 renderSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .commandBufferInfoCount = 1u,
        .pCommandBufferInfos = &commandBufferSubmitInfo
    };

    VK_CHECK(vkQueueSubmit2(m_VkQueue, 1u, &renderSubmitInfo, VK_NULL_HANDLE));
#endif
#ifdef VULKAN_1_2
    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1u,
        .pCommandBuffers = &m_VkCommandBuffer,
    };
    VK_CHECK(vkQueueSubmit(m_VkQueue, 1u, &submitInfo, VK_NULL_HANDLE));
#endif

    vkQueueWaitIdle(m_VkQueue);
    vkResetCommandPool(*m_pVkDevice, m_VkCommandPool, 0x0);
}

void StagingBuffer::destroy()
{
    vkDestroyCommandPool(*m_pVkDevice, m_VkCommandPool, nullptr);
}

VkDevice *StaticBuffer::device = nullptr;
VkPhysicalDeviceMemoryProperties *StaticBuffer::physDevMemProps = nullptr;

void StaticBuffer::init(VkDevice *_device, VkPhysicalDeviceMemoryProperties *_physDevMemProps)
{
    device = _device;
    physDevMemProps = _physDevMemProps;
}

StaticBuffer::StaticBuffer()
    : m_vkBuffer{VK_NULL_HANDLE}, m_vkBufferMemory{VK_NULL_HANDLE}, m_vkBufferMemProps{0x0}, m_bCreated{false}, m_bDestroyed{false}
{
}

StaticBuffer::~StaticBuffer()
{
    if (!m_bDestroyed && m_bCreated)
    {
        vkDestroyBuffer(*device, m_vkBuffer, nullptr);
        vkFreeMemory(*device, m_vkBufferMemory, nullptr);
    }
}

StaticBuffer::StaticBuffer(StaticBuffer &&rhs)
    : m_vkBuffer{std::move(rhs.m_vkBuffer)}, m_vkBufferMemory{std::move(rhs.m_vkBufferMemory)}, m_vkBufferMemProps{std::move(rhs.m_vkBufferMemProps)}, m_bCreated{std::move(rhs.m_bCreated)}, m_bDestroyed{std::move(rhs.m_bDestroyed)}
{
    rhs.m_vkBuffer = VK_NULL_HANDLE;
    rhs.m_vkBufferMemory = VK_NULL_HANDLE;
    rhs.m_bCreated = false;
    rhs.m_bDestroyed = false;
}

void StaticBuffer::create(const VkBufferCreateInfo &createInfo, const VkMemoryPropertyFlags memProperties)
{
    assert((device != nullptr && physDevMemProps != nullptr) && "Attempting to create buffer before proper init");

    m_vkBufferMemProps = memProperties;

    VK_CHECK(vkCreateBuffer(*device, &createInfo, nullptr, &m_vkBuffer));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(*device, m_vkBuffer, &memReqs);

    const VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memReqs.size,
        .memoryTypeIndex = getHeapIndex(memReqs.memoryTypeBits, m_vkBufferMemProps, *physDevMemProps),
    };

    VK_CHECK(vkAllocateMemory(*device, &allocInfo, nullptr, &m_vkBufferMemory));
    VK_CHECK(vkBindBufferMemory(*device, m_vkBuffer, m_vkBufferMemory, 0));

    m_bCreated = true;
}

void StaticBuffer::destroy()
{
    vkDestroyBuffer(*device, m_vkBuffer, nullptr);
    vkFreeMemory(*device, m_vkBufferMemory, nullptr);
    m_bDestroyed = true;
    m_bCreated = false;
}

void StaticBuffer::uploadData(const VkDeviceSize nbytes, const void *data)
{
    assert((m_vkBuffer != VK_NULL_HANDLE && m_vkBufferMemory != VK_NULL_HANDLE) && "Attempting to upload data to buffer before buffer creation!");

    if (m_vkBufferMemProps & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    {
        const VkBufferCreateInfo stagingBufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = nbytes,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        StaticBuffer stagingBuffer;
        stagingBuffer.create(stagingBufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        stagingBuffer.uploadData(stagingBufferCreateInfo.size, data);

        StagingBuffer::upload(stagingBuffer.m_vkBuffer, 0, m_vkBuffer, 0, nbytes);

        return;
    }
    else if (m_vkBufferMemProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        // Mapped Memory
        void *bufferData;
        vkMapMemory(*device, m_vkBufferMemory, 0, VK_WHOLE_SIZE, 0, &bufferData);
        memcpy(bufferData, data, nbytes);

        VkMappedMemoryRange range{
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .memory = m_vkBufferMemory,
            .offset = 0,
            .size = VK_WHOLE_SIZE,
        };

        vkFlushMappedMemoryRanges(*device, 1, &range);
        vkUnmapMemory(*device, m_vkBufferMemory);
    }
}