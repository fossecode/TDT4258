#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

void interrupt_handler(int n, siginfo_t *info, void *unused)
{
	uint8_t buttons = (uint8_t) (info->si_int);
	int newx = posX;
	int newy = posY;
	
	//Checks the button pressed values
	if(buttons == 1)
	{
		newx--;
	}
	else if(buttons == 2)
	{
		newy--;
	}
	else if(buttons == 4)
	{
		newx++;
	}
	else if(buttons == 8)
	{
		newy++;
	}
	else if(buttons == 128)
	{
		//Checks if the square is taken, if it is free the player will pick it
		if(board[posX][posY] == 0)
			select_frame();
		else
			printf("Square taken\n");
	}
	
	if(newy != posY || newx != posX)
	{
	//Checks if the move is legal, if it is, the active square will be changed
	if(check_move(newx, newy) == 1)
	move(newx, newy);
	else
	printf("Illegal move\n");
	}
}

int main(int argc, char *argv[])
{
	//Use current time as seed for random generator
	srand(time(NULL));
	initSnake();
	while (1){
		char ch = 0;
		printf("Type in a direction WASD and then click enter.\n");
		scanf(" %c", &ch);
		switch(ch){
			case 'w': case 'W':
				changeSnakeDirection(3);
				break;
			case 'a': case 'A':
				changeSnakeDirection(2);
				break;
			case 's': case 'S':
				changeSnakeDirection(1);
				break;
			case 'd': case 'D':
				changeSnakeDirection(0);	
				break;
		}
		moveSnake();
		printGame();
	}

	//printf("Hello World, I'm game!\n");



	exit(EXIT_SUCCESS);
}
