# IM110 – Filesystems

**Author:** Christian Osendorfer  
**Published:** April 2, 2026  
**Other Formats:** RevealJS  

> **Important**
> * Only submit your solution for the programming assignment (Section 3).
> * 🚨 indicates relevance for the final written exam.

---

## Part 1: Written Questions

### Direct, Singly Indirect and Doubly Indirect Block Numbers 🚨
Assume blocks are 512 bytes in size, block numbers are four-byte ints, and that inodes include space for 6 block numbers. The first three contain direct block numbers, the next two contain singly indirect block numbers, and the final one contains a doubly indirect block number. *(Note: This is a different schema than used in the programming assignment below!)*

1. What’s the maximum file size?
2. How large does a file need to be before the relevant inode requires the first singly indirect block number be used?
3. How large does a file need to be before the relevant inode requires the first doubly indirect block number be used?
4. Draw as detailed an inode as you can if it’s to represent a regular file that’s 2049 bytes in size.

### The V6 Filesystem 🚨
*(Note: The following questions refer to a figure discussed in class depicting a snapshot from a structure implementing the Unix V6 filesystem.)*

Answer the following questions in a complete but compact manner. Provide arguments for your answers!

1. The figure starts with a block numbered “2”. Why?
2. What do the first few blocks in this structure contain? Provide details!
3. What does the structure with the number “1” in the last row refer to?
4. What is the content of the block numbered “26”?
5. Block “89” seems to contain text. How is this block related to the rest of the figure? To what file does the block belong to?

### The Three Kernel Objects Behind a File Descriptor 🚨
When a process calls `open("log.txt", O_RDONLY)`, the kernel creates a chain of three data structures. Name each one, state whether it is per-process or shared across processes, and describe the role it plays: what problem it solves and what information it owns.

### Short Questions 🚨
1. Consider the Unix V6 filesystem, which uses 512-byte blocks and 32-byte inodes. Which sector should we read in order to get inode 256? If that sector is an array of inodes, which index should we go to in order to get inode 256? What about inode 345?
2. Unlike the Unix V6 filesystem we learned about in class, the ext2/3/4 family of filesystems (commonly used on Linux) use variable-length directory entries:

```c
struct ext3_dir_entry {  
    uint32_t inode_number;  
    uint16_t entry_length;  
    uint16_t name_length;  
    uint16_t file_type;  
    char[] file_name;
};
```
What is the benefit to designing directory entries this way? What are some drawbacks? *(Consider what happens when deleting files.)*

### The ABI in Practice
Consider a C program that calls `write(STDOUT_FILENO, "ok\n", 3)` through libc.

1. List every register that carries information at the moment the `syscall` instruction fires. For each register, state what value it holds and why.
2. The `syscall` instruction saves only two things automatically. What are they, and why is that not enough for the kernel to do its job? What must the kernel entry stub do on top of that?
3. The call succeeds. What value is in `rax` afterward? Now suppose the caller passes file descriptor `99` (which is not open). What raw value does the kernel place in `rax`, and what does libc’s `write()` return to the caller?

### `MAP_PRIVATE` Does Not Protect You From Everything
A colleague claims: *"I mapped a config file with `MAP_PRIVATE | PROT_READ`, so even if some other process writes to the file, I’m safe — I’ll keep reading my private copy."*

1. Is the colleague right about concurrent in-place writes? Why or why not?
2. Now suppose a deployment script does `ftruncate(fd, 0)` to zero out the config file while the colleague’s process is reading it. What happens? Is it a clean error?
3. What is the simplest defense?

### A Common `errno` Mistake
A student writes:

```c
int fd = open("config.txt", O_RDONLY);
if (errno != 0) {    
    perror("cannot open config");    
    exit(1);
}
```

1. What is wrong with this check? Describe a concrete scenario where this code triggers the error path even though `open` succeeded.
2. Even in a fresh program where `errno` starts at zero, there is a second bug. What is it?
3. Rewrite the check correctly.

---

## Part 2: Short Programming Exercises

### Writing Without libc
Write a minimal C program (under 20 lines) that prints `"hello from raw syscall\n"` to stdout using the `syscall()` function from `<sys/syscall.h>` — **without** calling libc’s `write()`. Store the return value in a `long` and print it.

