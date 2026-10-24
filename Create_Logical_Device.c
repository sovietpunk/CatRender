//
// Created by sovie on 2026/10/12.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"
int Create_Logical_Device(struct GPU_Physical_Device *physical_device,struct Logical_Device *logical_device) {
    if (physical_device == NULL ||
    physical_device->selectedGPU == VK_NULL_HANDLE||
    logical_device == NULL) {
        return -1;
    }
    VkPhysicalDeviceVulkan13Features supported13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
    };

    VkPhysicalDeviceFeatures2 supportedFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &supported13
    };

    vkGetPhysicalDeviceFeatures2(
        physical_device->selectedGPU,
        &supportedFeatures
    );

    if (supported13.dynamicRendering != VK_TRUE) {
        printf("Dynamic Rendering is not supported!\n");
        return 1;
    }
    if (supported13.synchronization2 != VK_TRUE) {
        printf("Synchronization2 is not supported!\n");
        return 1;
    }
    float priority=1.0f;
    VkPhysicalDeviceVulkan13Features features13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .dynamicRendering = VK_TRUE,
        .synchronization2 = VK_TRUE
    };
    VkDeviceQueueCreateInfo queueInfo = {//Queue 创建配置结构体
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,//结构体类型标识
        .queueFamilyIndex = logical_device->graphicsFamilyIndex,//申请的queue family
        .queueCount = 1,//申请几个queue
        .pQueuePriorities = &priority//优先级
    };
    const char *deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME//定义我们的扩展
    };
    VkDeviceCreateInfo deviceInfo = {//创建逻辑设备配置结构体
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,//结构体类型标识
        .queueCreateInfoCount = 1,//提供一个Queue 创建配置结构体
        .enabledExtensionCount = 1,//有一个扩展
        .ppEnabledExtensionNames = deviceExtensions,//我们的扩展
        .pQueueCreateInfos = &queueInfo,
        .pNext = &features13,
    };
    logical_device->device=VK_NULL_HANDLE;
    VkResult result = vkCreateDevice(
    physical_device->selectedGPU,      //物理设备
    &deviceInfo,            //配置结构体
    NULL,                   //使用默认的主机内存分配机制
    &logical_device->device          //输出
);
    if (result != VK_SUCCESS) {
        printf("Failed to create logical device! Error:%d\n",result);
        return -1;
    }
    printf("Successfully created logical device!\n");

    vkGetDeviceQueue(
    logical_device->device,                   //逻辑设备
    logical_device->graphicsFamilyIndex,      //Queue Family
    0,                             //队列编号
    &logical_device->graphicsQueue           //输出
);
    if (logical_device->graphicsQueue == VK_NULL_HANDLE) {
        printf("Failed to get graphics queue!\n");
        return -2;
    }

    printf("Successfully created graphics queue!\n");
    return 0;
}