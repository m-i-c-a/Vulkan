#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <memory>

#include "Buffer.hpp"

// Corresponds to a gltf mesh primitive
struct Renderable {
    // StaticBuffer& vertexBuffer;
    // StaticBuffer& indexBuffer; // also stores index count

    VkDescriptorSet descriptorSet; // per draw data

    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;
};

struct ModelPrototype {
    StaticBuffer m_VertexBuffer;    
    StaticBuffer m_IndexBuffer;

    StaticBuffer m_StaticBuffer;

    std::vector<VkImage> m_VkImages;
    std::vector<VkImageView> m_VkImageViews;
    std::vector<VkDeviceMemory> m_VkImageDeviceMemory;

    std::vector<uint32_t> m_vMaterialIds;
    std::vector<Renderable> m_Renderables;
};

struct Model
{
    Model(const std::shared_ptr<ModelPrototype> prototype)
    : m_pPrototype(prototype)
    {}

    uint16_t handle;
    const std::shared_ptr<ModelPrototype> m_pPrototype;
};

#endif // MODEL_HPP