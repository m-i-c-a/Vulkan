#include <iostream>
#include <assert.h>
#include <vector>
#include <array>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#define VK_CHECK( result ) do { \
   assert(result == VK_SUCCESS); \
} while ( 0 ) \

VkInstance createInstance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = nullptr;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceCreateInfo {};
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

VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow* window) {
    VkSurfaceKHR surface;
    VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
    return surface;
}

VkPhysicalDevice selectPhysicalDevice(VkInstance instance) {
    uint32_t numPhysicalDevices = 0;
    vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr);
    VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[numPhysicalDevices];
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
    delete [] physicalDevices;
    return physicalDevice;
}

uint32_t selectGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    uint32_t numQueueFamilyProperties = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilyProperties, nullptr);
    VkQueueFamilyProperties* queueFamilyProperties = new VkQueueFamilyProperties[numQueueFamilyProperties];
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
            presentQueueFamilyIndex  = i;
            break;
        }
    }

    delete [] queueFamilyProperties;

    assert(graphicsQueueFamilyIndex < UINT32_MAX && "no supported graphics queue family index");
    assert(presentQueueFamilyIndex  < UINT32_MAX && "no supported present queue family index");
    assert(graphicsQueueFamilyIndex == presentQueueFamilyIndex && "queue families (graphics/present) do not match");

    return graphicsQueueFamilyIndex;
}

VkDevice createDevice(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamilyIndex, const std::vector<const char*>& deviceExtensions) {

    // uint32_t propertyCount = 0u;
    // VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &propertyCount, nullptr));
    // std::vector<VkExtensionProperties> properties(propertyCount);
    // VK_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &propertyCount, properties.data()));

    const float queuePriority = 1.0f;

    const VkDeviceQueueCreateInfo queueCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = graphicsQueueFamilyIndex,
        .queueCount = 1u,
        .pQueuePriorities = &queuePriority
    };

    const VkDeviceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .queueCreateInfoCount = 1u,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledLayerCount = 0u,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = nullptr
    };

    VkDevice device;
    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));
    return device;
}

VkQueue createGraphicsQueue(VkDevice device, uint32_t graphicsQueueFamilyIndex) {
   VkQueue graphicsQueue;
   vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
   return graphicsQueue;
}

VkSwapchainKHR createSwapchain(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice device, uint32_t imageCount, VkFormat& format, VkExtent2D&& extent, VkPresentModeKHR presentMode) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));
    VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
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
        VkSurfaceFormatKHR* supportedSurfaceFormats = new VkSurfaceFormatKHR[numSupportedSurfaceFormats];
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

        delete [] supportedSurfaceFormats;
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
        VkPresentModeKHR* supportedPresentModes = new VkPresentModeKHR[numSupportedPresentModes];
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

        delete [] supportedPresentModes;
    }

    //** Creating Swapchain
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain;
    VK_CHECK(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));
    return swapchain;
}

void getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain, std::vector<VkImage>& images) {
    uint32_t numSwapchainImages = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &numSwapchainImages, nullptr));
    images.resize(numSwapchainImages);
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &numSwapchainImages, images.data()));

    std::cout << "Swapchain Image Count: " << numSwapchainImages << '\n';
}

void createSwapchainImageViews(VkDevice device, const std::vector<VkImage>& images, const VkFormat format, std::vector<VkImageView>& imageViews) {
    VkImageViewCreateInfo imageViewCreateInfo {};
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

VkRenderPass createRenderPass(VkDevice device, VkFormat swapchainImageFormat) {
    VkAttachmentDescription attachments[1] {};
    VkAttachmentReference attachmentReferences[1] {};

    // Back Buffer (Swapchain Image)
    attachments[0].format = swapchainImageFormat;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachmentReferences[0].attachment = 0;
    attachmentReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = attachmentReferences;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = nullptr; 
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

	VkSubpassDependency dependencies[2] {};

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;                             // Producer of the dependency
    dependencies[0].dstSubpass = 0;                                               // Consumer is our single subpass that will wait for the execution dependency
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Match our pWaitDstStageMask when we vkQueueSubmit
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // is a loadOp stage for color attachments
    dependencies[0].srcAccessMask = 0;                                            // semaphore wait already does memory dependency for us
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;         // is a loadOp CLEAR access mask for color attachments
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;                                               // Producer of the dependency is our single subpass
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;                             // Consumer are all commands outside of the renderpass
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // is a storeOp stage for color attachments
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;          // Do not block any subsequent work
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;         // is a storeOp `STORE` access mask for color attachments
    dependencies[1].dstAccessMask = 0;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    
    VkRenderPassCreateInfo renderPassCreateInfo {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.flags = 0x0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 2;
    renderPassCreateInfo.pDependencies = dependencies;

    VkRenderPass renderPass;
    VK_CHECK(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass));
    return renderPass;
}

std::vector<VkFramebuffer> createFramebuffers(VkDevice device, VkRenderPass renderPass, VkExtent2D swapchainExtent, std::vector<VkImageView>& swapchainImageViews)
{
    VkFramebufferCreateInfo framebufferCreateInfo {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.width = swapchainExtent.width;
    framebufferCreateInfo.height = swapchainExtent.height;
    framebufferCreateInfo.layers = 1;

    std::vector<VkFramebuffer> framebuffers {swapchainImageViews.size() };
    for (size_t i = 0; i < swapchainImageViews.size(); ++i)
    {
        VkImageView attachments[1] = {
            swapchainImageViews[i]
        };

        framebufferCreateInfo.pAttachments = attachments;

        VK_CHECK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]));
    }

    return framebuffers;
}



