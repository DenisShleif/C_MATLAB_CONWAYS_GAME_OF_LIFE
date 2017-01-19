///Libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

///Game of Life Rules
#define LIVE_MIN 2
#define LIVE_MAX 3
#define BORN_MIN 3
#define BORN_MAX 3

///Modes
#define DEBUG_MODE 0 ///If this mode is 1, it shows all the DEBUG information, if not it doesn't diplay DEBUG information. DISPLAY_MODE must be 1 for debug mode to be active
#define DISPLAY_MODE 1 ///If this mode is 1 it will display the board, if this mode is 0, it will calculate the time information

///MISC
#define LAYERS 2 ///Layers in the world Matrix
#define PERIOD 100 ///Time between iterations in miliseconds

///Cell types
#define ALIVE 1
#define DEAD 0

///Variables
int WORLDX;///In Cells
int WORLDY; /// In Cells
int TIME;/// In Seconds

///Creating a linked list for all the live cells
struct liveCell{
    int x;
    int y;
    int willLive;
    struct liveCell *nextLiveCell;
};

///--------------------------------------------------------------------------///
///                                                                          ///
///                              Generic Functions                           ///
///                                                                          ///
///--------------------------------------------------------------------------///

///Creates a time delay of mSec miliseconds
void delay (unsigned int mSec){
    clock_t ticks1 = clock(), ticks2 = ticks1;
    while ( ((ticks2 * 1000)/CLOCKS_PER_SEC - (ticks1 * 1000)/CLOCKS_PER_SEC ) < mSec)
        ticks2 = clock();
}

///Generates a random number between min and max inclusivly
int rb (int min, int max){
    return rand () % (max - min + 1) + min;
}

///Returns whether n is inbetween min and max
int inBetween (int n, int min, int max){
    return n >= min && n <= max;
}

