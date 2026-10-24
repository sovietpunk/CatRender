//
// Created by sovie on 2026/10/12.
//
#include <stdio.h>
#include <stdlib.h>

int Read_Shader_File(
    const char *filename,
    unsigned char **data,
    size_t *size
) {
    if (filename == NULL || data == NULL || size == NULL) {
        return 1;
    }

    *data = NULL;
    *size = 0;

    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        printf("Failed to open shader: %s\n", filename);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    if (fileSize <= 0 || fileSize % 4 != 0) {
        printf("Invalid SPIR-V file size!\n");
        fclose(file);
        return 1;
    }

    unsigned char *buffer = malloc((size_t)fileSize);

    if (buffer == NULL) {
        fclose(file);
        return 1;
    }

    size_t bytesRead = fread(buffer, 1, (size_t)fileSize, file);
    fclose(file);

    if (bytesRead != (size_t)fileSize) {
        free(buffer);
        return 1;
    }

    *data = buffer;
    *size = (size_t)fileSize;

    return 0;
}