/* ========================================================================== */
/* File: Avatar.c
 *
 * Author: Primary author Antony
 * Project name:  CS50 the Maze Challenge
 * Component name: Avatar
 *
 * Primary Author: Antony '22, Joshua' Davis '21
 * Date Created: Fri Feb 28 2020
 *
 *
 *
 * This file contains implementation for the Avatar object.
 *
 * Largely derived from Lect 24
 */
/* ========================================================================== */
#include <sys/socket.h>	       //socket funtions
#include <stdlib.h>           //exit, atoi
#include <stdio.h>            //printf
#include <string.h>
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
#include <pthread.h>

#include "set.h"
#include "avatar.h"
#include "amazing.h"
#include "maze.h"
#include "visualize.h"


#define MAXLEN 500

typedef struct pair{
  char *start;
  char *end;
}pair_t;

typedef struct avatarBio{
	int avID;
  int MazeWidth;
  int MazeHeight;
  XYPos* prev;
  XYPos* curr;
  XYPos* target;
  int MazePort;
	char filename[MAXLEN];
  int nAvatars;
  set_t *set;
  char ip[MAXLEN];
  int *d;
} avatarBio;


pthread_mutex_t exceededMoves;
pthread_mutex_t solved;
pthread_mutex_t error;
pthread_mutex_t turn;

int goalx = 0;
int goaly =0 ;






/*====================================================================*/
avatarBio* createAvatar(int avID,int MazeWidth,int MazeHeight,XYPos *prev, XYPos *curr, XYPos *target, int Mazeport,
                        char *filename,int nAvatars, char *ip, set_t *set, int *d)
{
  avatarBio *aB = malloc(sizeof(avatarBio));
  aB->avID = avID;
  aB->nAvatars = nAvatars;
  strcpy(aB->ip, ip);
  aB->MazePort = Mazeport;
  strcpy(aB->filename,filename);
  aB->set = set;
  aB->prev = prev;              
  aB->curr = curr;  
  aB->d = d;
  aB->target = target;           
  aB->MazeWidth = MazeWidth;    //used for maze printing
  aB->MazeHeight = MazeHeight;  //used for maze printing
  return aB;
}