```c
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {    
    /* your code here */
}
```

1. What does the printed return value represent on success?
2. Now pass file descriptor `999` instead of `STDOUT_FILENO`. What raw value do you get back? What `errno` code does that correspond to? *(Hint: `errno -l` on the command line, or check `<errno.h>`.)*
3. In one sentence: why does libc provide a `write()` wrapper rather than expecting every programmer to call `syscall()` directly?

### Observing Syscalls with `strace`
Write a minimal C program using only raw POSIX calls (`open`, `read`, `write`, `close` — no stdio) that opens a file you create, reads its contents, writes them to stdout, and closes the file. Compile it and run it under `strace`. Then answer:

1. List the syscalls that appear between `openat` and `close`. How many `read` calls were needed?
2. `strace` prints the actual bytes of small buffers, e.g., `read(3, "hello\n", 6) = 6`. How does `strace` know the contents of a pointer it received in a register — considering it is running in a separate process?
3. Pass a path that does not exist. Copy the exact `strace` output line for the failing `openat` call.
4. Explain why running a program under `strace` makes it noticeably slower, in terms of what happens at every syscall boundary.

### Read a File via `mmap`
Write a program that:
1. Opens a file named `input.txt` (create one with any text content first).
2. Uses `fstat` to get its size.
3. Maps it read-only with `mmap`.
4. Counts the number of newline characters `'\n'` by scanning the mapped memory directly.
5. Prints the count.
6. Calls `munmap` and `close`.

*(Do not use `read()` anywhere. Do not use C++ streams for file access.)*

### Shared-Memory Edit via `MAP_SHARED`
Write a program that:
1. Opens a file named `counter.bin` for read/write (create it first: `echo -n "0000" > counter.bin`).
2. Maps it with `PROT_READ | PROT_WRITE` and `MAP_SHARED`.
3. Interprets the mapped bytes as a 4-character ASCII decimal integer.
4. Increments the integer by 1 and writes the result back into the mapped memory.
5. Calls `msync` with `MS_SYNC` to flush to disk.
6. Calls `munmap` and `close`.

*(Run the program three times and verify `counter.bin` ends up containing `"0003"`.)*

### Anonymous `mmap` as a Scratchpad
Write a program that:
1. Uses `mmap` with `MAP_PRIVATE | MAP_ANONYMOUS` (no file descriptor) to allocate 4096 bytes.
2. Writes the string `"hello mmap"` into the buffer.
3. Reads it back and prints it with `write(1, ...)` — using the raw syscall wrapper, not `printf`.
4. Calls `munmap`.

### A Correct `read_full` 🚨
Implement the following function in C++ (≤ 20 lines, no helpers):

```cpp
// Reads exactly n bytes into buf from fd.
// Returns 0 on success, 1 on EOF before n bytes, -1 on error (errno set).
int read_full(int fd, void *buf, size_t n);
```

**Requirements:**
* Handles short reads (the kernel may return fewer bytes than requested).
* Handles `EINTR` (retry without counting it as an error).
* Does not use buffered I/O (`fread`, `std::ifstream`).
* Write a `main()` that calls `read_full` to read exactly 5 bytes from `stdin` and prints them.

### A Correct `write_full`
Implement the following function in C++ (≤ 20 lines, no helpers):

```cpp
// Writes exactly n bytes from buf to fd.
// Returns 0 on success, -1 on error (errno set).
int write_full(int fd, const void *buf, size_t n);
```

**Requirements:**
* Handles short writes (the kernel may accept fewer bytes than requested — common on sockets and pipes).
* Handles `EINTR` (retry without counting it as an error).
* Treats `write()` returning `0` as a permanent error (unlike `read()`, a zero write means the other end won’t accept data).
* Does not use buffered I/O (`fwrite`, `std::ofstream`).
* Write a `main()` that uses `write_full` to write the string `"hello, world\n"` to `stdout`, then calls it again to write to a file named `out.txt`.
*(Think about how the loop structure differs from `read_full`.)*

---

## Part 3: Filesystems Implementation

