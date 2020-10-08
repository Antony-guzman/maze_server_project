/* File: maze.c - Maze visualization and data handling 
 *
 * Author: Sean, BASIX
 * Date: Mar 2020
 *
 * The 'maze.c' module holds helper functions and visualization features to be used in the 
 * 'AMStartup.c' file. More information on 'maze.c' can be found in 'IMPLEMENTATION.md'
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
#define INT_MAX 500

/************************** internal structures ***************/
/*
 * a structure that stores the strings that mark the start and the end of a path.
 *
 */
typedef struct path{
  
  char*start;
  char*end;
  
}path_t;

/**************** path_new() ****************/
/* see maze.h for description */
path_t *path_new(char *a,char *b){
  
  path_t *path = malloc(sizeof(path_t));
  path->start=  malloc(sizeof(char*)*strlen(a+1));
  strcpy(path->start , a);
  path->end=  malloc(sizeof(char*)*strlen(b+1));
  strcpy(path->end , b);
  return path;
  
  
}

/**************** delete_path() ****************/
/* see maze.h for description */
void deletePath(path_t *wall){

  if (wall->start != NULL){
		free(wall->start);
	}
	if (wall->end){
		free(wall->end);
	}
	if (wall != NULL){
		free(wall);
	}
}

/**************** local functions ********************/

/**************** pos_to_string() ****************/
/* see maze.h for description */
void posToString(XYPos *pos, char *a)
{
  sprintf(a,"%d,%d", pos->x, pos->y);

}

/**************** build_wall() ****************/
/* see maze.h for description */
void build_wall(set_t *walls, path_t *wall){

        if (wall->start != NULL && wall->end != NULL){
	  set_insert(walls, wall->start, wall->end);
	  set_insert(walls, wall->end, wall->start);
	  calculateWalls(wall->start,wall->end);
	}

}

/**************** dead_end() ****************/
/* see maze.h for description */
int deadEnd(set_t *walls, XYPos *curr,XYPos *Pos,int nAvatars,int ID) 
{
	XYPos *temp = malloc(sizeof(XYPos));

	int way = -1;

	int options = 4;

	temp->x =  curr->x;
	temp->y = curr->y;

	bool inside = false;
	for(int index = 0;index <nAvatars;index++ ){
		if(Pos[index].x == curr->x && Pos[index].y == curr->y && index != ID){
			inside = true;
		}
		
	}
	
	// check if there is a wall east
	temp->x++;
	char start[10];
	char east[10];
	posToString(curr,start);
	posToString(temp,east);
	path_t *wallE = path_new(start,east);

	// there is a wall to the East reduce the number of paths leaving that cordinate. 
	if (set_find2(walls,wallE->start,wallE->end)){
		options--;
	}

	// else mark that as a viable path.
	else{
		way =3;

	}

	temp->x = curr->x;
	temp->y = curr->y;

	//check if wall south 
	temp->y++;
	char south[10];
	posToString(temp,south);
	path_t *wallS = path_new(start,south);

	// there is a wall to the South  reduce the number of paths leaving that cordinate. 
	if (set_find2(walls,wallS->start,wallS->end)){
		options--;
	}

	// else mark that as a viable path
	else{
		way = 2;

	}

	temp->x = curr->x;
	temp->y = curr->y;

	//check if wall west
	temp->x--;
	char west[10];
	posToString(temp,west);
	path_t *wallW = path_new(start,west);
	if (set_find2(walls,wallW->start,wallW->end)){
		options--;
	}
	else{
		way = 0;

	}

	temp->x = curr->x;
	temp->y = curr->y;

	//check if wall north 
	temp->y--;
	char north[10];
	posToString(temp,north);
	path_t *wallN = path_new(start,north);

	// there is a wall to the North reduce the number of paths leaving that cordinate. 
	if (set_find2(walls,wallN->start,wallN->end)){
		options--;
	}

	// else mark that as a viable path.
	else{
		way = 1;

	}

	// if you found three walls, you are enclosed in a tunnel
	if(options == 1){

		if (way == 3){
			if (!inside){
				build_wall(walls, wallE);
			}
			free(wallE->start);
			free(wallE->end);
			free(wallE);
			free(wallS->start);
			free(wallS->end);
			free(wallS);
			free(wallN->start);
			free(wallN->end);
			free(wallN);
			free(wallW->start);
			free(wallW->end);
			free(wallW);
			free(temp);
			return way;

		}
		if (way == 2){
			if (!inside){
				build_wall(walls,wallS);
			}
			free(wallE->start);
			free(wallE->end);
			free(wallE);
			free(wallS->start);
			free(wallS->end);
			free(wallS);
			free(wallN->start);
			free(wallN->end);
			free(wallN);
			free(wallW->start);
			free(wallW->end);
			free(wallW);
			free(temp);
			return way;

		}
		if (way == 0){
			if (!inside){
				build_wall(walls,wallW);
			}
			free(wallE->start);
			free(wallE->end);
			free(wallE);
			free(wallS->start);
			free(wallS->end);
			free(wallS);
			free(wallN->start);
			free(wallN->end);
			free(wallN);
			free(wallW->start);
			free(wallW->end);
			free(wallW);
			free(temp);
			return way;

		}
		if (way == 1){
		  if (!inside){
			build_wall(walls, wallN);
			}
		  free(wallE->start);
			free(wallE->end);
			free(wallE);
			free(wallS->start);
			free(wallS->end);
			free(wallS);
			free(wallN->start);
			free(wallN->end);
			free(wallN);
			free(wallW->start);
			free(wallW->end);
			free(wallW);
			free(temp);
			return way;
		}
	}
	free(wallE->start);
	free(wallE->end);
	free(wallE);
	free(wallS->start);
	free(wallS->end);
	free(wallS);
	free(wallN->start);
	free(wallN->end);
	free(wallN);
	free(wallW->start);
	free(wallW->end);
	free(wallW);
	free(temp);
	return -1;
}


