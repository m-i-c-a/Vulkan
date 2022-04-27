#include <iostream>

#include "PipelineBin.hpp"

PipelineManager* PipelineBin::m_pPipelineManager = nullptr;

void PipelineBin::render(VkCommandBuffer commandBuffer) const
{
    // bind pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pPipelineManager->getPipeline(m_eType));

    for (const auto& [matId, renderables] : m_vRenderables)
    {
        // bind mat

        for (const Renderable& renderable : renderables)
        {
            renderable.render(commandBuffer);
        }
    }
}
