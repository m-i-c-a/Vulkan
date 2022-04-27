#ifndef PIPELINE_BIN_HPP
#define PIPELINE_BIN_HPP

#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

#include "Types.hpp"
#include "Renderable.hpp"
#include "PipelineManager.hpp"

class PipelineBin
{
private:
    static PipelineManager* m_pPipelineManager;

    PipelineType m_eType;
    std::unordered_map<uint32_t, std::vector<Renderable>> m_vRenderables;

public:
    static void initialize(PipelineManager* pipelineManager)
    {
        m_pPipelineManager = pipelineManager;
    }

    PipelineBin()
    {
    }

    PipelineBin(PipelineType type)
        : m_eType{type}
    {
        m_pPipelineManager->createPipeline(type);
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