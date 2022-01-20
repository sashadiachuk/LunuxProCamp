#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define NPAGES 16

/* this is a test program that opens the mmap_drv.
   It reads out values of the kmalloc() and vmalloc()
   allocated areas and checks for correctness.
   You need a device special file to access the driver.
   The device special file is called 'node' and searched
   in the current directory.
   To create it
   - load the driver
     'insmod mmap_mod.o'
   - find the major number assigned to the driver
     'grep mmapdrv /proc/devices'
   - and create the special file (assuming major number 254)
     'mknod node c 254 0'
*/

 /*It used to be the case that kernel threads were discouraged from accessing user space,
  exactly because of the possible page fault hit that might occur, 
 if accessing unpaged/paged out memory in user space (recall, that wouldn't happen in kernel space, as above ensures).
 So copy_to/from would be normal memcpy, but wrapped in a page fault handler. 


As stated previously, the Linux Page Fault exception handler must distinguish exceptions caused by programming errors from those caused by a reference to a page that legitimately belongs to the process address space but simply hasn't been allocated yet. 


Page fault occurs when MMU lookup of a virtual address failed. For example : the page is currently not residing in memory, page is not readable/writable or not permitted for access. 
 In the event of page fault occurs, kernel switches to kernel mode if not already running in kernel mode. Then it calls a special routine called page fault handler. It takes action depending on the reason of page fault.
*/

int main(void)
{
  int fd;
  unsigned int *vadr;
  unsigned int *kadr;

  int len = NPAGES * getpagesize();

  if ((fd=open("node", O_RDWR|O_SYNC))<0)
  {
      perror("open");
      exit(-1);
  }

  vadr = mmap(0, len, PROT_READ, MAP_SHARED, fd, 0);
  
  if (vadr == MAP_FAILED)
  {
          perror("mmap");
          exit(-1);
  }
  if ((vadr[0]!=0xaffe0000) || (vadr[1]!=0xbeef0000)
      || (vadr[len/sizeof(int)-2]!=(0xaffe0000+len/sizeof(int)-2))
      || (vadr[len/sizeof(int)-1]!=(0xbeef0000+len/sizeof(int)-2)))
  {
       printf("0x%x 0x%x\n", vadr[0], vadr[1]);
       printf("0x%x 0x%x\n", vadr[len/sizeof(int)-2], vadr[len/sizeof(int)-1]);
  }
  else
  {
  	 printf("succesful vmmalloc mmaping\n");
  }
  
  kadr = mmap(0, len, PROT_READ|PROT_WRITE, MAP_SHARED| MAP_LOCKED, fd, len);
  
  if (kadr == MAP_FAILED)
  {
          perror("mmap");
          exit(-1);
  }

  if ((kadr[0]!=0xdead0000) || (kadr[1]!=0xbeef0000)
      || (kadr[len / sizeof(int) - 2] != (0xdead0000 + len / sizeof(int) - 2))
      || (kadr[len / sizeof(int) - 1] != (0xbeef0000 + len / sizeof(int) - 2)))
  {
      printf("0x%x 0x%x\n", kadr[0], kadr[1]);
      printf("0x%x 0x%x\n", kadr[len / sizeof(int) - 2], kadr[len / sizeof(int) - 1]);
  }
  {
  	 printf("succesful kmmalloc mmaping\n");
  }
  
  close(fd);
  return(0);
}


