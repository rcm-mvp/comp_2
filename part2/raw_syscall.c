#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    const char *msg = "hello from raw syscall\n";
    long ret = syscall(SYS_write, STDOUT_FILENO, msg, 23);
    printf("return value: %ld\n", ret);
    return 0;
}
