//
// Created by sovie on 2026/10/12.
//

#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Instance.h"
#include "window.h"

int Create_Window(struct Window_Info *window_info,int width,int height) {
    if (window_info == NULL) {
        return 1;
    }

    // 初始化 GLFW
    if (glfwInit() != GLFW_TRUE) {
        printf("Failed to initialize GLFW!\n");
        return 1;
    }

    // 不创建 OpenGL Context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//glfwWindowHint创建设置  GLFW_CLIENT_API设置窗口API为GLFW_NO_API（不设置API）

    // 创建窗口
    window_info->window = glfwCreateWindow(
        width,
        height,
        "My Vulkan Renderer",
        NULL,
        NULL
    );

    if (window_info->window == NULL) {
        printf("Failed to create GLFW window!\n");
        glfwTerminate();
        return 1;
    }

    printf("Successfully created GLFW window!\n");
    return 0;
}

int Create_Surface(VkInstance instance,struct Window_Info *Info) {
    if (Info == NULL ||
        instance == VK_NULL_HANDLE ||
        Info->window == NULL) {
        return 1;
        }

    VkResult result = glfwCreateWindowSurface(//创建界面
        instance,//Vulkan instance
        Info->window,//创建的窗口
        NULL,//不使用自定义内存分配回调，而是使用默认分配方式。
        &Info->surface//输出的界面
    );

    if (result != VK_SUCCESS) {
        printf("Failed to create Vulkan Surface: %d\n",
               result);
        return 1;
    }

    printf("Successfully created Vulkan Surface!\n");
    return 0;
}

void Destroy_Window(struct Window_Info *Info) {
    if (Info == NULL) {
        return;
    }

    if (Info->window != NULL) {
        glfwDestroyWindow(Info->window);
        Info->window = NULL;
    }

    glfwTerminate();
}
