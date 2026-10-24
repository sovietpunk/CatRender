//
// Created by sovie on 2026/10/23.
//

#ifndef VULKAN_CMDS_H
#define VULKAN_CMDS_H

#include "instance.h"
#include "vertex.h"
#include <stdio.h>

int Create_CMD_Pool(struct Logical_Device *Device,struct Command_Buffer *Info);

int Allocate_CMD_Buffer(struct Logical_Device *Device,struct Command_Buffer *Info);

int Record_CMD_Buffer(struct Instance *Info,uint32_t imageIndex,struct scene *scene);

void Transition_Image_To_Color(VkCommandBuffer commandBuffer,VkImage image);

void Transition_Image_To_Present(VkCommandBuffer commandBuffer,VkImage image);
#endif //VULKAN_CMDS_H
