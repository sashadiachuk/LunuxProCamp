#define _GNU_SOURCE
#include <sys/auxv.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	void *sysinfo_ehdr = (void *)getauxval(AT_SYSINFO_EHDR);
	if (!sysinfo_ehdr) {
		fputs("no AT_SYSINFO_EHDR: architecture w/o vDSO?\n", stderr);
		return EXIT_FAILURE;
	}
	printf("AT_SYSINFO_EHDR: %p\n", sysinfo_ehdr);
	return EXIT_SUCCESS;
}
