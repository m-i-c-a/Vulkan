#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#include <vulkan/vulkan.h>

#include "../Buffer.hpp"

struct Renderable {
    const StaticBuffer& m_IndexBuffer; // also stores index count
    const StaticBuffer& m_VertexBuffer;

    VkDescriptorSet descriptorSet; // per draw data

    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;

    Renderable(const StaticBuffer& indexBuffer, const StaticBuffer& vertexBuffer)
        : m_IndexBuffer { indexBuffer }
        , m_VertexBuffer { vertexBuffer }
    {}

    void render(VkCommandBuffer commandBuffer) const;
};



#endif // RENDERABLE_HPP