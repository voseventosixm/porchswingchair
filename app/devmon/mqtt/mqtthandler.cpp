#include "stdmacro.h"
#include "stdheader.h"
#include "sysheader.h"

#include "appdata.h"
#include "appmain.h"
#include "apputil.h"
#include "appconfig.h"
#include "appresource.h"

#include "mqttutil.h"
#include "mqtthandler.h"
#include "mqttcallback.h"
#include "mqttcallback_aws.h"

static bool init_mqtt_client();
static bool publish_topics();
static bool subscribe_topics();
static bool process_messages();
static void* mqtt_handler_func(void*);

bool start_mqtt_handler()
{
    s_app_data* aptr = get_data_ptr();
    s_mqtt_task& tref = get_task_ptr()->mqtt;

    tref.task_ready = false;
    tref.request_stop = false;

    pthread_t pid;
    int res = pthread_create(&pid, NULL, mqtt_handler_func, NULL);
    if (0 != res)
    {
        set_error_code(eCannotStartMqttHandler);
        return false;
    }

    return true;
}

bool join_mqtt_handler()
{
    s_mqtt_task& tref = get_task_ptr()->mqtt;

    while(!tref.task_ready);

    pthread_join(tref.thread_id, NULL);
}

bool stop_mqtt_handler()
{
    s_mqtt_task& tref = get_task_ptr()->mqtt;

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

static void* mqtt_handler_func(void* param)
{
    SC_INIT();

    ASSERT(NULL == param);

    s_app_data* aptr = get_data_ptr();
    s_mqtt_task& tref = get_task_ptr()->mqtt;

    tref.thread_id = pthread_self();
    tref.task_ready = true;

    LOGMSG("Start mqtt_handler_thread");

    bool status = false;
    do {
        SC_NEXT();
        if (false == init_mqtt_client()) break;

        SC_NEXT();
        if (false == publish_topics()) break;

        SC_NEXT();
        if (false == subscribe_topics()) break;

        SC_NEXT();
        if (false == process_messages()) break;

        SC_NEXT();
        status = true;
    } while(0);

    LOGMSGIF(!status, "Error somewhere. StepCode: %d", SC_GET());

    return NULL;
}
