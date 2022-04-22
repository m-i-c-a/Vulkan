#include <iostream>
#include <assert.h>
#include <vector>
#include <array>
#include <unordered_map>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Loader.hpp"

#define VK_CHECK(result)              \
    do                                \
    {                                 \
        assert(result == VK_SUCCESS); \
    } while (0)

VkInstance createInstance(const std::vector<const char *> &extensions, const std::vector<const char *> &layers)
{

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = nullptr;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = nullptr;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instanceCreateInfo.ppEnabledLayerNames = layers.data();
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    VkInstance instance;
    VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
    return instance;
}

VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow *window)
{
    VkSurfaceKHR surface;
    VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
    return surface;
}

VkPhysicalDevice selectPhysicalDevice(VkInstance instance)
{
    uint32_t numPhysicalDevices = 0;
    vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr);
    VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[numPhysicalDevices];
    vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, physicalDevices);

    std::cout << "# Physical Devices: " << numPhysicalDevices << '\n';
    for (uint32_t i = 0; i < numPhysicalDevices; ++i)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &props);
        std::cout << i << " : " << props.deviceName << '\n';
    }

    const uint32_t physDevIndex = 2u;
    std::cout << "\nUsing Physical Device " << physDevIndex << '\n';

    VkPhysicalDevice physicalDevice = physicalDevices[physDevIndex];
    delete[] physicalDevices;
    return physicalDevice;
}

uint32_t selectGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    uint32_t numQueueFamilyProperties = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilyProperties, nullptr);
    VkQueueFamilyProperties *queueFamilyProperties = new VkQueueFamilyProperties[numQueueFamilyProperties];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilyProperties, queueFamilyProperties);

    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t presentQueueFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i < numQueueFamilyProperties; ++i)
    {
        VkBool32 q_fam_supports_present = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &q_fam_supports_present);

        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && q_fam_supports_present == VK_TRUE)
        {
            graphicsQueueFamilyIndex = i;
            presentQueueFamilyIndex = i;
            break;
        }
    }

    delete[] queueFamilyProperties;

    assert(graphicsQueueFamilyIndex < UINT32_MAX && "no supported graphics queue family index");
    assert(presentQueueFamilyIndex < UINT32_MAX && "no supported present queue family index");
    assert(graphicsQueueFamilyIndex == presentQueueFamilyIndex && "queue families (graphics/present) do not match");

    return graphicsQueueFamilyIndex;
}

VkDevice createDevice(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamilyIndex, const std::vector<const char *> &deviceExtensions)
{

    const float queuePriority = 1.0f;

    const VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = graphicsQueueFamilyIndex,
        .queueCount = 1u,
        .pQueuePriorities = &queuePriority};

    constexpr VkPhysicalDeviceSynchronization2Features synchronization2Features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
        .pNext = nullptr,
        .synchronization2 = VK_TRUE};

    const VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .pNext = (void *)(&synchronization2Features),
        .dynamicRendering = VK_TRUE};

    const VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &dynamicRenderingFeatures, // nullptr,
        .queueCreateInfoCount = 1u,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledLayerCount = 0u,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = nullptr};

    VkDevice device;
    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));
    return device;
}

VkQueue createGraphicsQueue(VkDevice device, uint32_t graphicsQueueFamilyIndex)
{
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    return graphicsQueue;
}

