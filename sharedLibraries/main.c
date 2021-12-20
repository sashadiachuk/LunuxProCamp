#include <stdio.h>
#include "foo.h"
 
int main(void)
{
    puts("This is a shared library test...");
    foo();
    testNumber++;
    printf("from the app - %d\n", testNumber);
    return 0;
}
