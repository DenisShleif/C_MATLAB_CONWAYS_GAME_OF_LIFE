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
#define DISPLAY_MODE 1 ///If this mode is 1 it will display the board, if this mode is 0, it will calculate the time information

///MISC
#define PERIOD 100 ///Time between iterations in miliseconds
#define ALIVE 1
#define DEAD 0

///Variables
int WORLDX;///In Cells
int WORLDY; /// In Cells
int TIME;/// In Seconds

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

int inBetween (int n, int min, int max){
    return n >= min && n <= max;
}

///Checks is a cell is whithin the bounds of the world
int inBounds (int x, int y){
    return x >= 0 && y >= 0 && x < WORLDX && y < WORLDY;
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                               World Functions                            ///
///                                                                          ///
///--------------------------------------------------------------------------///

///Sets all elements in matrix World to num
int resetWorld (int world[WORLDX][WORLDY],int num){
    int i,j;

    ///Iterate through all the points in the matrix
     for (i = 0; i < WORLDX; i++)
        for (j = 0; j < WORLDY; j++)
                world [i][j] = num; ///Set the value to num

}

///Randomly generates world
int generateWorld (int world[WORLDX][WORLDY]){
    int i,j;
    srand(time(NULL)); ///Seed the random number generator
    for (i = 0; i < WORLDX; i++)
        for (j = 0; j < WORLDY; j++)
            world [i][j] = rb(DEAD,ALIVE); /// Generates either a live or dead cell;
}

///Sets worldA = worldB
int setWorld (int worldA[WORLDX][WORLDY], int worldB[WORLDX][WORLDY]){
     int i,j;
     ///Iterate through all points
     for (i = 0; i < WORLDX; i++)
        for (j = 0; j < WORLDY; j++)
                worldA [i][j] = worldB[i][j]; ///Set the value to num
}

int neighbourCount (int world[WORLDX][WORLDY],int x,int y){
    int i,j;
    int n = 0; ///Number of live neighbours
    ///Iterate through surounding points
    for (i = x - 1; i <= x + 1; i++)
        for (j = y - 1; j <= y + 1; j++)
            if (inBounds(i,j)) ///If inbounds and not center cell
                n += world [i][j];

    return (n - world [x][y]);
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                                   Output                                 ///
///                                                                          ///
///--------------------------------------------------------------------------///


///Prints the world (part of the game)
int printWorld (int world[WORLDX][WORLDY]){
    int i,j;

    system ("cls");
    ///Iterate through all the points
    for (i = 0; i < WORLDX; i++){
        for (j = 0; j < WORLDY; j++){
            if (world [i][j] == DEAD) ///If dead cell
                printf (" ");
            else if (world [i][j] == ALIVE) /// if live cell
                printf ("X");
        }
        printf ("\n"); ///prints a new line for the next row
    }
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                                    Game                                  ///
///                                                                          ///
///--------------------------------------------------------------------------///

int updateWorld (int world[WORLDX][WORLDY], int tmpWorld[WORLDX][WORLDY]){
    int i,j, neighbours;
    ///Iterate through all the points
     for (i = 0; i < WORLDX; i++){
        for (j = 0; j < WORLDY; j++){
            neighbours = neighbourCount (tmpWorld,i,j);
            if (tmpWorld[i][j] == ALIVE && !inBetween(neighbours, LIVE_MIN,LIVE_MAX)) ///If cell is alive and the cell shoudl die
                world [i][j] = 0; ///Set Cell to Dead
            else if (tmpWorld[i][j] == DEAD && inBetween(neighbours, BORN_MIN,BORN_MAX)) ///If cell is dead and cell should be born
                world [i][j] = 1; ///Set cell to Alive
        }
     }
}

int gameOfLife (){
    ///Variable Declaration
    int world [WORLDX][WORLDY];
    int tmpWorld [WORLDX][WORLDY];
    int t;

    ///Begining
    generateWorld (world); ///Generate random world and liveCellList

    for (t = 0; t < TIME; t++){
        setWorld(tmpWorld,world); ///Sets all values from world into tmpWorld
        updateWorld(world,tmpWorld); ///Checks all cells
        if (DISPLAY_MODE == 1){///If the mode is displaying
            printWorld(world); ///Displays World
            delay(PERIOD); ///Short Delay
        }
    }
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

        ///Loop Through 100 iterations
        for (i = 1; i <= 50; i++){
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
