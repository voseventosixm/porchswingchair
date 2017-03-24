#include "stdheader.h"
#include "stdmacro.h"
#include "fileutil.h"

#include "appdata.h"

#include "smart_interface.h"
#include "vtview_interface.h"

static void build_vtview_param(s_vtview_param& param);

// ---------------------------------------------------------------
// get vtview info
// ---------------------------------------------------------------

void build_vtview_param(s_vtview_param &param)
{
    const s_program_config& conf = get_config_ptr()->program;

    param.memptr = get_info_ptr()->shmem.memptr;

    param.backup_path = conf.backup_path;
    param.binary_path = conf.binary_path;
    param.log_config = conf.log_config;
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
        load_smartlog_backup(param);

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
