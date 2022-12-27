#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <iostream>
#include <string.h>

using namespace std;

// Define a struct to represent an image
typedef struct img{
    int height; // height of the image
    int width; // width of the image
    short bitCount; // number of bits per pixel
    unsigned char *information; // image data
}Image;

// Calculates the black and white value for each pixel in the image
void calculateBnWValue(Image * img);

// Loads a BMP image from a file
void loadBMP(Image * img, char * fileName);

// Saves a BMP image to a file
void saveBMP(Image * img, char * fileName);

int main(int argc, char* argv[]){

    // Allocate memory for an image struct
    Image * img = (Image *)malloc(sizeof(Image));

    // Initialize an empty string to store the file name
    char fileName[256] = "";

    // Concatenate the file name with the command line argument
    strcat(fileName, argv[1]);

    // Load the image from the file
    loadBMP(img, fileName);

    // Calculate the black and white values for the image
    calculateBnWValue(img);

    // Initialize an empty string to store the output file name
    char fileName2[256] = "/home/juan/Desktop/output.bmp";

    // Save the image to the output file
    saveBMP(img, fileName2);

    return 0;
}

// Calculates the black and white value for each pixel in the image
void calculateBnWValue(Image * img){
    
    // Calculate the number of iterations needed to process all pixels in the image
    int loops = img->bitCount == 32? (img->height * img->width) / 4 : (img->height * img->width) / 3;

    // Declare variables to store the red, green, and blue values for each pixel
    int red = 0, green = 0, blue = 0, average = 0;

    // Declare a variable to store the luminance value for each pixel
    double luminance;

    // Declare a pointer to the pixel data in the image
    unsigned char *pix = img->information;

    // Iterate through all pixels in the image
    for(int i = 0; i < loops;i++){
        // Store the blue value for the current pixel
        blue = (int)*pix;
        pix++;

        // Store the green value for the current pixel
        green = (int)*pix;
        pix++;

        // Store the red value for the current pixel
        red = (int)*pix;

        // Calculate the luminance value for the current pixel
        luminance = (red + green + blue) / 3;

        // Calculate the average value for the current pixel
        average = (int)round(luminance);

        // Set the pixel value to white if the average is above 80, otherwise set it to black
        average = average > 80? 255 : 0;

        // Move the pointer back two steps to the beginning of the current pixel
        pix = pix - 2;

        // Set the blue value for the current pixel to the average value
        *pix = (unsigned char)average;
        pix++;

        // Set the green value for the current pixel to the average value
        *pix = (unsigned char)average;
        pix++;

        // Set the red value for the current pixel to the average value
        *pix = (unsigned char)average;
        pix++;
		
        // If the image has a 32-bit color depth, move the pointer one step forward
        if(img->bitCount == 32){
            pix++;
		}
    }
}

// Loads a BMP image from a file
void loadBMP(Image * img, char * fileName){
    
    // Open the BMP file
    FILE *bitmapFile;
    bitmapFile = fopen(fileName, "rb");
    if (bitmapFile == NULL) {
        printf("It was not possible to load the file: %s\n", fileName);
        exit(-1);
    }

    // Read the "Bitmap Data Offset" field from the BMP file
    fseek(bitmapFile, 10, SEEK_SET);
    fread(&bmpDataOffset, sizeof(int), 1, bitmapFile);

    // Read the "height" field from the BMP file
    fseek(bitmapFile, 18, SEEK_SET);
    fread(&bmpWidth, sizeof(int), 1, bitmapFile);	

    // Read the "width" field from the BMP file
    fseek(bitmapFile, 22, SEEK_SET);
    fread(&bmpHeight, sizeof(int), 1, bitmapFile);

    // Read the "bit count" field from the BMP file
    fseek(bitmapFile, 28, SEEK_SET);
    fread(&bmpBitCount, sizeof(short), 1, bitmapFile);

    // Calculate the width of the image in bytes
    bmpWidth = bmpBitCount == 32? bmpWidth * 4 : bmpWidth * 3;

    // Calculate the residual bits in the image
    residue = (4 - (bmpWidth) % 4) & 3;

    // Store the width, height, and bit count in the image struct
    img->width = bmpWidth;
    img->height = bmpHeight;
    img->bitCount = bmpBitCount;

    // Allocate memory to store the pixel data in the image struct
    img->information = (unsigned char *)malloc(bmpWidth * bmpHeight * sizeof(unsigned char));

    // Set the file pointer to the beginning of the pixel data
    fseek(bitmapFile, bmpDataOffset, SEEK_SET);

    // Read the pixel data from the BMP file
    fread(img->information, sizeof(unsigned char), bmpWidth * bmpHeight, bitmapFile);

    // Close the BMP file
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

