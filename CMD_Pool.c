//
// Created by sovie on 2026/10/13.
//
#include "CMDs.h"

int Create_CMD_Pool(struct Logical_Device *Device,struct Command_Buffer *Info) {
    if (Info == NULL ||
        Device == NULL ||
        Device->device == VK_NULL_HANDLE ||
        Info->commandPool != VK_NULL_HANDLE) {
        return 1;
    }
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,//允许以后单独重置 Command Buffer
        .queueFamilyIndex = Device->graphicsFamilyIndex//指定这些 Command Buffer 将提交到哪个 Queue Family
    };
    VkResult result = vkCreateCommandPool(
    Device->device,
    &poolInfo,
    NULL,
    &Info->commandPool
);

    if (result != VK_SUCCESS) {
        printf("Failed to create Command Pool: %d\n", result);
        return 1;
    }

    printf("Successfully created Command Pool!\n");
    return 0;
}
int Allocate_CMD_Buffer(Logical_Device *Device,struct Command_Buffer *Info) {
    if (Info == NULL ||
        Device->device == VK_NULL_HANDLE ||
        Info->commandPool == VK_NULL_HANDLE) {
        return 1;
        }

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = Info->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkResult result = vkAllocateCommandBuffers(
        Device->device,
        &allocInfo,
        &Info->commandBuffer
    );

    if (result != VK_SUCCESS) {
        printf("Failed to allocate Command Buffer: %d\n", result);
        return 1;
    }

    printf("Successfully allocated Command Buffer!\n");
    return 0;
}

int Record_CMD_Buffer(struct Instance *Info,uint32_t imageIndex,scene *scene) {
    if (Info == NULL ||
        Info->logicalDevice.device == VK_NULL_HANDLE ||
        Info->commandBuffer.commandBuffer == VK_NULL_HANDLE) {
        return 1;
        }
    if (imageIndex >= Info->swapchain.swapchainImageCount) {
        return 1;
    }

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL
    };

    VkResult result = vkBeginCommandBuffer(
        Info->commandBuffer.commandBuffer,
        &beginInfo
    );

    if (result != VK_SUCCESS) {
        printf("Failed to begin Command Buffer: %d\n", result);
        return 1;
    }

    // 在这里记录绘图命令
    Transition_Image_To_Color(//将当前图像转换为颜色附件布局
    Info->commandBuffer.commandBuffer,
    Info->swapchain.swapchainImages[imageIndex]
    );

    VkRenderingAttachmentInfo colorAttachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = Info->swapchain.swapchainImageViews[imageIndex],//本帧要绘制到哪张 Image View
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,//图像已经转换到的布局
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,//开始渲染时怎样处理原有内容
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,//渲染结束后是否保存结果
        .clearValue = {
            .color = {{1.00f, 0.00f, 0.00f, 1.0f}}//清空图像时使用的颜色
        }
    };
    VkRenderingInfo renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,

        .renderArea = {//渲染区域有多大
            .offset = {0, 0},
            .extent = Info->capabilities.surfaceCapabilities.currentExtent
        },

        .layerCount = 1,
        .colorAttachmentCount = 1,//使用几个颜色附件
        .pColorAttachments = &colorAttachment
    };
    vkCmdBeginRendering(//开始渲染
    Info->commandBuffer.commandBuffer,
    &renderingInfo
    );

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(
       Info->commandBuffer.commandBuffer,
       0,                    // firstBinding
       1,                    // bindingCount
       &Info->vertexBuffer.vertexBuffer,  // 顶点缓冲区
       offsets               // 读取起始偏移
   );
    VkPipeline pipeline;
    for (int ObjectIndex = 0; ObjectIndex < scene->objectCount; ObjectIndex++) {
        if (scene->objects[ObjectIndex].type == VK_Object_Pipeline_Type_Line) {
            pipeline = Info->pipeline.linePipeline;
        }
        else if (scene->objects[ObjectIndex].type == VK_Object_Pipeline_Type_Triangle) {
            pipeline = Info->pipeline.graphicsPipeline;
        }
        else {
            printf("Object %d is not a valid pipeline, use defult one\n", ObjectIndex);
            pipeline = Info->pipeline.graphicsPipeline;
        }
        uint32_t firstVertex =
        (uint32_t)(scene->objects[ObjectIndex].location / sizeof(Vertex));

        vkCmdBindPipeline(//接下来的绘图命令使用哪一套 Graphics Pipeline（图形管线）
    Info->commandBuffer.commandBuffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    pipeline
);

        vkCmdDraw(
            Info->commandBuffer.commandBuffer,
            scene->objects[ObjectIndex].mesh->vertexCount,  // 三个顶点
            1,  // 一个实例
            firstVertex,  // 第一个顶点
            0   // 第一个实例
        );
    }

    vkCmdEndRendering(Info->commandBuffer.commandBuffer);
    Transition_Image_To_Present(
    Info->commandBuffer.commandBuffer,
    Info->swapchain.swapchainImages[imageIndex]
);

    result = vkEndCommandBuffer(Info->commandBuffer.commandBuffer);

    if (result != VK_SUCCESS) {
        printf("Failed to end Command Buffer: %d\n", result);
        return 1;
    }

    //printf("Successfully recorded Command Buffer!\n");
    return 0;
}

void Transition_Image_To_Color(
    VkCommandBuffer commandBuffer,
    VkImage image
) {
    VkImageMemoryBarrier2 barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,

        .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
        .srcAccessMask = VK_ACCESS_2_NONE,

        .dstStageMask =
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,

        .dstAccessMask =
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,

        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .image = image,

        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier
    };

    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
}

void Transition_Image_To_Present(
    VkCommandBuffer commandBuffer,
    VkImage image
) {
    VkImageMemoryBarrier2 barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,

        .srcStageMask =
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask =
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
        .dstAccessMask = VK_ACCESS_2_NONE,

        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,

        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .image = image,

        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier
    };

    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
}