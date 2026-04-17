#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
  if (pathname == NULL || pathname[0] != '/') return -1;

  // strsep mutates in place, work on a copy
  char pathcopy[1024];
  strncpy(pathcopy, pathname, sizeof(pathcopy) - 1);
  pathcopy[sizeof(pathcopy) - 1] = '\0';

  int cur = ROOT_INUMBER;
  char *rest = pathcopy;
  char *tok;

  while ((tok = strsep(&rest, "/")) != NULL) {
    if (tok[0] == '\0') continue;  // skip empty tokens
    struct direntv6 entry;
    if (directory_findname(fs, tok, cur, &entry) < 0)
      return -1;
    cur = entry.d_inumber;
  }

  return cur;
}