VkSwapchainKHR createSwapchain(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice device, uint32_t imageCount, VkFormat &format, VkExtent2D &extent, VkPresentModeKHR presentMode)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchainCreateInfo.surface = surface;

    //** Image Count
    {
        assert(imageCount > 0 && "Invalid requested image count for swapchain!");

        // If the minImageCount is 0, then there is not a limit on the number of images the swapchain
        // can support (ignoring memory constraints). See the Vulkan Spec for more information.
        if (surfaceCapabilities.maxImageCount == 0)
        {
            if (imageCount >= surfaceCapabilities.minImageCount)
            {
                swapchainCreateInfo.minImageCount = imageCount;
            }
            else
            {
                std::cout << "Failed to create Swapchain. The requested number of images " << imageCount << " does not meet the minimum requirement of " << surfaceCapabilities.minImageCount << '\n';
                exit(EXIT_FAILURE);
            }
        }
        else if (imageCount >= surfaceCapabilities.minImageCount &&
                 imageCount <= surfaceCapabilities.maxImageCount)
        {
            swapchainCreateInfo.minImageCount = imageCount;
        }
        else
        {
            std::cout << "The number of requested Swapchain images " << imageCount << " is not supported. Min: " << surfaceCapabilities.minImageCount << " Max: " << surfaceCapabilities.maxImageCount << '\n';
            exit(EXIT_FAILURE);
        }
    }

    //** Image Format
    {
        uint32_t numSupportedSurfaceFormats = 0;
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &numSupportedSurfaceFormats, nullptr));
        VkSurfaceFormatKHR *supportedSurfaceFormats = new VkSurfaceFormatKHR[numSupportedSurfaceFormats];
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &numSupportedSurfaceFormats, supportedSurfaceFormats));

        bool requestedFormatFound = false;
        for (uint32_t i = 0; i < numSupportedSurfaceFormats; ++i)
        {
            if (supportedSurfaceFormats[i].format == format)
            {
                swapchainCreateInfo.imageFormat = supportedSurfaceFormats[i].format;
                swapchainCreateInfo.imageColorSpace = supportedSurfaceFormats[i].colorSpace;
                requestedFormatFound = true;
                break;
            }
        }

        if (!requestedFormatFound)
        {
            swapchainCreateInfo.imageFormat = supportedSurfaceFormats[0].format;
            swapchainCreateInfo.imageColorSpace = supportedSurfaceFormats[0].colorSpace;
        }

        delete[] supportedSurfaceFormats;
        format = swapchainCreateInfo.imageFormat;
    }

    //** Extent
    {
        // The Vulkan Spec states that if the current width/height is 0xFFFFFFFF, then the surface size
        // will be deteremined by the extent specified in the VkSwapchainCreateInfoKHR.
        if (surfaceCapabilities.currentExtent.width != (uint32_t)-1)
        {
            swapchainCreateInfo.imageExtent = extent;
        }
        else
        {
            swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
        }

        extent = swapchainCreateInfo.imageExtent;
    }

    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;

    //** Pre Transform
    {
        if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        {
            swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }
        else
        {
            swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
            printf("WARNING - Swapchain pretransform is not IDENTITIY_BIT_KHR!\n");
        }
    }

    //** Composite Alpha
    {
        // Determine the composite alpha format the application needs.
        // Find a supported composite alpha format (not all devices support alpha opaque),
        // but we prefer it.
        // Simply select the first composite alpha format available
        // Used for blending with other windows in the system
        VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };
        for (size_t i = 0; i < 4; ++i)
        {
            if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i])
            {
                swapchainCreateInfo.compositeAlpha = compositeAlphaFlags[i];
                break;
            };
        }
    }

    //** Present Mode
    {
        uint32_t numSupportedPresentModes = 0;
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &numSupportedPresentModes, nullptr));
        VkPresentModeKHR *supportedPresentModes = new VkPresentModeKHR[numSupportedPresentModes];
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &numSupportedPresentModes, supportedPresentModes));

        // Determine the present mode the application needs.
        // Try to use mailbox, it is the lowest latency non-tearing present mode
        // All devices support FIFO (this mode waits for the vertical blank or v-sync)
        swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        for (uint32_t i = 0; i < numSupportedPresentModes; ++i)
        {
            if (supportedPresentModes[i] == presentMode)
            {
                swapchainCreateInfo.presentMode = presentMode;
                break;
            }
        }

        delete[] supportedPresentModes;
    }

    //** Creating Swapchain
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain;
    VK_CHECK(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));
    return swapchain;
}

void getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain, std::vector<VkImage> &images)
{
    uint32_t numSwapchainImages = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &numSwapchainImages, nullptr));
    images.resize(numSwapchainImages);
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &numSwapchainImages, images.data()));

    std::cout << "Swapchain Image Count: " << numSwapchainImages << '\n';
}

