#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

int main(void) {
    char *buf = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buf == MAP_FAILED) return 1;

    strcpy(buf, "hello mmap");
    write(1, buf, 10);
    write(1, "\n", 1);

    munmap(buf, 4096);
    return 0;
}