/*
 *
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */
void* avatar_thread(void *ptr)
{
  //Create a socket for the client

  AM_Message *ret_value= malloc(sizeof(AM_Message));

  int sockfd;
  struct sockaddr_in servaddr;

  avatarBio *aB = (avatarBio *) ptr;

  // for (int i =0;i < aB->nAvatars;i++){
  //   printf("this is the avatar ID %d\n",Avatars[i]avID);
  // }

  sockfd = socket(AF_INET, SOCK_STREAM,0);

  if (sockfd <0){
	  perror("problem in creatin the socket");
	  exit(2);
  }

  //creation of the socket
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;

  servaddr.sin_addr.s_addr = inet_addr(aB->ip);
  servaddr.sin_port = htons(aB->MazePort);

  //connect
  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
	  perror("problem in connecting to server\n");
	  exit(2);
  }

  //send ready message for each avatar
  AM_Message readyMessg;
  readyMessg.type = htonl(AM_AVATAR_READY);
  readyMessg.avatar_ready.AvatarId = htonl(aB->avID);

  send(sockfd,&readyMessg, sizeof(AM_Message),0);

  sleep(0.10);



  

  


  //——————————— listen to server
  AM_Message replyMessg;
  while(1){
    int reply = recv(sockfd,&replyMessg, sizeof(AM_Message),0);
    if (reply == 0){
      fprintf(stderr,"Server termindated prematurely\n");
      close(sockfd);
      freeAvatar(aB);
      close(sockfd);
    return NULL;
    }
    sleep(0.50);


    if(ntohl(replyMessg.type)== AM_AVATAR_TURN){
      // pthread_mutex_lock(&turn);
      XYPos Pos[AM_MAX_AVATAR];

      
      for(int b =0; b <aB->nAvatars; b++){
        Pos[b].x = ntohl(replyMessg.avatar_turn.Pos[b].x);
        Pos[b].y = ntohl(replyMessg.avatar_turn.Pos[b].y);
      }
      //printMaze(aB->MazeWidth,aB->MazeHeight,5,Pos,aB->nAvatars);


        //current Avatar's position 
      aB->curr->x = Pos[aB->avID].x;
      aB->curr->y = Pos[aB->avID].y;


      // if (aB->avID != 0){
      //   printMaze(aB->MazeWidth,aB->MazeHeight,4,Pos,aB->nAvatars);
      // }

      //set goal XYPos (avatar 0's location)
      if(aB->avID == 0){
        goalx = (int) aB->curr->x;
        goaly = (int) aB->curr->y;
        AM_Message moveMessg;
        moveMessg.type = htonl(AM_AVATAR_MOVE);
        moveMessg.avatar_move.AvatarId = htonl(aB->avID);
        moveMessg.avatar_move.Direction = htonl(8);
        send(sockfd,&moveMessg, sizeof(AM_Message),0);

      }

      else if (ntohl(replyMessg.avatar_turn.TurnId)== aB->avID)
      { pthread_mutex_lock(&turn);

        bool update = updateLog(aB->filename,aB->avID, aB->nAvatars,Pos);
        

        if (!update){
          close(sockfd);
          exit(8);
        }

        //printMaze(aB->MazeWidth, aB->MazeHeight, 3, Pos, aB->nAvatars);

        // if Avatar # is at Avatar 0's location, stop moving
        if (aB->curr->x == goalx && aB->curr->y == goaly){
          AM_Message moveMessg;
          moveMessg.type = htonl(AM_AVATAR_MOVE);
          moveMessg.avatar_move.AvatarId = htonl(aB->avID);
          moveMessg.avatar_move.Direction = htonl(8);
          send(sockfd,&moveMessg, sizeof(AM_Message),0);
        }

        else {

          char start[10];
	        char end[10];
	        posToString(aB->curr,start);
	        posToString(aB->target,end);
          path_t *wall = path_new(start,end);

          // wall->end = posToString(target); //needs to be updated before calling find_next_move
          // //check prev and curr
          if (aB->prev->x == aB->curr ->x && aB->prev->y == aB->curr->y){
	    foundWall(aB->filename,aB->avID);
             sleep(0.20);
            build_wall(aB->set, wall);
            sleep(0.20);
           
            printMaze(aB->MazeWidth,aB->MazeHeight,3,Pos,aB->nAvatars);
          
            
          }
          else{
            printMaze(aB->MazeWidth,aB->MazeHeight,3,Pos,aB->nAvatars);
          }
           //printMaze(aB->MazeWidth,aB->MazeHeight,3,Pos,aB->nAvatars);
          deletePath(wall);
	  int way = deadEnd(aB->set,aB->curr,Pos,aB->nAvatars,aB->avID);
            //int d = rand() % 4;
          int d = find_nextMove(aB->prev,aB->curr,aB->target, aB->set, aB->d);
          if (way >= 0){
            d =way;

          }
       
          

          writeDirection(aB->filename, aB->avID,d);
          aB->prev->x = aB->curr->x;
          aB->prev->y = aB->curr->y;



          //send move message
          AM_Message moveMessg;
          moveMessg.type = htonl(AM_AVATAR_MOVE);
          moveMessg.avatar_move.AvatarId = htonl(aB->avID);
          moveMessg.avatar_move.Direction = htonl(d);
          sleep(.50);

          send(sockfd,&moveMessg, sizeof(AM_Message),0);
         


        }
      
          
      } 
      //printMaze(aB->MazeWidth,aB->MazeHeight,3,Pos,aB->nAvatars);
      pthread_mutex_unlock(&turn);
    }

    else if(ntohl(replyMessg.type)==AM_TOO_MANY_MOVES){
      pthread_mutex_lock(&exceededMoves);
      printf("you exceeded the number of moves, game over\n");
      close(sockfd);
      freeAvatar(aB);
      pthread_mutex_unlock(&exceededMoves);
      *ret_value =replyMessg;
      return (void*)(ret_value);

    }
    else if(ntohl(replyMessg.type)==AM_NO_SUCH_AVATAR){
      pthread_mutex_lock(&error);
      printf("No such avatar\n");
       freeAvatar(aB);
      close(sockfd);
      pthread_mutex_unlock(&error);
      *ret_value = replyMessg;
      return (void*)(ret_value);
      
    }
    else if(ntohl(replyMessg.type)==AM_SERVER_DISK_QUOTA){
      pthread_mutex_lock(&error);
      printf("Reached server disk quota\n");
       freeAvatar(aB);
      close(sockfd);
      pthread_mutex_unlock(&error);
      *ret_value = replyMessg;
      return (void*)(ret_value);
    }

    else if(ntohl(replyMessg.type)==AM_SERVER_OUT_OF_MEM){
      pthread_mutex_lock(&error);
      printf("Server out of memory\n");
       freeAvatar(aB);
      close(sockfd);
      pthread_mutex_unlock(&error);
     *ret_value = replyMessg;
      return (void*)(ret_value);
    }

    else if(ntohl(replyMessg.type)==AM_UNKNOWN_MSG_TYPE){

      pthread_mutex_lock(&error);
      printf("Unknown message type\n");
       freeAvatar(aB);
      close(sockfd);
      pthread_mutex_unlock(&error);
      *ret_value = replyMessg;
      return (void*)(ret_value);
    }

    else if(ntohl(replyMessg.type)==AM_SERVER_TIMEOUT){
      pthread_mutex_lock(&error);
      printf("Server time out\n");
       freeAvatar(aB);
      close(sockfd);
      pthread_mutex_unlock(&error);
      *ret_value = replyMessg;
      return (void*)(ret_value);

    }

    else if(ntohl(replyMessg.type)==AM_UNEXPECTED_MSG_TYPE){
      // pthread_mutex_lock(&error);
      printf("unexpected message\n"); 
      freeAvatar(aB);
      close(sockfd);
       pthread_mutex_unlock(&error);
      *ret_value = replyMessg;
      return (void*)(ret_value);

    }
    else if(ntohl(replyMessg.type)== AM_MAZE_SOLVED){
      pthread_mutex_lock(&solved);
      // XYPos Pos[aB->nAvatars];
      //method to print out solve Maze
      //printMaze(aB->MazeWidth, aB->MazeHeight, 3, Pos, aB->nAvatars);
      freeAvatar(aB);
      close(sockfd);
      pthread_mutex_unlock(&solved);
      *ret_value = replyMessg;
      return (void*)(ret_value);
      
    }
  }
}

