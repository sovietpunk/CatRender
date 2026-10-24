//
// Created by sovie on 2026/10/12.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"
int Create_Swapchain(VkDevice device,struct Swapchain *swapchain, struct Capabilities *capabilities,struct Window_Info *window) {

    if (
        device == VK_NULL_HANDLE ||
        swapchain == NULL ||
        window->surface == VK_NULL_HANDLE||
        capabilities == NULL) {
        return 1;
        }

    uint32_t imageCount = capabilities->surfaceCapabilities.minImageCount + 1;//比最小要求大1

    if (capabilities->surfaceCapabilities.maxImageCount > 0 &&//如果最大为0就是无限
        imageCount > capabilities->surfaceCapabilities.maxImageCount) {//如果现在的值超过了最大要求

        imageCount = capabilities->surfaceCapabilities.maxImageCount;//那就干脆用最大的好了
        }

    VkSwapchainCreateInfoKHR createInfo = {//创建Swapchain设置
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,//类型
        .surface = window->surface,//我们创建的Surface
        .minImageCount = imageCount,//创建的图像数量
        .imageFormat = capabilities->selectedSurfaceFormat.format,
        .imageColorSpace = capabilities->selectedSurfaceFormat.colorSpace,
        .imageExtent = capabilities->surfaceCapabilities.currentExtent,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        /*VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT	作为渲染目标，接收绘制出的颜色
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT	作为图像复制的来源
        VK_IMAGE_USAGE_TRANSFER_DST_BIT	作为图像复制的目标
        VK_IMAGE_USAGE_SAMPLED_BIT	允许着色器采样图像
        */
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        /*VK_SHARING_MODE_EXCLUSIVE	图像在同一时间由一个 Queue Family 拥有
        VK_SHARING_MODE_CONCURRENT	多个指定 Queue Family 可以共享使用
        */
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,//这俩只要在VK_SHARING_MODE_CONCURRENT才有用
        .imageArrayLayers = 1,//每张交换链图像包含一层
        .preTransform = capabilities->surfaceCapabilities.currentTransform,//图像呈现前采用的表面变换：我们使用 Surface 当前报告的变换方式，例如某些显示设备可能涉及旋转。这样不需要在第一版渲染器里自己处理显示方向。
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,//窗口透明度合成方式 VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR表示把窗口内容作为不透明画面处理。正式创建前应确认
        .presentMode = capabilities->selectedPresentMode,//呈现模式
        .clipped = VK_TRUE,//是否允许忽略被遮挡区域的像素内容
        .oldSwapchain = VK_NULL_HANDLE//替换已有交换链时使用的旧句柄，因为我们现在是第一次创建交换链，没有旧交换链需要替换
    };

    if (!(capabilities->surfaceCapabilities.supportedUsageFlags &
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)) {
        printf("Surface does not support Color Attachment!\n");
        return 1;
      }

    if (!(capabilities->surfaceCapabilities.supportedCompositeAlpha &
          VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)) {
        printf("Surface does not support Opaque Composite Alpha!\n");
        return 1;
          }

    VkResult result=vkCreateSwapchainKHR(//创建swapchain
    device,//我们的逻辑设备
    &createInfo,//我们的设定
    NULL,//默认内存分配
    &swapchain->swapchain
);
    if (result != VK_SUCCESS) {
        printf("Failed to create swapchain. Error: %d\n",result);
        return 1;
    }


    printf("Successfully created Vulkan Swapchain!\n");
    //获取图像数量
    uint32_t swapchainImageCount = 0;

    VkResult imageResult = vkGetSwapchainImagesKHR(
        device,
        swapchain->swapchain,
        &swapchainImageCount,
        NULL
    );

    if (imageResult != VK_SUCCESS || swapchainImageCount == 0) {
        printf("Failed to get Swapchain Image Count: %d\n",
               imageResult);
        return 1;
    }

    printf("Swapchain Image Count: %u\n", swapchainImageCount);

    swapchain->swapchainImageCount = swapchainImageCount;

    swapchain->swapchainImages = malloc(
        swapchainImageCount * sizeof(VkImage)
    );

    if (swapchain->swapchainImages == NULL) {
        printf("Failed to allocate Swapchain Image array!\n");
        return 1;
    }

    result = vkGetSwapchainImagesKHR(
        device,
        swapchain->swapchain,
        &swapchain->swapchainImageCount,
        swapchain->swapchainImages
    );

    if (result != VK_SUCCESS) {
        printf("Failed to get Swapchain Images: %d\n", result);
        return 1;
    }

    printf("Successfully retrieved %u Swapchain Images!\n",
           swapchain->swapchainImageCount);
    return 0;
}

int Create_Swapchain_ImageViews(VkDevice device,struct Swapchain *swapchain,struct Capabilities *capabilities) {
    if (
        device == VK_NULL_HANDLE ||
        swapchain == NULL||
        swapchain->swapchainImages == NULL ||
        swapchain->swapchainImageCount == 0) {
        return 1;
        }

    swapchain->swapchainImageViews = calloc(
        swapchain->swapchainImageCount,
        sizeof(VkImageView)
    );

    if (swapchain->swapchainImageViews == NULL) {
        printf("Failed to allocate Image View array!\n");
        return 1;
    }

    // 下一步在这里创建每个 Image View
    for (int i = 0; i < swapchain->swapchainImageCount; i++) {
        VkImageViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain->swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = capabilities->selectedSurfaceFormat.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },

            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        VkResult result = vkCreateImageView(
        device,                  // Logical Device
        &createInfo,                   // 创建配置
        NULL,                          // 默认内存分配方式
        &swapchain->swapchainImageViews[i]  // 输出 Image View 句柄
        );
        if (result != VK_SUCCESS) {
            printf("Failed to create Image View %u: %d\n", i, result);
            return 1;
        }

        printf("Successfully created Image View %u!\n", i);
    }
    return 0;
}