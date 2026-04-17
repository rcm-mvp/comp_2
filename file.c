#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
  struct inode in;
  int err = inode_iget(fs, inumber, &in);
  if (err < 0) {
    return -1;
  }

  int sector = inode_indexlookup(fs, &in, blockNum);
  if (sector < 0) {
    return -1;
  }

  int bytes = diskimg_readsector(fs->dfd, sector, buf);
  if (bytes < 0) {
    return -1;
  }

  // last block may be partial
  int filesize   = inode_getsize(&in);
  int blockStart = blockNum * DISKIMG_SECTOR_SIZE;
  int validBytes = filesize - blockStart;

  if (validBytes <= 0) {
    return 0;
  }
  if (validBytes > DISKIMG_SECTOR_SIZE) {
    return DISKIMG_SECTOR_SIZE;
  }
  return validBytes;
}
