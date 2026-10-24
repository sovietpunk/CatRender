//
// Created by sovie on 2026/10/11.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"
#include "window.h"
#include "Geometry.h"
#include "CMDs.h"
#include "vertex.h"
#include "Transform.h"


int test();

int main() {
    struct GPU_Physical_Device physical_device ={0};
   struct Instance defalt={0};


    //创建窗口
    if (Create_Window(&defalt.windowInfo, 800, 600) != 0) {
        printf("Create_Window Failed!\n");
        goto cleanup;
    }
    //创建instance
    if (Create_Instance(&defalt)!= 0) {
        printf("Create_Instance Failed!\n");
        goto cleanup;
    }
    //创建Surface
    if (Create_Surface(defalt.instance,&defalt.windowInfo) != 0) {
        printf("Create_Surface Failed!\n");
        Destroy_Instance(&defalt);
    }
    //获取GPU信息
    if (Get_GPU_Info(defalt.instance,&defalt.physicalDevice)==1) {                                                     //GPU Info
        printf("No Vulkan GPU found!\n");
        goto cleanup;
    }
    //选择GPU
    int status;
    do {
        status =Select_GPU(&defalt.physicalDevice);                                               //Select GPU
    }while (status==1);
    if (status == 0) {
        VkPhysicalDeviceProperties properties;

        vkGetPhysicalDeviceProperties(
            defalt.physicalDevice.selectedGPU,
            &properties
        );

        printf("Selected GPU: %s\n", properties.deviceName);
    } else {
        printf("GPU selection cancelled.\n");
        goto cleanup;
    }
    //查询与寻找familyindex
    int familyindex = Find_Queue_Families(defalt.physicalDevice.selectedGPU);                   //Find Queue Family
    if (familyindex != 0) {
        printf("Find_Queue_Families Failed!\n");
        goto cleanup;
    }
    defalt.logicalDevice.graphicsFamilyIndex = (uint32_t)familyindex;
    printf("Selected Queue Family Index: %u\n", defalt.logicalDevice.graphicsFamilyIndex);
    //创建逻辑设备
    status=Create_Logical_Device(&defalt.physicalDevice,&defalt.logicalDevice);
    if ( status!= 0) {                                  //Create Logical Device
        printf("Create_Logical_Device & Queue Failed!\n");
        goto cleanup;
    }

    if (Get_GPU_Surface_Info(&defalt.physicalDevice,&defalt.logicalDevice,&defalt.windowInfo,&defalt.capabilities) != 0) {
        printf("Get_GPU_Surface_Info Failed!\n");
        goto cleanup;
    }
    //创建swapchain
    if (Create_Swapchain(defalt.logicalDevice.device,&defalt.swapchain,&defalt.capabilities,&defalt.windowInfo) != 0) {
        printf("Create_Swapchain Failed!\n");
        goto cleanup;
    }
    //创建ImageViews
    if (Create_Swapchain_ImageViews(defalt.logicalDevice.device,&defalt.swapchain,&defalt.capabilities)!=0) {
        printf("Create_Swapchain_ImageViews Failed!\n");
        goto cleanup;
    }
    //创建渲染句柄
    if (Create_Shader_Module(
    defalt.logicalDevice.device,
    "shaders/triangle.vert.spv",
    &defalt.shader.vertexShader

) != 0) {
        goto cleanup;
}

    if (Create_Shader_Module(
        defalt.logicalDevice.device,
        "shaders/triangle.frag.spv",
        &defalt.shader.fragmentShader
    ) != 0) {
        goto cleanup;
    }
    //图形管线
    if (Create_Graphics_Pipeline(defalt.logicalDevice.device,&defalt.shader,&defalt.capabilities,&defalt.pipeline) != 0) {
        goto cleanup;
    }

    if (Create_CMD_Pool(&defalt.logicalDevice,&defalt.commandBuffer) != 0) {
        goto cleanup;
    }

    if (Allocate_CMD_Buffer(&defalt.logicalDevice,&defalt.commandBuffer) != 0) {
       goto cleanup;;
    }

    if (Create_Sync_Objects(defalt.logicalDevice.device,&defalt.sync,defalt.swapchain.swapchainImageCount) != 0) {
        goto cleanup;
    }

    struct Vertex line_vertex[PETAL_SEGMENTS + 3] = {0};
    struct Vertex line_vertex_transformed[PETAL_SEGMENTS + 3] = {0};
    struct Vertex surface_vertex[PETAL_SEGMENTS * 3] = {0};
    struct Vertex surface_vertex_transformed[PETAL_SEGMENTS * 3] = {0};
    struct Mesh line={
        .vertexCount = PETAL_SEGMENTS + 3,
        .vertices = line_vertex,
    };
    struct Mesh surface ={
        .vertexCount = PETAL_SEGMENTS * 3,
        .vertices = surface_vertex
    };
    Generate_Petal(surface_vertex,line_vertex,PETAL_SEGMENTS,PETAL_HEIGHT,PETAL_RADIUS);
    struct Object2D Line={
        .mesh = &line,
        .transformed = line_vertex_transformed,
        .type =VK_Object_Pipeline_Type_Line,
        .angle = 0,
        .angularVelocity = 0.0f,
        .location = 0,
        .capacity = sizeof(Vertex)*line.vertexCount,
    };
    struct Object2D Surface ={
        .mesh = &surface,
        .transformed = surface_vertex_transformed,
        .type = VK_Object_Pipeline_Type_Triangle,
        .angle = 0,
        .angularVelocity = 0.0f,
        .location = Line.location+Line.capacity,
        .capacity = sizeof(Vertex)*surface.vertexCount,
    };
    Rotate_Vertices_2D(&Line);
    Map_To_Screen_2D(&Line,(float)defalt.capabilities.surfaceCapabilities.currentExtent.width,(float)defalt.capabilities.surfaceCapabilities.currentExtent.height);
    Rotate_Vertices_2D(&Surface);
    Map_To_Screen_2D(&Surface,(float)defalt.capabilities.surfaceCapabilities.currentExtent.width,(float)defalt.capabilities.surfaceCapabilities.currentExtent.height);
    //創建scene
    struct Object2D AllObjects[2]={Line,Surface};
    //創建scene
    struct scene scene1={
        .objectCount = 2,
        .objects = AllObjects,
    };
    Create_Vertex_Buffer(defalt.logicalDevice.device,&defalt.vertexBuffer,defalt.physicalDevice.selectedGPU);
    for (int ObjectIndex = 0; ObjectIndex < scene1.objectCount; ObjectIndex++) {
        Update_Vertex_Buffer(defalt.logicalDevice.device,&defalt.vertexBuffer,&scene1.objects[ObjectIndex]);

    }

    // 保持窗口运行
    double lastFrameTime = glfwGetTime(); // 上一帧时间
    double lastFpsTime = lastFrameTime;   // 上一次 FPS 统计时间

    uint32_t frameCount = 0;

    printf("Window running. Close it to exit.\n");

    while (!glfwWindowShouldClose(defalt.windowInfo.window)) {
        glfwPollEvents();

        // 1. 计算帧间隔
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastFrameTime);
        lastFrameTime = currentTime;

        // 2. 更新 CPU 场景
        for ( int ObjectIndex = 0; ObjectIndex < scene1.objectCount; ObjectIndex++) {
            scene1.objects[ObjectIndex].angle += scene1.objects[ObjectIndex].angularVelocity*deltaTime;
        }


        // 3. Vulkan 绘制
        if (Draw_Frame(&defalt,&scene1) != 0) {
            printf("Render Loop stopped!\n");
            break;
        }

        // 4. FPS 统计
        frameCount++;

        currentTime = glfwGetTime();
        double elapsed = currentTime - lastFpsTime;

        if (elapsed >= 1.0) {
            double fps = frameCount / elapsed;

            char title[128];
            snprintf(
                title,
                sizeof(title),
                "My Vulkan Renderer | FPS: %.1f",
                fps
            );

            glfwSetWindowTitle(defalt.windowInfo.window, title);

            frameCount = 0;
            lastFpsTime = currentTime;
        }
    }



    vkDeviceWaitIdle(defalt.logicalDevice.device);
    //清理所有资源
    cleanup:

    Destroy_Instance(&defalt);
    return 0;
}



