//
// Created by sovie on 2026/10/12.
//
#include <stdio.h>
#include <stdlib.h>
#include "instance.h"
int Select_GPU(struct GPU_Physical_Device *physical_device) {

    // 检查 GPU 列表是否有效
    if (physical_device == NULL ||
        physical_device->gpus == NULL ||
        physical_device->gpuCount == 0) {
        return 1;
        }
    int selectedGPU;
    printf("please select one of the GPUs to use:\n");
    int inputResult = scanf("%d", &selectedGPU);
    if (inputResult == EOF) {
        printf("Input closed!\n");
        return -1;
    }
    if (inputResult != 1) {
        printf("Please enter a valid integer!\n");
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            // 丢弃错误输入
        }
        return 1;
    }
    if (selectedGPU < 0 ||
    (uint32_t)selectedGPU >= physical_device->gpuCount) {
        printf("GPU index out of range!\n");
        return 1;
    }
    physical_device->selectedGPU = physical_device->gpus[selectedGPU];
    return 0;
}
