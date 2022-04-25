#include "VkStartup.hpp"
#include "Buffer.hpp"
#include "VkFrame.hpp"

namespace
{
    VkInstance createInstance(const std::vector<const char *> &extensions, const std::vector<const char *> &layers)
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = nullptr;
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

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

        constexpr VkPhysicalDeviceSynchronization2FeaturesKHR synchronization2Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
            .pNext = nullptr,
            .synchronization2 = VK_TRUE};

        const VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures{
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

}

void vulkanInit(const VulkanInitParams& initParams, VulkanResources& vkResources)
{
    vkResources.m_VkSwapchainExtent = {initParams.m_uWindowWidth, initParams.m_uWindowHeight};
    vkResources.m_VkInstance = createInstance(initParams.m_vInstanceExtensions, initParams.m_vInstanceLayers);
    vkResources.m_VkSurface = createSurface(vkResources.m_VkInstance, initParams.m_Window);
    vkResources.m_VkPhysicalDevice = selectPhysicalDevice(vkResources.m_VkInstance);
    vkResources.m_uGraphicsQueueFamilyIndex = selectGraphicsQueueFamilyIndex(vkResources.m_VkPhysicalDevice, vkResources.m_VkSurface);
    vkResources.m_VkDevice = createDevice(vkResources.m_VkPhysicalDevice, vkResources.m_uGraphicsQueueFamilyIndex, initParams.m_vDeviceExtensions);
    vkResources.m_VkGraphicsQueue = createGraphicsQueue(vkResources.m_VkDevice, vkResources.m_uGraphicsQueueFamilyIndex);
    vkResources.m_VkSwapchain = createSwapchain(vkResources.m_VkPhysicalDevice, vkResources.m_VkSurface, vkResources.m_VkDevice, initParams.m_uRequestedSwapchainImageCount, vkResources.m_VkSwapchainImageFormat, vkResources.m_VkSwapchainExtent, initParams.m_VkPresentMode);
    getSwapchainImages(vkResources.m_VkDevice, vkResources.m_VkSwapchain, vkResources.m_VkSwapchainImages);
    createSwapchainImageViews(vkResources.m_VkDevice, vkResources.m_VkSwapchainImages, vkResources.m_VkSwapchainImageFormat, vkResources.m_VkSwapchainImageViews);

    vkGetPhysicalDeviceMemoryProperties(vkResources.m_VkPhysicalDevice, &vkResources.m_VkPhysicalDeviceMemProps);

    vkResources.vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(vkResources.m_VkDevice, "vkCmdBeginRenderingKHR"));
    vkResources.vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(vkResources.m_VkDevice, "vkCmdEndRenderingKHR"));
    vkResources.vkQueueSubmit2KHR = reinterpret_cast<PFN_vkQueueSubmit2KHR>(vkGetDeviceProcAddr(vkResources.m_VkDevice, "vkQueueSubmit2KHR"));
    vkResources.vkCmdPipelineBarrier2KHR = reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(vkGetDeviceProcAddr(vkResources.m_VkDevice, "vkCmdPipelineBarrier2KHR"));

    StaticBuffer::init(&vkResources.m_VkDevice, &vkResources.m_VkPhysicalDeviceMemProps);
    StagingBuffer::init(&vkResources.m_VkDevice, vkResources.m_VkGraphicsQueue, vkResources.m_uGraphicsQueueFamilyIndex);
    VkFrame::setResources(&vkResources);
}

void vulkanDestroy(VulkanResources& vulkanResources)
{
    StagingBuffer::destroy();

    for (VkImageView imageView : vulkanResources.m_VkSwapchainImageViews)
        vkDestroyImageView(vulkanResources.m_VkDevice, imageView, nullptr);

    vkDestroySwapchainKHR(vulkanResources.m_VkDevice, vulkanResources.m_VkSwapchain, nullptr);

    vkDestroyDevice(vulkanResources.m_VkDevice, nullptr);
    vkDestroySurfaceKHR(vulkanResources.m_VkInstance, vulkanResources.m_VkSurface, nullptr);
    vkDestroyInstance(vulkanResources.m_VkInstance, nullptr);
}
