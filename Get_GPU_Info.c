//
// Created by sovie on 2026/10/12.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"
int Get_GPU_Info(VkInstance instance,struct GPU_Physical_Device *physical_device) {

    // 2. 获取 GPU 数量
    uint32_t gpuCount = 0;

    VkResult result = vkEnumeratePhysicalDevices(                    //vkEnumeratePhysicalDevices：输出设备上GPU的数量
       instance, &gpuCount, NULL                            //GPU_Info：我们之前创建的 Vulkan 实例句柄, &gpuCount：输出参数，用于接收 GPU 数量, NULL：不接收 GPU 列表，只查询数量
    );

    if (result != VK_SUCCESS ) {
        printf("Failed to enumerate GPUs!Error: %d\n",result);

        return 1;
    }
    if ( gpuCount == 0) {
        printf("No Vulkan GPU found!Error: %d\n",result);
        return 1;
    }

    printf("Found %u GPU(s):\n", gpuCount);

    // 3. 分配 GPU 句柄数组 VkPhysicalDevice:数组类型 gpus:数组名称 [gpuCount]:数组大小
    VkPhysicalDevice *gpus =malloc(gpuCount*sizeof(VkPhysicalDevice));
    if (gpus == NULL) {
        printf("Failed to allocate GPU memory!\n");
        return 1;
    }
    // 4. 获取 GPU 列表
    result = vkEnumeratePhysicalDevices(
        instance, &gpuCount, gpus
    );

    if (result != VK_SUCCESS) {
        printf("Failed to enumerate GPUs!Error: %d\n",result);
        return 1;
    }

    physical_device->gpus =gpus;
    physical_device->gpuCount = gpuCount;

    // 5. 打印 GPU 名称
    for (uint32_t i = 0; i < gpuCount; i++) {
        VkPhysicalDeviceProperties properties; //VkPhysicalDeviceProperties： 又一个结构体，大概是gpu信息什么的
        VkPhysicalDeviceMemoryProperties memoryProperties;//更多结构体，这个是存显存大小的

        vkGetPhysicalDeviceProperties(         //把咱们刚才数组里的第i个gpu的信息扔到结构体里
            gpus[i], &properties
        );

        vkGetPhysicalDeviceMemoryProperties(
        gpus[i], &memoryProperties
        );

        printf("GPU %u: %s\n",
               i, properties.deviceName);
        for (uint32_t j = 0; j < memoryProperties.memoryHeapCount; j++) {
            printf("\tHeap No.%u Size:%.2fGiB\t%s\n" ,j,(double)memoryProperties.memoryHeaps[j].size/(1024.0*1024.0*1024.0),
            (memoryProperties.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            ? "[DEVICE_LOCAL]"
            : "[NON_DEVICE_LOCAL]"
            );
        }
    }

    return 0;
}

int Get_GPU_Surface_Info(struct GPU_Physical_Device *physical_device,struct Logical_Device *logical_device,struct Window_Info *window_info,struct Capabilities *capabilities_output) {
    if (physical_device == NULL ||
    physical_device->selectedGPU == VK_NULL_HANDLE ||
    window_info->surface == VK_NULL_HANDLE) {
        return 1;
    }

        //检查是否支持显示图像
    VkBool32 presentSupported = VK_FALSE;

    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
        physical_device->selectedGPU,
        logical_device->graphicsFamilyIndex,
        window_info->surface,
        &presentSupported
    );
    if (result != VK_SUCCESS) {
        printf("Failed to check Present support: %d\n", result);
        return 1;
    }

    if (presentSupported == VK_FALSE) {
        printf("Selected Queue Family does not support Present!\n");
        return 1;
    }
    printf("Selected Queue Family supports Present!\n");
    //检查GPU 和窗口支持哪些显示参数
    VkSurfaceCapabilitiesKHR capabilities;//Surface 能力信息结构体

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device->selectedGPU,//要查询的物理设备
        window_info->surface,//要查询的页面
        &capabilities//输出
    );

    if (result != VK_SUCCESS) {
        printf("Failed to get Surface Capabilities: %d\n", result);
        return 1;
    }

    printf("Min Image Count: %u\n", capabilities.minImageCount);
    printf("Max Image Count: %u\n", capabilities.maxImageCount);
    printf("Current Extent: %u x %u\n",
           capabilities.currentExtent.width,
           capabilities.currentExtent.height);
    //查询 GPU 支持哪些 Surface 图像格式及色彩空间
    uint32_t formatCount = 0;

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device->selectedGPU,
        window_info->surface,
        &formatCount,
        NULL
    );

    if (result != VK_SUCCESS) {
        printf("Failed to query Surface Formats: %d\n", result);
        return 1;
    }

    printf("Supported Surface Format Count: %u\n", formatCount);

    if (formatCount == 0) {
        printf("No Surface Formats available!\n");
        return 1;
    }

    VkSurfaceFormatKHR surfaceFormats[formatCount];
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(
       physical_device->selectedGPU,
       window_info->surface,
       &formatCount,
       surfaceFormats
   );
    if (result != VK_SUCCESS) {
        printf("Failed to get Surface Formats: %d\n", result);
        return 1;
    }
    for (uint32_t i = 0; i < formatCount; i++) {
        printf(
            "Surface Format %u: format=%d, colorSpace=%d\n",
            i,
            (int)surfaceFormats[i].format,
            (int)surfaceFormats[i].colorSpace
        );
    }
    //选择合适格式
    VkSurfaceFormatKHR selectedFormat = surfaceFormats[0];

    for (uint32_t i = 0; i < formatCount; i++) {
        if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {

            selectedFormat = surfaceFormats[i];
            break;
            }
    }

    printf("Selected Surface Format: %d\n", selectedFormat.format);
    //查询GPU 和窗口系统支持哪些 Present Mode
    uint32_t presentModeCount = 0;

    result = vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device->selectedGPU,
        window_info->surface,
        &presentModeCount,
        NULL
    );

    if (result != VK_SUCCESS) {
        printf("Failed to query Present Modes: %d\n", result);
        return 1;
    }

    printf("Supported Present Mode Count: %u\n", presentModeCount);
    if (presentModeCount == 0) {
        printf("No Present Modes available!\n");
        return 1;
    }

    VkPresentModeKHR presentModes[presentModeCount];

    result = vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device->selectedGPU,
        window_info->surface,
        &presentModeCount,
        presentModes
    );

    if (result != VK_SUCCESS) {
        printf("Failed to get Present Modes: %d\n", result);
        return 1;
    }

    for (uint32_t i = 0; i < presentModeCount; i++) {
        printf("Present Mode %u: %d\n", i, (int)presentModes[i]);
    }
    printf("VK_PRESENT_MODE_IMMEDIATE_KHR	0	\nVK_PRESENT_MODE_MAILBOX_KHR	1	\nVK_PRESENT_MODE_FIFO_KHR	2	\nVK_PRESENT_MODE_FIFO_RELAXED_KHR	3	\n");
    VkPresentModeKHR selectedPresentMode =VK_PRESENT_MODE_FIFO_KHR;
    capabilities_output->selectedPresentMode = selectedPresentMode;
    capabilities_output->selectedSurfaceFormat = selectedFormat;
    capabilities_output->surfaceCapabilities = capabilities;
    return 0;
}
