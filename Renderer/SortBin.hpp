#ifndef SORT_BIN_HPP
#define SORT_BIN_HPP

#include <unordered_map>

#include "Types.hpp"
#include "PipelineBin.hpp"

class SortBin
{
private:
    SortBinType m_eType;
    std::unordered_map<PipelineType, PipelineBin> m_Pipelines;

public:
    SortBin() 
        : m_eType { SortBinType::COUNT }
    {}

    SortBin(SortBinType type)
        : m_eType { type } 
    {}

    void addPipeline(PipelineType type);

    void addRenderable(PipelineType type, uint32_t materialId, Renderable& renderable)
    {
        m_Pipelines[type].addRenderable(materialId, renderable);
    }

    void render(VkCommandBuffer commandBuffer) const
    {
        for (const auto& [type, bin] : m_Pipelines)
            bin.render(commandBuffer);
    }

    void reset()
    {
        for (auto& [type, bin] : m_Pipelines)
            bin.reset();
    }

    bool operator==(const SortBin& rhs) const { return m_eType == rhs.m_eType; }
};

#endif // SORT_BIN_HPP