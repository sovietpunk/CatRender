//
// Created by sovie on 2026/10/21.
//

#ifndef VERTEX_H
#define VERTEX_H

#include <stdint.h>
#include <stdbool.h>
#include "instance.h"
#define VK_Object_Pipeline_Type_Triangle 1
#define VK_Object_Pipeline_Type_Line     2
typedef struct Vertex {
    float position[2]; // x, y
    float color[3];    // r, g, b
} Vertex;

typedef struct Mesh {
    Vertex *vertices;
    uint32_t vertexCount;
} Mesh;

typedef struct Object2D {
    Mesh *mesh;             // 指向原始 Mesh

    Vertex *transformed;    // CPU 处理后的顶点

    float angle;
    float angularVelocity;

    uint64_t location;      // GPU Buffer 字节偏移
    uint64_t capacity;      // GPU Buffer 分配容量

    uint32_t type;
} Object2D;

typedef struct scene {
    uint32_t objectCount;
    Object2D *objects;
}scene;


struct Instance;

int Create_Vertex_Buffer(VkDevice device,struct  Vertex_Buffer *buffer,VkPhysicalDevice GPU);

int Update_Vertex_Buffer(
    VkDevice device,
   struct Vertex_Buffer *buffer,
   struct Object2D *object
);

#endif