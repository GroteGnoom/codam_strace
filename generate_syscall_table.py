#!/bin/env python3
import subprocess
with open("/home/daniel/Documents/linux/arch/ia64/kernel/syscalls/syscall.tbl", 'r') as tablefile:
    for line in tablefile.readlines()[:]:
        if line[0] == '#':
            continue
        if line[0] == '0':
            continue
        sline = line.split()
        print(sline)
        out = subprocess.run(["rg", "SYSCALL_DEFINE.\(" + sline[2] + "," , "/home/daniel/Documents/linux", "-A", "7"], capture_output = True, text = True).stdout
        sout = out.split('\n')
        #getpid has no arguments
        #print(len(sout))
        #print(len(sout[0]))
        if len(sout[0]) == 0:
            continue
        for i in range(7):
            print(sout[i])
            if sout[i][-1] == ")":
                break
        if i > 5:
            print("more than 6")
            exit()


# for complex commands, with many args, use string + `shell=True`:
#cmd_str = "ls -l /tmp | awk '{print $3,$9}' | grep root"
#subprocess.run(cmd_str, shell=True)


