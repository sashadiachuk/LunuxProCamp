#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>



#define MEM_BASE1 0x9f200000
#define REG_BASE1  0x9f201000

#define MEM_BASE2 0x9f210000
#define REG_BASE2 0x9f211000

#define MEM_SIZE	0x1000
#define REG_SIZE	8

#define MAX_DEVICES	2

char buffer[100]; 


int main(int argc, char **argv)
{
	volatile unsigned int *reg_addr = NULL, *count_addr, *flag_addr;
	volatile unsigned char *mem_addr = NULL;
	volatile unsigned char *mem_addr1 = NULL;
	unsigned int i, device, ret, len, count;
	struct stat st;
	uint8_t *buf;
	FILE *f;
	
	
	int fd = open("/dev/mem",O_RDWR|O_SYNC);
	if(fd < 0)
	{
		printf("Can't open /dev/mem\n");
		return -1;
	}
	mem_addr = (unsigned char *) mmap(0,MEM_SIZE, PROT_WRITE, MAP_SHARED, fd, MEM_BASE1);
	if(mem_addr == NULL)
	{
		printf("Can't mmap\n");
		return -1;
	}
	mem_addr1 = (unsigned char *) mmap(0,MEM_SIZE, PROT_WRITE, MAP_SHARED, fd, MEM_BASE2);
	if(mem_addr1 == NULL)
	{
		printf("Can't mmap1\n");
		return -1;
	}
	
	
	printf("dummy0 %d\n", *(mem_addr));
	printf("dummy1 %d\n", *(mem_addr1));
	
	return 0;
}
