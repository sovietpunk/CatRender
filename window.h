//
// Created by sovie on 2026/10/12.
//

#ifndef VULKAN_WINDOW_H
#define VULKAN_WINDOW_H
int Create_Window(struct Window_Info *window_info,int width,int height);
int Create_Surface(VkInstance instance,Window_Info *Info);
void Destroy_Window(struct Window_Info *Info);
#endif //VULKAN_WINDOW_H
