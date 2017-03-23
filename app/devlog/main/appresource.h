#ifndef __APPRESOURCE_H__
#define __APPRESOURCE_H__

#include "stdheader.h"
#include "sysheader.h"

#include "appconfig.h"

// Define application resources here (IPCs)

bool init_resource();
void close_resource();

// ---------------------------------------------------------
// shared memory
// ---------------------------------------------------------

struct s_shmem_info
{
    string name;
    void* memptr;
    unsigned int memsize;
};

bool init_shmem(s_shmem_info* param);
void close_shmem(s_shmem_info* param);

// ---------------------------------------------------------
// command queue between logger and loader
// ---------------------------------------------------------

enum e_logcmd_code
{
    LOGCMD_INVALID,
    LOGCMD_SAVE_CURRLOG,
    LOGCMD_SAVE_FULLLOG,
};

typedef pthread_mutex_t tQueueLock;
typedef deque<e_logcmd_code> tCmdQueue;
typedef tCmdQueue::iterator tCmdIter;
typedef tCmdQueue::const_iterator tCmdConstIter;

struct s_logcmd_queue
{
    tQueueLock lock;
    tCmdQueue cmdqueue;
};

bool peek_command(s_logcmd_queue& cmdq);
bool push_command(s_logcmd_queue& cmdq, e_logcmd_code code);
bool pop_command(s_logcmd_queue& cmdq, e_logcmd_code& code);

#endif
