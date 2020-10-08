# COSC 50, Winter 2020, Maze Project
## Group Name: BASIX
##
## 'DESIGN.md' - contains high level information on our solution to the Maze Project

Brief Project Overview: The Maze Project aims to have a team design, implement, and test an application that will simulate
friends locating each other in a maze that is automatically generated from a remote server. Each individual within the Maze is 
represented as an Avatar. The walls of the Maze are initially unknown to all avatars. There only exists one path between any two
points within the Maze. The Maze is solved when all avatars are located in the same position within the Maze.

### User Interface:

The Maze's interface is two-fold. First, the user makes a specific request to the server via the terminal; this request will
return a Maze with a specific difficulty and begin the program. The second aspect is a rudimentary GUI in the terminal, printing out
the current state of the Maze after each avatar movement.

### Inputs and Outputs:

#### Inputs:

To begin, a client runs a simple start-up script called ```AMStartup```. The script is run in the terminal and includes several additional
parameters. It must include the given server ```hostname```, the number of avatars we want initialized
within the Maze, and a given Maze between 0-10 chosen by the ```difficulty``` parameter.

Example Input Call: ```./AMStartup -n nAvatars -d Difficulty -h hostname```


#### Outputs: 

Several outputs will be available for viewing by the user. First, a log file will be created that will store the entire history of all
avatar moves, the log file is only created after server confirmation of port availability. Additionally, in the terminal window the program
will have a print method to visualize the current state of the Maze as well as the location of all avatars; each avatar movement will print a
new Maze image to the terminal and Maze images will be neatly and clearly separated. Additionally, the visual representation of the maze will include the 
novel discovery of walls visually.

All errors will be documented and printed out to an additional log file, designated solely for error messages.

### Functional Decomposition into modules:

The solving of the maze is decomposed into the following modules; each module will contain it's respective pseudocode for algorithmic flow, functions, and any
data structures utilized in the module's development:

 - AMStartup Module
 - Avatar Module
 - Maze Module
 - Visualize Module

#### AMStartup Module

Pseudocode for AMStartup:
1. Parse through the command line input using switch options
2. Ensure that each passed argument is valid
3. If there is no log file that currently exists, then make one for storing information
4. Start by creating one thread, ensuring successful creation
	1. Create a thread for each of the avatars passed, ```-n (nAvatars)```
5. Link all of the threads; now if one thread terminates, they won't all terminate

Functions for 'AMStartup.c':

```int main(int argc, char* argv[])``` is the main driver of AMStartup, taking the command line input and instantiating a thread for each avatar.

```void itemdelete(void *item)``` is a helper delete function to be passed when deleting a set.

```void printset(FILE *fp, const char *key, void *item)``` is a helper function for printing out a given set.

```int get_ip(char* hostname, char* ip)``` is a helper function to return the integer value of a ```char *ip```

No unique Data Structures used in 'AMStartup.c'


#### Avatar Module

Pseudocode for Avatar:
1. For each avatar, launch their respective thread:
	1. Each avatar argument passes an associated avatar struct, ```typedef struct avatarBio```, more about this structure can be found below
	2. Starting in the thread:
		1. Connect yourself (as an avatar) to the server, sending a success message in the form of a 'ready up' message
		2. If no message received, then the server ended prematurely, print out appropriate error message
	3. Begin while loop to start retrieving messages from the server
		1. If we receive any other message besides an avatar 'turn' message, then exit out of all the threads and end the game
		2. If we received a turn message from the server, then we must update all of the current positions of the avatars; additionally, we must
		update the log file to include all of the avatar current locations
		3. When we get the message, ```pthread mutex lock``` we lock the thread so that no others may execute simultaneously
		4. If it is avatar 0, then never move and always stand still
		5. Avatar 0 updates/assigns global goal vertex for other avatars
		6. All Avatars, by default, will attempt to go EAST during their first movement, following the left hand rule and interacting with their
		surroundings for the first time to establish themselves in the maze
		7. The avatar's direction ```int d```, is constantly updated according to avatar movement and represents cardinal directions
		8. If not avatar 0, check to see if your previous position is equal to your current position inside the maze
			1. If failed to move, then there must be a wall in the given direction
			2. Build the wall by adding it to the *set* of walls found, storing the ```char *key``` as a string representation of the current position
			and ```char *value``` which represents the 'target' vertex.
			3. Additionally, store the inverse situation into the set as well for the case of an avatar interacting with the wall from the other side
		9. Get new movement direction with ```int find_nextMove(XYPos *prev, XYPos *curr, XYPos *target, set_t *walls, int *dir)```; please find this functions respective pseudocode
		in the 'maze' module portion.
		10. Check if avatar is currently in a 'dead end' with ```int deadEnd(set_t *walls, XYPos *curr,XYPos *Pos,int nAvatars,int ID)```; please find this functions respective pseudocode
		in the 'maze' module portion.
		11. Write the *avatar*'s current direction to the log file
		12. Send a move message associated with the correct movement behavior given current observations: whether you are in a dead end, encountered a wall, or discovered the goal vertex
		13. Unlock the ```pthread mutex lock``` to allow other threads to run now that we have completed thread operations


