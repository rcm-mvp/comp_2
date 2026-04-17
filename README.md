# IM110 Filesystems

Run `make` to build, then test against the disk images:

```bash
make
./diskimageaccess -ip testdisks/basicDiskImage
./diskimageaccess -ip testdisks/depthFileDiskImage
./diskimageaccess -ip testdisks/dirFnameSizeDiskImage
```

## Part 2 exercises

Each file in `part2/` compiles on its own:

```bash
# raw syscall
gcc -o raw_syscall part2/raw_syscall.c && ./raw_syscall

# strace demo (create a test file first)
echo "hello world" > part2/testfile.txt
gcc -o strace_demo part2/strace_demo.c && strace ./strace_demo

# mmap newline counter (create input first)
echo -e "line1\nline2\nline3" > part2/input.txt
gcc -o mmap_count part2/mmap_count.c && ./mmap_count

# shared mmap counter (run 3 times, should end at 0003)
echo -n "0000" > part2/counter.bin
gcc -o mmap_shared part2/mmap_shared.c
./mmap_shared && ./mmap_shared && ./mmap_shared
cat part2/counter.bin

# anonymous mmap scratchpad
gcc -o mmap_anon part2/mmap_anon.c && ./mmap_anon

# read_full (type 5 chars then enter)
g++ -o read_full part2/read_full.cpp && echo "hello" | ./read_full

# write_full
g++ -o write_full part2/write_full.cpp && ./write_full
```
