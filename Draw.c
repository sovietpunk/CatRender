//
// Created by sovie on 2026/10/19.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"
#include "CMDs.h"
#include "Transform.h"

int Draw_Frame(struct Instance *Info,struct  scene *scene) {
    if (Info == NULL ||
        Info->logicalDevice.device == VK_NULL_HANDLE ||
        Info->swapchain.swapchain == VK_NULL_HANDLE) {
        return 1;
        }

    // 1. 等待上一帧 GPU 工作完成
    VkResult result = vkWaitForFences(
        Info->logicalDevice.device,
        1,
        &Info->sync.inFlightFence,
        VK_TRUE,
        UINT64_MAX
    );

    if (result != VK_SUCCESS) {
        printf("Failed to wait for Fence: %d\n", result);
        return 1;
    }
    //更新Buffer
    for (int objectIndex = 0; objectIndex < scene->objectCount; objectIndex++) {
        Rotate_Vertices_2D(&scene->objects[objectIndex]);
        Map_To_Screen_2D(&scene->objects[objectIndex],(float)Info->capabilities.surfaceCapabilities.currentExtent.width,(float)Info->capabilities.surfaceCapabilities.currentExtent.height);
        result=Update_Vertex_Buffer(Info->logicalDevice.device,&Info->vertexBuffer, &scene->objects[objectIndex]);
        if (result != 0) {
            printf("Failed to update Vertex Buffer: %d Error: %d\n", objectIndex,result);
            return 1;
        }
    }

    // 2. 获取下一张可用的 Swapchain Image
    uint32_t imageIndex = 0;

    result = vkAcquireNextImageKHR(
        Info->logicalDevice.device,
        Info->swapchain.swapchain,
        UINT64_MAX,
        Info->sync.imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        printf("Failed to acquire Swapchain Image: %d\n", result);
        return 1;
    }

    //printf("Acquired Swapchain Image: %u\n", imageIndex);

    // 后面还需要录制、提交、呈现这一帧
    result = vkResetCommandBuffer(
    Info->commandBuffer.commandBuffer,
    0
);

    if (result != VK_SUCCESS) {
        printf("Failed to reset Command Buffer: %d\n", result);
        return 1;
    }

    if (Record_CMD_Buffer(Info, imageIndex, scene) != 0) {
        printf("Failed to record Command Buffer!\n");
        return 1;
    }

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSemaphore signalSemaphore =
        Info->sync.renderFinishedSemaphores[imageIndex];

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,

        // 等待 Swapchain 图像可用
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &Info->sync.imageAvailableSemaphore,
        .pWaitDstStageMask = waitStages,

        // 要执行的 Command Buffer
        .commandBufferCount = 1,
        .pCommandBuffers = &Info->commandBuffer.commandBuffer,

        // GPU 执行完成时发出信号
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &signalSemaphore
    };

    result = vkResetFences(//重置fence（GPU正在工作）
    Info->logicalDevice.device,
    1,
    &Info->sync.inFlightFence
);

    if (result != VK_SUCCESS) {
        printf("Failed to reset Fence: %d\n", result);
        return 1;
    }

    result = vkQueueSubmit(//提交命令
    Info->logicalDevice.graphicsQueue, // 提交到哪个 GPU Queue
    1,                   // 提交几个 VkSubmitInfo
    &submitInfo,         // 提交内容
    Info->sync.inFlightFence  // GPU 完成后通知 CPU
);

    if (result != VK_SUCCESS) {
        printf("Failed to submit Command Buffer: %d\n", result);
        return 1;
    }

    VkSemaphore renderFinished =
    Info->sync.renderFinishedSemaphores[imageIndex];

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,

        // 等待 GPU 完成渲染
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &renderFinished,

        // 使用哪个 Swapchain
        .swapchainCount = 1,
        .pSwapchains = &Info->swapchain.swapchain,

        // 显示哪张 Image
        .pImageIndices = &imageIndex,

        .pResults = NULL
    };
    result = vkQueuePresentKHR(
    Info->logicalDevice.graphicsQueue,
    &presentInfo
);

    if (result != VK_SUCCESS &&
        result != VK_SUBOPTIMAL_KHR) {

        printf("Failed to present Swapchain Image: %d\n", result);
        return 1;
        }
    return 0;
}
