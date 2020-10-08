/*
 * Details of the printMaze function can be found in 'DESIGN.md'
 */ 
void printMaze(int mazeWidth, int mazeHeight, int mazeScale, XYPos *pos, int nAvatars);

/*
 * The maze_isOdd() function is designed to just determine the spacing of each 'cell' in the maze
 * and whether the cell's spacing is an odd or even integer (for assistance in printing); the result
 * is dependent on the mazeScale
 */ 
int maze_isOdd(int mazeScale);

/*
 * Getter function for the columnWalls global
 */
XYPos** getColumnWalls();

/*
 * Getter function for the rowWalls global
 */
XYPos** getRowWalls();

/*
 * The calculateWalls() function takes in character pointers and is used in determining two main factors; whether
 * the first values passed are -1 and then determining where exactly the wall is given two positions and comparing
 * the differences in the x and y coordinates
 *
 * User Provides:
 * - valid pointer to start and end
 *
 * We Return:
 * - Nothing, the function is void
 *
 */
void calculateWalls(char *start,char *end);

/*
 * The init_walls() function assists in allocating memory for all of the potential column and row walls that
 * we will find; it is based upon a set global variable INT_MAX that is set in the head of the file.
 */
void init_walls();

/*
 * Helper function for freeing all the memory allocated to the rowWalls array as well as the columnWalls array; also
 * based upon passed globally defined constant INT_MAX
 */
void freeWallsColumns();


/*
 * checkC() returns a boolean based on whether or not the passed XYPos column already exists within
 * the global array of column wall pointers.
 *
 * User Provides:
 * - Valid XYPos object
 *
 * We return:
 * - True if the column wall is already in the global list
 * - False if it is not within the list
 */
bool checkC(XYPos column);


/*
 * checkR() returns a boolean based on whether or not the passed XYPos row already exists within
 * the global array of row wall pointers.
 *
 * User Provides:
 * - Valid XYPos object
 *
 * We return:
 * - True if the row wall is already in the global list
 * - False if it is not within the list
 */
bool checkR(XYPos row);
