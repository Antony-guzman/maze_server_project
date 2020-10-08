/* File: AMStartup.c - Client Program 
 *
 * Author: Primary author Antony
 * Group name: BASIX
 * Date: MArch 2020
 *
 * This file contains the start up script for the MAze
 * Input:  Initial : -n [nAvatars] -d [Difficulty] -h [hostname]
 *
 * Command line options: -n [nAvatars] : (int) specifies the number of Avatars in the maze, frmo 2-10
 *                       -d [Difficulty] : (int) the difficulty level, from 0 to 9
 *                       -h [hostname] : (char*) hostname of server: flume.cs.dartmouth.edu
 *
 * Output: If an error is found, programs print the appropiate error.
 * 		   otherwise, the progrma calls avatar.c which runs the thread
 * 		   and executes the main algorithm defined in maze.	
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
#include <pthread.h>          //pthread_create
#include <dirent.h>
#include <pthread.h>

#include "amazing.h"
#include "avatar.h"
#include "maze.h"
#include "visualize.h"

#define MAXLEN 200

void itemdelete(void* item);
void printset(FILE *fp, const char *key, void *item);
//declare functions 
int get_ip(char *hostname, char *ip);
void endingFile(char *filename, void *status, int nAvatars, int Difficulty);

/* ========================================================================== */
/* Function that runs the start up script for solving the game
 * 
 * We provide:
 * 			the number of Avatars, between 2-9
 * 			the hostname (for this program, hostname
 * 			is flume.cs.dartmouth.edu)
 * 			the difficulty, between 0-9
 * 
 * We do:
 * 			parse through the command line arguments
 * 			write the init information to the logs file
 * 			create the threads 
 */
