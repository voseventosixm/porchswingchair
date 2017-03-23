#ifndef SMART_INTERFACE_H
#define SMART_INTERFACE_H

#include "vtview_interface.h"

bool verify_smartlog(const s_vtview_param& param);

bool load_smartlog_binary(const s_vtview_param& param);
bool load_smartlog_backup(const s_vtview_param& param);
bool load_smartlog_config(const s_vtview_param& param);

bool save_smartlog_binary(const s_vtview_param& param);
bool save_smartlog_backup(const s_vtview_param& param);
bool save_smartlog_config(const s_vtview_param& param);

bool get_smartlog_binary(s_vtview_info& info, const s_vtview_param& param);
bool get_smartlog_backup(s_vtview_info& info, const s_vtview_param& param);

#endif
