#ifndef APP_HPP
#define APP_HPP

#include <vector>
#include <array>

#include <vulkan/vulkan.h>

#include "Model.hpp"
#include "VkFrame.hpp"
#include "Renderer/RenderManager.hpp"

class GLFWwindow;

struct SceneResources
{   
    enum
    {
        PIPELINE_DEFAULT = 0,
        PIPELINE_COUNT   = 1
    };

    VkPipelineLayout m_VkPipelineLayout;

    RenderManager renderer;

    std::vector<Model> m_vModels;
    std::array<VkPipelineLayout, PIPELINE_COUNT> m_vVkPipelineLayouts;
    std::array<VkPipeline, PIPELINE_COUNT> m_vVkPipelines;
};

struct AppResources
{
    GLFWwindow *m_Window;
    uint32_t m_uWindowWidth;
    uint32_t m_uWindowHeight;

    std::array<VkFrame, 2> m_Frames;
    uint32_t m_uFrameIdx;
};

VkPipelineLayout createDefaultGraphicsPipelineLayout(VkDevice device);
VkPipeline createDefaultGraphicsPipeline(VkDevice device, VkExtent2D swapchainExent, VkFormat swapchainImageFormat, VkPipelineLayout pipelineLayout);

#endif // APP_HPP
