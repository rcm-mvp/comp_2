#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// run: echo -n "0000" > counter.bin  before first run
int main(void) {
    int fd = open("counter.bin", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char *data = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // parse 4-char ASCII number and increment
    int val = (data[0]-'0')*1000 + (data[1]-'0')*100
            + (data[2]-'0')*10  + (data[3]-'0');
    val++;
    data[0] = '0' + (val / 1000) % 10;
    data[1] = '0' + (val / 100)  % 10;
    data[2] = '0' + (val / 10)   % 10;
    data[3] = '0' + val % 10;

    msync(data, 4, MS_SYNC);
    munmap(data, 4);
    close(fd);
    return 0;
}
