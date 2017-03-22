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

    param.smartlog_backup = conf.smartlog_backup;
    param.smartlog_filename = conf.smartlog_filename;
}

bool load_vtview_info()
{
    s_vtview_param param;
    build_vtview_param(param);

    // save smartlog from memptr

    bool status = true;

    status &= write_data(param.smartlog_filename, param.memptr, param.memsize);

    return
}
