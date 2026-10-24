//
// Created by sovie on 2026/10/19.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"
int Create_Sync_Objects(VkDevice device, struct Sync *sync,uint32_t swapchainImageCount) {
    if (
        device == VK_NULL_HANDLE) {
        return 1;
        }

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkResult result = vkCreateSemaphore(
        device,
        &semaphoreInfo,
        NULL,
        &sync->imageAvailableSemaphore
    );

    if (result != VK_SUCCESS) {
        printf("Failed to create Image Available Semaphore: %d\n",
               result);
        return 1;
    }

    printf("Successfully created Image Available Semaphore!\n");

    sync->renderFinishedSemaphores = calloc(
    swapchainImageCount,
    sizeof(VkSemaphore)
);

    if (sync->renderFinishedSemaphores == NULL) {
        printf("Failed to allocate Render Finished Semaphores!\n");
        return 1;
    }

    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        result = vkCreateSemaphore(
            device,
            &semaphoreInfo,
            NULL,
            &sync->renderFinishedSemaphores[i]
        );

        if (result != VK_SUCCESS) {
            printf("Failed to create Render Finished Semaphore %u: %d\n",
                   i, result);
            return 1;
        }
        printf("Created Render Finished Semaphore %u Successfully\n",i);
    }

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    result = vkCreateFence(
    device,
    &fenceInfo,
    NULL,
    &sync->inFlightFence
    );

    if (result != VK_SUCCESS) {
        printf("Failed to create In Flight Fence: %d\n", result);
        return 1;
    }

    printf("Successfully created Sync Objects!\n");
    return 0;
}