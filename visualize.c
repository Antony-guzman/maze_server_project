/* File: visualize.c - Maze visualization and data handling 
 *
 * Author: BASIX Group
 * Date: Mar 2020
 *
 * The 'visualize.c' module holds helper functions for visualization features, to be used in the 
 * 'AMStartup.c' file. More information on 'visualize.c' can be found in 'visualize.h'
 *
*/

#include <sys/socket.h>		  //socket funtions
#include <stdlib.h>           //exit, atoi
#include <stdio.h>            //printf
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <getopt.h>           //getopt_long, struct option
#include <string.h>           //strlen
#include <ctype.h>            //isdigit
#include <errno.h>            // get ip address
#include <netdb.h>            // get ip address
#include <unistd.h>           // close()
#include <time.h>
#include <pthread.h>            //pthread_create
#include <dirent.h>
#include <math.h>				// help with math functions
#include <pthread.h>

#include "avatar.h"
#include "amazing.h"

#include "set.h"
#include "visualize.h"

#define INT_MAX 800

/********* global variables **************/

int nColumns = -1;
int nRows = -1;

XYPos *(columnWalls[INT_MAX]);
XYPos *(rowWalls[INT_MAX]);


/************* global functions ***************/


/****************** init_walls() *******************/
void init_walls()
{
    for(int i = 0; i< INT_MAX;i++){
		columnWalls[i] = (XYPos *) malloc(sizeof(XYPos));
		columnWalls[i]->x = -1;
		columnWalls[i]->y = -1;
		rowWalls[i] = (XYPos *) malloc(sizeof(XYPos));
		rowWalls[i]->x = -1;
		rowWalls[i]->y = -1;
	}


}

/****************** freeWallsColumns() *******************/
void freeWallsColumns()
{
	for(int i =0; i<INT_MAX;i++){
		free(columnWalls[i]);
	}
	for(int i =0; i<INT_MAX;i++){
		free(rowWalls[i]);
	}

    // if (columnWalls != NULL){
    //     free(columnWalls);
    // }
    // if (rowWalls != NULL){
    //     free(rowWalls);
    // }

}

/****************** printMaze() *******************/
/*
 * For more information on printMaze(), please see the 'DESIGN.md' document for more information
 */
