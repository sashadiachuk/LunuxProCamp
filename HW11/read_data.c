#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MEM_BASE 0x9f200000
#define MEM_SIZE	(4096)


int main(int argc, char **argv)
{
	volatile unsigned int *mem_addr = NULL;
	unsigned int i, num = 0, val;
	unsigned int jiffies_data = 0;

	int fd = open("/dev/mem",O_RDWR|O_SYNC);//emulate device via dev/mem
	if(fd < 0)
	{
		printf("Can't open /dev/mem\n");
		return -1;
	}
	
	//map registers of the device in address space of our machine 
	mem_addr = (unsigned int *) mmap(0, MEM_SIZE, PROT_WRITE |PROT_READ, MAP_SHARED, fd, MEM_BASE);
	if(mem_addr == NULL)
	{
		printf("Can't  time_addr mmap\n");
		return -1;
	}
	printf("%lu\n", *mem_addr);
	
	return 0;
}
