#include <stdio.h>
 
extern int testNumber;

void foo(void)
{    
    testNumber++;
    printf("lib2 -%d\n",testNumber);
    puts("Hello, I am a shared library2");
    testNumber++;
    testNumber++;
}
