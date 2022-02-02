#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TIME_BASE 0x9f201008
#define TIME_SIZE	(8)

volatile unsigned int *reg_addr = NULL;
volatile unsigned int  *time_addr = 0;


int main(int argc, char **argv)
{
	printf("start\n");
	volatile unsigned char *mem_addr = NULL;
	unsigned int i, num = 0, val;
	unsigned int jiffies_data = 0;

	int fd = open("/dev/mem",O_RDWR|O_SYNC);//emulate device via dev/mem
	if(fd < 0)
	{
		printf("Can't open /dev/mem\n");
		return -1;
	}
	
	/*reg_addr = (unsigned int *) mmap(0, REG_SIZE, PROT_WRITE |PROT_READ, MAP_SHARED, fd, REG_BASE);
	if(reg_addr == NULL)
	{
		printf("Can't reg_addr mmap\n");
		return -1;
	}*/
	//map registers of the device in address space of our machine 
	time_addr = (unsigned int *) mmap(0, TIME_SIZE, PROT_WRITE |PROT_READ, MAP_SHARED, fd, TIME_BASE);
	if(time_addr == NULL)
	{
		printf("Can't  time_addr mmap\n");
		return -1;
	}
	//printf("%d\n", *reg_addr);
	printf("%d\n", *time_addr);
	
	return 0;
}
