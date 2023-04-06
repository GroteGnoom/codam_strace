#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ptrace.h>

int main(int argc, char **argv) {
	int request;
	pid_t pid;
	caddr_t addr;
	int data;
	int error;

	request = PT_ATTACHEXC;
	if (argc != 2) {
		printf("only 1 argument: pid\n");
		exit(1);
	}
	pid = atoi(argv[1]);

	addr = 0; // is ignored
	data = 0; // is ignored

	error = ptrace(request, pid, addr, data);
	printf("error: %d\n", error);
	perror("bla");
}

