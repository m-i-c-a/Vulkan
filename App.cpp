#include "App.hpp"
#include "VkDefines.hpp"

// TODO - move this stuff to VkRuntime

VkShaderModule createShaderModule(VkDevice device, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Failed to open file %s!\n", filename);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    const size_t nbytes_file_size = (size_t)ftell(f);
    rewind(f);

    uint32_t *buffer = (uint32_t *)malloc(nbytes_file_size);
    fread(buffer, nbytes_file_size, 1, f);
    fclose(f);

    const VkShaderModuleCreateInfo ci_shader_module{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0x0,
        .codeSize = nbytes_file_size,
        .pCode = buffer,
    };

    VkShaderModule vk_shader_module;
    VkResult result = vkCreateShaderModule(device, &ci_shader_module, NULL, &vk_shader_module);
    if (result != VK_SUCCESS)
    {
        printf("Failed to create shader module for %s!\n", filename);
        exit(EXIT_FAILURE);
    }

    free(buffer);

    return vk_shader_module;
}

VkPipelineLayout createDefaultGraphicsPipelineLayout(VkDevice device)
{
    const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    VkPipelineLayout pipelineLayout;
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));
    return pipelineLayout;
}

VkPipeline createDefaultGraphicsPipeline(VkDevice device, VkExtent2D swapchainExent, VkFormat swapchainImageFormat, VkPipelineLayout pipelineLayout)
{
    const std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo{{{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                                                         .stage = VK_SHADER_STAGE_VERTEX_BIT,
                                                                                         .module = createShaderModule(device, "../shaders/default-vert.spv"),
                                                                                         .pName = "main",
                                                                                         .pSpecializationInfo = nullptr},
                                                                                        {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                                                         .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                                         .module = createShaderModule(device, "../shaders/default-frag.spv"),
                                                                                         .pName = "main",
                                                                                         .pSpecializationInfo = nullptr}}};

    const std::array<VkVertexInputBindingDescription, 1> vertexInputBindingDescription{{{.binding = 0,
                                                                                         .stride = sizeof(float) * 3,
                                                                                         .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}}};

    const std::array<VkVertexInputAttributeDescription, 1> vertexInputAttributeDescription{{
        {.location = 0,
         .binding = 0,
         .format = VK_FORMAT_R32G32B32_SFLOAT,
         .offset = 0},
    }};

    const VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = vertexInputBindingDescription.size(),
        .pVertexBindingDescriptions = vertexInputBindingDescription.data(),
        .vertexAttributeDescriptionCount = vertexInputAttributeDescription.size(),
        .pVertexAttributeDescriptions = vertexInputAttributeDescription.data()};

    const VkViewport viewport{
        .x = 0,
        .y = 0,
        .width = static_cast<float>(swapchainExent.width),
        .height = static_cast<float>(swapchainExent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    const VkRect2D scissorRect{
        .offset = {.x = 0, .y = 0},
        .extent = swapchainExent};

    const VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissorRect,
    };

    const VkPipelineColorBlendAttachmentState pipelineColorBlendState{
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    const VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &pipelineColorBlendState,
        .blendConstants = {0, 0, 0, 0}};

    const VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE};

    const VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp = 0.f,
        .depthBiasSlopeFactor = 0.f,
        .lineWidth = 1.f,
    };

    const VkPipelineMultisampleStateCreateInfo pipelineMulisampleStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    const VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &swapchainImageFormat};

    const VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &pipelineRenderingCreateInfo,
        .stageCount = pipelineShaderStageCreateInfo.size(),
        .pStages = pipelineShaderStageCreateInfo.data(),
        .pVertexInputState = &pipelineVertexInputStateCreateInfo,
        .pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
        .pViewportState = &pipelineViewportStateCreateInfo,
        .pRasterizationState = &pipelineRasterizationStateCreateInfo,
        .pMultisampleState = &pipelineMulisampleStateCreateInfo,
        .pColorBlendState = &pipelineColorBlendStateCreateInfo,
        .layout = pipelineLayout,
        .renderPass = nullptr,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipeline graphicsPipeline;
    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline));

    vkDestroyShaderModule(device, pipelineShaderStageCreateInfo[0].module, nullptr);
    vkDestroyShaderModule(device, pipelineShaderStageCreateInfo[1].module, nullptr);

    return graphicsPipeline;
}