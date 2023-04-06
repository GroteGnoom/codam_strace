
all: strace

strace: strace.c
	gcc -Wall -Wextra strace.c -o strace

