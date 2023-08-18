#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<vector>
#include<iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include<stdbool.h>
#include<fstream>


int readImage(unsigned char** imageAddress, int* Width, int* Height, int* NumOfChannels, char* ImageName){
	/*
		readImage Function:
		Inputs:
			- imageAddress: unsigned char reference to the pointer in which the image will be read to.
			- Width: pointer to variable to store image width
			- Height: pointer to variable to store image height
			- NumofChannels: pointer to variable to store number of channels in the image.
					-> NumofChannels has to be 1, otherwise code is terminated. Please enter grayscale
						images with NumofChannels = 1.
		Outputs: None

		Function reads the image into the pointer to the array to store it.
	*/

	*imageAddress = stbi_load(ImageName, Width, Height, NumOfChannels, 1);

	return 0;
}