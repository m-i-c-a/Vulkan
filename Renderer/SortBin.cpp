#include "SortBin.hpp"

#include <iostream>

void SortBin::addPipeline(PipelineType type)
{
    if (!m_Pipelines.emplace(std::make_pair(type, type)).second)
    {
        std::cout << "Attemping to add already added pipeline to SortBin!\n";
    }
}