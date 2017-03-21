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

static bool init_mqtt_client();
static bool publish_topics();
static bool subscribe_topics();
static bool process_messages();
static void* log_handler_func(void*);

bool start_log_handler()
{
    s_app_data* aptr = get_data_ptr();
    s_logger_task& tref = get_task_ptr()->mqtt;

    tref.task_ready = false;
    tref.request_stop = false;

    pthread_t pid;
    bool status = (0 == pthread_create(&pid, NULL, log_handler_func, NULL));

    return status;
}

bool join_log_handler()
{
    s_logger_task& tref = get_task_ptr()->mqtt;

    while(!tref.task_ready);

    pthread_join(tref.thread_id, NULL);
}

bool stop_log_handler()
{
    s_logger_task& tref = get_task_ptr()->mqtt;

    tref.request_stop = true;
}

static bool init_mqtt_client()
{
    if (true == is_aws_cloud()) return aws_init_mqtt_client();

    return false;
}

static bool publish_topics()
{
    if (true == is_aws_cloud()) return aws_publish_topics();

    return false;
}

static bool subscribe_topics()
{
    if (true == is_aws_cloud()) return aws_subscribe_topics();

    return false;
}

static bool process_messages()
{
    if (true == is_aws_cloud()) return aws_process_messages();

    return false;
}

static void* log_handler_func(void* param)
{
    ASSERT(NULL == param);

    s_app_data* aptr = get_data_ptr();
    s_logger_task& tref = get_task_ptr()->mqtt;

    tref.thread_id = pthread_self();
    tref.task_ready = true;

    SHOWMSG("Start log_handler_thread");

    bool status = false;
    do {
        if (false == init_mqtt_client()) break;

        if (false == publish_topics()) break;

        if (false == subscribe_topics()) break;

        if (false == process_messages()) break;

        status = true;
    } while(0);

    set_error_if(!status, eMqttHandlerFuncError);

    return NULL;
}
