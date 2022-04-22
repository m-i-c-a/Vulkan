

// Handles mapping an object (Model) to its associated material instance
// So, if 20 models share the same mateiral isntance, we don't have 20 material
// instances with the same data.
// Model will need to store a handle or something to material instance in here
// right now the case above is occurring (every model will have a unique material instance)
class MaterialInstanceMapper {

};



struct MaterialInstance {
    std::unordered_map<const char*, uint32_t> offset;
    std::vector<unsigned char> data;

    VkDescriptorSet set;
};



struct ModelPrototype {
    VkBuffer m_VkVertexBuffer;    
    VkDeviceMemory m_VkBufferDeviceMemory;

    std::vector<VkImage> m_VkImages;
    std::vector<VkImageView> m_VkImageViews;
    std::vector<VkDeviceMemory> m_VkImageDeviceMemory;
};

std::unordered_map<std::string, ModelPrototype> gNameToPrototypeMap;

struct PerObjData {
    glm::mat4 modelMat;
};

struct Model {
    uint16_t handle;

    // doesn't have to be the pipeline handle itself, just needs ot be someting that can identify 
    // which pipline we belong to
    VkPipeline pipeline;
    PerObjData perObjData;

    VkDescriptorSet objectSet;

    ModelPrototype& prototype;
    MaterialInstance& materialInstance; // materials are currently non modifialble with thise setup
};






struct Renderable {
    std::vector<float>& vertexBuffer;
    std::vector<uint32_t>& indexBuffer; // also stores index count

    VkDescriptorSet descriptorSet; // per draw data

    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;
};





struct MaterialLayoutRenderBin {
    VkPipeline pipeline;
};

struct MaterialInstanceRenderBin {
    VkDescSet set;
};

struct Renderable {
    VkDescSet set;
};

{
    // bind per frame desc set

    for (materialLayout : materialRenderBin)
    {
        // bind pipeline

        for (materialInstance : materialLayout.instances)
        {
            // bind material instance desc set

            for (renderable : materialInstance.renderables)
            {
                // bind renderable desc set

                // bind renderable buffers

                // render
            }
        }
    }
}