Functions for 'avatar.c', please find more information in 'avatar.h':
		
```void writeDirection(char *filename, int ID,int d);```

```bool updateLog(char *filename,int ID,int nAvatars, XYPos *pos);```

```void foundWall(char *filename, int ID);```

```void borderWalls(set_t *set, int MazeWidth, int Mazeheight);```

```avatarBio* createAvatar(int avID,int MazeWidth,int MazeHeight,XYPos *prev, XYPos *curr, XYPos *target, int Mazeport, char *filename,int nAvatars, char *ip, set_t *set, int *d)```

```void* avatar_thread(void* ptr);```

```void freeAvatar(avatarBio *aB);```


Data Structures for 'avatar.c':

``` typedef struct pair ``` is an assisting data structure for storing walls within our wall *set*, the structure contains two variables, ```char *start, char *end```

``` typedef struct avatarBio``` is a data structure designed to hold all necessary avatar and maze information for easy avatar access; it contains the following parameters:
	
	- ```int avID``` : contains the respective avatars id number
	- ```int MazeWidth``` : contains the width of the maze
	- ```int MazeHeight``` : contains the height of the maze
	- ```XYPos *prev``` : contains the previous XYPos of the avatar
	- ```XYPos *curr``` : contains the current XYPos of the avatar
	- ```XYPos *target``` : contains the 'targeted' XYPos of the avatar (for failed motion)
	- ```int Mazeport``` : contains the server port that we are connecting to
	- ```char *filename``` : contains the filename for logs to be written out to
	- ```int nAvatars``` : contains the total number of avatars
	- ```char *ip``` : contains a string representation of the IP for server connection
	- ```set_t *set``` : a set containing all the list of walls currently in the maze
	- ```int *d``` : an integer representing the current direction of the avatar according to cardinal directions

#### Maze Module

Pseudocode for 'maze.c' module:
*NOTE: Due to the nature of the 'maze.c' file, the pseudocode would be best illustrated by running through the engine functions' pseudocode*

Dead End Function Pseudocode:
1. If first movement, initial directional value set to -1
2. Check for registered walls around the avatar, if three walls are present:
	1. Change current direction to leave the 'dead end' space
	2. Simultaneously, check to determine if there is another avatar in the same 'dead end' space
	3. If there is another avatar, move but do not build a wall to seal the 'dead end'
	4. If there is not another avatar, build a wall while moving, thus completely obstructing access to the 'dead end'

Find Next Move Pseudocode:
1. List all the possible directions in cardinal directions (NORTH SOUTH EAST WEST)
2. Always check instance to the left, with respect to the avatar's perspective and direction; from our perspective this is shown as the avatar always choosing EAST first
3. If the avatar fails to go EAST:
	1. Try the current direction that it is facing
	2. If this movement fails also, go to the next direction and adjust avatar's current direction to such
	3. Otherwise, move to that location if valid movement and insert into the maze
	
	
Functions for 'maze.c' module:

```int deadEnd(set_t *walls, XYPos *curr,XYPos *Pos,int nAvatars,int ID)``` is explained above.

```int find_nextMove(XYPos *prev, XYPos *curr, XYPos *target, set_t *walls, int *dir)``` is explained above.

```path_t *path_new(char *a,char *b);``` allocates memory for a ```path_t *item``` and its respective ```char *start, char *end``` parameters.

```void deletePath(path_t *wall);``` function to assist in freeing the pointer memory of wall items.

```void posToString(XYPos *pos,char *a);``` is a helper function that converts a given XYPos to a passed character pointer.

```void build_wall(set_t *walls, path_t *wall);``` takes in a *set* of walls and a given ```path_t *wall```, inserting the wall into the set normally and inversely (for both cases of avatar interaction)

```void maze_insert(hashtable_t *hashtable, XYPos *current, XYPos *move, int a)``` checks whether, for a given avatar XYPos, each move was the first move of the game, 
a viable move, or if it found a wall, returning the appropriate associated value.

