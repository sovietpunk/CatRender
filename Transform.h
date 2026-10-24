//
// Created by sovie on 2026/10/21.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vertex.h"

void Rotate_Vertices_2D(
    struct Object2D *source
);
void Map_To_Screen_2D(
    struct Object2D *source,
    float width,
    float height
);

#endif