//
// Created by sovie on 2026/10/12.
//

#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


struct scene;
// Vulkan 状态


typedef struct GPU_Physical_Device {
    VkPhysicalDevice *gpus;
    uint32_t gpuCount;
    VkPhysicalDevice selectedGPU;
}GPU_Physical_Device;

typedef struct Logical_Device {
    VkDevice device;
    VkQueue graphicsQueue;
    uint32_t graphicsFamilyIndex;
}Logical_Device;

typedef struct Window_Info {
    GLFWwindow *window;
    VkSurfaceKHR surface;
}Window_Info;

typedef struct Capabilities {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VkSurfaceFormatKHR selectedSurfaceFormat;
    VkPresentModeKHR selectedPresentMode;
}Capabilities;

typedef struct Swapchain {
    VkSwapchainKHR swapchain;
    VkImage *swapchainImages;       // 交换链图像句柄数组
    uint32_t swapchainImageCount;   // 实际图像数量
    VkImageView *swapchainImageViews;
}Swapchain;

typedef struct Shader {
    VkShaderModule vertexShader;
    VkShaderModule fragmentShader;
}Shader;

typedef struct Graphics_Pipeline {
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkPipeline linePipeline;
}Graphics_Pipeline;

typedef struct Command_Buffer {
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
}Command_Buffer;

typedef struct Sync {
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore *renderFinishedSemaphores;
    VkFence inFlightFence;
}Sync;

typedef struct Vertex_Buffer {
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceSize vertexBufferSize;
}Vertex_Buffer;

struct Instance {
    VkInstance instance;

    GPU_Physical_Device physicalDevice;
    Logical_Device logicalDevice;
    Window_Info windowInfo;
    Capabilities capabilities;
    Swapchain swapchain;
    Shader shader;
    Graphics_Pipeline pipeline;
    Command_Buffer commandBuffer;
    Sync sync;
    Vertex_Buffer vertexBuffer;
};

// 函数声明
int Get_GPU_Info(VkInstance instance,struct GPU_Physical_Device *physical_device);

int Select_GPU(struct GPU_Physical_Device *physical_device);

int Find_Queue_Families(VkPhysicalDevice gpu);

int Create_Logical_Device(struct GPU_Physical_Device *physical_device,struct Logical_Device *logical_device);

void Destroy_Instance(struct Instance *Info);

int Create_Instance(struct Instance* Info);

int Get_GPU_Surface_Info(struct GPU_Physical_Device *physical_device,struct Logical_Device *logical_device,struct Window_Info *window_info,struct Capabilities *capabilities_output);

int Create_Swapchain(VkDevice device,struct Swapchain *swapchain, struct Capabilities *capabilities,struct Window_Info *window);

int Create_Swapchain_ImageViews(VkDevice device,struct Swapchain *swapchain,struct Capabilities *capabilities);

int Read_Shader_File(const char *filename,unsigned char **data,size_t *size);

int Create_Shader_Module(VkDevice device,const char *filename,VkShaderModule *shaderModule);

int Create_Graphics_Pipeline(VkDevice device,struct Shader *shader,struct Capabilities *capabilities,struct Graphics_Pipeline *pipeline);

int Create_Sync_Objects(VkDevice device, struct Sync *sync,uint32_t swapchainImageCount);



int Draw_Frame(struct Instance *Info,struct scene *scene);

#endif //VULKAN_INSTANCE_H