void printMaze(int mazeWidth, int mazeHeight, int mazeScale, XYPos *pos, int nAvatars) 
{
int i, j;
	
	// create variables, scale appropriately
	int newMazeHeight = mazeHeight * mazeScale;
	int newMazeWidth = mazeWidth * mazeScale;
	int midPoint = maze_isOdd(mazeScale);
	
	// variable for list of XYPos pointers
	XYPos *avList = pos;
	
	// begin maze update to be printed
	fprintf(stdout, "*************************************************\n");
	fprintf(stdout, "*************************************************\n");
	fprintf(stdout, "\n");
	
	// for each row
	for(i = 0; i <= newMazeHeight; i++)
	{
			
		// for each column
		for(j = 0; j <= newMazeWidth; j++)
		{
			// NOTE: Each print is cushioned with a space for cleaner visualization 
			
			// If its at the end or beginning of rows, cap with an asterisk
			if(i==0 || i == newMazeHeight)
			{
				// helps visually with geographic symmetry
				if((j % mazeScale) == 0)
				{
					fprintf(stdout, " +");
				} else {
				fprintf(stdout, " -");
				}
			}
			// same goes for columns
			else if(j==0 || j == newMazeWidth)
			{
				// if along the top row
				if((i % mazeScale) == 0)
				{
					// if top left corner
					if(j == 0)
					{
						fprintf(stdout, " + ");
					} 
					//if top right corner
					else if(j == newMazeWidth)
					{
						fprintf(stdout, "+");
					}
					// otherwise...
					else {
						fprintf(stdout, " +");
					}
				} 
				// if not along top row...
				else {
					// if on left most column
					if(j == 0)
					{
					fprintf(stdout,  " | ");
					} 
					// if right most column
					else if(j == newMazeWidth)
					{
						fprintf(stdout, "|");
					} 
					// otherwise
					else {
						fprintf(stdout, " |");
					}
				}
			}
			// if divisible by mazeScale vertically with no remainder
			else if((i % mazeScale) == 0)
			{
				// create boolean for notifying print
				bool itemPrinted = false;
				
				// If at a 'wall joining'...
				if((j % mazeScale) == 0){
				// represents almost a vertice in the graph
				fprintf(stdout, "+");
				itemPrinted = true;
				}
				
				// If we've printed...
				if(itemPrinted)
				{
					// only print one space
					fprintf(stdout, " ");
				} 
				// otherwise, check for row walls
				else {
				int k = 0;
				int x;
				int y;
				
				// iterate through all row walls to test if they are located in our
				// given position, if printed set boolean
				if (nRows >= 0){
					for(k = 0; k < nRows; k++)
					{
						XYPos *currWall = rowWalls[k];
						x = currWall->x;
						y = currWall->y + 1;
						
						if(y == floor(i / mazeScale) && x == floor(j / mazeScale))
						{
							if(((i - 1) % mazeScale) == 0)
							{
								fprintf(stdout, " -");
							}
							fprintf(stdout, "-");
							itemPrinted = true;
						}
					}
				}
				// if nothing printed
				if(!itemPrinted)
				{
					// print two spaces
					fprintf(stdout, "  ");
				} 
				// otherwise none
				else {
					fprintf(stdout, " ");
				}
				}
				
			} 
			// if divisible by mazeScale horizontally
			else if((j % mazeScale) == 0)
			{
				// same as done above but now for vertical walls
				int k = 0;
				float x;
				float y;
				bool wallPrinted = false;
				// see previous loop above...
				if (nColumns >= 0){
					for(k = 0; k < nColumns; k++)
					{
						XYPos *currWall = columnWalls[k];
						x = currWall->x + 1;
						y = currWall->y;	
						
						if(x == floor(j / mazeScale) && y == floor(i / mazeScale))
						{
							fprintf(stdout, "|");
							wallPrinted = true;
						}
					}
				}
				// if no walls were printed
				if(!wallPrinted)
				{
					// print two walls
					fprintf(stdout, "  ");
				} 
				// otherwise, print one wall
				else {
					fprintf(stdout, " ");
				}
			}
			
			// if the value is in the 'middle' of a cell (potential avatar position)
			else if((i % mazeScale) == midPoint && (j % mazeScale) == midPoint)
			{
				
				// create variables and allocate memory	
				int k = 0;
				int x;
				int y;
				int avs_in_pos = 0;
				int last_seen;

				
				// to check whether an avatar was printed in a given cell
				bool avPrinted = false;

				for(k = 0; k < nAvatars; k++)
				{
					// get avatar XYPos and set to vars
					XYPos currAv = avList[k];
					x = currAv.x;
					y = currAv.y;
					
					// If the current i and j position matches an avatar XY...
					if(y == floor(i / mazeScale) && x == floor(j / mazeScale))
					{
						// print the number out and let us know we printed
						avs_in_pos++;
						last_seen = k;
						avPrinted = true;
					}
				}
				// If a print occurred...
				if(avPrinted)
				{
					// if all avatars are in the same position...
					if(avs_in_pos == nAvatars)
					{
						// print the final case solution
						fprintf(stdout, "$");
					} else {
						// otherwise print the last seen avatar in the given position
						fprintf(stdout, "%d", last_seen);
					}
	
					// print one space
					fprintf(stdout, " ");
				} else {
					// else print two spaces
					fprintf(stdout, "  ");
				}
			} 
			else {
				// Otherwise, nothing to print...
				fprintf(stdout, "  ");
			}
		}
		// new line
		fprintf(stdout, "\n");
	}
	
	// End of maze update demarcation
	fprintf(stdout, "*************************************************\n");
	fprintf(stdout, "*************************************************\n");
}


