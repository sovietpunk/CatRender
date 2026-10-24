//
// Created by sovie on 2026/10/21.
//
#include "Geometry.h"
#include <math.h>

#define PI 3.14159265358979323846f

static const Vertex triangleVertices[3] = {

        // 顶部：红色
        {{ 0.0f,      -0.577350f}, {1.0f, 0.0f, 0.0f}},

        // 右下：绿色
        {{ 0.5f,       0.288675f}, {0.0f, 1.0f, 0.0f}},

        // 左下：蓝色
        {{-0.5f,       0.288675f}, {0.0f, 0.0f, 1.0f}}
    };


Mesh Create_Triangle(void) {
    Mesh mesh = {
        .vertices = triangleVertices,
        .vertexCount = 3
    };

    return mesh;
}

void Generate_Circle(
    Vertex *vertices,
    uint32_t segments,
    float radius,
    float centerX,
    float centerY
) {
    if (vertices == NULL || segments < 3 || radius <= 0.0f) {
        return;
    }

    for (uint32_t i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * (float)i / (float)segments;

        vertices[i].position[0] = centerX + radius * cosf(angle);
        vertices[i].position[1] = centerY + radius * sinf(angle);

        vertices[i].color[0] = 1.0f;
        vertices[i].color[1] = 1.0f;
        vertices[i].color[2] = 1.0f;
    }
}

void Generate_Petal(
    Vertex *fill,
    Vertex *outline,
    uint32_t segments,
    float radius,
    float height
) {
    if (!fill || !outline || segments == 0) {
        return;
    }

    const float yellow[3] = {1.0f, 0.84f, 0.0f};
    const float black[3]  = {0.0f, 0.0f, 0.0f};

    for (uint32_t i = 0; i <= segments; i++) {
        float angle = PI - PI * (float)i / (float)segments;

        float x = radius * cosf(angle);
        float y = height + radius * sinf(angle);

        // 黑色轮廓：从左侧圆弧端点走到右侧
        outline[i] = (Vertex){
            .position = {x, y},
            .color = {black[0], black[1], black[2]}
        };

        // 黄色填充：一个三角形扇区
        if (i < segments) {
            float nextAngle =
                PI - PI * (float)(i + 1) / (float)segments;

            fill[i * 3] = (Vertex){
                .position = {0.0f, 0.0f},
                .color = {yellow[0], yellow[1], yellow[2]}
            };

            fill[i * 3 + 1] = (Vertex){
                .position = {x, y},
                .color = {yellow[0], yellow[1], yellow[2]}
            };

            fill[i * 3 + 2] = (Vertex){
                .position = {
                    radius * cosf(nextAngle),
                    height + radius * sinf(nextAngle)
                },
                .color = {yellow[0], yellow[1], yellow[2]}
            };
        }
    }

    // 将轮廓连接到原点
    outline[segments + 1] = (Vertex){
        .position = {0.0f, 0.0f},
        .color = {0.0f, 0.0f, 0.0f}
    };

    // 闭合轮廓：回到左侧圆弧端点
    outline[segments + 2] = outline[0];
}