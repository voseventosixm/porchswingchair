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
#include "vtview_interface.h"

static void* log_handler_func(void*);
static bool process_log_command(e_logcmd_code code);

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

static void process_log_command(e_logcmd_code code)
{
    bool status = true;
    switch(code)
    {
        case LOGCMD_SAVE_CURRLOG: save_vtview_currlog(); break;
        case LOGCMD_SAVE_FULLLOG: save_vtview_fulllog(); break;
        default: status = false; break;
    }

    set_error_if(!status, eUnknownLogCommandCode);

    return status;
}

static void* log_handler_func(void* param)
{
    APP_DEF_VARS();

    ASSERT(NULL == param);

    logger.thread_id = pthread_self();
    logger.task_ready = true;

    SHOWMSG("Start log_handler_thread");

    while(!logger.request_stop)
    {
        e_logcmd_code code;

        if (true == pop_command(info->logcmd, code))
        {
            process_log_command(code);
        }
    }

    return NULL;
}
