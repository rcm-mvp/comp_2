#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
int write_full(int fd, const void *buf, size_t n) {
    const char *p = (const char *)buf;
    size_t left = n;

    while (left > 0) {
        ssize_t w = write(fd, p, left);
        if (w == 0) return -1;          // other end won't accept data
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        p    += w;
        left -= w;
    }
    return 0;
}

int main(void) {
    const char *msg = "hello, world\n";
    write_full(STDOUT_FILENO, msg, strlen(msg));

    int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return 1;
    write_full(fd, msg, strlen(msg));
    close(fd);
    return 0;
}
