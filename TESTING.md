#
#
# Group Name: BASIC
## Group Members: Antony, Sean, Greg, Alex
# Maze Project - 'TESTING.md' contains information on the testing of our 'maze solver.' Please find additional information in 'README.md' and 'DESIGN.md'

### Thread Testing: the following portion will contain sample output directly or via log files as to the testing process behind implementing threads

- In the beginning to test threading, I tested the memory leaks of 2 threads, that is 2 Avatars and see when I properly freed them. As well as testing the 
the argument within it––for example an integer—— and checking to see if it is incremented properly in order by the threads. Then testing this with multiple 
algorithms we used and eventually the current algorithm,checking if the walls are incremented.

### Algorithm Testing: the following portion will show the development of our chosen algorithm to solve any maze

 - Algorithm testing began with us brainstorming a potential solution to the maze given an unknown environment and only boundaries. Separating into two groups
 of two we each thought of a way to implement the discovery of new positions and walls, as well as the process of storing and representing them. Both of our
 groups had designed nearly identical implementation schemes for the maze. The idea behind the first algorithm may be seen below:
 
 	- In our first representation, each avatar begins to build a 'graph' object (in essence, a *hash table*), of viable moves found. Over time, each avatar
 	would develop a small 'graph' representing their past paths.
 	- After enough novel exploration of the maze, the graph of one avatar must intersect with the graph of another through the comparison of two *hash tables*
 	sharing a given key.
 	- Upon finding the other graph; the two *hash table*'s would merge, the 'discovery' avatar will follow the graph to the 'leading' avatar until they find 
 	another avatar graph and merging that *hash table* as well
 	- Once there exists only one 'graph' representation, there must exist some path between the two given points
 	- At this point, we planneed to leverage the breadth-first-search algorithm to generate a path of correct movements for each avatar to follow, eventually
 	leading them all to the same shared location
 	
 - When implementing the algorithm we anticipated too much complexity, ultimately raising concern about time constraints we may suffer from. The complexity of
 the algorithm stemmed from complex data structure nesting as we had a *set* storing a *hash table* of *hash tables*, etc.
 - Due to time concerns, our group elected to change the algorithm to a simpler implementation in the hopes of completing the project in a timely fashion (as 
 absolutely NO extensions are allowed)
 
 - The second algorithm we implemented was the LEFT-HAND RULE algorithm. Avatars are instantiated facing south; each turn, the avatar checks their 'left' direction
 relative to the direction in which they are currently facing. In the initial case, therefore, all avatars attempt to move EAST. Barring movement to their left, they
 next attempt to move is in the direction which the avatar is facing. If that movement if unsuccessful, then we change the direction of the avatar, thus checking the
 other two 'potential paths' out of our given position. Thus, the avatar appears to follow the right wall of the maze from our perspective, but is implemented as the
 left hand rule from the perspective of the avatar.
 
 - Subsequently, we realized that while discovering mazes, we were not optimizing our solution. Avatar movements into discovered 'dead end's' were not blocked in the maze. Thus, in hope
 of optimizing our maze solution to be more robust with both more avatars and higher difficulties we designed a method to slowly limit the available positions for any given avatar. By
 identifying a 'dead end' and blocking it off (assuming no alternate avatar still present in position), we are effectively limiting avatars from re-discovering old dead ends and thus 
 lowering viable position moves for all avatars; instead, like a mine shaft, the 'dead end' is shut off from all other avatars.
 
 - 'Dead ending' was also then integrated with our ```mazePrint``` method, allowing us to visually track wall discovery and 'dead ending' in maze real time.
 
 
### Visualization Testing: the following portion will walk through the development of our visualization method

- To begin, we only sought to print out the rough outlines of the maze provided a given ```mazeWidth``` and ```mazeHeight```. Two 
options were considered: the first was the generation of strings of ```#``` characters depending on a given height and width value, and
the second which was to loop over the maze horizontally and vertically, printing appropriate characters in their correct positions.

- After printing the border of the maze, I needed to complete visualization of the avatars. By passing an array of ```XYPos *pos```, I was able to check every
position to see if it was A) In the middle of a given 'box' in the grid (where we expect an avatar to be), and B) is there an avatar at my approximate position; if
so then print the number of the avatar in the grid

- With avatars and the boundaries in place, we moved on to representing walls inside the maze:
	
	1. As Avatars discover walls, they are added to a global list of either ```XYPos *columnWalls``` or ```XYPos *rowWalls```
	2. Additionally, the number of column or row walls is also incremented and made global
	3. Within the print function, we must have access to the global lists of all wall positions
	4. Provided the locations, check every place on the maze to determine if a wall has been registered as 'seen' there by an avatar
	5. If so, then print out the appropriate character for the wall
	
- After debugging, the maze would initially print empty, filling itself gradually with the discovery of walls and the addition of them to the global
lists. Furthermore, the 'dead end' aspect of our lab can be visualized in the output as each not viable path is 'walled off' by the last avatar to
visit it. Due to this aspect, the total number of walls allocated and stored is very high and increases rapidly with the expansion of either the X or Y scales.


*Output Notes: all outputs from testing the maze were stored and analyzed in the program's associated log files; please find log files containing information on execution after program operation*
