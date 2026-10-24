//
// Created by sovie on 2026/10/21.
//

#ifndef VULKAN_GEOMETRIC_H
#define VULKAN_GEOMETRIC_H
#include "Vertex.h"
#define CIRCLE_SEGMENTS 128
#define CIRCLE_VERTEX_COUNT (CIRCLE_SEGMENTS + 1)
#define TOTAL_VERTEX_COUNT (3 + CIRCLE_VERTEX_COUNT)
#define PETAL_SEGMENTS 32
#define PETAL_RADIUS   0.07f
#define PETAL_HEIGHT   0.35f
Mesh Create_Triangle(void);
void Generate_Circle(
    Vertex *vertices,
    uint32_t segments,
    float radius,
    float centerX,
    float centerY
);
void Generate_Petal(
    Vertex *fill,
    Vertex *outline,
    uint32_t segments,
    float radius,
    float height
);
#endif //VULKAN_GEOMETRIC_H
