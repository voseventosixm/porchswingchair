#include "stdheader.h"
#include "stdmacro.h"

#include "appdata.h"

#include "smart_interface.h"

#include "vtview_interface.h"

static void build_vtview_param(s_vtview_param& param);

// ---------------------------------------------------------------
// get vtview info
// ---------------------------------------------------------------

void build_vtview_param(s_vtview_param &param)
{
    param.memptr = get_info_ptr()->shmem.memptr;
    param.capacity = get_config_ptr()->identify.capacity;
}

bool get_vtview_info(s_vtview_info &info)
{
    s_vtview_param param;
    build_vtview_param(param);

    bool status = false;
    do {
        memset(&info, 0x00, sizeof(info));

        // Update storage information
        if (false == BuildVtViewInfo_Storage(info, param)) break;

        // Update temperature information
        if (false == BuildVtViewInfo_Temperature(info, param)) break;

        status = true;
    } while(0);

    return status;
}
