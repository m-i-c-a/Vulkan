// #include "SortBinType.hpp"
#include "RenderManager.hpp"
// #include "PipelineBin.hpp"
// #include "SortBin.hpp"


int main()
{
    RenderManager mananger;
    mananger.addSortBin(SortBinType::OPAQUE, { PIPELINE_DEFAULT } );

    Renderable renderable;

    for (int i = 0; i < 10; ++i)
    {
        mananger.addRenderable(SortBinType::OPAQUE, PIPELINE_DEFAULT, 0, renderable);
        mananger.render();
        mananger.reset();
    }
}