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

    while(!logger.request_stop)
    {
        sleep(1);
		
		// push request to logger
    }

    return NULL;
}
