#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
/*#include "linked-list.c"
#define XCOORDS 40
#define YCOORDS 30

/*int length = 4;

//0 = east, 1 = south, 2 = west, 3 = north
int direction = 0;

struct coordinate* createNewFood()
{
	//Generate new food object at a random coordinate (not under snake)
	random = lol;
	if (isCoordinateOnSnake(random)){
		createNewFood();
	} else {
		return random;
	}
}

void moveSnake()
{
	//Move the snake

	//check if new coordinate contains food.
	if (coordinateIsFood()){
		createNewFood();
	}
}

void moveIsOK()
{
	//Check if snake hits the wall or itself.
	if(isCoordinateOnSnake() || hitTheWall()){

	}
}

void isCoordinateOnSnake(){

}

void hitTheWall(){

}

void gameOver()
{

}

void timerInterrupt()
{

	if(moveIsOK()){
		moveSnake();
	}else{
		gameOver();
	}
}*/

int main(int argc, char *argv[])
{
	printf("Hello, I'm game!\n");

	char buf[100];
	char i = 0;
	memset(buf, 0, 100);

	int fp = open("/dev/driver-gamepad", O_RDONLY);

	while(1){
		while(read(fp, &buf[i++],100));
		printf("%s", buf);

			
	}
	
	exit(EXIT_SUCCESS);
}