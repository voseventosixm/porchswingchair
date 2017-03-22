#include "shmem_util.h"

static int shmem_len;
static void *shmem_ptr = NULL;

void *shmem_get(int length)
{
    if(1)
    {
        shmem_ptr = (void*) malloc(length);
        if (NULL == shmem_ptr) return NULL;

        shmem_len = length;
        return shmem_ptr;
    }

    if(0) 
    {
        const char *dev_name = "/dev/shmem0";

        FILE* fp = fopen(dev_name, "r+");
        if (!fp) return NULL;
	
        int fd = fileno(fp);

        shmem_ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, 0);
        if (shmem_ptr == (void *)MAP_FAILED) { shmem_ptr = NULL; return NULL; }

        shmem_len = length;
        fclose(fp);
        return shmem_ptr;
    }
}

void shmem_release(void)
{
    if (1)
    {
        if (shmem_ptr) free(shmem_ptr);

	    shmem_ptr = NULL;
    }

    if (0)
    {
        if (shmem_ptr) munmap(shmem_ptr, shmem_len);

	    shmem_ptr = NULL;
    }
}

