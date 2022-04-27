#ifndef PIPELINE_MANAGER_HPP
#define PIPELINE_MANAGER_HPP

#include <array>

#include <vulkan/vulkan.h>

#include "Types.hpp"

class PipelineManager
{
private:
    VkDevice m_VkDevice;
    VkExtent2D m_VkSwapchainExtent;
    VkFormat m_VkSwapchainFormat;

    struct Pipeline
    {
        VkPipeline m_VkPipeline;
        VkPipelineLayout m_VkPipelineLayout;
        std::array<VkDescriptorSetLayout, 2> m_vVkDescriptorSetLayouts; // 0=PIPELINE, 1=OBJECT

        Pipeline()
            : m_VkPipeline{VK_NULL_HANDLE}, m_VkPipelineLayout{VK_NULL_HANDLE}, m_vVkDescriptorSetLayouts{VK_NULL_HANDLE, VK_NULL_HANDLE}
        {
        }
    };

    std::array<Pipeline, PIPELINE_COUNT> m_vPipelines;

public:
    PipelineManager(VkDevice device, VkExtent2D extent, VkFormat format)
        : m_VkDevice{device}, m_VkSwapchainExtent{extent}, m_VkSwapchainFormat{format}
    {
    }

    ~PipelineManager();

    VkPipeline getPipeline(PipelineType type)
    {
        return m_vPipelines[type].m_VkPipeline;
    }

    void createPipeline(PipelineType type);
    void destroy();
};

#endif // PIPELINE_MANAGER_HPP