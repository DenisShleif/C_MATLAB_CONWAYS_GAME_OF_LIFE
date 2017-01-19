# C_MATLAB_CONWAYS_GAME_OF_LIFE

This repository contains all files created for the purpose of simulating Conway's Game of Life.

Rules:
 1) Cells are either dead or alive
 2) Initially cells are determined to be dead or alive randomly
 2) Any live cell with fewer than two live neightbours dies
 3) Any live cell with more than three live neighbours dies
 4) Any live cell with two or three live neighbours lives, unchanged, to the next generation
 5) Any dead cell with exactly three live neighbours will come to life

Simple Algorithm:
Create an n x m Matrix called original where live cells are 1s and dead cells are worth 0.
Loop through t iterations of the following system
	Create an n x m Matrix called new
	Loop through every cell in original matrix.
		Add all the cell values surounding the current cell (do not include the current cell value). Call this value Sum
		If the cell is dead and the Sum is 3, set the cell to 1 on the new matrix
		If the cell is alive and the Sum is less than 2, set the cell to 0 on the new matrix
		If the cell is alive and the Sum is greater than 3, set the cell to 0 on the new matrix
		If the cell is alive and the Sum is 2 or 3, set the cell to 1 on the new matrix

This algorithm was written in both C and Matlab. The algorithm in C increased run-time by 7800%

Optimized Algorithm:
Create an n x m Matrix called board where live cells are 1s and dead cells are worth 0.
Create a linked list called live_cells where each node contains the x and y coordinates of the live cell and a boolean variable (willLive) determining if the live cell will live to the next round
Create another linked list called newCells where cells that become alive will be placed
Loop through t iterations of the following system
	Iterate through live_cells
		Check the cells around the current cell. Add the cell values surounding the current cells using the board Matrix
		If the cell should die, change willLive to 0
		If the cell should live, keep willLive to 1
		Iterate through the 8 surounding cells.
			If they are alive do nothing
			If they are dead, Add the cell values surounding the dead cell using the board Matrix
				If the sum is 3, add the cell to the newCells linked list. (ensure willLive is set to 1)
	Combine both the live_cells linked list and the newCells linked list
	Iterate through the live_cells link list
		If willLive is set to 1, Update the board Matrix and set that cell to 1
		If willLive is set to 0, Update the board Matrix and set that cell to 0, ensure this node is removed from the linked list

Though in some cases this algorithm requires the program to check through certain cells multiple times, and requires the program to iterate through the live_cells list twice, due to the fact that the size of live_cells drops dramatically in the first 10 iterations, these inefficiencies become more efficient than the short algorithm. This algorithm has been shown to increase the program runtime by 100%

Wolves and Rabbits Rules:
2 Entities exist in this game, wolves and rabbits.
Rabbits follow the same rules as live cells in the regular game of life. Wolves are complex entities that move around the board That have the following characteristics:
 - Gender
 - Age
 - Agression Level
 - Reproductive Rate
 - Mother and Father
 - Health

If two males or two females met they would fight and one would overcome the other based on age, agressive level and slight random chance
If a male and a female met, they would reproduce making a wolf cub with an average of the characteristics (slight deviation randomly)
No reprouction or fights would occur if the wolves are directly related to each other (2 generations max)

Skills Used:
Doubly Linked Lists
Game Rule Adjustment
Data Mining
