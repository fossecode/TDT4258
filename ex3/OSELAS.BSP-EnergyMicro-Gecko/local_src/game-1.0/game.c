#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include "linked-list.c"
#define XCOORDS 40
#define YCOORDS 30

//Prototype functions;
void createNewFood();
void moveSnake();
bool moveIsOK(struct coordinate coordinate);
bool isCoordinateOnSnake(struct coordinate coordinate);
bool hitTheWall();
void gameOver();
void timerInterrupt();
bool coordinateIsFood(struct coordinate coordinate);
void changeSnakeDirection();
void printGame();

//0 = east, 1 = south, 2 = west, 3 = north. Starting direction east.
int direction = 0;

struct coordinate *food = NULL;

void createNewFood()
{
	if (food == NULL)
		food = (struct coordinate*)malloc(sizeof(struct coordinate));

	//Generate new food object at a random coordinate (not under snake)
	food->x = rand() % XCOORDS;
	food->y = rand() % YCOORDS;

	if (isCoordinateOnSnake(*food)){
		printf("\nHit snake on: [x: %d, y: %d], trying to generate food again.",food->x, food->y);
		createNewFood();
	} else {
		printf("\nFood generated on [x: %d, y: %d] \n",food->x, food->y);
	}
	return;
}

void moveSnake()
{
	//Move the snake
	struct coordinate *head = get_head_of_snake();
	struct coordinate *newCoord = (struct coordinate*)malloc(sizeof(struct coordinate));
    switch (direction){
    	case 0: //Go east
    		newCoord->x = head->x + 1;
    		newCoord->y = head->y;
    		break;
    	case 1: //Go south
    		newCoord->y = head->y - 1;
    		newCoord->x = head->x;
    		break;
    	case 2: //Go west
    		newCoord->x = head->x - 1;
    		newCoord->y = head->y;
    		break;
    	case 3: //Go north
    		newCoord->y = head->y + 1;
    		newCoord->x = head->x;
    		break;
    }

    
    if(! moveIsOK(*newCoord))
    	gameOver();
	else{
	    //Add to beginning of snake
	    add_to_list(newCoord->x, newCoord->y, false);

		//check if new coordinate contains food.
		if (coordinateIsFood(*newCoord)){
			printf("Found food on: [%d, %d] ;) YUM YUM\n", newCoord->x, newCoord->y);
			createNewFood();
		} else {
			//If no food, just remove the last element of the snake.
			delete_last();
		}
	}
}

bool moveIsOK(struct coordinate newCoord)
{
	//Check if snake hits the wall or itself.
	if(isCoordinateOnSnake(newCoord) || hitTheWall(newCoord)){
		return false;
	}
	return true;
}

bool isCoordinateOnSnake(struct coordinate coord)
{
	struct coordinate *ptr = &coord;
	return search_in_list(ptr->x, ptr->y);
}

bool coordinateIsFood(struct coordinate coord)
{
	struct coordinate *ptr = &coord;
	return (ptr->x == food->x && ptr->y == food->y);
}

bool deleteThisFunctionCoordIsFood(int x, int y){
	return (x == food->x && y == food->y);
}

bool hitTheWall(struct coordinate coord)
{
	struct coordinate *ptr = &coord;
	return (ptr->x > 40 || ptr->x <= 0 || ptr->y > 30 || ptr->y <= 0);
}

void gameOver()
{
	printf("GAME OVER\n");
	exit(EXIT_SUCCESS);
	//Must implement something that resets game here.
}

void timerInterrupt()
{
	moveSnake();
}

void initSnake()
{
	//Create a snake with a length of 4 in the middle of the screen
	int length = 4;
	int y = 15; //Middle of screen
	for(int x = 1; x<=length; x++)
        add_to_list(x, y, false);
    //Generate food
    createNewFood();
    printGame();
}

void changeSnakeDirection(int dir)
{
	direction = dir;
}

void printGame(){
	printf("__________________________________________________________________________________\n");
	for (int y = 30; y>0; y--){
		printf("|");
		for (int x = 1; x<=40; x++){
			if (search_in_list(x,y)){
				printf(" x");
			}else if (deleteThisFunctionCoordIsFood(x,y)){
				printf(" o");
			}
			else{
				printf("  ");
			}
		}
		printf("|\n");
	}
	printf("__________________________________________________________________________________\n");
}


int main(int argc, char *argv[])
{

	int fp = open("/dev/driver-gamepad", O_RDONLY);
	//Use current time as seed for random generator
	srand(time(NULL));
	initSnake();
	while (1){
		sleep(1);
		char buf[100];
		char i = 0;
		memset(buf, 0, 100);
		while(read(fp, &buf[i++],100));
		int tempDir;
		if(strcmp(buf, "none") == 0){
			tempDir = 5;
		}else if(strcmp(buf, "right") == 0){
			tempDir = 0;
		}else if(strcmp(buf, "left") == 0){
			tempDir = 2;
		}else if(strcmp(buf, "up") == 0){
			tempDir = 3;
		}else if(strcmp(buf, "down") == 0){
			tempDir = 1;
		}
		if(tempDir != 5)
			changeSnakeDirection(tempDir);
		
		moveSnake();
		printGame();

		

		
	}

	//printf("Hello World, I'm game!\n");



	exit(EXIT_SUCCESS);
}
