//
// Created by sovie on 2026/10/12.
//
#include<stdio.h>
#include<stdlib.h>
#include"Instance.h"
#include "window.h"
int Create_Instance(struct Instance* Info) {
    if (Info == NULL) {
    return -1;
    }
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "My Vulkan Renderer",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "My Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3
    };
    // 1. 获取 GLFW 所需的 Vulkan 扩展数量
    uint32_t extensionCount = 0;

    // 2. 获取扩展名称数组
    const char **extensions =
        glfwGetRequiredInstanceExtensions(&extensionCount);

    if (extensions == NULL || extensionCount == 0) {
        printf("Failed to get GLFW Vulkan extensions!\n");
        return 1;
    }

    VkInstance instance;
    VkInstanceCreateInfo createInfo = {                     //VkInstanceCreateInfo:结构体类型 createInfo:名字叫createInfo
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,    //sType：成员，声明当前结构体的类型
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = extensionCount,
        .ppEnabledExtensionNames = extensions
    };

    VkResult result = vkCreateInstance(                     //vkCreateInstance函数,创建一个实例并写到刚才创建的VkInstance里
        &createInfo, NULL, &instance                        //&createInfo：输入参数，传入创建实例所需的配置结构体地址。NULL：输入参数，表示使用默认内存分配器，
    );                                                      //不提供自定义分配回调。&GPU_Info：输出参数，Vulkan 会在创建成功后向这里写入实例句柄。

    if (result != VK_SUCCESS) {
        printf("Failed to create Vulkan Instance!Error: %d\n", result);
        return 1;
    }
    Info->instance = instance;
    printf("Successfully created Vulkan Instance!\n");
    return 0;
}
void Destroy_Instance(struct Instance *Info) {
    if (Info == NULL) {
        return;
    }

    if (Info->vertexBuffer.vertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(Info->logicalDevice.device, Info->vertexBuffer.vertexBuffer, NULL);
        Info->vertexBuffer.vertexBuffer = VK_NULL_HANDLE;
    }

    if (Info->vertexBuffer.vertexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(
            Info->logicalDevice.device,
            Info->vertexBuffer.vertexBufferMemory,
            NULL
        );
        Info->vertexBuffer.vertexBufferMemory = VK_NULL_HANDLE;
    }

    if (Info->sync.renderFinishedSemaphores != NULL) {
        for (uint32_t i = 0; i < Info->swapchain.swapchainImageCount; i++) {
            if (Info->sync.renderFinishedSemaphores[i] != VK_NULL_HANDLE) {
                vkDestroySemaphore(
                    Info->logicalDevice.device,
                    Info->sync.renderFinishedSemaphores[i],
                    NULL
                );
            }
        }

        free(Info->sync.renderFinishedSemaphores);
        Info->sync.renderFinishedSemaphores = NULL;
    }

    if (Info->sync.inFlightFence != VK_NULL_HANDLE) {
        vkDestroyFence(
            Info->logicalDevice.device,
            Info->sync.inFlightFence,
            NULL
        );
        Info->sync.inFlightFence = VK_NULL_HANDLE;
    }

    if (Info->sync.imageAvailableSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(
            Info->logicalDevice.device,
            Info->sync.imageAvailableSemaphore,
            NULL
        );

        Info->sync.imageAvailableSemaphore = VK_NULL_HANDLE;
    }

    if (Info->pipeline.linePipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(
            Info->logicalDevice.device,
            Info->pipeline.linePipeline,
            NULL
        );

        Info->pipeline.linePipeline = VK_NULL_HANDLE;
    }

    if (Info->pipeline.graphicsPipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(
            Info->logicalDevice.device,
            Info->pipeline.graphicsPipeline,
            NULL
        );

        Info->pipeline.graphicsPipeline = VK_NULL_HANDLE;
    }
    if (Info->commandBuffer.commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(
            Info->logicalDevice.device,
            Info->commandBuffer.commandPool,
            NULL
        );
        Info->commandBuffer.commandPool = VK_NULL_HANDLE;
    }
    if (Info->pipeline.pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(
            Info->logicalDevice.device,
            Info->pipeline.pipelineLayout,
            NULL
        );

        Info->pipeline.pipelineLayout = VK_NULL_HANDLE;
    }
    if (Info->shader.vertexShader != VK_NULL_HANDLE) {
        vkDestroyShaderModule(Info->logicalDevice.device,
            Info->shader.vertexShader, NULL
            );
    }

    if (Info->shader.fragmentShader != VK_NULL_HANDLE) {
        vkDestroyShaderModule(Info->logicalDevice.device,
            Info->shader.fragmentShader, NULL);
    }

    if (Info->logicalDevice.device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(Info->logicalDevice.device);
    }
    if (Info->swapchain.swapchainImageViews != NULL) {

        for (uint32_t i = 0; i < Info->swapchain.swapchainImageCount; i++) {

            if (Info->swapchain.swapchainImageViews[i] != VK_NULL_HANDLE) {
                vkDestroyImageView(
                    Info->logicalDevice.device,
                    Info->swapchain.swapchainImageViews[i],
                    NULL
                );
            }
        }

        free(Info->swapchain.swapchainImageViews);
        Info->swapchain.swapchainImageViews = NULL;
    }
    free(Info->swapchain.swapchainImages);
    Info->swapchain.swapchainImages = NULL;
    Info->swapchain.swapchainImageCount = 0;
    if (Info->swapchain.swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(
        Info->logicalDevice.device,      // 创建 Swapchain 的 Logical Device
        Info->swapchain.swapchain,   // 要销毁的 Swapchain
        NULL              // 默认内存分配器
        );
        Info->swapchain.swapchain = VK_NULL_HANDLE;
    }
    if (Info->logicalDevice.device != VK_NULL_HANDLE) {
        vkDestroyDevice(Info->logicalDevice.device, NULL);
        Info->logicalDevice.device = VK_NULL_HANDLE;
    }
    if (Info->windowInfo.surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(
            Info->instance,
            Info->windowInfo.surface,
            NULL
        );
        Info->windowInfo.surface = VK_NULL_HANDLE;
    }
    free(Info->physicalDevice.gpus);
    Info->physicalDevice.gpus = NULL;
    Info->physicalDevice.gpuCount = 0;
    Info->physicalDevice.selectedGPU = VK_NULL_HANDLE;

    if (Info->instance != VK_NULL_HANDLE) {
        vkDestroyInstance(Info->instance, NULL);
        Info->instance = VK_NULL_HANDLE;
    }
    Destroy_Window(&Info->windowInfo);
}
