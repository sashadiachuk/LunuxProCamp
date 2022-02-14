/*
 * This is an example from man syscall(2).
 *
 * Please note, the call of getpid() should return the same value as tid.
 *
 * The "return 0;" statement in this example is never reached. It is kept
 * to prevent compiler warning/error only.
 *
 * From signal(7):
 * ...
 * Signal	Value	Action	Comment
 * --------------------------------------------------------------------
 * SIGHUP	1	Term	Hangup detected on controlling terminal
 *				or death of controlling process
 * ...
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	pid_t tid;

	tid = syscall(SYS_gettid);
	syscall(SYS_tgkill, getpid(), tid, SIGHUP);

	return 0;
}
