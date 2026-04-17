#include <fcntl.h>
#include <unistd.h>

// open a file, read contents, write to stdout, close
int main(void) {
    char buf[256];

    int fd = open("testfile.txt", O_RDONLY);
    if (fd < 0) return 1;

    int n = read(fd, buf, sizeof(buf));
    if (n > 0)
        write(STDOUT_FILENO, buf, n);

    close(fd);
    return 0;
}