> **Important**
> The text and the associated code is a blatant copy of **CS110 Assignment 2**!
> Read through the complete assignment first before starting to code. This assignment requires some careful low-level C coding, so don’t be surprised when you cannot apply the latest C++ concepts.

An archaeological expedition in Murray Hill, NJ has uncovered some magnetic disk drives from the mid-1970s. After considerable effort, the dig’s technical expert read the contents of the drives, and for each disk drive, she created a local file to be an exact bitwise copy of the disk’s data.

The archaeologists determined the data on the disks was stored using the Version 6 Unix file system. Sadly, none of the archaeologists went to this lecture, so they haven’t been able to read the image contents. Your task is to write a program that understands the Unix v6 file system to extract the file system data.

### Learning Goals
* To fully understand the design and implementation of a legacy filesystem as outlined in class.
* To fully grasp the benefit of layered decomposition in the design of a larger system that makes bland hardware work as a fully operational filesystem.
* To gain more practice working with a multiple-file codebase.

### Overview of Unix v6 FS and this Assignment
The key design principle to learn in this assignment is that of **layering**: one way to manage complexity in systems is to break it down into layers that sit on top of each other.

There are several layers involved in the Unix v6 filesystem:
* **Block Layer:** Among the lowest software layers. It manages the details of communication with the disk, enabling us to read or write sectors. Sits underneath almost every filesystem operation.
* **Inode Layer:** Supplies higher layers with metadata about files. When we need to know which block number is storing a particular portion of a file, the inode layer tells us that. 
* **File Layer:** Supplies higher layers with actual file contents. We request some number of bytes from a file at a particular offset, and the file layer populates a buffer with that information.
* **Filename Layer:** Allows us to find a file within a directory. Given a filename and a directory presumably containing that file, this layer figures out what inode stores the information.
* **Pathname Layer:** Implements full absolute path lookups, starting from the root directory.

For this assignment, the block layer is fully implemented, and you’ll flesh out the others. By the end of the assignment, your program will be able to list and read files from a disk formatted with the Unix v6 filesystem.

> **Note:** It probably will take some time to figure out the starter code. There are a lot of files in the repository. Take time to read through all the starter code and figure out how the files all fit together. Sketching out file interactions on paper, or using IDE tools like VSCode's `ctrl-click`, `vim`'s `ctags`, or `helix`'s `goto-mode` will be very helpful.

### Unix v6 File System Supplement
The lecture slides cover enough material to finish your implementation. An even better reference would be *Section 2.5 of the Salzer and Kaashoek book*, but unfortunately, HAW Landshut overlooked renewing the electronic access to the book 😕.

### Getting Started
After cloning the code from the assignment repository, check out the folder structure. You should see a folder `testdisks` containing the aforementioned disk copies.

Run `make` in the main folder. You may encounter an error regarding a missing file `openssl/sha.h`. 
* On Ubuntu, fix this by installing `libssl-dev`. 
* On OSX, run `brew install openssl`. 

If there is still an issue, ensure your compiler knows the location of the OpenSSL files:
```bash
# These paths are specific to my OSX machine!
export CPPFLAGS="$CPPFLAGS -I/opt/homebrew/Cellar/openssl@3/3.4.1/include/"
export LDFLAGS="$LDFLAGS -L/opt/homebrew/Cellar/openssl@3/3.4.1/lib"
```

If `make` succeeds, you should have a binary named `diskimageaccess`. Run it with two arguments:
```bash
diskimageaccess <options> <diskImagePath>
```
Where `<diskImagePath>` can be one of `basicDiskImage`, `depthFileDiskImage`, or `dirFnameSizeDiskImage` under `testdisks`.

**Options Flags:**
* `-b`: test `inode_iget` in isolation.
* `-m`: test `inode_iget` and `inode_indexlookup` in isolation (overrides `-b`).
* `-i`: test the inode and file layers (overrides `-m` and `-b`).
* `-p`: test the filename and pathname layers.

*Example:*
```bash
./diskimageaccess -ip testdisks/basicDiskImage
```

### Repository Structure
You are welcome to modify any of the provided files, provided your changes are backward-compatible with the testing infrastructure.