int test(struct Instance *defalt) {
    //初始化物体渲染

    struct Vertex triangleTransformed[3] = {0};
    struct Vertex circleTransformed[CIRCLE_SEGMENTS + 1] = {0};
    struct Mesh triangle = Create_Triangle();
    struct Vertex circleVertices[CIRCLE_SEGMENTS+1];
    struct Mesh circle ={
        .vertexCount = CIRCLE_SEGMENTS+1,
        .vertices = circleVertices
    };
    Generate_Circle(circle.vertices,CIRCLE_SEGMENTS,0.57735027f,0,0);
    struct Object2D Triangle={
        .mesh = &triangle,
        .transformed = triangleTransformed,
        .type =VK_Object_Pipeline_Type_Triangle,
        .angle = 0,
        .angularVelocity = 1.0f,
        .location = 0,
        .capacity = sizeof(Vertex)*triangle.vertexCount,
    };
    struct Object2D Circle ={
        .mesh = &circle,
        .transformed = circleTransformed,
        .type = VK_Object_Pipeline_Type_Line,
        .angle = 0,
        .angularVelocity = 0.0f,
        .location = Triangle.location+Triangle.capacity,
        .capacity = sizeof(Vertex)*(CIRCLE_SEGMENTS+1),
    };
    Rotate_Vertices_2D(&Triangle);
    Map_To_Screen_2D(&Triangle,(float)defalt->capabilities.surfaceCapabilities.currentExtent.width,(float)defalt->capabilities.surfaceCapabilities.currentExtent.height);
    Rotate_Vertices_2D(&Circle);
    Map_To_Screen_2D(&Circle,(float)defalt->capabilities.surfaceCapabilities.currentExtent.width,(float)defalt->capabilities.surfaceCapabilities.currentExtent.height);
    //創建scene
    struct Object2D AllObjects[2]={Triangle,Circle};


    //创建buffer并初始化
    if (Create_Vertex_Buffer(defalt->logicalDevice.device,&defalt->vertexBuffer,defalt->physicalDevice.selectedGPU) != 0) {
        return 1;
    }

    if (Update_Vertex_Buffer(defalt->logicalDevice.device,&defalt->vertexBuffer, &Triangle) != 0) {
        return 1;
    }

    if (Update_Vertex_Buffer(defalt->logicalDevice.device,&defalt->vertexBuffer, &Circle) != 0) {
        return 1;
    }
}