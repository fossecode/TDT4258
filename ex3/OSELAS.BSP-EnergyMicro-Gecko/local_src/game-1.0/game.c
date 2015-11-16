#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <linux/fb.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "linked-list.c"
#define XCOORDS 32
#define YCOORDS 24

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

//0 = east, 1 = south, 2 = west, 3 = north. Starting direction east.
int direction = 0;

struct coordinate *food = NULL;

int fbfd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
char *fbp = 0;
int x = 0, y = 0;
long int location = 0;

void createNewFood()
{
	if (food == NULL)
		food = (struct coordinate*)malloc(sizeof(struct coordinate));

	//Generate new food object at a random coordinate (not under snake)
	food->x = rand() % XCOORDS;
	food->y = rand() % YCOORDS;

	if (isCoordinateOnSnake(*food)){
		createNewFood();
	} else {
		drawRect(food->x, food->y, true);
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

    
    if(! moveIsOK(*newCoord)){
    	gameOver();
	} else{
	    //Add to beginning of snake
	    add_to_list(newCoord->x, newCoord->y, false);
	    drawRect(newCoord->x, newCoord->y, false);

		//check if new coordinate contains food.
		if (coordinateIsFood(*newCoord)){
			createNewFood();
		} else {
			//If no food, just remove the last element of the snake.
			struct coordinate *last = delete_last();
			removeRect(last->x, last->y);
			free(last);
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
	return (ptr->x >= XCOORDS || ptr->x < 0 || ptr->y >= YCOORDS || ptr->y < 0);
}

void gameOver()
{
	printf("GAME OVER\n");
	sleep(2);
	clear_list();
   	clearScreen();
   	changeSnakeDirection(0);
   	initSnake();
}

void timerInterrupt()
{
	moveSnake();
}

void initSnake()
{
	//Create a snake with a length of 4 in the middle of the screen
	int length = 4;
	int y = 12; //Middle of screen
	for(int x = 0; x<length; x++)
        add_to_list(x, y, false);
    //Generate food
    createNewFood();
}

void changeSnakeDirection(int dir)
{
	direction = dir;
}

void drawRect(int x, int y, bool food){
	if (food)
		memset(fbp, 0x03F0, screensize);
	else	
		memset(fbp, 0x0F80, screensize);
    struct fb_copyarea rect;
	rect.dx= (x*10) + 1;
	rect.dy= (y*10) + 1;
	rect.width= 8;
	rect.height= 8;

	ioctl(fbfd,0x4680,&rect);
}

void removeRect(int x, int y){
	memset(fbp, 0x0000, screensize);
    struct fb_copyarea rect;
	rect.dx= (x*10) + 1;
	rect.dy= (y*10) + 1;
	rect.width=8;
	rect.height=8;

	ioctl(fbfd,0x4680,&rect);
}

void clearScreen(){
	memset(fbp, 0x0000, screensize);

    struct fb_copyarea rect;
	rect.dx=0;
	rect.dy=0;
	rect.width=320;
	rect.height=240;

	ioctl(fbfd,0x4680,&rect);	
}

int main(int argc, char *argv[])
{

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    screensize = 240 * 320 * 16 / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    
   	clearScreen();

	int fp = open("/dev/driver-gamepad", O_RDONLY);

	if(fp == -1){
		exit(EXIT_SUCCESS);
	}

	//Use current time as seed for random generator
	srand(time(NULL));
	initSnake();
	while (1){
		usleep(75000);
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
		}else if(strcmp(buf, "down") == 0){
			tempDir = 3;
		}else if(strcmp(buf, "up") == 0){
			tempDir = 1;
		}else if(strcmp(buf, "exit") == 0){
			break;
		}

		if(tempDir != 5)
			changeSnakeDirection(tempDir);
		
		moveSnake();
		
	}


	exit(EXIT_SUCCESS);
}
