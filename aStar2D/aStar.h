#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<vector>
#include<iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include<stdbool.h>
#include<fstream>
#include<cfloat>
#include <set>

#define ROW 128
#define COL 128 

// generate structure to store global information about the domain

typedef struct{
	unsigned int xSize;
	unsigned int ySize;
	bool verbose;
}domainInfo;

// node struct will hold information on cell parent cells, f, g, and h.

typedef struct{
	int parentRow, parentCol;
	float f,g,h;
}node;

// define pair for coords

typedef std::pair<int, int> coordPair;

// define pair <double, pair<i,j>> for open list

typedef std::pair<float, std::pair<int,int> > OpenListInfo;



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

void tracePath(domainInfo info, node nodeInfo[ROW][COL], int* path){
	/*
		tracePath Function:
		Inputs:
			- node information data structure
			- path array to store the path (if one exists)
		Outputs:
			- none

		Function will trace a path found and store the coordinates of those points on the path array.
	*/

	int col = info.xSize - 1;
	int row;
	int count = 0;

	for(int i = 0; i<info.ySize; i++){
		if(nodeInfo[i][col].parentCol != -1 && nodeInfo[i][col].parentRow != -1){
			row = i;
			break;
		}
	}

	// Path array stores on row-col format

	path[0] = row;
	path[1] = col;
	count++;

	if(info.verbose == true){
		printf("Tracing Path\n");
		printf("Row = %d, col = %d\n", nodeInfo[row][col].parentRow, nodeInfo[row][col].parentCol);
	}

	while(col != 0){
		int tempRow = nodeInfo[row][col].parentRow;
		int tempCol = nodeInfo[row][col].parentCol;
		row = tempRow;
		col = tempCol;
		path[count*2 + 0] = row;
		path[count*2 + 1] = col;
		printf("-> (%d,%d)",row, col);
		count++;
	}

	return;
}

