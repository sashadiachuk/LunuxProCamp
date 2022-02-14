/*
 * Invoke hello demo system call and print the result.
 */

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#define SYS_hello		402
#define HELLO_STR_LEN_MAX	64

int main(void)
{
	int ret;
	char hello_buf[HELLO_STR_LEN_MAX];

	printf("Invoke SYS_hello (%d) demo system call...\n", SYS_hello);

	ret = syscall(SYS_hello, hello_buf, HELLO_STR_LEN_MAX);

	if (ret == -1)
		printf("failed, %s (%d)\n", strerror(errno), errno);
	else
		printf("success, the result is: \"%s\"\n", hello_buf);

	return ret;
}
