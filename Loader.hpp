#ifndef MICA_LOADER_HPP
#define MICA_LOADER_HPP

#include <iostream>

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h>
#include <glm/glm.hpp>

bool loadModel(tinygltf::Model& model, const char* filename)
{
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }

  if (!res)
    std::cout << "Failed to load glTF: " << filename << std::endl;
  else
    std::cout << "Loaded glTF: " << filename << std::endl;

  return res;
}


struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

void loadMaterials(tinygltf::Model& gltfModel)
{
  for (tinygltf::Material &mat : gltfModel.materials) {
    std::cout << mat.extras.
  }
}


void processGLTF(const std::string& filepath)
{
  // We are starting to process a new model that we have NOT processed before

    tinygltf::Model gltfModel;
    if (!loadModel(gltfModel, filepath.c_str())) return;

    loadMaterials(gltfModel);

    std::vector<uint32_t> indexBuffer;
    std::vector<Vertex> vertexBuffer;

    const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i){
        assert((scene.nodes[i] >= 0) && (scene.nodes[i] < gltfModel.nodes.size()));

        const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];

    }



}


#endif // MICA_LOADER_HPP