void createSwapchainImageViews(VkDevice device, const std::vector<VkImage> &images, const VkFormat format, std::vector<VkImageView> &imageViews)
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); ++i)
    {
        imageViewCreateInfo.image = images[i];
        VK_CHECK(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]));
    }
}

/**
 * @brief Create a Command Pool object
 * 
 * @param device 
 * @param graphicsQueueFamilyIndex 
 * @return VkCommandPool 
 * 
 * CommandPools should be created without RESET_COMMAND_BUFFER_BIT and be reset manually via vkResetCommandPool
 *  for performance. See (https://arm-software.github.io/vulkan_best_practice_for_mobile_developers/samples/performance/command_buffer_usage/command_buffer_usage_tutorial.html)
 */
VkCommandPool createCommandPool(VkDevice device, uint32_t graphicsQueueFamilyIndex)
{
    const VkCommandPoolCreateInfo commandPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0x0,
        .queueFamilyIndex = graphicsQueueFamilyIndex};

    VkCommandPool commandPool;
    VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));
    return commandPool;
}

VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool pool)
{
    const VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer));
    return commandBuffer;
}

VkFence createFence(VkDevice device, bool signaled = false)
{
    const VkFenceCreateInfo fenceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = (signaled) ? VK_FENCE_CREATE_SIGNALED_BIT : 0u,
    };

    VkFence fence;
    VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));
    return fence;
}

VkSemaphore createSemaphore(VkDevice device)
{
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkSemaphore semaphore;
    VK_CHECK(vkCreateSemaphore(device, &createInfo, nullptr, &semaphore));
    return semaphore;
}






// GLTF -> Pipeline Hash

class VkFrame
{
private:
    void resetCommandPools();
    void transitionAttachmentsStartOfFrame();
    void transitionAttachmentsEndOfFrame();

    enum
    {
        ATTACHMENT_COLOR = 0
    };

    enum
    {
        COMMMAND_BUFFER_RENDER = 0,
    };

public:
    VkFrame();
    // ~VkFrame();

    void init();
    void cleanup();

    void setColorAttachment(VkImage image);
    VkCommandBuffer render();

    std::array<VkCommandPool, 1> m_VkCommandPools;
    std::array<VkCommandBuffer, 1> m_VkCommandBuffers;

    std::array<VkImage, 1> m_VkImageAttachments;

    VkSemaphore m_VkAcquireCompleteSemaphore;
    VkSemaphore m_VkRenderCompleteSemaphore;
    VkFence m_VkCommandBufferIsExecutableFence;
};

struct VkResources
{
    VkInstance m_VkInstance;
    VkSurfaceKHR m_VkSurface;
    VkPhysicalDevice m_VkPhysicalDevice;
    VkDevice m_VkDevice;

    VkSwapchainKHR m_VkSwapchain;
    VkFormat m_VkSwapchainImageFormat;
    VkExtent2D m_VkSwapchainExtent;
    std::vector<VkImage> m_VkSwapchainImages;
    std::vector<VkImageView> m_VkSwapchainImageViews;
    uint32_t m_uSwapchainImageIdx;

    VkBuffer m_VkBuffers;
    VkDeviceMemory m_VkBuferDeviceMemorys;

    uint32_t m_uGraphicsQueueFamilyIndex;
    VkQueue m_VkGraphicsQueue;

    VkPipelineLayout m_VkPipelineLayout;
    VkPipeline m_VkPipeline;

    std::array<VkFrame, 2> m_Frames;
    uint32_t m_uFrameIdx;
} gVkResources;

VkFrame::VkFrame()
{
    for (uint32_t i = 0; i < m_VkCommandPools.size(); ++i)
    {
        m_VkCommandPools[i] = VK_NULL_HANDLE;
        m_VkCommandBuffers[i] = VK_NULL_HANDLE;
    }

    m_VkAcquireCompleteSemaphore = VK_NULL_HANDLE;
    m_VkRenderCompleteSemaphore = VK_NULL_HANDLE;
    m_VkCommandBufferIsExecutableFence = VK_NULL_HANDLE;
}

