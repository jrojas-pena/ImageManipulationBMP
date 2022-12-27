#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <iostream>
#include <string.h>

using namespace std;

// Define a struct to represent an image
struct Image {
    int height;
    int width;
    short bitCount;
    unsigned char *information; 
};

// Transforms an image from memory to a two-dimensional array
void transformToArray(Image * img, unsigned char * array[]) {
    unsigned char * pix = img->information; 
    for(int i = 0; i < img->height; i++) {
        array[i] = (unsigned char *)calloc(img->width, sizeof(unsigned char));
        for(int j = 0; j < img->width; j++) {
            array[i][j] = *pix;
            pix++;
        }
    }
}

// Blurs an image from an array
void blur(unsigned char * array[], int pixelsToBlur, Image * img) {

    int l;

    // Iterate over the rows of the array
    for(int i = pixelsToBlur; i < img->height; i += 2*pixelsToBlur + 1) {
        // Iterate over the columns of the array
        for(int j = pixelsToBlur * 3; j < img->width; (j+6*pixelsToBlur + 3) >= img->width ? j += 3 * (pixelsToBlur + ((img->width / 3) % (2 * pixelsToBlur + 1))) : j += 6*pixelsToBlur + 3) {
            // Iterate over the rows within the blur radius
            for(int k = i + pixelsToBlur; k >= i - pixelsToBlur && k < img->height; k--) {
                // Iterate over the columns within the blur radius
                for(j == img->width - 3? l = j - (img->width % (3 * (2 * pixelsToBlur + 1))) : l = j - 3*pixelsToBlur; l < j + 3*pixelsToBlur + 3 && l < img->width; l += 3){
                    array[k][l] = array[i][j];
                    array[k][l + 1] = array[i][j + 1];
                    array[k][l + 2] = array[i][j + 2];
                }
            }
        }
    }
    int res = img->height - (img->height % (2 * pixelsToBlur + 1));
    // Iterate over the columns of the array
    for(int j = pixelsToBlur * 3; j < img->width; (j+6*pixelsToBlur + 3) >= img->width ? j += 3 * (pixelsToBlur + ((img->width / 3) % (2 * pixelsToBlur + 1))) : j += 6*pixelsToBlur + 3) {
        // Iterate over the rows within the blur radius
        for(int k = img->height - 1; k >= res; k--) {
            // Iterate over the columns within the blur radius
            for(j == img->width - 3? l = j - (img->width % (3 * (2 * pixelsToBlur + 1))) : l = j - 3*pixelsToBlur; l < j + 3*pixelsToBlur + 3 && l < img->width; l += 3){
                array[k][l] = array[res][j];
                array[k][l + 1] = array[res][j + 1];
                array[k][l + 2] = array[res][j + 2];
            }
        }
    }
}

// Transforms an image from an array to memory
void transformFromArray(Image * img, unsigned char * array[]) {
    unsigned char * pix = img->information; 
    for(int i = 0; i < img->height; i++) {
        for(int j = 0; j < img->width; j++) {
            *pix = array[i][j];
            pix++;
        }
    }
}

// Loads a BMP image from a file
void loadBMP(Image * img, char * fileName) {
    // Open the input file
    FILE *bitmapFile = fopen(fileName, "rb");
    if (bitmapFile == NULL) {
        printf("It was not possible to load the file: %s\n", fileName);
        exit(-1);
    }

    // Read the Bitmap Data Offset field from the file
    int bmpDataOffset;
    fseek(bitmapFile, 10, SEEK_SET);
    fread(&bmpDataOffset, sizeof(int), 1, bitmapFile);

    // Read the width field from the file
    int bmpWidth;
    fseek(bitmapFile, 18, SEEK_SET);
    fread(&bmpWidth, sizeof(int), 1, bitmapFile);

    // Read the height field from the file
    int bmpHeight;
    fseek(bitmapFile, 22, SEEK_SET);
    fread(&bmpHeight, sizeof(int), 1, bitmapFile);

    // Read the bit count field from the file
    short bmpBitCount;
    fseek(bitmapFile, 28, SEEK_SET);
    fread(&bmpBitCount, sizeof(short), 1, bitmapFile);

    // Calculate the width of the image in bytes
    bmpWidth = bmpBitCount == 32 ? bmpWidth * 4 : bmpWidth * 3;

    // Calculate the number of bits residual after aligning to 32-bit words
    int residue = (4 - (bmpWidth) % 4) & 3;

    // Set the fields of the Image struct
    img->width = bmpWidth;
    img->height = bmpHeight;
    img->bitCount = bmpBitCount;
    img->information = (unsigned char *)calloc(bmpWidth * bmpHeight, sizeof(unsigned char));

    // Read the pixel data from the file
    fseek(bitmapFile, bmpDataOffset, SEEK_SET);
    fread(img->information, sizeof(unsigned char), bmpWidth * bmpHeight, bitmapFile);

    // Close the input file
    fclose(bitmapFile);
}

// Saves a BMP image to a file
void saveBMP(Image * img, char * fileName){
    
    // Open the output file
    FILE *outputFile;
    outputFile = fopen(fileName, "wb");
    if (outputFile == NULL) {
        printf("It was not possible to save the file: %s\n", fileName);
        exit(-1);
    }

    // Calculate the image size in bytes
    int imageSize = img->height * img->width;

    // Declare variables for the BMP file header
    unsigned char bmpFileHeader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpInfoHeader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmpPad[3] = {0,0,0};

    // Write the BMP file header to the output file
    fwrite(bmpFileHeader, 1, 14, outputFile);

    // Write the BMP info header to the output file
    fwrite(bmpInfoHeader, 1, 40, outputFile);

    // Write the pixel data to the output file
    fwrite(img->information, 1, imageSize, outputFile);

    // Calculate the number of padding bytes needed
    int padding = (4 - (imageSize) % 4) & 3;

    // Write the padding bytes to the output file
    for (int i = 0; i < padding; i++) {
        fwrite(bmpPad, 1, 1, outputFile);
    }

    // Close the output file
    fclose(outputFile);
}

           