///Checks is a cell is whithin the bounds of the world
int inBounds (int x, int y){
    return x >= 0 && y >= 0 && x < WORLDX && y < WORLDY;
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                           Linked Lists Functions                         ///
///                                                                          ///
///--------------------------------------------------------------------------///


///Function adds a new cell to the end of the linked list
int createNewCell (struct liveCell **liveCellList,int X,int Y,int WillLive){
    struct liveCell *newCell; ///Creates a new node
    newCell = (struct liveCell*) malloc( sizeof(struct liveCell)); ///gives memory to the node in the heap
    newCell -> nextLiveCell = *liveCellList; ///Sets the root of the linked list to the end of this element
    /// Sets the values of the node
    newCell -> x = X;
    newCell -> y = Y;
    newCell -> willLive = WillLive;

    *liveCellList = newCell; ///sets the root of the linked list to this cell
}

///Frees the memory of the linked list
int clearLiveList (struct liveCell *liveCellList){
    struct liveCell *currentCell = liveCellList;
    struct liveCell *nextCell = liveCellList -> nextLiveCell;

    ///Iterate through all cells and free the memory
    while (nextCell != NULL){
        free(currentCell);
        currentCell = nextCell;
        nextCell = nextCell -> nextLiveCell;
    }
    free(currentCell);
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                              Intro Functions                             ///
///                                                                          ///
///--------------------------------------------------------------------------///

///Sets all elements in matrix World to num
int resetWorld (int world[WORLDX][WORLDY][LAYERS],int num){
    int i,j,k;

    ///Iterate through all the points in the matrix
     for (i = 0; i < WORLDX; i++)
        for (j = 0; j < WORLDY; j++)
            for (k = 0; k < 2; k++)
                world [i][j][k] = num; ///Set the value to num

}

///Adds num to all cells surounding (x,y)
int addSuroundingCells (int world[WORLDX][WORLDY][LAYERS], int x, int y, int num){
    int i,j;

    ///Iterates through the 9 points surounding the cell
    for (i = x - 1; i <= x + 1; i++)
        for (j = y - 1; j <= y + 1; j++)
            if (inBounds (i,j) && !(i == x && j == y)) /// if the cell is inbounds and not the middle one, add num
                world [i][j][1] += num;
}

///Randomly generates world and fills both layers as well as the liveCellList
int generateWorld (int world[WORLDX][WORLDY][LAYERS], struct liveCell **liveCellList, int *numLive){
    int i,j;
    srand(time(NULL)); ///Seed the random number generator

    ///Iterate through all cells of the world matrix
    for (i = 0; i < WORLDX; i++){
        for (j = 0; j < WORLDY; j++){
            world [i][j][0] = rb(DEAD,ALIVE); /// Generates either a live or dead cell;
            if (world [i][j][0] == ALIVE){ ///If cell is alive
                addSuroundingCells (world,i,j,1); ///Add 1 to the surounding cells in the second layer
                createNewCell (liveCellList,i,j,ALIVE); ///Add the cell to the live cell list
                (*numLive)++; ///Adds one to the number of cells Alive
            }
        }
    }
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                                   Output                                 ///
///                                                                          ///
///--------------------------------------------------------------------------///


///Outputs the values stores in the layer layer or world (FOR DEBUG)
int printWorldDebug (int world[WORLDX][WORLDY][LAYERS]){
    int i,j;
    ///Iterate through all the points
    for (i = 0; i < WORLDX; i++){
        for (j = 0; j < WORLDY; j++){ ///Prints values in first layer
           if (world [i][j][0] == DEAD) ///If dead cell
                printf (" ");
            else if (world [i][j][0] == ALIVE) /// if live cell
                printf ("X");
        }
        printf (" ");
        for (j = 0; j < WORLDY; j++){
            printf ("%i ",world[i][j][1]); ///Print the value in the second layer
        }
        printf ("\n"); ///prints a new line for the next row
    }
    printf ("\n");
}

///Prints the list of cells (FOR DEBUG)
int printLiveCellList (int world[WORLDX][WORLDY][LAYERS], struct liveCell *liveCellList){
    printf ("X\tY\twill\talive\tneighbours\n");
        while (liveCellList!= NULL){
            printf( "%i\t%i\t%i\t%i\t%i\n", liveCellList -> x, liveCellList -> y, liveCellList -> willLive, world [liveCellList -> x][liveCellList -> y][0],world [liveCellList -> x][liveCellList -> y][1]);
            liveCellList = liveCellList -> nextLiveCell;
        }
    printf ("\n");
}

///Prints the world (part of the game)
int printWorld (int world[WORLDX][WORLDY][LAYERS]){
    int i,j;

    ///Iterate through all the points
    for (i = 0; i < WORLDX; i++){
        for (j = 0; j < WORLDY; j++){
            if (world [i][j][0] == DEAD) ///If dead cell
                printf (" ");
            else if (world [i][j][0] == ALIVE) /// if live cell
                printf ("X");
        }
        printf ("\n"); ///prints a new line for the next row
    }
}

///Outputs the world
int printDebug (int world[WORLDX][WORLDY][LAYERS], struct liveCell *liveCellList, int numLive, int numBorn, int numDied, int t){
    printf ("\n");
    printWorldDebug (world); ///Prints both laters of world
    printf ("numLive = %i, numBorn = %i, numDied = %i, t = %i\n\n", numLive, numBorn, numDied,t); ///Prints
    //printLiveCellList(world,liveCellList); ///Prints the liveCellList
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                                    Game                                  ///
///                                                                          ///
///--------------------------------------------------------------------------///

///Resets variables at the begining of the iteration
int resetIteration (int *newCells, int *numBorn, int *numDied){
    (*newCells) = NULL;
    (*numBorn) = 0;
    (*numDied) = 0;
}

///Checks all the live cells
int checkWorld (int world[WORLDX][WORLDY][LAYERS], struct liveCell *liveCellList,struct liveCell **newCells, int *numBorn, int *numDied){
    struct liveCell *currentCell = liveCellList;
    int i,j;

    while (currentCell != NULL){ ///Iterate through liveCellList
        ///Iterate through surrounding cells
        for (i = (currentCell -> x) - 1; i <= (currentCell -> x) + 1; i++){
            for (j = (currentCell -> y) - 1; j <= (currentCell -> y) + 1; j++){
                if (inBounds (i,j)){ /// If the cell is inbounds
                    if (i == (currentCell -> x) && j == (currentCell -> y)){ ///If the cell is the center cell
                        if (!(world [i][j][1] == 2 || world [i][j][1] == 3)){ /// If cell does not have 3 of 4 neighbours
                            currentCell -> willLive = DEAD; ///sets that the cell will die
                            (*numDied) += 1; ///Updates NumDied
                        }
                    }
                    else if (world [i][j][0] == DEAD){ ///If the cell is not in the center cell and the cell is dead
                        if (world [i][j][1] == 3){ /// If cell has 4 neighbours
                            createNewCell (newCells,i,j,ALIVE); ///Adds a cell to the end of newCells
                            world [i][j][0] = 2; ///notes that this cell has already been put into the newCell list
                            (*numBorn) += 1; ///Updates NumBorn
                        }
                    }
                }
            }
        }
    currentCell = currentCell -> nextLiveCell; ///Go to next liveCell
    }
}

///Updates the liveCell List and the world matric
int update (int world[WORLDX][WORLDY][LAYERS], struct liveCell **liveCellList, struct liveCell *newCells, int *numLive, int numBorn, int numDied){
    struct liveCell *currentCell = *liveCellList;
    struct liveCell *previousCell = NULL;
    int i,j;

    while (currentCell != NULL){ ///Iterate through liveCellList
        if (currentCell -> willLive == DEAD){ ///If the cell will die
            addSuroundingCells (world,currentCell -> x, currentCell -> y,-1); ///subtract 1 from surounding cells
            world [currentCell -> x][currentCell -> y][0] = DEAD; ///set the cell to dead on the first layer
            if (previousCell == NULL){ /// If start of the linked list
                (*liveCellList) = currentCell -> nextLiveCell; ///set the root of the list to the next cell
                free(currentCell); ///Frees the memory
                currentCell = *liveCellList; ///reset current cell
                previousCell = NULL; ///set the previous cell to null
            }
            else{
                 previousCell -> nextLiveCell = currentCell -> nextLiveCell; ///Set the previous cell to the cell after the current cell
                 free(currentCell); ///Frees the memory
                 currentCell = previousCell -> nextLiveCell; ///Set the current Cell to the next Cell
            }
        }
        else{ ///if the current cell will live (move onto next cell)
            previousCell = currentCell; ///Sets the previous cell to the current cell
            currentCell = currentCell -> nextLiveCell; ///Sets the current cell to the next cell
        }
    }

    previousCell -> nextLiveCell = newCells; ///Concatinates both lists
    currentCell = newCells; ///Sets the current cell pointer to the start of the new Cells
    while (currentCell != NULL){ ///Iterate through newCells
        addSuroundingCells (world,currentCell -> x, currentCell -> y,1); ///add 1 on surounding cells
        world [currentCell -> x][currentCell -> y][0] = ALIVE; ///set the cell to alive on the first layer
        previousCell = currentCell; ///Sets the previous cell to the current cell
        currentCell = currentCell -> nextLiveCell; ///Sets the current cell to the next cell
    }

    ///Update numLive
    (*numLive) += numBorn;
    (*numLive) -= numDied;
}

///Game
int gameOfLife (){
    ///Variable Declaration
    int world [WORLDX][WORLDY][LAYERS];
    int numLive, numBorn, numDied;
    struct liveCell *liveCellList = NULL;
    struct liveCell *newCells = NULL;
    int t;

    ///Begining
    numLive = 0;
    resetWorld (world, DEAD); ///Resets the world
    generateWorld (world, &liveCellList, &numLive); ///Generate random world and liveCellList

    for (t = 0; t < TIME; t++){
        ///Update iteration
        resetIteration (&newCells,&numBorn, &numDied); ///Reset to 0
        checkWorld(world, liveCellList,&newCells, &numBorn, &numDied); ///Determines which cell will be born and die this iteration
        if (numLive + numBorn - numDied == 0) ///Checks if any cells will be alive at the end of the iteration
            break; /// if not, exit the loop
        update(world,&liveCellList,newCells, &numLive, numBorn, numDied); ///Updates the liveCellList, world and numLive.

        ///Output World
        if (DISPLAY_MODE == 1){ ///If the display function is on
            if (DEBUG_MODE == 0){
                system ("cls");///Clear Screen
                printWorld (world); ///Prints the world
                delay (PERIOD); ///Creates delay
            }
            else{ /// if debugging
                 printDebug (world, liveCellList, numLive, numBorn, numDied, t); ///Print Debug Data
                system("pause"); ///Pause program each iteration
            }
        }

    }

    ///Clears the linked list
    clearLiveList (liveCellList);
    clearLiveList (newCells);

    if (DEBUG_MODE == 0 && DISPLAY_MODE == 1) ///If not debugging
        system ("cls"); ///Clear screen at the end
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                                    Main                                  ///
///                                                                          ///
///--------------------------------------------------------------------------///

int main (){
    if (DISPLAY_MODE == 1){///If set to display
        WORLDX = 24; ///24 for optimal display
        WORLDY = 79; ///79 for optimal display
        TIME = 100; ///Iterations
        gameOfLife ();
    }
    else{ ///For timing
        ///Variable Declaration
        int i;
        clock_t begin, end;
        double time_spent;
        FILE *fp;


        fp = fopen("data.txt", "w"); ///Open File
        TIME = 100; ///Iterations

        ///Loop Through 500 iterations
        for (i = 1; i <= 50; i++){
            ///Board size from 10 x 10 to 500 x 500
            WORLDX = i * 10;
            WORLDY = i * 10;

            begin = clock(); ///Start the clock
            gameOfLife (); ///Run Game
            end = clock(); /// End Clock
            time_spent = (double)(end - begin) / CLOCKS_PER_SEC; ///Calculate elapsed time
            fprintf(fp, "%f\n", time_spent); ///Store elasped time
            printf ("iteration %i/%i\n",i,100); ///Print current iteration
        }
        fclose(fp); ///Close file
    }
}
