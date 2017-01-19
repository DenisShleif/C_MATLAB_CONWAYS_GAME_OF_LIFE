///Libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

///Rabbit Rules
#define RABBIT_LIVE_MIN 2 ///Min neighbours the rabbit needs to survive
#define RABBIT_LIVE_MAX 3 ///Max neighbours the rabbit needs to survive
#define RABBIT_BORN_MIN 3 ///Min neighbours an empty cell needs to be born into a rabbit
#define RABBIT_BORN_MAX 3 ///Max neighbours an empty cell needs to be born into a rabbit

///Wolf Rules
///Agression is a constant that all wolves have that determine how agressive they will be
///The higher the agression the more likely they are to win in a fight
#define MIN_AGRESSION 0.0001
#define MAX_AGRESSION 0.9999
///Reproductive rate in the minimum health a wolf required to mate
#define MIN_REPRODUCTIVE_RATE 3
#define MAX_REPRODUCTIVE_RATE 7
///Health
#define MIN_HEALTH 0
#define MAX_HEALTH 100
///Initial conditions
///These are the stats of the proto wolves, off of which each successive generation is mutated
#define INITIAL_AGRESSION 0.5
#define INITIAL_REPRODUCTIVE_RATE 4
#define INITIAL_HEALTH 20
///Fight rules
#define ATTACKING_MULTIPLIER 20
#define AGRESSION_MULTIPLIER 100
#define AGE_MULTIPLIER 1
#define WOLVES_FOUGHT_MULTIPLIER 20
#define HEALTH_MULTIPLIER 20

///Misc
#define RABBIT_FOOD_BENEFIT 4 ///Health gained by eating a rabbit
#define BIRTH_HEALTH_LOSS 2 ///Health lost during birth

///Cell Types
#define EMPTY 0
#define RABBIT 1
#define RABBIT_BORN 2
#define MALE_WOLF 3
#define FEMALE_WOLF 4
#define RABBIT_EATEN 5

///Rabbit States
#define DYING 0
#define ALIVE 1

///Inital Cell Chances
#define DEAD_CHANCE 60 ///Actual chance is 60/100 = 60%
#define RABBIT_CHANCE 95 ///Actual chance is 35/100 = 35%
#define WOLF_CHANCE 100 ///Actual chance is 5/100 = 5%
#define TOTAL_CHANCE 100

///Modes
#define DEBUG_MODE 0 ///If this mode is 1, it shows all the DEBUG information, if not it doesn't diplay DEBUG information. DISPLAY_MODE must be 1 for debug mode to be active

///MISC
#define LAYERS 2
#define PERIOD 100 ///Time between iterations in miliseconds


///Variables
int WORLDX;///In Cells
int WORLDY; /// In Cells
int TIME;/// In Seconds

///--------------------------------------------------------------------------///
///                                                                          ///
///                                 Structures                               ///
///                                                                          ///
///--------------------------------------------------------------------------///


///rabbit list
struct rabbitType{
    int x; ///stores x coordinate
    int y; ///stores y coordinate
    int willLive; ///Checks if it will live to the next turn
    struct rabbitType *nextRabbit; ///Points to the next wolf
    struct rabbitType *previousRabbit; ///Points to the previous wolf
};

///The wolf's characteristics
struct wolfCharacteristicsType{
    int gender; ///Gender
    int age; ///Curent Age
    double agression; ///Chance of winning a fight with other wolves of the same gender (0,1);
    int reproductiveRate; ///Min health required to have a child
};

///Keeps track of statistics
struct wolfStatsType{
    int averageHealth; ///Adds the health of the wolf each turn
    int rabbitsEaten; ///Counts the number of rabits the wolf has eaten
    int malesBirthed;
    int femalesBirthed;
    int wolvesFought;
    struct wolfType *mother; ///Points to its mother
    struct wolfType *father; ///Points to its father
};

///Keeps track of the current state of the wolf
struct wolfStateType{
    int x; ///Current X location
    int y; ///Current Y location
    int health; ///Health Level
};

///rabbit list
struct wolfType{
    struct wolfStateType currentState;
    struct wolfCharacteristicsType characteristic;
    struct wolfStatsType statistics;
    struct wolfType *nextWolf; ///Points to the next wolf
    struct wolfType *previousWolf; ///Points to the Previous wolf
};

struct animalStatType{
    int numLive;
    int numBorn;
    int numDied;
};

///Stats
struct statsType{
    struct animalStatType rabbit;
    struct animalStatType femaleWolves;
    struct animalStatType maleWolves;
};