bool updateLog(char *filename,int ID,int nAvatars, XYPos *pos)
{
  FILE *fp;
  fp = fopen(filename,"a");

  if(!fp){
    fprintf(stderr,"Error opening logs file\n");
    return false;
  }
  fprintf(fp, "\nIt's Avatar %d turn\n",ID);
  for(int b =0; b <nAvatars; b++){
      fprintf(fp,"Avatar %i is at (%i,%i)\n",b,pos[b].x,pos[b].y);
  }
  fclose(fp);
  return true;
}

void LogWrite(char *filename,int ID,int nAvatars, XYPos *pos)
{
  FILE *fp;
  fp = fopen(filename,"w");
  printf("this is the filename %s\n",filename);
  if (fp){
    printf("updating log folder\n");
    fprintf(fp, "It's Avatar %d turn\n",ID);
      for(int b =0; b <nAvatars; b++){
        fprintf(fp,"Avatar %i is at (%i,%i)\n",b,pos[b].x,pos[b].y);
      }
  }
  fclose(fp);

}

void foundWall(char *filename, int ID)
{
  FILE *fp;
  fp = fopen(filename,"a");
  if(!fp){
    fprintf(stderr,"Error opening logs file\n");
  }
  fprintf(fp,"Avatar %d found a wall\n",ID);
  fclose(fp);
}

void writeDirection(char *filename, int ID,int d)
{
 FILE *fp;
  fp = fopen(filename,"a");
  if(!fp){
    fprintf(stderr,"Error opening logs file\n");
  }
  if (d == 0){
  fprintf(fp,"Avatar %d tries to move west\n",ID);
  }
  else if (d == 1){
    fprintf(fp,"Avatar %d tries to move north\n",ID);
  }
  else if (d == 2){
    fprintf(fp,"Avatar %d tries to move south \n",ID);
  }
  else if (d == 3){
    fprintf(fp,"Avatar %d tries to move east \n",ID);
  }
  fclose(fp);
}

/*
 * Function to free the Avatar and the pointers inside
 * 
 * 
 * 
 * 
 */
void freeAvatar(avatarBio *aB)
{
  free(aB->prev);
  free(aB->curr);
  free(aB->target);
  free(aB->d);
  free(aB);
}


/* Function to preload the set of walls with the borders
 * 
 * We provide:
 *     a valid pointer to a set of walls and the width and 
 *     height of the maze
 * We do:
 *     insert the outer bounds (so -1 and the value of MazeWidth
 *     and MazeHeight—since the maze goes from 0 up to but not 
 *     including to the width/height) 
 * 
 */
void borderWalls(set_t *set, int MazeWidth, int MazeHeight)
{
  //variables that will be used for the loop
  int x;
  int y;

  //y outer bound values
  for (x = 0; x < MazeWidth; x++){
    char key1[12];
    char key2[12];
    sprintf(key1, "%d,%d",x,0);
    sprintf(key2, "%d,%d",x,-1);
    set_insert(set,key2,key1);
    sprintf(key1, "%d,%d",x,MazeHeight - 1);
    sprintf(key2, "%d,%d",x,MazeHeight);
    set_insert(set,key2,key1);
  }
  //x out of bound values
  for (y = 0; y < MazeHeight; y++){
    char key1[12];
    char key2[12];
    sprintf(key1, "%d,%d",0,y);
    sprintf(key2, "%d,%d",-1,y);
    set_insert(set,key2,key1);
    sprintf(key1, "%d,%d",MazeWidth - 1,y);
    sprintf(key2, "%d,%d",MazeWidth,y);
    set_insert(set,key2,key1);
  }

}
