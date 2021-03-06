#include <array>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "App.hpp"
#include "VkStartup.hpp"
#include "VkDefines.hpp"
#include "Loader.hpp"
#include "Model.hpp"


#include "Renderer/RenderManager.hpp"

void appInit(AppResources &appResources, VulkanResources &vulkanResources)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    appResources.m_uWindowWidth = 500u;
    appResources.m_uWindowHeight = 500u;
    appResources.m_Window = glfwCreateWindow(static_cast<int>(appResources.m_uWindowWidth), static_cast<int>(appResources.m_uWindowHeight), "App", nullptr, nullptr);
    assert(appResources.m_Window != nullptr);

    uint32_t numInstanceExtensions = 0;
    const char **instanceExtensions = glfwGetRequiredInstanceExtensions(&numInstanceExtensions);

    std::array<const char *, 1> instanceLayers{"VK_LAYER_KHRONOS_validation"};
    std::array<const char *, 3> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};

    VulkanInitParams vulkanInitParams{};

    vulkanInitParams.m_uWindowWidth = appResources.m_uWindowWidth;
    vulkanInitParams.m_uWindowHeight = appResources.m_uWindowHeight;
    vulkanInitParams.m_Window = appResources.m_Window;

    vulkanInitParams.m_vInstanceExtensions.insert(vulkanInitParams.m_vInstanceExtensions.end(), &instanceExtensions[0], &instanceExtensions[numInstanceExtensions]);
    vulkanInitParams.m_vInstanceLayers.insert(vulkanInitParams.m_vInstanceLayers.end(), instanceLayers.begin(), instanceLayers.end());
    vulkanInitParams.m_vDeviceExtensions.insert(vulkanInitParams.m_vDeviceExtensions.end(), deviceExtensions.begin(), deviceExtensions.end());

    vulkanInitParams.m_uRequestedSwapchainImageCount = 2u;
    vulkanInitParams.m_VkPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    vulkanInit(vulkanInitParams, vulkanResources);

    for (VkFrame &frame : appResources.m_Frames)
        frame.init();
}

void run(AppResources &appResources, VulkanResources &vulkanResources)
{
    SceneResources sceneResources { vulkanResources.m_VkDevice, vulkanResources.m_VkSwapchainExtent, vulkanResources.m_VkSwapchainImageFormat };

    PipelineBin::initialize(&sceneResources.pipelineManger);

    sceneResources.renderer.addSortBin(SortBinType::OPAQUE, { PIPELINE_DEFAULT } );





    std::vector<Model> models = processGLTF("../models/SimplePlane.gltf");
    std::move(models.begin(), models.end(), std::back_inserter(sceneResources.m_vModels));
    models.clear();
    models = processGLTF("../models/Plane.gltf");
    std::move(models.begin(), models.end(), std::back_inserter(sceneResources.m_vModels));

    while (!glfwWindowShouldClose(appResources.m_Window))
    {
        glfwPollEvents();

        VkFrame &frame = appResources.m_Frames[appResources.m_uFrameIdx];

        VK_CHECK(vkAcquireNextImageKHR(vulkanResources.m_VkDevice, vulkanResources.m_VkSwapchain, UINT64_MAX, frame.m_VkAcquireCompleteSemaphore, VK_NULL_HANDLE, &vulkanResources.m_uSwapchainImageIdx));

        VK_CHECK(vkWaitForFences(vulkanResources.m_VkDevice, 1, &frame.m_VkCommandBufferIsExecutableFence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(vulkanResources.m_VkDevice, 1, &frame.m_VkCommandBufferIsExecutableFence));

        frame.setColorAttachment(vulkanResources.m_VkSwapchainImages[vulkanResources.m_uSwapchainImageIdx]);

        // Cull - everything passes rn
        for (const Model& model : sceneResources.m_vModels)
        {
            for (const Renderable& renderable : model.m_pPrototype->m_Renderables)
            {
                sceneResources.renderer.addRenderable(SortBinType::OPAQUE, PIPELINE_DEFAULT, 0u, renderable);
            }
        }

        // Render
        VkCommandBuffer commandBuffer = frame.render(sceneResources.renderer);

        sceneResources.renderer.reset();

        const VkSemaphoreSubmitInfoKHR acquireCompleteSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
            .semaphore = frame.m_VkAcquireCompleteSemaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, // defines second sync scope - "block on color_attachment_output" until acquire semaphore is signaled
        };

        const VkSemaphoreSubmitInfoKHR renderCompleteSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
            .semaphore = frame.m_VkRenderCompleteSemaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, // defines first sync scope - "signal render complete when color_attachment_output completes"
        };

        const VkCommandBufferSubmitInfoKHR commandBufferSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR,
            .commandBuffer = commandBuffer,
        };

        const VkSubmitInfo2KHR renderSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR,
            .waitSemaphoreInfoCount = 1u,
            .pWaitSemaphoreInfos = &acquireCompleteSemaphoreSubmitInfo,
            .commandBufferInfoCount = 1u,
            .pCommandBufferInfos = &commandBufferSubmitInfo,
            .signalSemaphoreInfoCount = 1u,
            .pSignalSemaphoreInfos = &renderCompleteSemaphoreSubmitInfo,
        };

        VK_CHECK(vulkanResources.vkQueueSubmit2KHR(vulkanResources.m_VkGraphicsQueue, 1u, &renderSubmitInfo, frame.m_VkCommandBufferIsExecutableFence));

        const VkPresentInfoKHR presentInfoKHR{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1u,
            .pWaitSemaphores = &frame.m_VkRenderCompleteSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &vulkanResources.m_VkSwapchain,
            .pImageIndices = &vulkanResources.m_uSwapchainImageIdx,
        };
        VK_CHECK(vkQueuePresentKHR(vulkanResources.m_VkGraphicsQueue, &presentInfoKHR));

        appResources.m_uFrameIdx = (appResources.m_uFrameIdx + 1) % appResources.m_Frames.size();
    }

    VK_CHECK(vkDeviceWaitIdle(vulkanResources.m_VkDevice));
}

void cleanup(AppResources &appResources, VulkanResources &vulkanResources)
{
    for (VkFrame &frame : appResources.m_Frames)
        frame.cleanup();

    vulkanDestroy(vulkanResources);

    glfwDestroyWindow(appResources.m_Window);
    glfwTerminate();
}

int main()
{
    AppResources appResources;
    VulkanResources vulkanResources;

    appInit(appResources, vulkanResources);
    run(appResources, vulkanResources);
    cleanup(appResources, vulkanResources);

    return 0;
}