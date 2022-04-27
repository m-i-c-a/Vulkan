#include<iostream>

#include "Defines.hpp"
#include "PipelineManager.hpp"

namespace
{
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
}

namespace Default
{
    void createDescriptorSetLayouts(VkDevice device, std::array<VkDescriptorSetLayout, 2>& layouts)
    {

        const std::array<VkDescriptorSetLayoutBinding, 1> pipelineBindings {{
            {
                .binding = 0u,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1u,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            }
        }};

        const std::array<VkDescriptorSetLayoutBinding, 1> objectBindings {{
            {
                .binding = 0u,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1u,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            }
        }};

        const VkDescriptorSetLayoutCreateInfo pipelineSetCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0x0,
            .bindingCount = static_cast<uint32_t>(pipelineBindings.size()),
            .pBindings = pipelineBindings.data()
        };

        const VkDescriptorSetLayoutCreateInfo objectSetCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0x0,
            .bindingCount = static_cast<uint32_t>(pipelineBindings.size()),
            .pBindings = pipelineBindings.data()
        };

        VK_CHECK(vkCreateDescriptorSetLayout(device, &pipelineSetCreateInfo, nullptr, &layouts[0]));
        VK_CHECK(vkCreateDescriptorSetLayout(device, &objectSetCreateInfo  , nullptr, &layouts[1]));
    }

    void createPipelineLayout(VkDevice device, std::array<VkDescriptorSetLayout, 2>& layouts, VkPipelineLayout& layout)
    {
        const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = static_cast<uint32_t>(layouts.size()),
            .pSetLayouts = layouts.data(),
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };

        VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &layout));
    }

    void createPipeline(VkDevice device, VkExtent2D swapchainExtent, VkFormat swapchainFormat, VkPipelineLayout layout, VkPipeline& pipeline)
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
            .width = static_cast<float>(swapchainExtent.width),
            .height = static_cast<float>(swapchainExtent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        const VkRect2D scissorRect{
            .offset = {.x = 0, .y = 0},
            .extent = swapchainExtent};

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
            .pColorAttachmentFormats = &swapchainFormat};

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
            .layout = layout,
            .renderPass = nullptr,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
        };

        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline));

        vkDestroyShaderModule(device, pipelineShaderStageCreateInfo[0].module, nullptr);
        vkDestroyShaderModule(device, pipelineShaderStageCreateInfo[1].module, nullptr);
    }
}

PipelineManager::~PipelineManager()
{
    for (Pipeline pipeline : m_vPipelines)
    {
        for (const auto layout : pipeline.m_vVkDescriptorSetLayouts)
            vkDestroyDescriptorSetLayout(m_VkDevice, layout, nullptr);

        vkDestroyPipelineLayout(m_VkDevice, pipeline.m_VkPipelineLayout, nullptr);
        vkDestroyPipeline(m_VkDevice, pipeline.m_VkPipeline, nullptr);
    }
}

void PipelineManager::createPipeline(PipelineType type)
{
    switch (type)
    {
    case PIPELINE_DEFAULT:
    {
        Pipeline& pipeline = m_vPipelines[PIPELINE_DEFAULT];
        Default::createDescriptorSetLayouts(m_VkDevice, pipeline.m_vVkDescriptorSetLayouts);
        Default::createPipelineLayout(m_VkDevice, pipeline.m_vVkDescriptorSetLayouts, pipeline.m_VkPipelineLayout);
        Default::createPipeline(m_VkDevice, m_VkSwapchainExtent, m_VkSwapchainFormat, pipeline.m_VkPipelineLayout, pipeline.m_VkPipeline);
        break;
    }
    default:
        std::cout << "Tried to create invalid pipeline!\n";
    }
}
