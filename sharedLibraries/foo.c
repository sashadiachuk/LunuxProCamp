#include <stdio.h>
 
extern int testNumber;

void foo(void)
{    
    printf("lib1 -%d\n",testNumber);
    puts("Hello, I am a shared library");
    testNumber++;
}