void VkFrame::init()
{
    for (uint32_t i = 0; i < m_VkCommandPools.size(); ++i)
    {
        m_VkCommandPools[i] = createCommandPool(gVkResources.m_VkDevice, gVkResources.m_uGraphicsQueueFamilyIndex);
        m_VkCommandBuffers[i] = createCommandBuffer(gVkResources.m_VkDevice, m_VkCommandPools[i]);
    }

    m_VkAcquireCompleteSemaphore = createSemaphore(gVkResources.m_VkDevice);
    m_VkRenderCompleteSemaphore = createSemaphore(gVkResources.m_VkDevice);
    m_VkCommandBufferIsExecutableFence = createFence(gVkResources.m_VkDevice, true);
}

void VkFrame::cleanup()
{
    for (VkCommandPool commandPool : m_VkCommandPools)
        vkDestroyCommandPool(gVkResources.m_VkDevice, commandPool, nullptr);

    vkDestroySemaphore(gVkResources.m_VkDevice, m_VkAcquireCompleteSemaphore, nullptr);
    vkDestroySemaphore(gVkResources.m_VkDevice, m_VkRenderCompleteSemaphore, nullptr);
    vkDestroyFence(gVkResources.m_VkDevice, m_VkCommandBufferIsExecutableFence, nullptr);
}

VkCommandBuffer VkFrame::render()
{
    resetCommandPools();

    static const VkCommandBufferBeginInfo commandBufferBeginInfo { 
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    VK_CHECK(vkBeginCommandBuffer(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], &commandBufferBeginInfo));

    transitionAttachmentsStartOfFrame();

    static VkClearValue clearColor {
        .color = { 0.14f, 0.68f, 0.23f, 1.0f }
    };

    const VkRenderingAttachmentInfo colorAttachmentInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = gVkResources.m_VkSwapchainImageViews[gVkResources.m_uSwapchainImageIdx],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clearColor
    };

    const VkRect2D renderArea{
        .offset = {0u, 0u},
        .extent = gVkResources.m_VkSwapchainExtent};

    const VkRenderingInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0x0,
        .renderArea = renderArea,
        .layerCount = 1u,
        .viewMask = 0,
        .colorAttachmentCount = 1u,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
    };

    vkCmdBeginRendering(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], &renderingInfo);

    vkCmdEndRendering(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER]);

    transitionAttachmentsEndOfFrame();

    VK_CHECK(vkEndCommandBuffer(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER]));

    return m_VkCommandBuffers[COMMMAND_BUFFER_RENDER];
}

void VkFrame::setColorAttachment(VkImage image)
{
    m_VkImageAttachments[ATTACHMENT_COLOR] = image;
}

void VkFrame::resetCommandPools()
{
    for (VkCommandPool commandPool : m_VkCommandPools)
        vkResetCommandPool(gVkResources.m_VkDevice, commandPool, 0x0);
}

void VkFrame::transitionAttachmentsStartOfFrame()
{
    static VkImageMemoryBarrier2 imageMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .srcAccessMask = VK_ACCESS_2_NONE,
        .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = gVkResources.m_uGraphicsQueueFamilyIndex,
        .dstQueueFamilyIndex = gVkResources.m_uGraphicsQueueFamilyIndex,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0u,
            .levelCount = 1u,
            .baseArrayLayer = 0u,
            .layerCount = 1u}};

    static VkDependencyInfo dependencyInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext = nullptr,
        .dependencyFlags = 0x0,
        .memoryBarrierCount = 0u,
        .pMemoryBarriers = nullptr,
        .bufferMemoryBarrierCount = 0u,
        .pBufferMemoryBarriers = nullptr,
        .imageMemoryBarrierCount = 1u};

    imageMemoryBarrier.image = m_VkImageAttachments[ATTACHMENT_COLOR];
    dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

    vkCmdPipelineBarrier2(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], &dependencyInfo);
}