int aStarMain(unsigned int* GRID, domainInfo info, int* path){
	/*
		aStarMain Function
		Inputs:
			- unsigned int GRID: grid, at each location either a 1 or a 0.
				1 means solid, 0 void. Those are the boundary conditions.
			- domainInfo info: data structure with info regarding height and width of the domain
		Output:
			- either a one or a zero. One means there is a path, zero means there isn't.
	*/

	// Initialize both lists, open and closed as arrays

	bool* closedList = (bool *)malloc(sizeof(bool)*info.xSize*info.ySize);

	memset(closedList, false, sizeof(closedList));

	// Declare 2D array of structure type "node"
	// Node contains information such as parent coordinates, g, h, and f

	node nodeInfo[ROW][COL];

	// Initialize all paremeters

	for(int i = 0; i<info.ySize; i++){
		for(int j = 0; j<info.xSize; j++){
			nodeInfo[i][j].f = FLT_MAX;
			nodeInfo[i][j].g = FLT_MAX;
			nodeInfo[i][j].h = FLT_MAX;
			nodeInfo[i][j].parentCol = -1;
			nodeInfo[i][j].parentRow = -1;
		}
	}

	// Initialize parameters for all starting nodes

	for(int i = 0; i<info.ySize; i++){
		if(GRID[i*info.xSize + 0] == 0){
			nodeInfo[i][0].f = 0.0;
			nodeInfo[i][0].g = 0.0;
			nodeInfo[i][0].h = 0.0;
			nodeInfo[i][0].parentCol = 0;
			nodeInfo[i][0].parentRow = i;
		}
	}

	// Create open list

	std::set<OpenListInfo> openList;

	// Insert all starting nodes into the open list

	for(int i = 0; i<info.ySize; i++){
		openList.insert(std::make_pair(0.0, std::make_pair(i,0)));
	}

	// set destination flag to false

	bool foundDest = false;

	// begin loop to find path. If openList is empty, terminate the loop

	while(!openList.empty()){
		// First step is to pop the fist entry on the list
		OpenListInfo pop = *openList.begin();

		// remove from open list
		openList.erase(openList.begin());

		// Add to the closed list
		int row = pop.second.first; // first argument of the second pair
		int col = pop.second.second; // second argument of second pair
		closedList[row*info.xSize + col] = true;

		/*
			Now we need to generate all 4 successors from the popped cell.
			The successors are north, south, east, and west.
			
			North index = i - 1, j
			South index = i + 1, j
			East index =  i    , j + 1
			West index =  i    , j - 1
		*/
		float gNew, hNew, fNew;

		// Evaluate North
		
		int tempCol = col;
		int tempRow = row;

		// adjust North for periodic boundary condition

		if(row == 0){
			tempRow = info.ySize - 1;
		} else{
			tempRow = row - 1;
		}

		// check if we reached destination, which is the entire right boundary
		if(tempCol == info.ySize - 1 && GRID[tempRow*info.xSize + tempCol] != 1){
			nodeInfo[tempRow][tempCol].parentRow = row;
			nodeInfo[tempRow][tempCol].parentCol = col;
			if(info.verbose == true){
				printf("The destination cell was found.\n");
			}
			// Call trace path function
			tracePath(info, nodeInfo, path);
			foundDest = true;
			return foundDest;
		} else if(closedList[tempRow*info.xSize + tempCol] == false && GRID[tempRow*info.xSize + tempCol] == 0) // check if successor is not on closed list and not a solid wall
		{
			gNew = nodeInfo[row][col].g + 1.0;	// cost from moving from last cell to this cell
			hNew = (info.xSize - 1) - tempCol; // Since entire right boundary is the distance, h is just a count of the number of columns from the right.	
			fNew = gNew + hNew;					// total cost is just h+g
			// Check if on open list. If yes, update f,g, and h accordingly.
			// If not, add it to open list.
			if(nodeInfo[tempRow][tempCol].f == FLT_MAX || nodeInfo[tempRow][tempCol].f > fNew){
				openList.insert(std::make_pair(fNew, std::make_pair(tempRow, tempCol)));
				nodeInfo[tempRow][tempCol].f = fNew;
				nodeInfo[tempRow][tempCol].g = gNew;
				nodeInfo[tempRow][tempCol].h = hNew;
				nodeInfo[tempRow][tempCol].parentRow = row;
				nodeInfo[tempRow][tempCol].parentCol = col;
			}
		}
			

		// Evaluate South

		tempCol = col;
		tempRow = row;

		// Adjust for periodic BC

		if(row == info.ySize - 1){
			tempRow = 0;
		} else{
			tempRow = row + 1;
		}

		// check if we reached destination, which is the entire right boundary
		if(tempCol == info.ySize - 1 && GRID[tempRow*info.xSize + tempCol] != 1){
			nodeInfo[tempRow][tempCol].parentRow = row;
			nodeInfo[tempRow][tempCol].parentCol = col;
			if(info.verbose == true){
				printf("The destination cell was found.\n");
			}
			// Call trace path function
			tracePath(info, nodeInfo, path);
			foundDest = true;
			return foundDest;
		} else if(closedList[tempRow*info.xSize + tempCol] == false && GRID[tempRow*info.xSize + tempCol] == 0) // check if successor is not on closed list and not a solid wall
		{
			gNew = nodeInfo[row][col].g + 1.0;	// cost from moving from last cell to this cell
			hNew = (info.xSize - 1) - tempCol; // Since entire right boundary is the distance, h is just a count of the number of columns from the right.	
			fNew = gNew + hNew;					// total cost is just h+g
			// Check if on open list. If yes, update f,g, and h accordingly.
			// If not, add it to open list.
			if(nodeInfo[tempRow][tempCol].f == FLT_MAX || nodeInfo[tempRow][tempCol].f > fNew){
				openList.insert(std::make_pair(fNew, std::make_pair(tempRow, tempCol)));
				nodeInfo[tempRow][tempCol].f = fNew;
				nodeInfo[tempRow][tempCol].g = gNew;
				nodeInfo[tempRow][tempCol].h = hNew;
				nodeInfo[tempRow][tempCol].parentRow = row;
				nodeInfo[tempRow][tempCol].parentCol = col;
			}
		}

		// Evaluate East (if it exists)

		if(col != info.xSize - 1){
			tempRow = row;
			tempCol = col + 1;

			// check if we reached destination, which is the entire right boundary
			if(tempCol == info.ySize - 1 && GRID[tempRow*info.xSize + tempCol] != 1){
				nodeInfo[tempRow][tempCol].parentRow = row;
				nodeInfo[tempRow][tempCol].parentCol = col;
				if(info.verbose == true){
					printf("The destination cell was found.\n");
				}
				// Call trace path function
				tracePath(info, nodeInfo, path);
				foundDest = true;
				return foundDest;
			} else if(closedList[tempRow*info.xSize + tempCol] == false && GRID[tempRow*info.xSize + tempCol] == 0) // check if successor is not on closed list and not a solid wall
			{
				gNew = nodeInfo[row][col].g + 1.0;	// cost from moving from last cell to this cell
				hNew = (info.xSize - 1) - tempCol; // Since entire right boundary is the distance, h is just a count of the number of columns from the right.	
				fNew = gNew + hNew;					// total cost is just h+g
				// Check if on open list. If yes, update f,g, and h accordingly.
				// If not, add it to open list.
				if(nodeInfo[tempRow][tempCol].f == FLT_MAX || nodeInfo[tempRow][tempCol].f > fNew){
					openList.insert(std::make_pair(fNew, std::make_pair(tempRow, tempCol)));
					nodeInfo[tempRow][tempCol].f = fNew;
					nodeInfo[tempRow][tempCol].g = gNew;
					nodeInfo[tempRow][tempCol].h = hNew;
					nodeInfo[tempRow][tempCol].parentRow = row;
					nodeInfo[tempRow][tempCol].parentCol = col;
				}
			}
		}

		// Evaluate West

		if(col != 0){
			tempRow = row;
			tempCol = col;

			// check if we reached destination, which is the entire right boundary
			if(tempCol == info.ySize - 1 && GRID[tempRow*info.xSize + tempCol] != 1){
				nodeInfo[tempRow][tempCol].parentRow = row;
				nodeInfo[tempRow][tempCol].parentCol = col;
				if(info.verbose == true){
					printf("The destination cell was found.\n");
				}
				// Call trace path function
				tracePath(info,nodeInfo, path);
				foundDest = true;
				return foundDest;
			} else if(closedList[tempRow*info.xSize + tempCol] == false && GRID[tempRow*info.xSize + tempCol] == 0) // check if successor is not on closed list and not a solid wall
			{
				gNew = nodeInfo[row][col].g + 1.0;	// cost from moving from last cell to this cell
				hNew = (info.xSize - 1) - tempCol; // Since entire right boundary is the distance, h is just a count of the number of columns from the right.	
				fNew = gNew + hNew;					// total cost is just h+g
				// Check if on open list. If yes, update f,g, and h accordingly.
				// If not, add it to open list.
				if(nodeInfo[tempRow][tempCol].f == FLT_MAX || nodeInfo[tempRow][tempCol].f > fNew){
					openList.insert(std::make_pair(fNew, std::make_pair(tempRow, tempCol)));
					nodeInfo[tempRow][tempCol].f = fNew;
					nodeInfo[tempRow][tempCol].g = gNew;
					nodeInfo[tempRow][tempCol].h = hNew;
					nodeInfo[tempRow][tempCol].parentRow = row;
					nodeInfo[tempRow][tempCol].parentCol = col;
				}
			}
		}
	}
	if(info.verbose == true){
		printf("Failed to find a path.\n");
	}
	return foundDest;	

}