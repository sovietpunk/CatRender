//
// Created by sovie on 2026/10/13.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"
#include "vertex.h"
int Create_Graphics_Pipeline(VkDevice device,struct Shader *shader,struct Capabilities *capabilities,struct Graphics_Pipeline *pipeline) {
    VkPipelineShaderStageCreateInfo shaderStages[2] = {//配置 Shader Stage
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,//Vulkan 结构体类型
            .stage = VK_SHADER_STAGE_VERTEX_BIT,//Shader 运行在哪个管线阶段
            .module = shader->vertexShader,//对应的 VkShaderModule 句柄
            .pName = "main"//Shader 的入口函数名称
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shader->fragmentShader,
            .pName = "main"
        }
    };
    VkVertexInputBindingDescription bindingDescription = {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };
    VkVertexInputAttributeDescription attributes[2] = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, position)
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, color)
        }
    };
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {//顶点输入
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,//Vulkan 结构体类型
        .vertexBindingDescriptionCount = 1,//顶点缓冲区绑定描述的数量
        .pVertexBindingDescriptions = &bindingDescription,//顶点缓冲区的布局描述数组
        .vertexAttributeDescriptionCount = 2,//顶点属性描述的数量
        .pVertexAttributeDescriptions = attributes//顶点属性的布局描述数组
    };
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {//图元组装
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,//Vulkan 结构体类型
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,//每三个顶点组成一个独立三角形。
        .primitiveRestartEnable = VK_FALSE//某些使用索引的图元组装模式下，通过特殊索引值重新开始一个图元序列
    };
    VkViewport viewport = {//把坐标映射到窗口
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)capabilities->surfaceCapabilities.currentExtent.width,
        .height = (float)capabilities->surfaceCapabilities.currentExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    VkRect2D scissor = {//哪些像素允许绘制
        .offset = {0, 0},//矩形左上角的位置
        .extent = capabilities->surfaceCapabilities.currentExtent//矩形的宽度和高度
    };
    VkPipelineViewportStateCreateInfo viewportState = {//Pipeline 配置
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,//Vulkan 结构体类型
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };
    VkPipelineRasterizationStateCreateInfo rasterizer = {//配置 Rasterization State（光栅化状态）
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,//Vulkan 结构体类型
        .depthClampEnable = VK_FALSE,//不启用深度钳制
        .rasterizerDiscardEnable = VK_FALSE,//不丢弃光栅化结果
        .polygonMode = VK_POLYGON_MODE_FILL,//填充整个三角形
        .cullMode = VK_CULL_MODE_NONE,//不剔除正面或背面
        .frontFace = VK_FRONT_FACE_CLOCKWISE,//以顺时针作为正面判断方式
        .depthBiasEnable = VK_FALSE,//不使用深度偏移
        .lineWidth = 1.0f//线宽为 1
    };
    VkPipelineMultisampleStateCreateInfo multisampling = {//MSAA抗锯齿
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,//Vulkan 结构体类型
        .sampleShadingEnable = VK_FALSE,//是否启用逐采样着色功能
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT//光栅化时使用的采样数
    };
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {//描述一个颜色附件的混合规则。
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT |//允许写入哪些颜色分量
                          VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE//是否开启颜色混合
    };
    VkPipelineColorBlendStateCreateInfo colorBlending = {//颜色混合
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,//Vulkan 结构体类型
        .logicOpEnable = VK_FALSE,//不启用逻辑运算混合
        .attachmentCount = 1,//一个附件
        .pAttachments = &colorBlendAttachment//附件
    };

    VkPipelineLayoutCreateInfo layoutInfo = {//管线布局配置
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,//Vulkan 结构体类型
        .setLayoutCount = 0,//Descriptor Set Layout 的数量
        .pSetLayouts = NULL,//Descriptor Set Layout 数组
        .pushConstantRangeCount = 0,//Push Constant 范围数量
        .pPushConstantRanges = NULL//Push Constant 范围数组
    };
    VkResult result = vkCreatePipelineLayout(//生成管线布局
    device,
    &layoutInfo,
    NULL,
    &pipeline->pipelineLayout
);

    if (result != VK_SUCCESS) {
        printf("Failed to create Pipeline Layout: %d\n", result);
        return 1;
    }

    printf("Successfully created Pipeline Layout!\n");

    VkFormat colorFormat = capabilities->selectedSurfaceFormat.format;

    VkPipelineRenderingCreateInfo renderingInfo = {//Rendering Info结构体：我们的 Graphics Pipeline 准备向什么格式的图像输出颜色
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,//Vulkan 结构体类型
        .colorAttachmentCount = 1,//颜色附件的数量
        .pColorAttachmentFormats = &colorFormat,//指向颜色附件格式的数组
        .depthAttachmentFormat = VK_FORMAT_UNDEFINED,//深度附件格式
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED//模板附件格式
    };
    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,//Vulkan 结构体类型

        // Dynamic Rendering
        .pNext = &renderingInfo,//Rendering Info结构体

        // Shader
        .stageCount = 2,//配置 Shader Stage
        .pStages = shaderStages,

        // 固定功能阶段
        .pVertexInputState = &vertexInputInfo,//顶点输入
        .pInputAssemblyState = &inputAssembly,//图元组装
        .pViewportState = &viewportState,//pipeline配置
        .pRasterizationState = &rasterizer,//光栅化状态
        .pMultisampleState = &multisampling,//MSAA
        .pDepthStencilState = NULL,
        .pColorBlendState = &colorBlending,////颜色混合
        .pDynamicState = NULL,

        // Pipeline Layout
        .layout = pipeline->pipelineLayout,

        // Dynamic Rendering 不使用传统 Render Pass
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,

        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };
    VkResult pipelineResult = vkCreateGraphicsPipelines(//创建 Graphics Pipeline
       device,
       VK_NULL_HANDLE,//暂时不使用 Pipeline Cache
       1,//创建一条 Pipeline
       &pipelineInfo,//创建配置
       NULL,//默认内存分配
       &pipeline->graphicsPipeline//接收 Pipeline 句柄
   );

    if (pipelineResult != VK_SUCCESS) {
        printf("Failed to create Graphics Pipeline: %d\n",
               pipelineResult);
        return 1;
    }

    printf("Successfully created Graphics Pipeline!\n");
    // 将图元拓扑切换为线条
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

    // 创建第二条 Pipeline
    result = vkCreateGraphicsPipelines(
        device,
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        NULL,
        &pipeline->linePipeline
    );

    if (result != VK_SUCCESS) {
        printf("Failed to create Line Pipeline: %d\n", result);
        return 1;
    }

    printf("Successfully created Line Pipeline!\n");
    return 0;
}