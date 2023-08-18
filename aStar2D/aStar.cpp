#include "aStar.h"

bool verbose = true;

int main(int argc, char const *argv[])
{
	// Read Input image

	int width, height, channel;
	unsigned char *targetData;

	char filename[50];

	sprintf(filename,"00003.jpg\n");

	readImage(&targetData, &width, &height, &channel, filename);

	if(verbose == true){
		printf("Target Image = %s\n", filename);
		printf("Width = %d, Height = %d, Channels = %d", width, height, channel);
	}

	// Can't currently read RGB, return error if image is not grayscale

	if(channel != 1){
		printf("Wrong number of channels. Input must be grayscale. Terminating...\n");
		free(targetData);
		std::exit(0);
	}

	// 

	return 0;

}