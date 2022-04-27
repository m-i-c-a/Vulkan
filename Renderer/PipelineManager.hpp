#ifndef PIPELINE_MANAGER_HPP
#define PIPELINE_MANAGER_HPP

#include <vector>
#include <array>

#include <vulkan/vulkan.h>

#include "Types.hpp"

class PipelineManager
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
public:
};

#endif // PIPELINE_MANAGER_HPP