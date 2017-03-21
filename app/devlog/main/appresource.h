#ifndef __APPRESOURCE_H__
#define __APPRESOURCE_H__

#include "stdheader.h"
#include "sysheader.h"

#include "appconfig.h"

// Define application resources here (IPCs)

bool init_resource();
void close_resource();

struct s_shmem_info
{
    string name;
    void* memptr;
    unsigned int memsize;
};

bool init_shmem(s_shmem_info* param);
void close_shmem(s_shmem_info* param);

#endif
