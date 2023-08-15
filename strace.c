#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void run_target(char** programname)
{
    printf("target started. will run '%s'\n", programname[0]);

	//TODO is this necessary?
	/*
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        perror("ptrace");
        exit(1);
    }
	*/
	raise(SIGSTOP);

    execv(programname[0], programname);
}

/* arch_prctl(0x3001 /1* ARCH_??? *1/, 0x7ffdd0ae4900) = -1 EINVAL (Invalid argument) */
/* access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory) */
/* openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3 */
/* newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=165227, ...}, AT_EMPTY_PATH) = 0 */

void run_tracer(pid_t child_pid)
{   
	int status;
	bool syscall_enter = true;
	(void) child_pid;
    printf("tracer started\n");

	// Check if the child is stopped by a signal
    struct user_regs_struct regs;
    struct iovec iov = {&regs, sizeof(regs)};

	int request = PTRACE_SEIZE;
	ptrace(request, child_pid, NULL, PTRACE_O_TRACESYSGOOD);
	//PTRACE_SYSCALL  = SIGTRAP on entry or exit of syscall
	while (1) {
        // Wait for the child to stop
        waitpid(child_pid, &status, 0);

        // Check if the child exited
        if (WIFEXITED(status)) {
            break;
        }

        // Get the register state using PTRACE_GETREGSET
		ptrace(PTRACE_GETREGSET, child_pid, NULL, &iov);

        // Print syscall number on syscall entry
        if (syscall_enter) {
            printf("Syscall number: %lld\n", regs.orig_rax);
        }

        // Continue to the next syscall
        syscall_enter = !syscall_enter;
        ptrace(PTRACE_SYSCALL, child_pid, 0, 0);
    }

	/*
	long res = ptrace(PTRACE_SYSCALL, child_pid, addr, data);
	if (res == -1)
		perror("ptrace syscall request");

	//NT_PRSTATUS instead of NULL
	res = ptrace(PTRACE_GETREGSET, child_pid, NULL, &iov);
	if (res == -1)
		perror("ptrace getregset request");
    printf("RIP: 0x%llx\n", regs.rip);
    printf("RSP: 0x%llx\n", regs.rsp);
	*/
	printf("\ndone reading registers\n");
}

int main(int argc, char **argv) {
	//ATTACH sends a SIGSTOP
	//request = PT_ATTACH;
	if (argc < 2) {
		printf("no executable given\n");
		exit(1);
	}

    pid_t child_pid = fork();
    if (child_pid == 0)
        run_target(argv + 1);
    else if (child_pid > 0)
        run_tracer(child_pid);
    else {
        perror("fork");
        return -1;
    }

}

