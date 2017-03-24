#include "stdmacro.h"
#include "stdheader.h"
#include "sysheader.h"
#include "debuglog.h"

#include "appdata.h"
#include "appmain.h"
#include "apputil.h"
#include "appconfig.h"
#include "appresource.h"

#include "loadutil.h"
#include "loadhandler.h"
#include "vtview_interface.h"

static void* load_handler_func(void*);

bool start_load_handler()
{
    APP_DEF_VARS();

    loader.task_ready = false;
    loader.request_stop = false;

    pthread_t pid;
    bool status = (0 == pthread_create(&pid, NULL, load_handler_func, NULL));

    return status;
}

bool join_load_handler()
{
    APP_DEF_VARS();

    while(!loader.task_ready);

    pthread_join(loader.thread_id, NULL);
}

bool stop_load_handler()
{
    APP_DEF_VARS();

    loader.request_stop = true;
}

static void* load_handler_func(void* param)
{
    ASSERT(NULL == param);

    APP_DEF_VARS();

    loader.thread_id = pthread_self();
    loader.task_ready = true;

    SHOWMSG("Start load_handler_thread");
	
	if (false == load_vtview_info()) 
    {
        set_error_code(eCannotLoadSmartlogData);
        return false;
    }

    unsigned int curr_counter = 0;
    unsigned int full_counter = 0;

    while(!logger.request_stop)
    {
        curr_counter++;
        if (curr_counter >= program.freq_currlog)
        {
            curr_counter = 0;
            push_command(info->logcmd, LOGCMD_SAVE_CURRLOG);
        }

        full_counter++;
        if (full_counter >= program.freq_fulllog)
        {
            full_counter = 0;
            push_command(info->logcmd, LOGCMD_SAVE_FULLLOG);
        }

        sleep(1);
    }

    return NULL;
}
