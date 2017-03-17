#ifndef __APPDATA_H__
#define __APPDATA_H__

#include "stdheader.h"
#include "sysheader.h"

#include "applog.h"
#include "apperror.h"
#include "appconfig.h"
#include "appresource.h"

enum e_operation_code
{
    OP_INVALID,
    OP_SHOW_HELP,
    OP_SHOW_VERSION,
    OP_SHOW_LICENSE,

    OP_GENCONF_APP,
    OP_GENCONF_CLOUD,
    OP_GENCONF_DEVICE,
    OP_GENCONF_IDENTIFY,

    OP_MONITOR_DEVICE,
};

struct s_mqtt_task
{
    bool task_ready;
    bool request_stop;

    pthread_t thread_id;

    void* client;   // Cast to specific client (aws / azure)
};

struct s_app_task
{
    s_mqtt_task mqtt;
};

struct s_app_info
{
    string ipaddr;
    s_shmem_info shmem;
};

struct s_app_data
{
    // working info
    e_operation_code opcode;

    s_app_info info;
    s_app_task task;

    // configuration info
    s_app_config conf;

    string conf_app;      // json file, private configuration for this devmon
    string conf_device;   // json file, working configuration
    string conf_identify; // json file, serial number, device model, ...
    string conf_cloud;    // json file, cloud configuration

    // app logger
    s_app_error ecode;
};

extern s_app_data app_data;

s_app_data* get_data_ptr();
s_app_task* get_task_ptr();
s_app_info* get_info_ptr();
s_app_config* get_config_ptr();
s_mqtt_task* get_mqtt_task();
s_app_error* get_error_stack();

#endif