void VkFrame::transitionAttachmentsEndOfFrame()
{   
    static VkImageMemoryBarrier2 imageMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
        .dstAccessMask = VK_ACCESS_2_NONE,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = gVkResources.m_uGraphicsQueueFamilyIndex,
        .dstQueueFamilyIndex = gVkResources.m_uGraphicsQueueFamilyIndex,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0u,
            .levelCount = 1u,
            .baseArrayLayer = 0u,
            .layerCount = 1u}};

    static VkDependencyInfo dependencyInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext = nullptr,
        .dependencyFlags = 0x0,
        .memoryBarrierCount = 0u,
        .pMemoryBarriers = nullptr,
        .bufferMemoryBarrierCount = 0u,
        .pBufferMemoryBarriers = nullptr,
        .imageMemoryBarrierCount = 1u};

    imageMemoryBarrier.image = m_VkImageAttachments[ATTACHMENT_COLOR];
    dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

    vkCmdPipelineBarrier2(m_VkCommandBuffers[COMMMAND_BUFFER_RENDER], &dependencyInfo);
}

struct AppResources
{
    GLFWwindow *m_Window;
    uint32_t m_uWindowWidth;
    uint32_t m_uWindowHeight;

} gAppResources;

VkShaderModule createShaderModule(VkDevice device, const char* filename)
{
    FILE* f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Failed to open file %s!\n", filename);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    const size_t nbytes_file_size = (size_t)ftell(f);
    rewind(f);

    uint32_t* buffer = (uint32_t*)malloc(nbytes_file_size);
    fread(buffer, nbytes_file_size, 1, f);
    fclose(f);

    const VkShaderModuleCreateInfo ci_shader_module {
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

VkPipelineLayout createPipelineLayout(VkDevice device)
{
    const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {
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

VkPipeline createGraphicsPipeline(VkDevice device, VkExtent2D swapchainExent, VkPipelineLayout pipelineLayout)
{
    const std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfo {{
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = createShaderModule(device, "../shaders/default-vert.spv"),
            .pName = "main",
            .pSpecializationInfo = nullptr
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = createShaderModule(device, "../shaders/default-frag.spv"),
            .pName = "main",
            .pSpecializationInfo = nullptr
        }
    }};

    const std::array<VkVertexInputBindingDescription, 1> vertexInputBindingDescription {{
        {
            .binding = 0,
            .stride = sizeof(float) * 3,
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    }};

    const std::array<VkVertexInputAttributeDescription, 1> vertexInputAttributeDescription {{
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = 0
        },
    }};

    const VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = vertexInputBindingDescription.size(),
        .pVertexBindingDescriptions = vertexInputBindingDescription.data(),
        .vertexAttributeDescriptionCount = vertexInputAttributeDescription.size(),
        .pVertexAttributeDescriptions = vertexInputAttributeDescription.data()
    };

    const VkViewport viewport {
        .x = 0,
        .y = 0,
        .width = static_cast<float>(swapchainExent.width),
        .height = static_cast<float>(swapchainExent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    const VkRect2D scissorRect {
        .offset = {.x = 0, .y = 0},
        .extent = swapchainExent
    };

    const VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissorRect,
    };

    const VkPipelineColorBlendAttachmentState pipelineColorBlendState {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
    
    const VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &pipelineColorBlendState,
        .blendConstants = { 0, 0, 0, 0 }
    };

    const VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
    };
    
    const VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo {
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

    const VkPipelineMultisampleStateCreateInfo pipelineMulisampleStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    const VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &gVkResources.m_VkSwapchainImageFormat
    };

    const VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo {
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


struct InitParams
{
    GLFWwindow *m_Window;
    uint32_t m_uWindowWidth;
    uint32_t m_uWindowHeight;

    std::vector<const char *> m_vInstanceExtensions;
    std::vector<const char *> m_vInstanceLayers;
    std::vector<const char *> m_vDeviceExtensions;

    uint32_t m_uRequestedSwapchainImageCount;
    VkPresentModeKHR m_VkPresentMode;
};

struct CullEntity {
    uint16_t m_uHandle;
    float m_vPos[3];
};

std::vector<CullEntity> gCullList;

struct ModelPrototype {
    VkBuffer m_VkVertexBuffer;    
    VkDeviceMemory m_VkBufferDeviceMemory;

    std::vector<VkImage> m_VkImages;
    std::vector<VkImageView> m_VkImageViews;
    std::vector<VkDeviceMemory> m_VkImageDeviceMemory;
};

std::unordered_map<std::string, ModelPrototype> gNameToPrototypeMap;

struct Model {
    uint16_t handle;
    ModelPrototype& prototype;
};

void sceneInit()
{
    gVkResources.m_VkPipelineLayout = createPipelineLayout(gVkResources.m_VkDevice);
    gVkResources.m_VkPipeline = createGraphicsPipeline(gVkResources.m_VkDevice, gVkResources.m_VkSwapchainExtent, gVkResources.m_VkPipelineLayout);

    // setup buffers and objects
    // loadModel("../models/cube.gltf");
}

InitParams appInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    gAppResources.m_uWindowWidth = 300u;
    gAppResources.m_uWindowHeight = 300u;
    gAppResources.m_Window = glfwCreateWindow(static_cast<int>(gAppResources.m_uWindowWidth), static_cast<int>(gAppResources.m_uWindowHeight), "App", nullptr, nullptr);
    assert(gAppResources.m_Window != nullptr);

    uint32_t numInstanceExtensions = 0;
    const char **instanceExtensions = glfwGetRequiredInstanceExtensions(&numInstanceExtensions);

    std::array<const char *, 1> instanceLayers{"VK_LAYER_KHRONOS_validation"};
    std::array<const char *, 2> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME};

    InitParams params{};

    params.m_uWindowWidth = 300u;
    params.m_uWindowHeight = 300u;
    params.m_Window = gAppResources.m_Window;

    params.m_vInstanceExtensions.insert(params.m_vInstanceExtensions.end(), &instanceExtensions[0], &instanceExtensions[numInstanceExtensions]);
    params.m_vInstanceLayers.insert(params.m_vInstanceLayers.end(), instanceLayers.begin(), instanceLayers.end());
    params.m_vDeviceExtensions.insert(params.m_vDeviceExtensions.end(), deviceExtensions.begin(), deviceExtensions.end());

    params.m_uRequestedSwapchainImageCount = 2u;
    params.m_VkPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    return params;
}

void vkInit(const InitParams &initParams)
{
    gVkResources.m_VkSwapchainExtent = { initParams.m_uWindowWidth, initParams.m_uWindowHeight };

    gVkResources.m_VkInstance = createInstance(initParams.m_vInstanceExtensions, initParams.m_vInstanceLayers);
    gVkResources.m_VkSurface = createSurface(gVkResources.m_VkInstance, initParams.m_Window);
    gVkResources.m_VkPhysicalDevice = selectPhysicalDevice(gVkResources.m_VkInstance);
    gVkResources.m_uGraphicsQueueFamilyIndex = selectGraphicsQueueFamilyIndex(gVkResources.m_VkPhysicalDevice, gVkResources.m_VkSurface);
    gVkResources.m_VkDevice = createDevice(gVkResources.m_VkPhysicalDevice, gVkResources.m_uGraphicsQueueFamilyIndex, initParams.m_vDeviceExtensions);
    gVkResources.m_VkGraphicsQueue = createGraphicsQueue(gVkResources.m_VkDevice, gVkResources.m_uGraphicsQueueFamilyIndex);
    gVkResources.m_VkSwapchain = createSwapchain(gVkResources.m_VkPhysicalDevice, gVkResources.m_VkSurface, gVkResources.m_VkDevice, initParams.m_uRequestedSwapchainImageCount, gVkResources.m_VkSwapchainImageFormat, gVkResources.m_VkSwapchainExtent, initParams.m_VkPresentMode);
    getSwapchainImages(gVkResources.m_VkDevice, gVkResources.m_VkSwapchain, gVkResources.m_VkSwapchainImages);
    createSwapchainImageViews(gVkResources.m_VkDevice, gVkResources.m_VkSwapchainImages, gVkResources.m_VkSwapchainImageFormat, gVkResources.m_VkSwapchainImageViews);

    for (VkFrame &frame : gVkResources.m_Frames)
        frame.init();
}

void run()
{
    while (!glfwWindowShouldClose(gAppResources.m_Window))
    {
        glfwPollEvents();

        VkFrame &frame = gVkResources.m_Frames[gVkResources.m_uFrameIdx];

        VK_CHECK(vkAcquireNextImageKHR(gVkResources.m_VkDevice, gVkResources.m_VkSwapchain, UINT64_MAX, frame.m_VkAcquireCompleteSemaphore, VK_NULL_HANDLE, &gVkResources.m_uSwapchainImageIdx));

        VK_CHECK(vkWaitForFences(gVkResources.m_VkDevice, 1, &frame.m_VkCommandBufferIsExecutableFence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(gVkResources.m_VkDevice, 1, &frame.m_VkCommandBufferIsExecutableFence));

        frame.setColorAttachment(gVkResources.m_VkSwapchainImages[gVkResources.m_uSwapchainImageIdx]);

        VkCommandBuffer commandBuffer = frame.render();

        const VkSemaphoreSubmitInfo acquireCompleteSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = frame.m_VkAcquireCompleteSemaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, // defines second sync scope - "block on color_attachment_output" until acquire semaphore is signaled
        };

        const VkSemaphoreSubmitInfo renderCompleteSemaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = frame.m_VkRenderCompleteSemaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, // defines first sync scope - "signal render complete when color_attachment_output completes"
        };

        const VkCommandBufferSubmitInfo commandBufferSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = commandBuffer,
        };

        const VkSubmitInfo2 renderSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = 1u,
            .pWaitSemaphoreInfos = &acquireCompleteSemaphoreSubmitInfo,
            .commandBufferInfoCount = 1u,
            .pCommandBufferInfos = &commandBufferSubmitInfo,
            .signalSemaphoreInfoCount = 1u,
            .pSignalSemaphoreInfos = &renderCompleteSemaphoreSubmitInfo,
        };

        VK_CHECK(vkQueueSubmit2(gVkResources.m_VkGraphicsQueue, 1u, &renderSubmitInfo, frame.m_VkCommandBufferIsExecutableFence));

        const VkPresentInfoKHR presentInfoKHR{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1u,
            .pWaitSemaphores = &frame.m_VkRenderCompleteSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &gVkResources.m_VkSwapchain,
            .pImageIndices = &gVkResources.m_uSwapchainImageIdx,
        };
        VK_CHECK(vkQueuePresentKHR(gVkResources.m_VkGraphicsQueue, &presentInfoKHR));

        gVkResources.m_uFrameIdx = (gVkResources.m_uFrameIdx + 1) % gVkResources.m_Frames.size();
    }

    VK_CHECK(vkDeviceWaitIdle(gVkResources.m_VkDevice));
}

void cleanup()
{
    vkDestroyPipelineLayout(gVkResources.m_VkDevice, gVkResources.m_VkPipelineLayout, nullptr);
    vkDestroyPipeline(gVkResources.m_VkDevice, gVkResources.m_VkPipeline, nullptr);

    for (VkFrame& frame : gVkResources.m_Frames)
        frame.cleanup();

    for (VkImageView imageView : gVkResources.m_VkSwapchainImageViews)
        vkDestroyImageView(gVkResources.m_VkDevice, imageView, nullptr);

    vkDestroySwapchainKHR(gVkResources.m_VkDevice, gVkResources.m_VkSwapchain, nullptr);

    vkDestroyDevice(gVkResources.m_VkDevice, nullptr);
    vkDestroySurfaceKHR(gVkResources.m_VkInstance, gVkResources.m_VkSurface, nullptr);
    vkDestroyInstance(gVkResources.m_VkInstance, nullptr);

    glfwDestroyWindow(gAppResources.m_Window);
    glfwTerminate();
}

int main()
{
    InitParams initParams = appInit();

    // No State Changes After Here!! Below is all vulkan stuff

    vkInit(initParams);

    sceneInit();

    run();

    cleanup();

    return 0;
}