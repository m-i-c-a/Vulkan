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
    Model(std::shared_ptr<ModelPrototype> prototype)
    : m_pPrototype(prototype)
    {}

    Model(Model&& other)
    : m_uHandle(std::move(other.m_uHandle))
    , m_pPrototype(std::move(other.m_pPrototype))
    {
        other.m_uHandle = -1;
        other.m_pPrototype = nullptr;
    }

    Model& operator=(Model&& rhs)
    {
        m_uHandle = rhs.m_uHandle;
        m_pPrototype = rhs.m_pPrototype;

        rhs.m_uHandle = -1;
        rhs.m_pPrototype = nullptr;

        return *this;
    }

    uint16_t m_uHandle;
    std::shared_ptr<ModelPrototype> m_pPrototype;
};

#endif // MODEL_HPP