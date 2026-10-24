//
// Created by sovie on 2026/10/12.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"

int Create_Shader_Module(
    VkDevice device,
    const char *filename,
    VkShaderModule *shaderModule) {
    if (
    device == VK_NULL_HANDLE ||
    filename == NULL ||
    shaderModule == NULL) {
        return 1;
    }

    *shaderModule = VK_NULL_HANDLE;

    // 1. 读取 SPIR-V 文件
    unsigned char *shaderData = NULL;
    size_t shaderSize = 0;

    if (Read_Shader_File(filename, &shaderData, &shaderSize) != 0) {
        printf("Failed to read Shader file!\n");
        return 1;
    }
    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = shaderSize,
        .pCode = (const uint32_t *)shaderData
    };

    VkResult result = vkCreateShaderModule(
    device,
    &createInfo,
    NULL,
    shaderModule
);
    free(shaderData);
    shaderData = NULL;

    if (result != VK_SUCCESS) {
        printf("Failed to create Shader Module: %d\n", result);
        return 1;
    }
    else {
        printf("Successfully created Shader Module!\n");
        return 0;
    }
}