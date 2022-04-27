#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Loader.hpp"
#include "Buffer.hpp"
#include "Model.hpp"

namespace
{
    constexpr bool LOADER_DEBUG = true;

    struct Vertex
    {
        glm::vec3 pos;
        // glm::vec3 normal;
        // glm::vec2 uv;
    };

#ifdef MATERIAL_LOADING
    static std::unordered_map<std::string, std::unique_ptr<BaseMaterial>> g_MaterialMap;

    static void loadMaterials(tinygltf::Model &gltfModel)
    {
        static const std::string prototypeMemberName = "prototype-id";

        for (tinygltf::Material &gltfMat : gltfModel.materials)
        {

            // Check if mat has already been loaded
            if (g_MaterialMap.contains(gltfMat.name))
            {
                if constexpr (LOADER_DEBUG)
                {
                    // TODO - if mat is already loaded, add optional check to see if mat's
                    //      contents match the current one
                }

                // This mat has already been loaded...
                continue;
            }

            // Check for material prototype
            if constexpr (LOADER_DEBUG)
            {
                assert(gltfMat.additionalValues.contains(prototypeMemberName));
            }

            const uint32_t prototypeId = static_cast<uint32_t>(gltfMat.additionalValues.at(prototypeMemberName).number_value);

            std::unique_ptr<BaseMaterial> material = BaseMaterial::create(prototypeId);
            material->load(gltfMat);

            // Add mat to global set
            g_MaterialMap[material->getName()] = std::move(material);
        }
    }
#endif

    bool loadModel(tinygltf::Model &model, const char *filename)
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
        if (!warn.empty())
        {
            std::cout << "WARN: " << warn << std::endl;
        }

        if (!err.empty())
        {
            std::cout << "ERR: " << err << std::endl;
        }

        if (!res)
            std::cout << "Failed to load glTF: " << filename << std::endl;
        else
            std::cout << "Loaded glTF: " << filename << std::endl;

        return res;
    }

    std::shared_ptr<ModelPrototype> processGLTFNode(const tinygltf::Model &gltfModel, const tinygltf::Node &gltfNode)
    {
        // Process Mesh (Group of renderables)
        const tinygltf::Mesh &gltfMesh = gltfModel.meshes[gltfNode.mesh];

        std::shared_ptr<ModelPrototype> prototype = std::make_shared<ModelPrototype>();

        std::vector<Renderable> rednerables;

        std::vector<Vertex> vertexData;
        uint32_t vertexOffset = 0u;

        std::vector<uint32_t> indexData;
        uint32_t indexOffset = 0u;

        for (uint32_t i = 0; i < gltfMesh.primitives.size(); ++i)
        {
            const tinygltf::Primitive &gltfPrimitive = gltfMesh.primitives[i];

            // Assign vertex/index buffer and
            const tinygltf::Accessor &gltfIndexAccessor = gltfModel.accessors[gltfPrimitive.indices];
            const tinygltf::BufferView &gltfIndexBufferView = gltfModel.bufferViews[gltfIndexAccessor.bufferView];
            const void *gltfIndexBuffer = &(gltfModel.buffers[gltfIndexBufferView.buffer].data[gltfIndexBufferView.byteOffset]);
            const uint32_t indexCount = gltfIndexAccessor.count;

            auto iter = gltfPrimitive.attributes.find("POSITION");
            assert(iter != gltfPrimitive.attributes.end());
            const tinygltf::Accessor &gltfPositionAccessor = gltfModel.accessors[iter->second];
            const tinygltf::BufferView &gltfPositionBufferView = gltfModel.bufferViews[gltfPositionAccessor.bufferView];
            const float *gltfPositionBuffer = reinterpret_cast<const float *>(&(gltfModel.buffers[gltfPositionBufferView.buffer].data[gltfPositionAccessor.byteOffset + gltfPositionBufferView.byteOffset]));
            const int32_t posStride = gltfPositionAccessor.ByteStride(gltfPositionBufferView) / sizeof(float);

            for (uint32_t vertexIndex = 0; vertexIndex < gltfPositionAccessor.count; ++vertexIndex)
            {
                Vertex vertex;
                vertex.pos = glm::make_vec3(&(gltfPositionBuffer[vertexIndex * posStride]));

                vertexData.push_back(vertex);
            }

            switch (gltfIndexAccessor.componentType)
            {
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
            {
                const uint32_t *buf = static_cast<const uint32_t *>(gltfIndexBuffer);
                for (size_t index = 0; index < indexCount; index++)
                {
                    indexData.push_back(buf[index]);
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
            {
                const uint16_t *buf = static_cast<const uint16_t *>(gltfIndexBuffer);
                for (size_t index = 0; index < indexCount; index++)
                {
                    indexData.push_back(buf[index]);
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
            {
                const uint8_t *buf = static_cast<const uint8_t *>(gltfIndexBuffer);
                for (size_t index = 0; index < indexCount; index++)
                {
                    indexData.push_back(buf[index]);
                }
                break;
            }
            default:
                std::cerr << "Index component type " << gltfIndexAccessor.componentType << " not supported!" << std::endl;
                exit(EXIT_FAILURE);
            }

            Renderable renderable { prototype->m_IndexBuffer, prototype->m_VertexBuffer };
            renderable.vertexOffset = vertexOffset;
            renderable.firstIndex = indexOffset;
            renderable.indexCount = indexCount;

            prototype->m_Renderables.push_back(renderable);

            vertexOffset += gltfPositionAccessor.count;
            indexOffset += gltfIndexAccessor.count;
        }

        VkBufferCreateInfo vertexBufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = static_cast<VkDeviceSize>(vertexData.size() * sizeof(Vertex)),
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        prototype->m_VertexBuffer.create(vertexBufferCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        prototype->m_VertexBuffer.uploadData(vertexBufferCreateInfo.size, vertexData.data());

        VkBufferCreateInfo indexBufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = static_cast<VkDeviceSize>(indexData.size() * sizeof(uint32_t)),
            .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        prototype->m_IndexBuffer.create(indexBufferCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        prototype->m_IndexBuffer.uploadData(indexBufferCreateInfo.size, indexData.data());

        return prototype;
    }
}

std::vector<Model> processGLTF(const std::string &filepath)
{
    // We are starting to process a new model that we have NOT processed before
    std::vector<Model> models;

    tinygltf::Model gltfModel;
    if (!loadModel(gltfModel, filepath.c_str()))
        return models;

#ifdef MATERIAL_LOADING
    loadMaterials(gltfModel);
#endif

    const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i)
    {
        assert((scene.nodes[i] >= 0) && (scene.nodes[i] < gltfModel.nodes.size()));

        const tinygltf::Node &gltfNode = gltfModel.nodes[scene.nodes[i]];
        std::shared_ptr<ModelPrototype> prototype = processGLTFNode(gltfModel, gltfNode);

        // Extract translation
        glm::mat4 transform { 1.0f };

        if (!gltfNode.translation.empty())
        {
            const glm::vec3 translation = glm::make_vec3(gltfNode.translation.data());
            transform = glm::translate(transform, translation);
        }

        if (!gltfNode.rotation.empty())
        {
            const glm::quat quaternion = glm::make_quat(gltfNode.rotation.data());
            const glm::mat4 rotationMatrix = glm::toMat4(quaternion);
            transform = (rotationMatrix * transform);
        }

        if (!gltfNode.scale.empty())
        {
            const glm::vec3 scale = glm::make_vec3(gltfNode.scale.data());
            transform = glm::scale(transform, scale);
        }

        models.emplace_back(prototype, transform);
    }

    return models;
}