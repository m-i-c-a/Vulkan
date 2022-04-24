#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>

#include <vulkan/vulkan.h>

class StaticBuffer
{
private:
    static VkDevice* device;
    static VkPhysicalDeviceMemoryProperties* physDevMemProps;

    VkBuffer m_vkBuffer;
    VkDeviceMemory m_vkBufferMemory;
    VkMemoryPropertyFlags m_vkBufferMemProps;

    bool m_bDestroyed;
    bool m_bCreated;
public:
    static void init(VkDevice* device, VkPhysicalDeviceMemoryProperties* physDevMemProps);

    StaticBuffer();
    ~StaticBuffer();
    StaticBuffer(StaticBuffer&& rhs);

    void create(const VkBufferCreateInfo& createInfo, const VkMemoryPropertyFlags memProperties);
    void destroy();
    void uploadData(const VkDeviceSize nbytes, const void* data);

    VkBuffer getBuffer() const { return m_vkBuffer; }
    const VkBuffer* getBufferPointer() const { return &m_vkBuffer; }
};

class DynamicBuffer
{
    std::vector<StaticBuffer> m_vBuffer;
};

class StagingBuffer
{
private:
    static VkDevice* m_pVkDevice;
    static VkQueue m_VkQueue;

    static VkCommandPool m_VkCommandPool;
    static VkCommandBuffer m_VkCommandBuffer;
public:
    static void init(VkDevice *device, VkQueue queue, uint32_t queueFamilyIndex);
    static void upload(VkBuffer srcBuffer, VkDeviceSize srcOffset, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize nbytes);
    static void destroy();
};

#endif // BUFFER_HPP