// VkDevice createDevice() {

// }



// GLTF -> Pipeline Hash

struct VkResources {
    VkInstance m_VkInstance;
    VkSurfaceKHR m_VkSurface;
    VkPhysicalDevice m_VkPhysicalDevice;
    VkDevice m_VkDevice;

    VkSwapchainKHR m_VkSwapchain;
    VkFormat m_VkSwapchainImageFormat;
    VkExtent2D m_VkSwapchainExtent;
    std::vector<VkImage> m_VkSwapchainImages;
    std::vector<VkImageView> m_VkSwapchainImageViews;

    VkBuffer m_VkBuffers;
    VkDeviceMemory m_VkBuferDeviceMemorys;

    VkSemaphore m_VkSemaphores;
    VkFence m_VkFences;

    uint32_t m_uGraphicsQueueFamilyIndex;
    VkQueue m_VkGraphicsQueue;
} gVkResources;

struct AppResources {
    GLFWwindow* m_Window;
    uint32_t m_uWindowWidth;
    uint32_t m_uWindowHeight;

} gAppResources;

struct InitParams {
    GLFWwindow* m_Window;
    uint32_t m_uWindowWidth;
    uint32_t m_uWindowHeight;

    std::vector<const char*> m_vInstanceExtensions;
    std::vector<const char*> m_vInstanceLayers;
    std::vector<const char*> m_vDeviceExtensions;

    uint32_t m_uRequestedSwapchainImageCount;
    VkPresentModeKHR m_VkPresentMode;
};



void sceneInit() {
    // setup buffers and objects

}







InitParams appInit() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    gAppResources.m_uWindowWidth = 300u;
    gAppResources.m_uWindowHeight = 300u;
    gAppResources.m_Window = glfwCreateWindow(static_cast<int>(gAppResources.m_uWindowWidth), static_cast<int>(gAppResources.m_uWindowHeight), "App", nullptr, nullptr);
    assert(gAppResources.m_Window != nullptr);

    uint32_t numInstanceExtensions = 0;
    const char** instanceExtensions = glfwGetRequiredInstanceExtensions(&numInstanceExtensions);

    std::array<const char*, 1> instanceLayers { "VK_LAYER_KHRONOS_validation" };
    std::array<const char*, 1> deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    InitParams params {};

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

void vkInit(const InitParams& initParams) {

    gVkResources.m_VkInstance = createInstance(initParams.m_vInstanceExtensions, initParams.m_vInstanceLayers);
    gVkResources.m_VkSurface = createSurface(gVkResources.m_VkInstance, initParams.m_Window);
    gVkResources.m_VkPhysicalDevice = selectPhysicalDevice(gVkResources.m_VkInstance);
    gVkResources.m_uGraphicsQueueFamilyIndex = selectGraphicsQueueFamilyIndex(gVkResources.m_VkPhysicalDevice, gVkResources.m_VkSurface);
    gVkResources.m_VkDevice = createDevice(gVkResources.m_VkPhysicalDevice, gVkResources.m_uGraphicsQueueFamilyIndex, initParams.m_vDeviceExtensions);
    gVkResources.m_VkGraphicsQueue = createGraphicsQueue(gVkResources.m_VkDevice, gVkResources.m_uGraphicsQueueFamilyIndex);
    gVkResources.m_VkSwapchain = createSwapchain(gVkResources.m_VkPhysicalDevice, gVkResources.m_VkSurface, gVkResources.m_VkDevice, initParams.m_uRequestedSwapchainImageCount, gVkResources.m_VkSwapchainImageFormat, { initParams.m_uWindowWidth, initParams.m_uWindowHeight }, initParams.m_VkPresentMode);
    getSwapchainImages(gVkResources.m_VkDevice, gVkResources.m_VkSwapchain, gVkResources.m_VkSwapchainImages);
    createSwapchainImageViews(gVkResources.m_VkDevice, gVkResources.m_VkSwapchainImages, gVkResources.m_VkSwapchainImageFormat, gVkResources.m_VkSwapchainImageViews);

    sceneInit();
}

void run() {

}

void cleanup() {

}


int main() {

    InitParams initParams = appInit();


    // No State Changes After Here!! Below is all vulkan stuff

    vkInit(initParams);

    // run();

    // cleanup();

    return 0;
}