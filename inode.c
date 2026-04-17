#include <stdio.h>
#include <assert.h>

#include "inode.h"
#include "diskimg.h"

#define INODES_PER_SECTOR 16  // 512/32
#define PTRS_PER_INDIRECT 256  // 512/2

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
  if (inumber < 1) {
    return -1;
  }

  // 1-based
  int offset = inumber - 1;
  int sector = INODE_START_SECTOR + offset / INODES_PER_SECTOR;
  int index  = offset % INODES_PER_SECTOR;

  struct inode buf[INODES_PER_SECTOR];
  int bytes = diskimg_readsector(fs->dfd, sector, buf);
  if (bytes < 0) {
    return -1;
  }

  *inp = buf[index];
  return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
  if (blockNum < 0) {
    return -1;
  }

  if (!(inp->i_mode & ILARG)) {  // small file directly
    if (blockNum >= 8) {
      return -1;
    }
    return inp->i_addr[blockNum];
  }

  // singly-indirect
  if (blockNum < 7 * PTRS_PER_INDIRECT) {
    int slot   = blockNum / PTRS_PER_INDIRECT;
    int offset = blockNum % PTRS_PER_INDIRECT;

    uint16_t indirect[PTRS_PER_INDIRECT];
    if (diskimg_readsector(fs->dfd, inp->i_addr[slot], indirect) < 0) {
      return -1;
    }
    return indirect[offset];
  }

  // doubly-indirect via i_addr[7]
  int remaining = blockNum - 7 * PTRS_PER_INDIRECT;
  if (remaining >= PTRS_PER_INDIRECT * PTRS_PER_INDIRECT) {
    return -1;
  }

  int first  = remaining / PTRS_PER_INDIRECT;
  int second = remaining % PTRS_PER_INDIRECT;

  uint16_t dbl[PTRS_PER_INDIRECT];
  if (diskimg_readsector(fs->dfd, inp->i_addr[7], dbl) < 0) {
    return -1;
  }

  uint16_t sgl[PTRS_PER_INDIRECT];
  if (diskimg_readsector(fs->dfd, dbl[first], sgl) < 0) {
    return -1;
  }

  return sgl[second];
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