int main(int argc, char *argv[])
{ 

	// ---------------- variables
	int nAvatars;                 			   //number of Avatars 	
	int Difficulty;							   //difficulty of the maze
	char *hostname;							   //the hostname of the server
	char *program;							   //AMStartup to print out 
	int MazePort = atoi(AM_SERVER_PORT);       //the port to connect to the server
	int MazeWidth;							   
	int MazeHeight;					
	char ip[MAXLEN];							   //ip address of the host


	//parse through the command line argumetns
  	program = argv[0];
  	if (argc != 7) {
    	fprintf(stderr,"Please input : -n [Number of Avatars(2~10)] -d [Difficulty(0~9)] -h [hostname]. \n");
    	exit(1);
  	} 
	int opt;
	char* temp;
	while((opt = getopt(argc,argv,"n:d:h:")) != -1){

		switch(opt){

			case 'n':
			temp =optarg;

			//check if nAvatars is a numer
			while(*temp != '\0'){
				if(!isdigit(*temp)){
					fprintf(stderr,"Argument for -n is not a valid number!\n");
					exit(1);
				}
				temp++;
				
			}


			//check if nAvatars is an acceptable number
			nAvatars = (int)atol(optarg);
			if(nAvatars < 2 || nAvatars > AM_MAX_AVATAR){
				fprintf(stderr,"nAvatars should be between 2 and %d", AM_MAX_AVATAR);
				exit(AM_INIT_TOO_MANY_AVATARS);
			}

			break;

			case 'd':
			temp = optarg;
			
			while(*temp != '\0'){
				if(!isdigit(*temp)){
					fprintf(stderr,"Argument for -d is not a valid number!\n");
					exit(1);
				}
				temp++;
				
			}

			//Check if we have correct difficulty
			Difficulty = (int)atol(optarg);

			if(Difficulty < 0 || Difficulty > AM_MAX_DIFFICULTY){
				fprintf(stderr, "Argument for -d should be between 0 and %d", AM_MAX_DIFFICULTY);
				exit(1);
			}

			break;

			case 'h': 	

			//Check if the argument for hostname is correct
			hostname = optarg;

			// For the program, the hostname should be same as predefined hostname
			if(strcmp(hostname, "flume.cs.dartmouth.edu")!=0){
				fprintf(stderr,"Please input flume.cs.dartmouth.edu\n");
				exit(1);
			}
                           
			break;

			case '?':
				fprintf(stderr,"Please input : -n [Number of Avatars(2~10)] -d [Difficulty(0~9)] -h [hostname]. \n");
				exit(1);
			break;
		}

	}

	//check to see if log directory exists
	
	DIR *dirname = opendir("logs");
	if (dirname){
		closedir(dirname);
	}
	else if(ENOENT == errno){

		printf(" Logs Directory does not exist\n");
		exit(5);
	}
	else{
	      printf("directory failed to open for some reason\n");
	      exit(6);	      
	}



	// ---------------- create the socket
	int sockfd;		
	sockfd = socket (AF_INET, SOCK_STREAM, 0);

	//error case
	if (sockfd < 0){
		perror("opening socket\n");
		exit(2);
	}

	// initialize the fields  of the server address.
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(AM_SERVER_PORT));

	// look for the hostname passed as the 2nd parameter on the command line
	struct hostent *hostp = gethostbyname(hostname);
	if (hostp == NULL){
		fprintf(stderr, "%s: unknown host '%s'\n", program, hostname);
		close(sockfd);
		exit(2);
	}
	memcpy(&server.sin_addr, hostp->h_addr_list[0], hostp->h_length);

	// connect the socket to that server.
	if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0){
		perror("problem in connecting stream socket\n");
		close(sockfd);
		exit(2);
	}
	printf("connected successfuly!\n");


	//create a message to send
	AM_Message Sendmessg;
	Sendmessg.type = htonl(AM_INIT);
	Sendmessg.init.nAvatars = htonl(nAvatars);
	Sendmessg.init.Difficulty = htonl(Difficulty);
	send(sockfd,&Sendmessg,sizeof(AM_Message),0);

	// receive message
	AM_Message Receivedmessg;
	if(recv(sockfd,&Receivedmessg, AM_MAX_MESSAGE,0)==0){
		fprintf(stderr,"Server termindated prematurely");
		close(sockfd);
		exit(3);
	}

	uint32_t messg = ntohl(Receivedmessg.type);
	if(messg == AM_INIT_OK){
		printf("Init Success!\n");
		MazeWidth = ntohl(Receivedmessg.init_ok.MazeWidth);
		MazeHeight = ntohl(Receivedmessg.init_ok.MazeHeight);
		MazePort = ntohl(Receivedmessg.init_ok.MazePort);
		printf("MazePort : %d, MazeWidth : %d, MazeHeight : %d\n", MazePort, MazeWidth, MazeHeight);

	}
	else if(messg == AM_INIT_FAILED){
		fprintf(stderr,"Init failed \n");
		fprintf(stderr, "Error: 0x%x\n", ntohl(Receivedmessg.init_failed.ErrNum));
		close(sockfd);
		exit(3);
	}
	else{
		fprintf(stderr,"undefined error\n");
		close(sockfd);
		exit(2);
	}



	//——————————————write to file
	char filename[MAXLEN];
	sprintf(filename, "./logs/Amazing_%s_%d_%d.log", getenv("USER"), nAvatars, Difficulty);
	FILE *fp;
	char date[100];
	fp = fopen(filename,"w");
	if (fp){
		time_t currtime = time(NULL);
		strftime(date, 100, "%a %d %Y, %H:%M", localtime(&currtime));
		fprintf(fp, "%s, %u, %s\n", getlogin(), MazePort, date);
		fprintf(fp, "Difficulty: %i\nNumber of Avatars: %i\n", Difficulty, nAvatars);
		fprintf(fp, "Mazeport: %i\n",MazePort);
		fprintf(fp, "MazeWidth %i\n",MazeWidth);
		fprintf(fp, "MazeHeight %i\n",MazeHeight);
		fclose(fp);
	}

	//get IP address for avatarBio
	get_ip(hostname, ip);

	//close the socket
	close(sockfd);

	//create list of walls
	set_t *set = set_new();
	borderWalls(set,MazeWidth,MazeHeight);
	init_walls();

	pthread_t aThreads[nAvatars];
	// char* thread_res = NULL;
	void *status;
	int rt;

	for (int a =0 ; a < nAvatars; a++){
		XYPos* prev = malloc(sizeof(XYPos));
		XYPos* curr = malloc(sizeof(XYPos));
		XYPos *target = malloc(sizeof(XYPos));
		prev->x = -2;
		prev->y = -2;
		curr->x = -3;
		curr->y = -3;
		target->x = -1;
		target->y = -1;
		int *p = malloc(sizeof(int));
		*p = -1;
		avatarBio *aB = createAvatar(a,MazeWidth, MazeHeight, prev, curr, target, MazePort,filename,nAvatars,ip,set, p);

		//Avatars[a] = aB;
		printf("creating thread %d in main()\n",a);
      	rt = pthread_create(&aThreads[a],  NULL, avatar_thread, (void *) aB);
		if (rt){
			printf("eeror in joining\n");
			exit(-1);
		} 


	}
	
	//for loop that joinds for the threads 
	//adn then free each avatar 
	for(int b =  0; b < nAvatars; b++) {
    	int res = pthread_join(aThreads[b],  &status);
		if (res){
			printf("eeror in joining\n");
			exit(-1);
		} 
    } 

	endingFile(filename,status, nAvatars, Difficulty);
	free(status);
	
	freeWallsColumns();
	set_delete(set,itemdelete);
  	printf("Main: program completed. Exiting.\n");
 	pthread_exit(NULL);

}