/**************** find_next_move() ****************
 *
 * algorithm to determine the best next move for the avatar whose turn it is to move.
 * see maze.h for description 
 */
int find_nextMove(XYPos *prev, XYPos *curr, XYPos *target, set_t *walls, int *dir)
{	
	int direction[4] = {3,2,0,1}; //array of directions in clockwise order, ESWN
	if(target->x == -1){ ///initial condition when game starts? target == -1 ?
		target->x = curr->x;
		target->y = curr->y;
		target->x++; 
		*dir = 2;      //target the east
		printf("--------\n");
		return 3; //return east
	}

	char start[10];

	target->x = curr->x;
	target->y = curr->y;

	int index;
	for (int a =0; a<4;a++){
		if (direction[a]== *dir){
			index = a;
		}
	}
	
	target->x = curr->x;
	target->y = curr->y;

	int previous_direction;
	if (index == 0){
		previous_direction = direction[3];
	}
	else{
		previous_direction = direction[index -1];
	}
	if (previous_direction ==0){
		target->x--;
	}
	else if (previous_direction ==1){
		target->y--;
	}
	else if (previous_direction==2){
		target->y++;
	}
	else if (previous_direction == 3){
		target->x++;
	}	

	char c[10];
	posToString(curr,start);
	posToString(target,c);
	path_t *option1= path_new(start,c);
	if (!set_find2(walls,option1->start,option1->end)){
		free(option1->start);
		free(option1->end);
		free(option1);
		*dir = previous_direction;
		return previous_direction;
	}

	free(option1->start);
	free(option1->end);
	free(option1);
	target->x = curr->x;
	target->y = curr->y;
	
	int current_direction = direction[index];
	if (current_direction ==0){
		target->x--;
	}
	else if (current_direction ==1){
		target->y--;
	}
	else if (current_direction ==2){
		target->y++;
	}
	else if (current_direction == 3){
		target->x++;
	}

	char d[10];

	posToString(curr,start);
	posToString(target,d);
	path_t *option2= path_new(start,d);
	if (!set_find2(walls,option2->start,option2->end)){
		free(option2->start);
		free(option2->end);
		free(option2);
		*dir = current_direction;
		return current_direction;
	}
	free(option2->start);
	free(option2->end);
	free(option2);
	target->x = curr->x;
	target->y = curr->y;

	int next_d = ((index+1)%4);
	int next_direction = direction[next_d];


	if (next_direction ==0){
		target->x--;
	}
	else if (next_direction ==1){
		target->y--;
	}
	else if (next_direction ==2){
		target->y++;
	}
	else if (next_direction == 3){
		target->x++;
	}
	char e[10];
	posToString(curr,start);
	posToString(target,e);
	path_t *option3= path_new(start,e);

	if (!set_find2(walls,option3->start,option3->end))
	{
		free(option3->start);
		free(option3->end);
		free(option3);
		*dir = next_direction;
		return next_direction;
	}
	free(option3->start);
	free(option3->end);
	free(option3);
	return 0;
}