* **Test Harness:** `diskimageaccess.c`, `chksumfile.[ch]`, `unixfilesystems.[ch]`. Provides infrastructure for building and testing.
* **File System Module:** Interfaces using a layered API. Each layer has a header file and a `.c` file for your implementation:
  * **Block Layer** (`diskimg.[ch]`): Interface for reading/writing sectors. *Already implemented.*
  * **Inode Layer** (`inode.[ch]`): Interface for reading inodes. Includes lookup by inumber and getting the block/sector number of the n-th block of data.
  * **File Layer** (`file.[ch]`): Interface for reading blocks of data from a file by specifying its inumber.
  * **Filename Layer** (`directory.[ch]`): Interface for implementing Unix directories on top of files. Gets info about a single directory entry.
  * **Pathname Layer** (`pathname.[ch]`): Interface to look up a file by its absolute pathname.
* **V6 Headers:** `filsys.h`, `ino.h`, `direntv6.h`.

### Advice on Implementation Order
The starter code contains unfinished functions. We suggest tackling them in this order:
1. `inode_iget` and `inode_indexlookup` in `inode.c`.
2. `file_getblock` in `file.c`. *(After this step, your code should pass the inode level tests).*
3. `directory_findname` in `directory.c`.
4. `pathname_lookup` in `pathname.c`.

### Details
The provided C structs correspond directly to the filesystem’s on-disk data structures.
* `struct filsys` (`filsys.h`): The superblock.
* `struct inode` (`ino.h`): A single inode.
* `struct direntv6` (`direntv6.h`): A directory entry.
* `unixfilesystem.h`: File system layout descriptions, including superblock/inode sector addresses.

#### Legacy of an Old Machine
The Unix V6 file system goes to lengths to reduce stored data sizes. Many integer values are 16 bits (using `uint16_t`). The inode's `size` field is a 24-bit integer, represented by two fields: `i_size1` (least-significant 16 bits) and `i_size0` (most-significant 8 bits). Use `inode_getsize` in `inode.c` to assemble these.

#### The First Inode
There is no inode with an inumber of 0. The first inode in the first inode block has an inumber of 1 (corresponding to the root directory). **Do not assume the first inode is 0, or you will encounter off-by-one errors.**

#### Inode’s `i_mode`
The 16-bit `i_mode` field uses bits to indicate properties. Check `ino.h` for `#defines`.
* **Allocated:** Bit 15 indicates if the inode points to an existing file. `(i_mode & IALLOC) == 0` is true if unallocated.
* **Large File Mapping:** Bit 12 indicates indirect/doubly-indirect mapping. `(i_mode & ILARG) != 0`.
* **File Type:** Bits 14 and 13. `0` for regular files, `2` (`IFDIR`) for directories. `(i_mode & IFMT) == IFDIR` is true if it's a directory.

### Tips and Suggestions
* **Constants:** Check `unixfilesystem.h` for constants like `INODE_START_SECTOR` and `ROOT_INUMBER`. Do not hardcode numbers like `2` and `1`.
* **Superblock:** You can largely ignore `struct filsys` for the code you need to write.
* **`inode_iget`:** Define constants like `INODES_PER_BLOCK`. Declare an array of `struct inode` records when reading sectors, rather than doing complicated pointer arithmetic on byte buffers (`void *`).
* **`inode_indexlookup`:** Marshaling indirect block numbers is tricky.
  * *Hint 1:* Indirect blocks contain 256 two-byte integers (`uint16_t`).
  * *Hint 2:* Doubly indirect blocks lead to singly indirect blocks, which lead to sector numbers. Avoid copy-pasting code; build a unified translation process.
* **`file_getblock`:** This returns the number of *meaningful bytes* in a block. If a file isn't a multiple of 512 bytes, the final block payload will return a number less than 512.
* **`directory_findname`:** Declare an array of `struct direntv6` instead of a 512-byte char array. Make use of `strncmp` (not `strcmp`), because some stored directory names are not `\0`-terminated (though they are max 14 characters).
* **`pathname_lookup`:** Familiarize yourself with `strlen`, `strcpy`, `strsep`, and your `directory_findname` function.
* **Memory Leaks:** Rely on `valgrind`. None of the code you write needs dynamic memory allocation (`malloc`, `free`, etc.).

🎉 **Congratulations!**