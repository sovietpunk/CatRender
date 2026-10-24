//
// Created by sovie on 2026/10/21.
//
#include "Transform.h"
#include <math.h>

void Rotate_Vertices_2D(
    struct Object2D *source
) {
    float c = cosf(source->angle);
    float s = sinf(source->angle);

    for (uint32_t i = 0; i < source->mesh->vertexCount; i++) {
        float x = source->mesh->vertices[i].position[0];
        float y = source->mesh->vertices[i].position[1];

        source->transformed[i] = source->mesh->vertices[i];

        source->transformed[i].position[0] = x * c - y * s;
        source->transformed[i].position[1] = x * s + y * c;
    }
}
void Map_To_Screen_2D(
    struct Object2D *source,
    float width,
    float height
) {
    if (source->mesh == NULL || width <= 0.0f || height <= 0.0f) {
        return;
    }

    float aspect = width / height;

    for (uint32_t i = 0; i < source->mesh->vertexCount; i++) {
        source->transformed[i].position[0] /= aspect;
    }
}