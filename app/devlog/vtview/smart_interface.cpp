#include "fileutil.h"

#include "smart_converter.h"
#include "smart_interface.h"

// ---------------------------------------------------------------
// Build vtview info from storage info
// ---------------------------------------------------------------

bool load_smartlog_binary(const s_vtview_param &param)
{

    return true;
}

bool load_smartlog_backup(const s_vtview_param &param)
{

    return true;
}

bool load_smartlog_config(const s_vtview_param &param)
{

    return true;
}

bool save_smartlog_binary(const s_vtview_param &param)
{

    return true;
}

bool save_smartlog_backup(const s_vtview_param &param)
{

    return true;
}

bool save_smartlog_config(const s_vtview_param &param)
{

    return true;
}

bool get_smartlog_binary(s_vtview_info &info, const s_vtview_param &param)
{
    return read_filelist(param.binary_path, info.binary_list);
}

bool get_smartlog_backup(s_vtview_info &info, const s_vtview_param &param)
{
    return read_filelist(param.backup_path, info.backup_list);
}
