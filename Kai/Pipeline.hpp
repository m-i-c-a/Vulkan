#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <array>

#include <vulkan/vulkan.h>

class Pipeline
{
private:
    VkPipeline m_VkPipeline;
    VkPipelineLayout m_VkPipelineLayout;
    VkPipelineBindPoint m_VkBindPoint;

    std::array<VkDescriptorSetLayout, 4> m_VkDescSetLayouts;
public:
    void bind();
};

#endif