int get_ip(char *hostname, char *ip)
{
   struct hostent *he;     
   struct in_addr **addr_list;     
   int i;     
   if ( (he = gethostbyname( hostname ) ) == NULL)     
   { perror("gethostbyname");         
     return 1;}     
   addr_list = (struct in_addr **) he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++)
    {   strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;}
	free(he);
	free(addr_list);
    return 1;
}

void itemdelete(void* item)
{
	if(item != NULL){
		free(item);
	}			
}

void printset(FILE *fp, const char *key, void *item)
{

	fprintf(fp,"%s | %s\n",key,(char*)item);
	
}

/*
 *
 * 
 * 
 * 
 * 
 * 
 */
void endingFile(char *filename, void *status, int nAvatars, int Difficulty)
{

	FILE *fp;
	fp = fopen(filename,"a");
	AM_Message messg = (*(AM_Message*)status);
	uint32_t type = ntohl(messg.type);
	// AM_Message *messg = (AM_Message)status;
	if(fp){
		if (type== AM_MAZE_SOLVED){
			fprintf(fp,"\nyou solved the maze!\n");
			fprintf(fp,"Difficult %d  nAvatars %d nMoves %d Hash %d \n",ntohl(messg.maze_solved.Difficulty), ntohl(messg.maze_solved.nAvatars),ntohl(messg.maze_solved.nMoves), ntohl(messg.maze_solved.Hash));
			printf("Maze was solved!\n");
		}
		else if(type ==AM_UNKNOWN_MSG_TYPE){
			fprintf(fp,"\nunknown message!\n");
			fprintf(fp," message: %d\n",ntohl(messg.unknown_msg_type.BadType));

		}
		else if(type ==AM_UNEXPECTED_MSG_TYPE){
			fprintf(fp,"\nreceived unexpected message, server will abort\n");
		}
		else if(type ==AM_AVATAR_OUT_OF_TURN){
			fprintf(fp, "\nAn avatar tried to move out of turn \n");

		}
		else if(type == AM_TOO_MANY_MOVES ){
			fprintf(fp, "\nYou exceeded the amount of moves %d\n", (AM_MAX_MOVES*(Difficulty+1)*nAvatars));
		}
		else if(type == AM_SERVER_TIMEOUT ){
			fprintf(fp, "\nThe server timed out\n");
		}
		else if (type == AM_SERVER_DISK_QUOTA){
			fprintf(fp,"\nRan out of disk space\n");
		}
		else if(type == AM_SERVER_OUT_OF_MEM){
			fprintf(fp, "\nOut of memory\n");
		}
	}

	fclose(fp);
}
