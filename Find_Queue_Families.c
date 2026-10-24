//
// Created by sovie on 2026/10/12.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"
int Find_Queue_Families(VkPhysicalDevice gpu) {

    // 1. 查询 Queue Family 数量
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(
        gpu, &queueFamilyCount, NULL
    );

    printf("Found %u Queue Families:\n",
           queueFamilyCount);
    int count = queueFamilyCount;

    if (queueFamilyCount == 0) {
        return-1;
    }

    // 2. 分配存储空间
    VkQueueFamilyProperties *families = malloc(
        queueFamilyCount * sizeof(VkQueueFamilyProperties)
    );

    if (families == NULL) {
        printf("Memory allocation failed!\n");
        return-1;
    }

    // 3. 读取 Queue Family 信息
    vkGetPhysicalDeviceQueueFamilyProperties(
        gpu, &queueFamilyCount, families
    );

    // 4. 打印每个 Queue Family 的能力
    for (uint32_t i = 0; i < queueFamilyCount; i++) {

        VkQueueFamilyProperties properties = families[i];

        printf("Queue Family %u:\n", i);
        printf("  Queue Count: %u\n",
               properties.queueCount);

        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            printf("  [GRAPHICS]\n");
        }

        if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            printf("  [COMPUTE]\n");
        }

        if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            printf("  [TRANSFER]\n");
        }
    }
    int selectedIndex=-1;

    for (uint32_t i = 0; i < count; i++) {

        if ((families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
            families[i].queueCount > 0) {

            selectedIndex = (int)i;
            break;
            }
    }

    free(families);
    return selectedIndex;
}
