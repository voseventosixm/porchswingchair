#include "debuglog.h"

#include "appdata.h"
#include "apputil.h"
#include "appresource.h"

// -----------------------------------------------------------------
// manage shmem
// -----------------------------------------------------------------

static bool init_user_shmem(s_shmem_info *param);
static void free_user_shmem(s_shmem_info *param);

static bool init_kernel_shmem(s_shmem_info *param);
static void free_kernel_shmem(s_shmem_info *param);

bool init_shmem(s_shmem_info *param)
{
    return init_user_shmem(param);
    // return init_kernel_shmem(param);
}

void close_shmem(s_shmem_info *param)
{
    return free_user_shmem(param);
    // return free_kernel_shmem(param);
}

static bool init_user_shmem(s_shmem_info *param)
{
    if (NULL == param) return false;
    param->memptr = malloc(param->memsize);
    return NULL != param->memptr;
}

static void free_user_shmem(s_shmem_info *param)
{
    if (NULL == param) return;
    if (param->memptr) free (param->memptr);
    param->memptr = NULL;
}

static bool init_kernel_shmem(s_shmem_info *param)
{
    if (NULL == param) return false;

    FILE* fp = fopen (param->name.c_str(), "r+");
    if (!fp) return false;

    int fd = fileno(fp);

    void* memptr = mmap(NULL, param->memsize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, 0);
    if (memptr == (void *)MAP_FAILED)
    {
        memptr = NULL;
        return false;
    }

    param->memptr = memptr;

    fclose(fp);

    return true;
}

static void free_kernel_shmem(s_shmem_info *param)
{
    if (NULL == param) return;

    if (param->memptr)
        munmap(param->memptr, param->memsize);

    param->memptr = NULL;
}

// ---------------------------------------------------------
// manage command queue
// ---------------------------------------------------------

bool peek_command(s_logcmd_queue &cmdq)
{
    tQueueLock& lock = cmdq.lock;
    tCmdQueue& queue = cmdq.cmdqueue;

    pthread_mutex_lock(&lock);

    bool status = (0 != queue.size());

    pthread_mutex_unlock(&lock);

    return status;
}

void push_command(s_logcmd_queue& cmdq, e_logcmd_code code)
{
    tQueueLock& lock = cmdq.lock;
    tCmdQueue& queue = cmdq.cmdqueue;

    pthread_mutex_lock(&lock);

    queue.push_front(code);

    pthread_mutex_unlock(&lock);
}

bool pop_command(s_logcmd_queue& cmdq, e_logcmd_code& code)
{
    tQueueLock& lock = cmdq.lock;
    tCmdQueue& queue = cmdq.cmdqueue;

    pthread_mutex_lock(&lock);

    bool status = (0 != queue.size());

    if (true == status)
    {
        code = queue.back();
        queue.pop_back();
    }

    pthread_mutex_unlock(&lock);

    return status;
}

// -----------------------------------------------------------------
// manage resources
// -----------------------------------------------------------------

bool init_resource()
{
    s_app_data* adptr = get_data_ptr();

    enum eInitState
    {
        STATE_INIT_NONE  = 0x00000000,
        STATE_INIT_SHMEM = 0x00000001,
    };

    bool status = false;
    int state = STATE_INIT_NONE;

    do {
        if (false == init_shmem(&adptr->info.shmem)) {
            set_error_code(eCannotInitializeShmem); break; }

        state |= STATE_INIT_SHMEM;

        status = true;
    } while(0);

    if (false == status)
    {
        if (state & STATE_INIT_SHMEM) close_shmem(&adptr->info.shmem);
    }

    return status;
}

void close_resource()
{
    s_app_data* adptr = get_data_ptr();

    close_shmem(&adptr->info.shmem);
}
