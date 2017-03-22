#ifndef __SHMEM_UTIL_H__
#define __SHMEM_UTIL_H__

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

void shmem_release(void);
void *shmem_get(int length);

#endif
