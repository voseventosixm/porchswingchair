
#include "appdata.h"

s_app_data app_data;

s_app_data* get_data_ptr() { return &app_data; }
s_app_task* get_task_ptr() { return &app_data.task; }
s_app_info* get_info_ptr() { return &app_data.info; }
s_app_config* get_config_ptr() { return &app_data.conf; }
s_logger_task* get_logger_task() { return &app_data.task.logger; }
s_error_stack* get_error_stack() { return &app_data.estack; }
