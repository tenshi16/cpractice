#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // Use fixed-size integer types for portability

// GCC specific packing
typedef struct {
    uint8_t bfType[2];
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} __attribute__ ((packed)) BMPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    // ... other fields in the info header (around 40 bytes total)
} __attribute__ ((packed)) BMPINFOHEADER;

// blue = counter
// green = counter +1
// red = counter + 2
unsigned char clamp(float val) {
    if (val < 0) return 0;
    if (val > 255) return 255;
    return (unsigned char)(val + 0.5f);
}

void ycbcr_conversion(unsigned char* original_image, unsigned char* ycbcrData, size_t size) {
    for(size_t i = 0; i < size; i+=3){
        float B = (float)original_image[i];
        float G = (float)original_image[i + 1];
        float R = (float)original_image[i + 2];
        // BT.601 Conversion Formula with +0.5 for mathematical rounding
        ycbcrData[i]     = clamp( 0.299f * R + 0.587f * G + 0.114f * B); 
        ycbcrData[i + 1] = clamp(128.0f - 0.1687f * R - 0.3313f * G + 0.5f * B);
        ycbcrData[i + 2] = clamp(128.0f + 0.5f * R - 0.4187f * G - 0.0813f * B);
    }
}

int main() {
    FILE *file = fopen("colorful.bmp", "rb");

    if (file == NULL) {
        printf("Invalid file\n");
    }
    BMPFILEHEADER fileHeader;
    BMPINFOHEADER infoHeader;

    fread(&fileHeader, sizeof(BMPFILEHEADER), 1, file);
    fread(&infoHeader, sizeof(BMPINFOHEADER), 1, file);

    // Verify the file signature ('BM')
    if (fileHeader.bfType[0] != 'B' || fileHeader.bfType[1] != 'M') {
        fclose(file);
        printf("Invalid File\n");
    }
    // Example for 24-bit BMP (3 bytes per pixel)
    int width = infoHeader.biWidth;
    int height = infoHeader.biHeight;
    int bytesPerPixel = infoHeader.biBitCount / 8; // e.g., 3 for 24-bit

    // Calculate padding for each row
    int padding = (4 - (width * bytesPerPixel) % 4) % 4;

    // Allocate memory for the entire image
    size_t image_size = height * (width * bytesPerPixel + padding);
    unsigned char *imageData = malloc(image_size);
    unsigned char *ycbcrData = malloc(image_size);

    // Seek to the start of pixel data
    fseek(file, fileHeader.bfOffBits, SEEK_SET);
    printf("Size of %zu\n", image_size); 

    // Read all pixel data
    fread(imageData, 1, height * (width * bytesPerPixel + padding), file);
    ycbcr_conversion(imageData, ycbcrData, image_size);
    //printf("%u\n", ycbcrData[i]);
    printf("Printing first pixel in bgr, %u,%u,%u", imageData[0], imageData[1], imageData[2]);
    printf("Printing first pixel in ycbcr, %u,%u,%u", ycbcrData[0], ycbcrData[1], ycbcrData[2]);
    return 0;
}