/********************* maze_isOdd() **************************/
int maze_isOdd(int mazeScale)
{
	// check scale size
	if(mazeScale < 3)
	{
		// if less than 3...
		fprintf(stdout, "Maze Error: Scale too small\n");
		return -1;
	}
	// if its odd
	else if((mazeScale % 2) == 1)
	{
		int midPoint = floor(mazeScale / 2) + 1;
		return midPoint;
	} else {
		int midPoint = floor(mazeScale / 2);
		return midPoint;
	}
}

/****************** getColumnWalls() *******************/
XYPos** getColumnWalls() {
	return columnWalls;
}

/****************** getRowWalls() *******************/
XYPos** getRowWalls() {
	return rowWalls;
}

/****************** column_increment() *******************/
void column_increment() {
	nColumns++;
}

/****************** row_increment() *******************/
void row_increment() {
	nRows++;
}

/****************** calculateWalls() *******************/
void calculateWalls(char *start,char *end)
{
	// check that pointers aren't null
	if (start != NULL && end != NULL){
	
	// allocate memory and copy
    char *start1 = malloc(sizeof(char*)*strlen(start+1));
    strcpy(start1,start);
    
    // create variables
    int startx;
    int starty;
    
    // create string and free
    sscanf(start1, "%d,%d",&startx,&starty);
    free(start1);
    
    // allocate memory for end and copy, same as above for start
    char *end1 = malloc(sizeof(char*)*strlen(end+ 1));
    strcpy(end1,end);
    int endx;
    int endy;
    sscanf(end1, "%d,%d",&endx,&endy);
    free(end1);

 
    XYPos column;
    XYPos row;


	// if the end x is greater than the start x...
    if (endx >startx){

        //give start x and start y to NCOLUMS
        column.x = startx;
        column.y = starty;
		
		// check if column exists
		bool check = checkC(column);
		// if not...
		if (!check){
			// increment columns and add to global column list
			nColumns++;
        	columnWalls[nColumns]->x = column.x;
			columnWalls[nColumns]->y = column.y;
		}

	}
	// if end y is greater than start y...
    else if(endy>starty){

        //give startx and starty to nrows
        row.x = startx;
        row.y = starty;
        // check if the row already exists
		bool check = checkR(row);
		// if not...
		if (!check){
		// increment rows, and add to global row list
        nRows++;
        rowWalls[nRows]->x = row.x;
		rowWalls[nRows]->y = row.y;
	}


	// if startx is greater than endx...
    }
    else if(endx <startx){

		// give endx and endy to XYPos
        column.x = endx;
        column.y = endy;
		// check for wall existence
        bool check = checkC(column);
        // if does not exist...
		if (!check){
			// increment columns and add to global column list
			nColumns++;
        	columnWalls[nColumns]->x = column.x;
			columnWalls[nColumns]->y = column.y;
		}

    }
    // if starty is greater than endy...
    else if(endy<starty){
		// get x and y variables
        row.x = endx;
        row.y = endy;
		// check to see if the row exists already
       	bool check = checkR(row);
       	// if it doesn't exist
		if (!check){
			// increment row walls and add to global row list
			nRows++;
       		rowWalls[nRows]->x = row.x;
			rowWalls[nRows]->y = row.y;
		}
    }
}

    
}

/****************** checkC() *******************/
bool checkC(XYPos column)
{
	if (nColumns != -1){
		for(int i= 0;i<INT_MAX;i++){;

			if((columnWalls[i]->x == column.x) && (columnWalls[i]->y == column.y)){

				return true;
			}
		}
	}
	return false;

}

/****************** checkR() *******************/
bool checkR(XYPos row)
{
	if (nRows != -1){
		for(int i= 0;i<INT_MAX;i++){

			if((rowWalls[i]->x == row.x) && (rowWalls[i]->y == row.y)){

				return true;
			}
		}
	}
	return false;

}
