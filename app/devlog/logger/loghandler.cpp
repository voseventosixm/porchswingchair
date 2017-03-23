#include "stdmacro.h"
#include "stdheader.h"
#include "sysheader.h"
#include "debuglog.h"

#include "appdata.h"
#include "appmain.h"
#include "apputil.h"
#include "appconfig.h"
#include "appresource.h"

#include "logutil.h"
#include "loghandler.h"

static void* log_handler_func(void*);

bool start_log_handler()
{
    s_app_data* aptr = get_data_ptr();
    s_logger_task& tref = get_task_ptr()->logger;

    tref.task_ready = false;
    tref.request_stop = false;

    pthread_t pid;
    bool status = (0 == pthread_create(&pid, NULL, log_handler_func, NULL));

    return status;
}

bool join_log_handler()
{
    s_logger_task& tref = get_task_ptr()->logger;

    while(!tref.task_ready);

    pthread_join(tref.thread_id, NULL);
}

bool stop_log_handler()
{
    s_logger_task& tref = get_task_ptr()->logger;

    tref.request_stop = true;
}

static void* log_handler_func(void* param)
{
    APP_DEF_VARS();

    ASSERT(NULL == param);

    logtask.thread_id = pthread_self();
    logtask.task_ready = true;

    SHOWMSG("Start log_handler_thread");

    while(!logtask.request_stop)
    {
        SHOWMSG("logging smart");

        save_vtview_info();

        sleep(1);
    }

    return NULL;
}