///Stores the data in the world matrix
struct worldCell{
    int entity;
    int neighbourrs;
    struct wolfType *occupyingWolf;
    struct rabbitType *occupyingRabbit;
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

///Generates a random float between min and max inclusivly
double rbFloat(double min, double max){
    return ((rand() * 1.0)/(RAND_MAX * 1.0)) * (1.0 * (max - min)) + (1.0 * min);
}

///Returns weather n is inbetween min and max
int inBetween (int n, int min, int max){
    return n >= min && n <= max;
}

///Checks is a cell is whithin the bounds of the world
int inBounds (int x, int y){
    return x >= 0 && y >= 0 && x < WORLDX && y < WORLDY;
}

///Adds num to all cells surounding (x,y)
int addSuroundingCells (struct worldCell world[WORLDX][WORLDY], int x, int y, int num){
    int i,j;

    ///Iterates through the 9 points surounding the cell
    for (i = x - 1; i <= x + 1; i++)
        for (j = y - 1; j <= y + 1; j++)
            if (inBounds (i,j) && !(i == x && j == y)) /// if the cell is inbounds and not the middle one, add num
                world [i][j].neighbourrs += num;
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                             Rabbit Functions                             ///
///                                                                          ///
///--------------------------------------------------------------------------///

///Function adds a new rabbit
int createNewRabbit (struct worldCell world [WORLDX][WORLDY], struct rabbitType **rabbitList,int X,int Y){
    struct rabbitType *newRabbit; ///Creates a new node

    ///Linked list
    newRabbit = (struct rabbitType*) malloc( sizeof(struct rabbitType)); ///gives memory to the node in the heap
    newRabbit -> nextRabbit = (*rabbitList); ///Sets the root of the linked list to the end of this element
    newRabbit -> previousRabbit = NULL; ///Sets the previous rabbit pointer to NULL (because its the first element)
    if ((*rabbitList) != NULL) /// if pointing to a cell
        (*rabbitList) -> previousRabbit = newRabbit; ///Sets the previous cell of the root of the next cell to this cell

    /// Sets the values of the node
    newRabbit -> x = X;
    newRabbit -> y = Y;
    newRabbit -> willLive = ALIVE;

    world [X][Y].occupyingRabbit = newRabbit;
    (*rabbitList) = newRabbit; ///sets the root of the linked list to this cell
}

///Dunction kills the rabbit
int killRabbit (struct worldCell world[WORLDX][WORLDY], struct rabbitType **rabbitList, struct rabbitType *currentRabbit){
    struct rabbitType *tmp;
    addSuroundingCells (world,currentRabbit -> x, currentRabbit -> y,-1); ///subtract 1 from surounding cells
    world [currentRabbit -> x][currentRabbit -> y].occupyingRabbit = NULL; ///Sever the link to this rabbit
    if (currentRabbit -> previousRabbit == NULL){ /// If start of the linked list
        (*rabbitList) = currentRabbit -> nextRabbit; ///set the root of the list to the next cell
        free(currentRabbit); ///Frees the memory
        currentRabbit = (*rabbitList); ///reset current cell
        if (currentRabbit != NULL) ///Make sure that the current rabbit is not a null pointer
            currentRabbit -> previousRabbit = NULL; ///set the previous cell to null
    }
    else{
        currentRabbit -> previousRabbit -> nextRabbit = currentRabbit -> nextRabbit; ///Set the previous cell to the cell after the current cell
        if (currentRabbit -> nextRabbit != NULL) ///checks that there is a next pointer
            currentRabbit -> nextRabbit -> previousRabbit = currentRabbit -> previousRabbit; ///Sets the next cell previous cell to the current's cell previous cell
        tmp = currentRabbit;
        free(tmp); ///Frees the memory
    }
}

///Frees the memory for the Tabbit type Linked List
int clearRabbitList (struct rabbitType *rabbitList){
    if (rabbitList == NULL); ///Checks for null pointer
        return -1;

    struct rabbitType  *nextCell = rabbitList -> nextRabbit;

    while (nextCell -> nextRabbit != NULL){
        free(nextCell -> previousRabbit);
        nextCell = nextCell -> nextRabbit;
    }
    free(nextCell -> previousRabbit);
    free(nextCell);
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                              Wolf Functions                              ///
///                                                                          ///
///--------------------------------------------------------------------------///

int createOriginalWolves (struct wolfType *wolf, int GENDER){
    /// Sets the values of the node
    wolf -> currentState.x = 0;
    wolf -> currentState.y = 0;
    wolf -> currentState.health = INITIAL_HEALTH;

    ///Characteristics
    wolf -> characteristic.gender = GENDER;
    wolf -> characteristic.agression = INITIAL_AGRESSION;
    wolf -> characteristic.reproductiveRate = INITIAL_REPRODUCTIVE_RATE;
}

///Moves the proto wolves to a new location
int moveWolves (struct wolfType *wolf1, struct wolfType *wolf2, int X, int Y){
    wolf1 -> currentState.x = X;
    wolf1 -> currentState.y = Y;
    wolf2 -> currentState.x = X;
    wolf2 -> currentState.y = Y;
}

///Averages the characteristic and adds a slight mutation
int averageCharacteristic (int mom, int dad, int min, int max, int shift){
    int n = rb (-shift,shift) + (mom + dad)/2;
    if (n < min)
        n = min;
    if (n > max)
        n = max;
    return n;
}

///Averages the characteristic and adds a slight mutation
double averageCharacteristicFloat (double mom, double dad, double min, double max, double shift){
    float n = rbFloat (-shift,shift) * 1.0 + 1.0 * ((mom + dad) * 1.0)/(2.0);
    if (n < min)
        n = min * 1.0;
    if (n > max)
        n = max * 1.0;
    return n * 1.0;
}

///Counts the number of free cells around the mother and return it
int freeCells (struct worldCell world [WORLDX][WORLDY],struct wolfType *mom){
    int i,j;
    int counter = 0;

    for (i = (mom -> currentState.x) - 1; i <= (mom -> currentState.x) + 1; i++)
        for (j = (mom -> currentState.y) - 1; j <= (mom -> currentState.y) + 1; j++)
            if (world[i][j].entity == EMPTY || world[i][j].entity == RABBIT)
                counter += 1;
    return counter;
}

///Checks to make sure that the dad and the mom can mate
int checkMate (struct worldCell world[WORLDX][WORLDY],struct wolfType *dad, struct wolfType *mom){
    if (dad == NULL || mom == NULL) ///Checls that dad and mom exist
        return 0;
    else if (dad -> statistics.mother == mom || mom -> statistics.father == dad) ///Checks that the wolves aren't inbreeding
        return 0;
    else if (dad -> currentState.health < dad -> characteristic.reproductiveRate || mom -> currentState.health < mom -> characteristic.reproductiveRate) ///If parents don't have enough health to reproduce
        return 0;
    else if (freeCells (world,mom) == 0)///No free cells surounding mother
        return 0;
    else if (dad -> characteristic.gender != MALE_WOLF || mom -> characteristic.gender != FEMALE_WOLF)///Not proper genders
        return 0;
    else
        return 1;
}

///Picks new coordinates for birthed wolf
int findNewCoordinates (struct worldCell world[WORLDX][WORLDY],int momX, int momY, int *newX, int *newY){
    do{
        *newX = rb(momX - 1, momX + 1);
        *newY = rb(momY - 1, momY + 1);
    }while (!inBounds(*newX,*newY) || world[*newX][*newY].entity == MALE_WOLF || world[*newX][*newY].entity == FEMALE_WOLF);
}

///Births a new wolf
int createNewWolf (struct worldCell world[WORLDX][WORLDY], struct wolfType **wolfList, struct wolfType *dad, struct wolfType *mom, struct rabbitType **rabbbitList, struct statsType *stats, int mode){
    if (mode == 0){///If during gameplay
        if (checkMate (world,dad,mom) == 0) ///If something is wrong
            return 0;
    }

    struct wolfType *newWolf = (struct wolfType*) malloc( sizeof(struct wolfType)); ///get memory for a wolf

    ///Characteristics
    newWolf -> characteristic.age = 0;
    newWolf -> characteristic.agression = averageCharacteristicFloat(mom -> characteristic.agression, dad -> characteristic.agression,MIN_AGRESSION,MAX_AGRESSION,0.1);
    newWolf -> characteristic.gender = rb(MALE_WOLF,FEMALE_WOLF);
    newWolf -> characteristic.reproductiveRate = averageCharacteristic (mom -> characteristic.reproductiveRate, dad -> characteristic.reproductiveRate,MIN_REPRODUCTIVE_RATE,MAX_REPRODUCTIVE_RATE,1);

    ///Current State
    if (mode != 0){
        newWolf -> currentState.x = mom -> currentState.x;
        newWolf -> currentState.y = mom -> currentState.y;
    }
    else
        findNewCoordinates(world,mom -> currentState.x, mom -> currentState.y,&(newWolf -> currentState.x),&(newWolf -> currentState.y));
    newWolf -> currentState.health = averageCharacteristic (mom -> currentState.health, dad -> currentState.health,MIN_HEALTH,MAX_HEALTH,1);

    ///Current Statistics
    newWolf -> statistics.averageHealth = newWolf -> currentState.health;
    if (mode == 0){
        newWolf -> statistics.father = dad;
        newWolf -> statistics.mother = mom;
    }
    else{
        newWolf -> statistics.father = NULL;
        newWolf -> statistics.mother = NULL;
    }
    newWolf -> statistics.malesBirthed = 0;
    newWolf -> statistics.femalesBirthed = 0;
    newWolf -> statistics.rabbitsEaten = 0;
    newWolf -> statistics.wolvesFought = 0;

    ///Update World
    if (world[newWolf -> currentState.x][newWolf -> currentState.y].entity == RABBIT){
        (newWolf -> currentState.health)+= RABBIT_FOOD_BENEFIT;
        killRabbit(world,rabbbitList,world[newWolf -> currentState.x][newWolf -> currentState.y].occupyingRabbit);
    }
    world[newWolf -> currentState.x][newWolf -> currentState.y].entity = newWolf -> characteristic.gender;
    world[newWolf -> currentState.x][newWolf -> currentState.y].occupyingWolf = newWolf;

    ///Stats
    if (newWolf -> characteristic.gender == MALE_WOLF)
        (stats -> maleWolves.numLive) += 1;
    else
        (stats -> femaleWolves.numLive) += 1;

    ///Linked List
    newWolf -> nextWolf = (*wolfList);
    if ((*wolfList) != NULL) ///If not NULL set the previous wolf to the currentWolf
        (*wolfList) -> previousWolf = newWolf;
    newWolf -> previousWolf = NULL;
    (*wolfList) = newWolf;
}

///Kills wolf
int killWolf (struct worldCell world[WORLDX][WORLDY], struct wolfType** wolfList,struct wolfType** deadWolfList, struct wolfType *wolf, struct statsType *stats){
    if (wolf == NULL) ///If the wolf doesn't exist
        return 0;

    if (wolf -> previousWolf == NULL){ ///If first wolf
        if (wolf -> nextWolf != NULL){ ///If next wolf exits
            wolf -> nextWolf -> previousWolf = NULL; ///Set the next wolf's previous wolf to 0
            (*wolfList) = wolf -> nextWolf; ///Sets the head pointer to the next wolf
        }
        else
            (*wolfList) = NULL; ///Since there are no wolves left
    }
    else{///If not the first wolf
        if (wolf -> nextWolf != NULL){ ///If next wolf exits
            wolf -> previousWolf -> nextWolf = wolf -> nextWolf; ///Sets the previous wolf
            wolf -> nextWolf -> previousWolf = wolf -> previousWolf; ///Sets the next wolf
        }
        else{///If this is the last wolf in the list
            wolf -> previousWolf -> nextWolf = NULL; ///Sets the
        }
    }

    ///Put the wolf into the deadwolf List
    wolf -> nextWolf = (*deadWolfList);
    wolf -> previousWolf = NULL;
    if ((*deadWolfList) != NULL) ///If there are wolves in deadwolf
        (*deadWolfList) -> previousWolf = wolf;
    (*deadWolfList) = wolf;

    ///Fix stats
    if (wolf -> characteristic.gender == MALE_WOLF)
        stats -> maleWolves.numLive -= 1;
    else
        stats -> femaleWolves.numLive -= 1;

    ///Fix World
    world[wolf -> currentState.x][wolf -> currentState.y].entity = EMPTY;
    world[wolf -> currentState.x][wolf -> currentState.y].occupyingWolf = NULL;
}

///Checks if there should even be a fight
int checkFight (struct wolfType* aggressor, struct wolfType* defender){
    if (aggressor == NULL || defender == NULL) ///Checls that dad and mom exist
        return 0;
    else if (aggressor -> statistics.mother == defender || aggressor -> statistics.father == defender) ///Checks that the wolves aren't fighting grandparents
        return 0;
    else if (defender -> statistics.mother == aggressor || defender -> statistics.father == aggressor) ///Checks that the wolves aren't fighting grandparents
        return 0;
    else
        return 1;
}

///Manges the fight between two wolves
int fight (struct worldCell world[WORLDX][WORLDY],struct wolfType **wolfList, struct wolfType **deadWolfList, struct wolfType* aggressor, struct wolfType* defender, struct statsType *stats){
    if (checkFight(aggressor,defender) == 0)///If failure
        return 0; ///Return Failure

    double chanceA = (aggressor -> characteristic.agression * AGRESSION_MULTIPLIER * 1.0) +
                     (aggressor -> characteristic.age * AGE_MULTIPLIER * 1.0) +
                     (aggressor -> statistics.wolvesFought * WOLVES_FOUGHT_MULTIPLIER * 1.0) +
                     (aggressor -> currentState.health * HEALTH_MULTIPLIER * 1.0) +
                     (ATTACKING_MULTIPLIER * 1.0);
    double chanceD = (defender -> characteristic.agression * AGRESSION_MULTIPLIER * 1.0) +
                     (defender -> characteristic.age * AGE_MULTIPLIER * 1.0) +
                     (defender -> statistics.wolvesFought * WOLVES_FOUGHT_MULTIPLIER * 1.0) +
                     (defender -> currentState.health * HEALTH_MULTIPLIER * 1.0);
    double result = rbFloat(0,chanceA + chanceD);

    if (result <= chanceA){///Agressor won

        ///Empty old cell in world
        world[aggressor -> currentState.x][aggressor -> currentState.y].entity = EMPTY;
        world[aggressor -> currentState.x][aggressor -> currentState.y].occupyingWolf = NULL;

        ///Update Node
        aggressor -> currentState.x = defender -> currentState.x;
        aggressor -> currentState.y = defender -> currentState.y;

        ///kill the wolf
        killWolf(world,wolfList,deadWolfList,defender,stats);

        ///Update World
        world[aggressor -> currentState.x][aggressor -> currentState.y].entity = aggressor -> characteristic.gender;
        world[aggressor -> currentState.x][aggressor -> currentState.y].occupyingWolf = aggressor;
    }
    else{ ///Defender Won
        killWolf(world,wolfList,deadWolfList,aggressor,stats);
    }
    ///Update stats
    aggressor -> statistics.wolvesFought += 1;
    defender -> statistics.wolvesFought += 1;
}

int shuffleWolves (struct wolfType **wolfList){


}

///Generate new coordinates that are not the same and that are inbounds
int newCoordinates (int x, int y, int *newX, int *newY){
    do{
        *newX = rb(x - 1, x + 1);
        *newY = rb(y - 1, y + 1);
    }while (!inBounds(*newX, *newY) || (*newX == x && *newY == y));
}

///Function that allows a wolf to eat a rabbit
int eatRabbit (struct worldCell world[WORLDX][WORLDY],struct wolfType **wolfList,struct wolfType *wolf, struct rabbitType **rabbitList,struct rabbitType *rabbit,struct statsType *stats){
    ///Update Node Stats
    (wolf -> currentState.health) += RABBIT_FOOD_BENEFIT;
    (wolf -> statistics.rabbitsEaten) += 1;

    ///Update World
    world[rabbit -> x][rabbit -> y].entity = wolf -> characteristic.gender;
    world[rabbit -> x][rabbit -> y].occupyingWolf = wolf;
    world[wolf -> currentState.x][wolf -> currentState.y].entity = EMPTY;
    world[wolf -> currentState.x][wolf -> currentState.y].occupyingWolf = NULL;

    ///Move wolf in node
    wolf -> currentState.x = rabbit -> x;
    wolf -> currentState.y = rabbit -> y;

    ///Kill Rabbit
    killRabbit(world,rabbitList,rabbit);
    (stats -> rabbit.numLive) -= 1;
}

int wolvesInteract(struct worldCell world[WORLDX][WORLDY],struct wolfType **wolfList, struct wolfType **deadWolfList, struct wolfType *wolf1,struct wolfType *wolf2,struct rabbitType **rabbitList ,struct statsType *stats){
    int gender1 = wolf1 -> characteristic.gender;
    int gender2 = wolf2 -> characteristic.gender;

    if (gender1 == gender2){ ///If same genders
        fight(world,wolfList,deadWolfList,wolf1,wolf2,stats);
    }
    else if(gender1 == FEMALE_WOLF && gender2 == MALE_WOLF){
        createNewWolf(world,wolfList,wolf2,wolf1,rabbitList,stats,0);
    }
    else if (gender1 == MALE_WOLF && gender2 == FEMALE_WOLF){
        createNewWolf(world,wolfList,wolf1,wolf2,rabbitList,stats,0);
    }
}

///Goes trough all the wolves and updates their location
int updateWolves(struct worldCell world[WORLDX][WORLDY],struct wolfType **wolfList, struct wolfType **deadWolfList,struct rabbitType **rabbitList,struct statsType *stats){
    struct wolfType *currentWolf;
    struct wolfType *tmpWolf;
    int newX, newY;

    shuffleWolves (wolfList); ///Shuffles the wolves

    currentWolf = (*wolfList);

    while (currentWolf != NULL){ ///Iterate through all wolves
        tmpWolf = currentWolf -> nextWolf;

        newCoordinates(currentWolf -> currentState.x, currentWolf -> currentState.y,&newX,&newY);

        (currentWolf -> characteristic.age) += 1;
        (currentWolf -> currentState.health) -= 1;
        (currentWolf -> statistics.averageHealth) += (currentWolf -> currentState.health);

        if (currentWolf -> currentState.health <= 0 && world[newX][newY].entity != RABBIT){///If the wolf should die
            killWolf (world,wolfList,deadWolfList,currentWolf,stats);
        }
        else{///if the wolf should live
            if (world[newX][newY].entity == RABBIT){
                eatRabbit(world,wolfList,currentWolf,rabbitList,world[newX][newY].occupyingRabbit,stats);
            }
            else if (world[newX][newY].entity == EMPTY){
                world[currentWolf -> currentState.x][currentWolf -> currentState.y].entity = EMPTY;
                world[currentWolf -> currentState.x][currentWolf -> currentState.y].occupyingWolf = NULL;
                world[newX][newY].entity = currentWolf -> characteristic.gender;
                world[newX][newY].occupyingWolf = currentWolf;
                currentWolf -> currentState.x = newX;
                currentWolf -> currentState.y = newY;
            }
            else if (world[newX][newY].entity == MALE_WOLF || world[newX][newY].entity == FEMALE_WOLF){
                wolvesInteract (world,wolfList,deadWolfList,currentWolf,world[newX][newY].occupyingWolf,rabbitList,stats);
            }
        }
        currentWolf = tmpWolf;
    }
}

///Frees the memory for the wolfType linked list;
int clearWolfList (struct wolfType *wolfList){
    if (wolfList == NULL); ///Checks for null pointer
        return -1;

    struct wolfType *nextCell = wolfList -> nextWolf;

    while (nextCell -> nextWolf != NULL){
        free(nextCell -> previousWolf);
        nextCell = nextCell -> nextWolf;
    }
    free(nextCell -> previousWolf);
    free(nextCell);
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                           Generate Functions                             ///
///                                                                          ///
///--------------------------------------------------------------------------///

///Sets all elements in matrix World to num
int resetWorld (struct worldCell world[WORLDX][WORLDY],int num){
    int i,j;

    ///Iterate through all the points in the matrix
     for (i = 0; i < WORLDX; i++){
        for (j = 0; j < WORLDY; j++){
            world[i][j].entity = EMPTY;
            world[i][j].neighbourrs = 0;
            world[i][j].occupyingRabbit = NULL;
            world[i][j].occupyingWolf = NULL;
        }
     }
}

///Randomly generates world and fills both layers as well as the liveCellList
int generateWorld (struct worldCell world[WORLDX][WORLDY],struct rabbitType **rabbitList, struct wolfType **wolfList, struct statsType *stats){
    int i,j,n;
    struct wolfType adam, eve;

    ///Generate Proto-wolves
    createOriginalWolves (&adam,MALE_WOLF);
    createOriginalWolves (&eve,FEMALE_WOLF);

    srand(time(NULL)); ///Seed the random number generator
    for (i = 0; i < WORLDX; i++){
        for (j = 0; j < WORLDY; j++){
            n = rb(1,TOTAL_CHANCE); ///Generate a random number
            if (n <= DEAD_CHANCE)
                world [i][j].entity = EMPTY; ///Sets the cell to empty
            else if (n <= RABBIT_CHANCE){
                world [i][j].entity = RABBIT; ///Sets the cell to rabbit
                addSuroundingCells (world,i,j,1); ///Add 1 to the surounding cells in the second layer
                createNewRabbit (world,rabbitList,i,j); ///Adds a rabbit to the rabbit list
                (stats -> rabbit.numLive)++; ///Adds one to the rabbit counter
                }
            else if (n <= WOLF_CHANCE){
                moveWolves (&adam,&eve,i,j); ///Moves protowolves to spot
                createNewWolf(world,wolfList,&adam,&eve,rabbitList,stats,1); ///Adds a wolf to the wolf list
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
int printWorldDebug (struct worldCell world[WORLDX][WORLDY]){
    int i,j;
    ///Iterate through all the points
    for (i = 0; i < WORLDX; i++){
        for (j = 0; j < WORLDY; j++){ ///Prints values in first layer
           if (world [i][j].entity == EMPTY) ///If empty cell
                printf (" ");
            else if (world [i][j].entity == RABBIT) /// if rabbit
                printf ("R");
            else if (world [i][j].entity == MALE_WOLF) /// if rabbit
                printf ("M");
            else if (world [i][j].entity == FEMALE_WOLF) /// if rabbit
                printf ("F");
        }
        printf (" ");
        for (j = 0; j < WORLDY; j++){
            printf ("%i ",world[i][j].neighbourrs); ///Print the value in the second layer
        }
        printf ("\n"); ///prints a new line for the next row
    }
    printf ("\n");
}

///Prints the rabbit list (FOR DEBUG)
int printRabbitList (struct worldCell world[WORLDX][WORLDY], struct rabbitType *rabbitList){
    printf ("Rabbits:\n");
    printf ("X Y will alive neighbours previous current next\n");
        while (rabbitList!= NULL){
            printf( "%i %i %i %i %i %X %X %X\n", rabbitList -> x, rabbitList -> y, rabbitList -> willLive, world [rabbitList -> x][rabbitList -> y].entity,world [rabbitList -> x][rabbitList -> y].neighbourrs,rabbitList -> previousRabbit,rabbitList,rabbitList -> nextRabbit);
            rabbitList = rabbitList -> nextRabbit;
        }
    printf ("\n");
}

///Prints the list of cells (FOR DEBUG)
int printWolfList (struct wolfType *wolfList){
    printf ("\nWolves:\n");
    printf ("X Y health gender age agression reproduction averageHealth\nrabbits_eaten males_birthed females_birthed prev current next\n");
        while (wolfList!= NULL){
            printf( "%i %i %i %i %i %f %i %i %i %i %i %X %X %X\n",
                    wolfList -> currentState.x,
                    wolfList -> currentState.y,
                    wolfList -> currentState.health,
                    wolfList -> characteristic.gender,
                    wolfList -> characteristic.age,
                    wolfList -> characteristic.agression,
                    wolfList -> characteristic.reproductiveRate,
                    wolfList -> statistics.averageHealth,
                    wolfList -> statistics.rabbitsEaten,
                    wolfList -> statistics.malesBirthed,
                    wolfList -> statistics.femalesBirthed,
                    wolfList -> previousWolf, wolfList,
                    wolfList -> nextWolf);
            wolfList = wolfList -> nextWolf;
        }
    printf ("\n");
}

///Prints out all the stats.
int printStats(struct statsType *stats){
    printf("\t\tnumLive\tnumBorn\tnumDied\n");
    printf("Rabbits\t\t%i\t%i\t%i\n",stats -> rabbit.numLive, stats -> rabbit.numBorn, stats -> rabbit.numDied);
    printf("MaleWolf\t%i\t%i\t%i\n",stats -> maleWolves.numLive, stats -> maleWolves.numBorn, stats -> maleWolves.numDied);
    printf("FemaleWolf\t%i\t%i\t%i\n",stats -> femaleWolves.numLive, stats -> femaleWolves.numBorn, stats -> femaleWolves.numDied);
}

///Prints the world (part of the game)
int printWorld (struct worldCell world[WORLDX][WORLDY]){
    int i,j;

    ///Iterate through all the points
    for (i = 0; i < WORLDX; i++){
        for (j = 0; j < WORLDY; j++){
            if (world [i][j].entity == EMPTY) ///If empty cell
                printf (" ");
            else if (world [i][j].entity == RABBIT) /// if rabbit
                printf ("R");
            else if (world [i][j].entity == MALE_WOLF) /// if rabbit
                printf ("M");
            else if (world [i][j].entity == FEMALE_WOLF) /// if rabbit
                printf ("F");
        }
        printf ("\n"); ///prints a new line for the next row
    }
}

///Outputs the world
int printDebug (struct worldCell world[WORLDX][WORLDY], struct rabbitType *rabbitList, struct wolfType *wolfList,struct wolfType *deadwolfList, struct statsType *stats,int t){
    printf ("\n");
    printWorldDebug (world); ///Prints both laters of world

    ///Prints stats table
    printStats (stats);
    printf ("time iteration: %i",t);

    ///Prints the list of rabits and wolves
    //printRabbitList (world,rabbitList);
    printWolfList (wolfList);
    //printWolfList (deadwolfList);

     printf ("\n");
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                                    Game                                  ///
///                                                                          ///
///--------------------------------------------------------------------------///

///Resets variables at the begining of the iteration
int resetStatsIteration (struct statsType *stats){
    stats -> rabbit.numBorn = 0;
    stats -> rabbit.numDied = 0;
    stats -> maleWolves.numBorn = 0;
    stats -> maleWolves.numDied = 0;
    stats -> femaleWolves.numBorn = 0;
    stats -> femaleWolves.numDied = 0;
}


int resetStats (struct statsType *stats){
    stats -> rabbit.numLive = 0;
    stats -> maleWolves.numLive = 0;
    stats -> femaleWolves.numLive = 0;
}

int checkRabbitList (struct worldCell world[WORLDX][WORLDY], struct rabbitType *rabbitList,struct rabbitType **bornRabbitList, struct statsType *stats){
    struct rabbitType *currentRabbit = rabbitList;
    int i,j;

    while (currentRabbit != NULL){ ///Iterate through liveCellList
        ///Iterate through surrounding cells
        for (i = (currentRabbit -> x) - 1; i <= (currentRabbit -> x) + 1; i++){
            for (j = (currentRabbit -> y) - 1; j <= (currentRabbit -> y) + 1; j++){
                if (inBounds (i,j)){ /// If the cell is inbounds
                    if (i == (currentRabbit -> x) && j == (currentRabbit -> y)){ ///If the cell is the center cell
                        if (!(inBetween(world [i][j].neighbourrs,RABBIT_LIVE_MIN,RABBIT_LIVE_MAX))){ /// If cell does not have 3 of 4 neighbours
                            currentRabbit -> willLive = DYING; ///sets that the cell will die
                            (stats -> rabbit.numDied)++; ///Updates NumDied
                        }
                    }
                    else if (world [i][j].entity == EMPTY){ ///If the cell is not in the center cell and the cell is empty
                        if (inBetween(world [i][j].neighbourrs,RABBIT_BORN_MIN,RABBIT_BORN_MAX)){ /// If cell has 4 neighbours
                            createNewRabbit (world,bornRabbitList,i,j); ///Adds a cell to the end of newCells
                            world [i][j].entity = RABBIT_BORN; ///notes that this cell has already been put into the newCell list
                            (stats -> rabbit.numBorn)++; ///Updates NumBorn
                        }
                    }
                }
            }
        }
    currentRabbit = currentRabbit -> nextRabbit; ///Go to next liveCell
    }
}

int updateRabbit (struct worldCell world[WORLDX][WORLDY], struct rabbitType **rabbitList, struct rabbitType *bornRabbitList,struct statsType *stats){
    struct rabbitType *currentRabbit = *rabbitList;
    struct rabbitType *tmp = NULL; ///Counter 1 less then currentRabbit
    int i,j;
    while (currentRabbit != NULL){ ///Iterate through liveCellList
        tmp = currentRabbit -> previousRabbit;
        if (currentRabbit -> willLive == DYING){ ///If the cell will die
            world [currentRabbit -> x][currentRabbit -> y].entity = EMPTY; ///set the cell to dead on the first layer
            killRabbit (world,rabbitList,currentRabbit);
        }
        currentRabbit = currentRabbit -> nextRabbit; ///Sets the current cell to the next cell
    }
    if (tmp == NULL) ///If rabbit list empty
        *rabbitList = bornRabbitList;
    else{
        while (tmp -> nextRabbit != NULL)
            tmp = tmp -> nextRabbit; ///Double check to make sure that you permute to next cell
        tmp -> nextRabbit = bornRabbitList; ///Concatinates both lists
        currentRabbit = bornRabbitList; ///Set the current cell to start of born list
        if (bornRabbitList != NULL) ///if the list is not empty
            currentRabbit -> previousRabbit = tmp; ///concatinate through previous cell
    }
    while (currentRabbit != NULL){ ///Iterate through newCells
        addSuroundingCells (world,currentRabbit -> x, currentRabbit -> y,1); ///add 1 on surounding cells
        world [currentRabbit -> x][currentRabbit -> y].entity = RABBIT; ///set the cell to alive on the first layer
        currentRabbit = currentRabbit -> nextRabbit; ///Sets the current cell to the next cell
    }
    ///Update numLive
    stats -> rabbit.numLive += (stats -> rabbit.numBorn - stats -> rabbit.numDied);
}

int gameOfLife (){
    ///Variable Declaration
    struct worldCell world [WORLDX][WORLDY];
    struct statsType stats;
    struct rabbitType *rabbitList = NULL;
    struct rabbitType *bornRabbitList = NULL;
    struct wolfType *wolfList = NULL;
    struct wolfType *deadWolfList = NULL;
    FILE *fp;
    int t;

    ///Begining
    fp = fopen("popData.txt", "w"); ///Open File
    resetWorld (world, EMPTY); ///Resets the world
    resetStats (&stats);

    generateWorld (world,&rabbitList,&wolfList,&stats); ///Generate random world and liveCellList

    for (t = 0; t < TIME; t++){
        resetStatsIteration (&stats); ///numBorn and numDied
        bornRabbitList = NULL;
        checkRabbitList(world,rabbitList,&bornRabbitList,&stats);
        updateRabbit(world,&rabbitList,bornRabbitList,&stats);
        updateWolves(world,&wolfList,&deadWolfList,&rabbitList,&stats);

        ///*
        ///Output World
        if (DEBUG_MODE == 0){
            system ("cls");///Clear Screen
            printWorld (world); ///Prints the world
            delay (PERIOD); ///Creates delay
        }
        else{ /// if debugging
            printDebug (world,rabbitList,wolfList,deadWolfList,&stats,t); ///Print Debug Data
            system("pause"); ///Pause program each iteration
        }
        //*/
        /*
        fprintf(fp, "%i\t%i\t%i\t%f\n", t,
                stats.rabbit.numLive,
                stats.maleWolves.numLive + stats.femaleWolves.numLive,
                (1000.0 * (stats.maleWolves.numLive + stats.femaleWolves.numLive + stats.rabbit.numLive))/(WORLDX * WORLDY * 1.0));
        */
    }

    ///Clear all the linked lists
    clearRabbitList (rabbitList);
    clearRabbitList (bornRabbitList);
    clearWolfList (wolfList);
    clearWolfList (deadWolfList);

    if (DEBUG_MODE == 0) ///If not debugging
        system ("cls"); ///Clear screen at the end
}

///--------------------------------------------------------------------------///
///                                                                          ///
///                                    Main                                  ///
///                                                                          ///
///--------------------------------------------------------------------------///

int main (){
    WORLDX = 24; ///24 for optimal display
    WORLDY = 79; ///79 for optimal display
    TIME = 100; ///Iterations
    srand(time(NULL)); ///Seed the random number generator
    gameOfLife ();
   }
