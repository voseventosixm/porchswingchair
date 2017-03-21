#include "CoreData.h"
#include "CoreUtil.h"
#include "SmartUtil.h"

#include "smart_converter.h"

#include "smart_interface.h"

// ---------------------------------------------------------------
// Build vtview info from storage info
// ---------------------------------------------------------------

bool BuildVtViewInfo_Storage(s_vtview_info& info, const s_vtview_param& param)
{
    sDeviceData data;

    LoadHistory(data, param);
    return BuildVtViewInfo(info, data);
}
