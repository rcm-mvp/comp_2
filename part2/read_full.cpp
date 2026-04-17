#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int read_full(int fd, void *buf, size_t n) {
    char *p = (char *)buf;
    size_t left = n;

    while (left > 0) {
        ssize_t r = read(fd, p, left);
        if (r == 0) return 1;           // EOF before n bytes
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        p    += r;
        left -= r;
    }
    return 0;
}

int main(void) {
    char buf[5];
    int res = read_full(STDIN_FILENO, buf, 5);
    if (res == 0)
        write(STDOUT_FILENO, buf, 5);
    else
        fprintf(stderr, "read_full failed: %d\n", res);
    return res;
}
