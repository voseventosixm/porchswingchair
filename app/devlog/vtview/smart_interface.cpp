#include "fileutil.h"

#include "smart_converter.h"
#include "smart_interface.h"

// ---------------------------------------------------------------
// Build vtview info from storage info
// ---------------------------------------------------------------

bool load_smartlog_binary(const s_vtview_param &param)
{
    cmn_smart_buffer* bufptr = (cmn_smart_buffer*) param.memptr;

    int devidx = 0;
    cmn_smart_device* devptr = bufptr->device_list[devidx];
    cmn_smart_data* dataptr = get_smart_data(devptr->smart_pool_idx);

    cmn_smart_fulllog* logptr = &dataptr->fulllog;
    cmn_raw_attr* rawptr = &dataptr->currlog.raw_attr;

    bool status = false;
    do {
        if (false == read_data(param.currlog_file, 0, rawptr, sizeof(*rawptr))) break;

        if (false == read_data(param.fulllog_file, 0, logptr, sizeof(*logptr))) break;

        status = true;
    } while(0);

    if (false == status)
    {
        memset((void*) rawptr, 0x00, sizeof(*rawptr));
        memset((void*) logptr, 0x00, sizeof(*logptr));
    }

    return status;
}

bool load_smartlog_backup(const s_vtview_param &param)
{

    return true;
}

bool load_smartlog_config(const s_vtview_param &param)
{
    cmn_smart_buffer* bufptr = (cmn_smart_buffer*) param.memptr;

    int devidx = 0;
    cmn_smart_device* devptr = bufptr->device_list[devidx];

    // Init devptr
    devptr->smart_pool_idx = devidx;
    return read_data(param.config_file, 0,
                     &devptr->smart_config,
                     sizeof(cmn_smart_config));
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
