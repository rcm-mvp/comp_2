#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *name,
		       int dirinumber, struct direntv6 *dirEnt) {
  struct inode in;
  if (inode_iget(fs, dirinumber, &in) < 0) return -1;
  if (!(in.i_mode & IALLOC))       return -1;
  if ((in.i_mode & IFMT) != IFDIR) return -1;

  int size      = inode_getsize(&in);
  int numBlocks = size / DISKIMG_SECTOR_SIZE;
  if (size % DISKIMG_SECTOR_SIZE != 0) {
    numBlocks++;
  }

  struct direntv6 block[32];  // 512/16 = 32 entries per block

  for (int bno = 0; bno < numBlocks; bno++) {
    int bytes = file_getblock(fs, dirinumber, bno, block);
    if (bytes < 0) {
      return -1;
    }

    int nEntries = bytes / sizeof(struct direntv6);
    for (int i = 0; i < nEntries; i++) {
      if (block[i].d_inumber == 0) {
        continue;
      }
      // d_name not always null-terminated
      if (strncmp(block[i].d_name, name, sizeof(block[i].d_name)) == 0) {
        *dirEnt = block[i];
        return 0;
      }
    }
  }
  return -1;
}
