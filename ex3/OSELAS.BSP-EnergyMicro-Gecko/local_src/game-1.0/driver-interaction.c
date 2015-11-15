#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

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
}

