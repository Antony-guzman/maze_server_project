/*
 * 'maze.h' - contains details for functions and data structures
 * found within the 'maze.c' module
 *
 *
 * Author: Sean Keough, Winter 2020
 * Maze Project Group: BASIX
 *
 */
 

#ifndef __MAZE_H
#define __MAZE_H

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
#include "amazing.h"
#include "set.h"

/************* internal structures ****************/
typedef struct path path_t;

/**************** path_new ****************/
/* Create a new (empty) path.
 *
 * We return:
 *   pointer to a new path, or NULL if error.
 * We guarantee:
 *   The path is initialized with empty start
 *   and end strings.
 * Caller is responsible for:
 *   later calling 'deletePath()'.
 */
path_t *path_new(char *a,char *b);


/**************** delete_path ****************/
/* Delete the path.
 *
 * Caller provides:
 *   a valid path pointer.
 * We guarantee:
 *   we delete start and end strings in path.
 *   we ignore NULL path.
 */
void deletePath(path_t *wall);


/**************** local functions ********************/

// /**************** pos_to_string ****************/
/* converts XYPos item @pos into string and stores
 * in cstring @a
 *
 * @pos: an XYPos type structure
 * @a: a cstring
 *
 * Caller provides:
 *   1. a valid XYPos pointer
 *   2. a valid cstring pointer
 * We guarantee:
 *   cstring will store values of XYPos in the form:
 *   "x,y"
 * Caller is responsible for:
 *   later freeing the string
 * NOTE: it is highly recommended the user provides
 * a char array rather than a char pointer, for much
 * cleaner memory handling, as they do not have to 
 * free the array's stack-alloced memory.
 */
void posToString(XYPos *pos,char *a);


// /******************** build_wall ********************/
/* inserts a new wall into a set of walls
 * 
 * @walls: the set which stores all known walls
 * @wall: the path which defines the new wall
 *
 * Caller provides:
 *   1. a valid pointer to the set of walls
 *   2. a valid pointer to the path of the wall
 * We guarantee:
 *   if the wall does not already exist . . .
 *     the set will insert the wall from both sides
 *     of the wall:
 *        1. wall.start -> wall.end
 *        2. wall.end -> wall.start
 *     no insertion will occur if set already 
 *     contains the wall
 *     the wall will then be sent to the calculateWalls()
 *     method (see visualize.h) to be added to the
 *     list of rows and columns of the maze for printing
 *     the updated maze
 *   if the wall does already exist, nothing happens 
 * Caller is responsible for:
 *   later freeing the set and path
 */
void build_wall(set_t *walls, path_t *wall);


// /******************* dead_end ********************/
/* determines if this avatar's current XYPos 
 * is surrounded by three walls (a dead end), 
 * and returns the way out as an integer 
 * representing a direction
 * XYPos to the way out
 *
 * walls off the dead end, if no other avatar's
 * are currently in the dead end
 * 
 * 
 * @walls: a set of walls
 * @curr: this avatar's current XY position
 * @Pos: XYPos of all the avatars
 * @nAvatars: total number of avatars
 * @ID: current avatar's ID
 *
 * Caller provides:
 *   1. a valid set pointer
 *   2. this avatar's curr XYPos 
 *   3. list of current XYPos for all avatars
 *   4. total number of avatars
 *   5. this avatar's ID
 * We guarantee:
 *   1. the exit direction for the avatar, if it
 *      is found to be at a dead end
 *   2. to wall off the dead end, if no other avatars
 *      are currently at this avatar's current position
 */
 int deadEnd(set_t *walls, XYPos *curr, XYPos *Pos, int nAvatars,int ID);

// /******************* find_next_move ********************/
/* finds the next viable direction for this avatar to move,
 * according to the left-hand rule
 * 
 * @prev: this avatar's previous XY position
 * @curr: this avatar's current XY position
 * @target: this avatar's target XY position
 * @walls: the set which stores all known walls
 * @dir: this avatar's current direction
 *
 * Caller provides:
 *   1. this avatar's prev XYPos 
 *   2. this avatar's curr XYPos 
 *   3. this avatar's target XYPos 
 *   4. a valid pointer to the set of walls
 *   5. the avatar's current direction integer
 * We guarantee:
 *   1. to return the next direction the avatar should move
 *   2. to update the avatar's target XYPos according to this
 *      new direction
 */
int find_nextMove(XYPos *prev, XYPos *curr, XYPos *target, set_t *walls, int *dir);



#endif // __MAZE_H
