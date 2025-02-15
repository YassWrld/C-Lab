#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <image> <output_width> [output_height]\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int output_width = atoi(argv[2]);
    int output_height;

    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        fprintf(stderr, "Error loading image %s\n", filename);
        return 1;
    }

   
    if (argc >= 4) {
        output_height = atoi(argv[3]);
    } else {
        output_height = (height * output_width) / width;
        output_height = output_height > 0 ? output_height : 1;
    }

    // grayscaling the image
    unsigned char *gray = malloc(width * height);
    for (int i = 0; i < width * height; i++) {
        unsigned char r, g, b;
        if (channels >= 3) {
            r = data[i * channels];
            g = data[i * channels + 1];
            b = data[i * channels + 2];
        } else {
            r = g = b = data[i * channels];
        }
        gray[i] = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);
    }


    const char *ramp = "@%#*+=-:. "; // Ascii caracters ramp from dark to light
    //const char *ramp = "@@%%##*+=-   "; 
    int ramp_len = strlen(ramp);


    float x_ratio = (output_width > 1) ? (float)(width - 1) / (output_width - 1) : 0;
    float y_ratio = (output_height > 1) ? (float)(height - 1) / (output_height - 1) : 0;

    // Generate ASCII art
    for (int y = 0; y < output_height; y++) {
        for (int x = 0; x < output_width; x++) {
            int ox = (int)(x * x_ratio);
            int oy = (int)(y * y_ratio);
            
            // Clamp coordinates
            ox = ox < 0 ? 0 : (ox >= width ? width - 1 : ox);
            oy = oy < 0 ? 0 : (oy >= height ? height - 1 : oy);

            unsigned char g = gray[oy * width + ox];
            int index = (g * (ramp_len - 1)) / 255;
            index = index < 0 ? 0 : (index >= ramp_len ? ramp_len - 1 : index);
            putchar(ramp[index]);
        }
        putchar('\n');
    }

    stbi_image_free(data);
    free(gray);

    return 0;
}
