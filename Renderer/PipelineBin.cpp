#include <iostream>

#include "PipelineBin.hpp"
#include "PipelineCreator.hpp"

void PipelineBin::render(VkCommandBuffer commandBuffer) const
{
    // bind pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vPipelines[m_eType].m_VkPipeline);

    for (const auto& [matId, renderables] : m_vRenderables)
    {
        // bind mat

        for (const Renderable& renderable : renderables)
        {
            renderable.render(commandBuffer);
        }
    }
}

void PipelineBin::createPipeline(PipelineType type)
{
    switch (type)
    {
        case PIPELINE_DEFAULT:
        {
            m_vPipelines[PIPELINE_DEFAULT].m_vVkDescriptorSetLayouts = PipelineCreator::createDefaultDescriptorSetLayouts();
            m_vPipelines[PIPELINE_DEFAULT].m_VkPipelineLayout = PipelineCreator::createDefaultPipelineLayout(m_vPipelines[PIPELINE_DEFAULT].m_vVkDescriptorSetLayouts);
            m_vPipelines[PIPELINE_DEFAULT].m_VkPipeline = PipelineCreator::createDefaultPipeline(m_vPipelines[PIPELINE_DEFAULT].m_VkPipelineLayout);
            break;
        }
        default:
            std::cout << "Tried to create invalid pipeline!\n";
    }
}
