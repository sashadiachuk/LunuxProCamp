#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MEM_BASE 0x9f200000
#define REG_BASE  0x9f201000
#define TIME_BASE 0x9f201008
#define MEM_SIZE	(1024)
#define REG_SIZE	(8)
#define TIME_SIZE	(4)

#define PLAT_IO_FLAG_REG		(0) /*Offset of flag register*/
#define PLAT_IO_SIZE_REG		(4) /*Offset of flag register*/
#define PLAT_IO_DATA_READY	(1) /*IO data ready flag */


extern int errno;

int main(int argc, char **argv)
{
	volatile unsigned int *reg_addr = NULL, *time_addr;
	volatile unsigned char *mem_addr = NULL;
	unsigned int i, num = 0, val;
	unsigned int jiffies_data = 0;

	int fd = open("/dev/mem",O_RDWR|O_SYNC);//emulate device via dev/mem
	if(fd < 0)
	{
		printf("Can't open /dev/mem\n");
		return -1;
	}
	
	reg_addr = (unsigned int *) mmap(0, REG_SIZE, PROT_WRITE |PROT_READ, MAP_SHARED, fd, REG_BASE);
	
	//map registers of the device in address space of our machine 
	time_addr = (unsigned int *) mmap(0, TIME_SIZE, PROT_WRITE |PROT_READ, MAP_SHARED, fd, TIME_BASE);
	if(reg_addr == NULL)
	{
		printf("Can't mmap\n");
		return -1;
	}
	printf("tut\n");
	printf("%p\n", time_addr);
	jiffies_data=*time_addr;
	printf("jiffies_data: %u\n", jiffies_data);

	return 0;
}
