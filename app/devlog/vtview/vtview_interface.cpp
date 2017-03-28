#include "stdheader.h"
#include "stdmacro.h"
#include "fileutil.h"

#include "appdata.h"

#include "smart_interface.h"
#include "vtview_interface.h"

static void sync_vtview_param(s_vtview_param& param);
static void build_vtview_param(s_vtview_param& param);

// ---------------------------------------------------------------
// get vtview info
// ---------------------------------------------------------------

void sync_vtview_param(s_vtview_param &param)
{
    param.device_name = param.device_path.find("/dev/") ?
                            param.device_path : param.device_path.substr(5);

    param.config_file  = param.config_path + "/" + param.device_name + ".cfg";

    param.currbin_file = param.binary_path + "/" + param.device_name + "currlog.bin";
    param.fullbin_file = param.binary_path + "/" + param.device_name + "fulllog.bin";
    param.currbak_file = param.backup_path + "/" + param.device_name + "currlog.bak";
    param.fullbak_file = param.backup_path + "/" + param.device_name + "fulllog.bak";
}

void build_vtview_param(s_vtview_param &param)
{
    const s_program_config& conf = get_config_ptr()->program;

    // Copy vtview param from app data
    param.memptr = get_info_ptr()->shmem.memptr;
    param.config_path = conf.config_path;
    param.backup_path = conf.backup_path;
    param.binary_path = conf.binary_path;
    param.device_path = conf.device_path;

    sync_vtview_param(param);
}

bool get_vtview_info(s_vtview_info &info)
{
    s_vtview_param param;
    build_vtview_param(param);

    get_smartlog_binary(info, param);
    get_smartlog_backup(info, param);

    return true;
}

bool load_vtview_info()
{
    s_vtview_param param;
    build_vtview_param(param);

    bool status = false;
    do {
        load_smartlog_config(param);
        load_smartlog_binary(param);

        if (false == verify_smartlog(param)) break;

        status = true;
    } while(0);

    return status;
}

bool save_vtview_info()
{
    s_vtview_param param;
    build_vtview_param(param);

    save_smartlog_config(param);
    save_smartlog_binary(param);
    save_smartlog_backup(param);

    return true;
}

bool save_vtview_currlog()
{
    s_vtview_param param;
    build_vtview_param(param);


    return true;
}

bool save_vtview_fulllog()
{
    s_vtview_param param;
    build_vtview_param(param);


    return true;
}
