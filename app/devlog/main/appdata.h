#ifndef __APPDATA_H__
#define __APPDATA_H__

#include "stdheader.h"
#include "sysheader.h"
#include "debuglog.h"
#include "errorstack.h"

#include "appconfig.h"
#include "appresource.h"

struct s_logger_task
{
    bool task_ready;
    bool request_stop;

    pthread_t thread_id;
};

struct s_loader_task
{
    bool task_ready;
    bool request_stop;

    pthread_t thread_id;
};

struct s_app_task
{
    s_logger_task logger;
    s_loader_task loader;
};

struct s_app_info
{
    s_shmem_info shmem;

    s_logcmd_queue logcmd;
};

enum e_operation_code
{
    OP_INVALID,
    OP_SHOW_HELP,
    OP_SHOW_VERSION,
    OP_SHOW_LICENSE,

    OP_GENCONF_APP,

    OP_LOG_VTVIEW,
};

struct s_app_data
{
    // working info
    e_operation_code opcode;

    s_app_info info;
    s_app_task task;

    // configuration info
    s_app_config conf;

    string conf_program;  // json file, private configuration for this program

    // app logger
    s_error_stack estack;
};

extern s_app_data app_data;

s_app_data* get_data_ptr();
s_app_task* get_task_ptr();
s_app_info* get_info_ptr();
s_app_config* get_config_ptr();
s_logger_task* get_logger_task();
s_loader_task* get_loader_task();
s_error_stack* get_error_stack();

#define DUMPSTACK() do { \
        s_error_stack* estack = get_error_stack(); \
        LOGSTR("Error Stack: \n%s", estack->to_string()); \
    } while(0)

#define APP_DEF_VARS() \
    s_app_data* aptr = get_data_ptr(); \
    s_app_info* info = get_info_ptr(); \
    s_app_config* conf = get_config_ptr(); \
    s_logger_task& logger = get_task_ptr()->logger; \
    s_loader_task& loader = get_task_ptr()->loader; \
    s_program_config& program = conf->program;

#endif
