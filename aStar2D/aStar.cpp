#include "aStar.h"

bool verbose = true;
bool saveImg = true;

int main(int argc, char const *argv[])
{
	// Read Input image

	char filename[100];

	sprintf(filename,"00009.jpg");

	int width, height, channel;
	bool status = false;

	unsigned char *targetData;

	readImage(&targetData, &width, &height, &channel, filename);

	if(verbose == true){
		printf("Target Image = %s\n", filename);
		printf("Width = %d, Height = %d, Channels = %d\n", width, height, channel);
	}

	// Can't currently read RGB, return error if image is not grayscale

	if(channel != 1){
		printf("Wrong number of channels. Input must be grayscale. Terminating...\n");
		free(targetData);
		std::exit(0);
	}

	// Declare Domain info data structure

	domainInfo info;

	info.xSize = width;
	info.ySize = height;
	info.verbose = verbose;

	// Declare domain boundaries

	unsigned int *Grid = (unsigned int*)malloc(sizeof(unsigned int)*info.xSize*info.ySize);

	for(int i = 0; i<info.ySize; i++){
		for(int j = 0; j<info.xSize; j++){
			if(targetData[i*info.xSize + j] > 150){
				Grid[i*info.xSize + j] = 1;
			} else{
				Grid[i*info.xSize + j] = 0;
			}
		}
	}

	// Free original image allocation if we don't need it anymore

	if(saveImg == false){
		free(targetData);
	}

	// Search

	int *path = (int*)malloc(sizeof(int)*info.xSize*info.ySize*2);

	memset(path, -1, sizeof(path));

	status = aStarMain(Grid, info, path);

	if(saveImg == true){
		for(int i = 0; i<info.xSize*info.ySize; i++){
			if(path[i*2 + 0] != -1 && path[i*2 + 1] != -1){
				int row = path[i*2 + 0];
				int col = path[i*2 + 1];
				targetData[row*info.xSize + col] = 127;
			} else{
				break;
			}
		}
		stbi_write_jpg("pathTest.jpg", info.xSize, info.ySize, 1, targetData, 100);
		free(targetData);
	}

	free(path);

	return 0;

}