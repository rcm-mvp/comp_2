#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(void) {
    int fd = open("input.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct stat st;
    fstat(fd, &st);

    char *data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    int count = 0;
    for (int i = 0; i < st.st_size; i++) {
        if (data[i] == '\n')
            count++;
    }

    printf("newlines: %d\n", count);

    munmap(data, st.st_size);
    close(fd);
    return 0;
}
