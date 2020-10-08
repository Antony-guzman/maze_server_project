/*
 *  avatar.h --- header file for avatar.c 
 *  
 *  Maze Challenge Project
 * 
 * 
 *  Author: Antony, BASIX
 *  Date created: March 220
 * 
 */

/*======================================================================*/

#ifndef __AVATAR_H
#define __AVATAR_H

#include <stdio.h>

#include "maze.h"
#include "amazing.h"
#include "set.h"

typedef struct pair pair_t;
typedef struct avatarBio avatarBio;

void writeDirection(char *filename, int ID,int d);

bool updateLog(char *filename,int ID,int nAvatars, XYPos *pos);
void foundWall(char *filename, int ID);
void borderWalls(set_t *set, int MazeWidth, int Mazeheight);

avatarBio* createAvatar(int avID,int MazeWidth,int MazeHeight,XYPos *prev, XYPos *curr, XYPos *target, int Mazeport,
                        char *filename,int nAvatars, char *ip, set_t *set, int *d);

void* avatar_thread(void* ptr);
void freeAvatar(avatarBio *aB);

#endif //__AVATAR_H
