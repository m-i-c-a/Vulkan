#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include <unordered_map>

#include "Types.hpp"
#include "SortBin.hpp"
#include "PipelineBin.hpp"

class RenderManager
{
private:
    std::unordered_map<SortBinType, SortBin> m_vSortBins;
public:
    RenderManager() = default;

    void addSortBin(SortBinType type, std::vector<PipelineType> pipelines)
    {
        m_vSortBins.emplace(std::make_pair(type, type));

        for (const auto& pipeline : pipelines)
            m_vSortBins[type].addPipeline(pipeline);
    }

    void addRenderable(SortBinType sortBinType, PipelineType pipelineType, uint32_t matId, Renderable renderable)
    {
        m_vSortBins[sortBinType].addRenderable(pipelineType, matId, renderable); 
    }

    void render(VkCommandBuffer commandBuffer) const
    {
        for (const auto& [type, bin] : m_vSortBins)
        {
            bin.render(commandBuffer);
        }
    }

    void reset()
    {
        for (auto& [type, bin] : m_vSortBins)
            bin.reset();
    }
};

#endif // RENDER_MANAGER_HPP