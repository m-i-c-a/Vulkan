#ifndef PIPELINE_BIN_HPP
#define PIPELINE_BIN_HPP

#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

#include "Types.hpp"
#include "Renderable.hpp"
#include "PipelineCreator.hpp"

class PipelineBin
{
private:
    struct Pipeline
    {
        VkPipeline m_VkPipeline;
        VkPipelineLayout m_VkPipelineLayout;
        std::vector<VkDescriptorSetLayout> m_vVkDescriptorSetLayouts;

        Pipeline()
            : m_VkPipeline{VK_NULL_HANDLE}, m_VkPipelineLayout{VK_NULL_HANDLE}
        {
        }
    };

    inline static std::array<Pipeline, PIPELINE_COUNT> m_vPipelines;
    static void createPipeline(PipelineType type);

    PipelineType m_eType;
    std::unordered_map<uint32_t, std::vector<Renderable>> m_vRenderables;

public:
    static void destroy(VkDevice device)
    {
        for (Pipeline pipeline : m_vPipelines)
        {
            for (const auto layout : pipeline.m_vVkDescriptorSetLayouts)
                vkDestroyDescriptorSetLayout(device, layout, nullptr);

            vkDestroyPipelineLayout(device, pipeline.m_VkPipelineLayout, nullptr);
            vkDestroyPipeline(device, pipeline.m_VkPipeline, nullptr);
        }
    }

    PipelineBin()
    {
    }

    PipelineBin(PipelineType type)
        : m_eType{type}
    {
        if (m_vPipelines[type].m_VkPipeline == VK_NULL_HANDLE)
            createPipeline(type);
    }

    void addRenderable(uint32_t matId, const Renderable &renderable)
    {
        m_vRenderables[matId].push_back(renderable);
    }

    void reset()
    {
        m_vRenderables.clear();
    }

    void render(VkCommandBuffer commandBuffer) const;

    bool operator==(const PipelineBin &rhs) const { return m_eType == rhs.m_eType; }
};

#endif // PIPELINE_BIN_HPP