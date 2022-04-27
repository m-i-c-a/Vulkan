#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <memory>

#include <glm/mat4x4.hpp>

#include "Buffer.hpp"
#include "Renderer/Renderable.hpp"


struct ModelPrototype {
    StaticBuffer m_VertexBuffer;    
    StaticBuffer m_IndexBuffer;

    std::vector<VkImage> m_VkImages;
    std::vector<VkImageView> m_VkImageViews;
    std::vector<VkDeviceMemory> m_VkImageDeviceMemory;

    std::vector<uint32_t> m_vMaterialIds;
    std::vector<Renderable> m_Renderables;
};

struct Model
{
    Model(std::shared_ptr<ModelPrototype> prototype, glm::mat4 transform)
    : m_pPrototype(prototype)
    , m_m4Transform(transform)
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
    glm::mat4 m_m4Transform;
};

#endif // MODEL_HPP