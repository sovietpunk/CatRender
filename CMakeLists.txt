
cmake_minimum_required(VERSION 3.21)

project(Vulkan C)

set(CMAKE_C_STANDARD 17)

set(ENV{VULKAN_SDK} "E:/CST/vulkan/sdk/1.4.363.0")

find_package(Vulkan REQUIRED)

include(FetchContent)

# 禁用 GLFW 不需要的组件
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

add_executable(Vulkan
        main.c
        window.c
        window.c
        instance.h
        Get_GPU_Info.c
        Instance.c
        Select_GPU.c
        Find_Queue_Families.c
        Create_Logical_Device.c
        window.h
        Create_Swapchain.c
        Read_Shader_File.c
        Create_Shader_Module.c
        Graphic_Pipline.c
        CMD_Pool.c
        Synchronization.c
        Draw.c
        vertex.h
        Vertex_Buffer.c
        Transform.h
        Transform.c
        Geometry.c
        Geometry.h
        CMDs.h

)

target_link_libraries(
        Vulkan PRIVATE
        Vulkan::Vulkan
        glfw
)
