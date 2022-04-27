#ifndef PIPELINE_CREATOR_HPP
#define PIPELINE_CREATOR_HPP

#include <vector>

#include <vulkan/vulkan.h>

class PipelineCreator
{
private:
    static VkDevice m_VkDevice;
    static VkExtent2D m_VkSwapchainExtent;
    static VkFormat m_VkSwapchainFormat;

public:

    static void initiailize(VkDevice device, VkExtent2D extent, VkFormat format)
    {
        m_VkDevice = device;
        m_VkSwapchainExtent = extent;
        m_VkSwapchainFormat = format;
    }

    static std::vector<VkDescriptorSetLayout> createDefaultDescriptorSetLayouts();
    static VkPipelineLayout createDefaultPipelineLayout(std::vector<VkDescriptorSetLayout>& layouts);
    static VkPipeline createDefaultPipeline(VkPipelineLayout layout);
};

#endif // PIPELINE_CREATOR_HPP