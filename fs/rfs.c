#include "rfs.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define K_PAGE_SIZE 4096

void *get_aligned_buffer(uint64_t bufer_size) {
  void *buf = NULL;
  if (posix_memalign(&buf, K_PAGE_SIZE, bufer_size)) {
    fprintf(stderr, "[ERROR] get aligned page size failed, erro = %s\n",
            strerror(errno));
    return NULL;
  }
  return buf;
}

// read first sector
void read_dev(void) {
  int fd = -1;
  const char *dev_name = "/dev/sdc";
  fd = open(dev_name, O_RDWR | O_NOATIME | O_DIRECT | O_SYNC, 0644);
  if (-1 == fd) {
    fprintf(stderr, "[ERROR] open file %s meet error %s\n", dev_name,
            strerror(errno));
    return;
  }

  char *buf = get_aligned_buffer(K_PAGE_SIZE);
  int read_bytes = read(fd, buf, K_PAGE_SIZE);
  printf("read bytes = %d\n", read_bytes);

  free(buf);

  close(fd);
}

int main(void) {
    read_dev();
    return 0;
}
