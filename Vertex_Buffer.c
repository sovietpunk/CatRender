//
// Created by sovie on 2026/10/21.
//

#include "vertex.h"
#include <stdio.h>
#include <string.h>



int Create_Vertex_Buffer(VkDevice device,struct  Vertex_Buffer *buffer,VkPhysicalDevice GPU) {
    if (buffer == NULL || device == VK_NULL_HANDLE) {
        return 1;
    }
    VkDeviceSize bufferSize =
    20ull*1024*1024;

    buffer->vertexBufferSize = bufferSize;

    VkBufferCreateInfo bufferInfo = {//创建缓存区设定
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferSize,//要求大小
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,//声明它用于存放顶点
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE//使用独占队列族模式
    };


    VkResult result = vkCreateBuffer(
        device,
        &bufferInfo,
        NULL,
        &buffer->vertexBuffer
    );

    if (result != VK_SUCCESS) {
        printf("Failed to create Vertex Buffer: %d\n", result);
        return 1;
    }

    printf("Successfully created Vertex Buffer!\n");

    VkMemoryRequirements memRequirements;
    /*
    *VkMemoryRequirements 有三个重要成员：
    size	Vulkan 要求分配的内存大小
    alignment	内存绑定偏移必须满足的对齐要求
    memoryTypeBits	哪些内存类型可以用于这个 Buffer
     */

    vkGetBufferMemoryRequirements(
        device,
        buffer->vertexBuffer,
        &memRequirements
    );

    printf("Buffer size: %llu bytes\n",
           (unsigned long long)memRequirements.size);

    printf("Buffer alignment: %llu bytes\n",
           (unsigned long long)memRequirements.alignment);

    printf("Memory type bits: 0x%X\n",
           memRequirements.memoryTypeBits);

    //查询CPU内存类型
    VkPhysicalDeviceMemoryProperties memProperties;

    vkGetPhysicalDeviceMemoryProperties(
        GPU,
        &memProperties
    );

    uint32_t memoryTypeIndex = UINT32_MAX;

    VkMemoryPropertyFlags requiredProperties =//我们要这两种属性的内存
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {

        // 这个 Buffer 是否允许使用第 i 种内存？
        bool supported =
            (memRequirements.memoryTypeBits & (1u << i)) != 0;

        // 第 i 种内存是否具有我们要求的属性？
        bool suitable =
            (memProperties.memoryTypes[i].propertyFlags &
             requiredProperties) == requiredProperties;

        if (supported && suitable) {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX) {
        printf("Failed to find suitable memory type!\n");
        return 1;
    }

    printf("Selected Memory Type: %u\n", memoryTypeIndex);

    //分配物理内存
    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = memoryTypeIndex
    };

    result = vkAllocateMemory(
        device,
        &allocInfo,
        NULL,
        &buffer->vertexBufferMemory
    );

    //绑定物理与逻辑内存
    result = vkBindBufferMemory(
    device,
    buffer->vertexBuffer,
    buffer->vertexBufferMemory,
    0
);

    if (result != VK_SUCCESS) {
        printf("Failed to bind Vertex Buffer memory: %d\n", result);
        return 1;
    }

    printf("Successfully allocated and bound Vertex Buffer memory!\n");


    return 0;
}

int Update_Vertex_Buffer(
     VkDevice device,
    struct Vertex_Buffer *buffer,
    struct Object2D *object
) {
    // 1. 检查参数
    if (buffer == NULL || object == NULL ||
    object->transformed == NULL ||device == VK_NULL_HANDLE||
    buffer->vertexBuffer == VK_NULL_HANDLE ||
    buffer->vertexBufferMemory == VK_NULL_HANDLE)
         {
        return 1;
        }

    // 2. 计算需要上传的数据大小
    VkDeviceSize dataSize =
        sizeof(Vertex) * object->mesh->vertexCount;

    // 3. 检查是否超过 Buffer 容量
    if (dataSize > object->capacity) {
        printf("Vertex Buffer is too small!\n");
        return 1;
    }

    // 4. 映射设备内存
    void *mappedData = NULL;

    VkResult result = vkMapMemory(
        device,
        buffer->vertexBufferMemory,
        0,
        buffer->vertexBufferSize,
        0,
        &mappedData
    );

    if (result != VK_SUCCESS) {
        printf("Failed to map Vertex Buffer memory: %d\n",
               result);
        return 1;
    }

    // 5. 将 CPU 数据复制到映射内存
    memcpy(
    (char *)mappedData + object->location,
    object->transformed,
    (size_t)dataSize
);

    // 6. 解除映射
    vkUnmapMemory(
        device,
        buffer->vertexBufferMemory
    );

    return 0;
}