Data Structures for the 'maze.c' module:

```typedef struct path_t``` is heavily leveraged throughout the 'maze.c' implementation; please see the 'avatar.c' data structure section for more information.


#### Visualization Module

Pseudocode for 'visualize.c' module:

The maze is printed through the following pseudocode:
1. Given an ```int mazeWidth, int mazeHeight, int mazeScale```, scale both the width and height by multiplying by ```int mazeScale```
2. Now, we have a blown up grid, we are also passed a list of avatar positions, ```XYPos *pos``` and the number of avatars ```nAvatars```
3. For every row in the grid:
	1. For every column in the grid:
		1. Check to see if we are at a boundary case; if so, then print out the appropriate border character for the maze
		2. Check to see if we are at a wall joining (or vertex), printing a (+) for geographic symmetry
		3. Check to see if an Avatar is located within the given XYPos cell by comparing to our list of avatars, ```XYPos *pos```; if so, then print the value of the avatar in the maze
			1. If multiple avatars in the same position, print the ```int last_seen``` avatar
		4. Check to see if a wall is located within our *set* at the location; if there is then print dashes between marking a wall
		5. Otherwise if nothing to print, then just print an empty space

*NOTE: In essence, the print function scales up the maze and iterates through every potential (X, Y) position, attempting to find the existence of certain objects at specific points*

Functions for 'visualize.c' module:

```void printMaze(int mazeWidth, int mazeHeight, int mazeScale, XYPos *pos, int nAvatars);``` is explained above.

```int maze_isOdd(int mazeScale);``` is a helper function in determining whether the provided ```int mazeScale``` is odd or not.

```XYPos** getColumnWalls();``` is a getter function for the array of vertical wall pointers.

```XYPos** getRowWalls();``` is a getter function for the array of horizontal wall pointers.

```void calculateWalls(char *start,char *end);``` correctly determines the (X, Y) location of the new wall to be stored, based off changes in ```char* start, char *end```.

```void init_walls();``` initializes and memory for all vertical and horizontal walls, setting them equal to (-1, -1).

```void freeWallsColumns();``` assists in freeing all of the allocated memory for our list of walls.

```bool checkC(XYPos column);``` is a helper function passed ```XYPos column``` to see if the wall already exists.

```bool checkR(XYPos row);``` is a helper function passed ```XYPos row``` to see if the wall already exists.


Data Structures for 'visualize.c' module:

No additional data structures were created for this module; global variables, however, were used to track the discovery of walls.

### Pseudocode for logic/algorithmic flow:

    1. execute from the command-line with the relevant parameters.
    2. parse the command-line, validate the parameters, and initialize the modules.
    3. Initialize the number of avatars and the Difficulty and send that as `AM_INIT` message to the server.
    4. Receive the `AM_INT_OK` message from the server.
    5. Start the relevant number of threads and extract the `MazePort` from the `AM_INIT_OK` message.
    6. Each avatar sends the `AM_AVATAR_READY` message via the `MazePort`
    7. While connection is not broken, max number of moves and wait time is not exceeded, and all avatars are not in the same xy position:
       1. servers sends each avatars location as well as which avatars turn.
       2. the moving avatar sends a message to the server about their desired move.
       3. The server approves and updates the position or disapproves the move and the avatar stays in pos.
       4. The server sends the updated maze to each avatar.
       5. We print current state of the maze.
       6. We update the logfile.
    8. Sends `AM_SOLVED_MESSAGE` when all avatars are in the same location.
    9. free memory.



### Dataflow throw the modules:

    - The main in the AMStartup parses the commmand-line parameters, starts the relevant # of threads and sends an initial message, `AM_INIT` to the server.
    - The Server communicator parses the message from the server and sends the `AM_INT_OK` message from each individual avatar. 
    - Each Avatar, in order, determines its movement based off of local observations such as wall placements.
    - The server communicator then transmits the desired movement to the server and then relays the movement info and maze update to all the avatars.
    - The maze module prints the visual representation of the maze and avatars after each avatar move. The maze also outputs progress/errors to the log file.
    

### Testing Plan:

A test program 'testing.sh' is executed to run a series of tests, ensuring that the module works properly and that potential avenues
for errors are monitored and guarded. For each test case, the results are printed out to the terminal.

To begin, we will try to beat levels 0-4; these mazes have nice, pre-defined solutions for us to reference when augmenting our current
code to try and improve solution performance. After we can solve all 4 of these levels, we will begin to consider search efficiency in hopes
of improving performance.

Read more about the our given test cases in 'TESTING.md'.
