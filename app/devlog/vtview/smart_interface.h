#ifndef SMART_INTERFACE_H
#define SMART_INTERFACE_H

#include "vtview_interface.h"

// Build vtview info from SMART data in shared-memory
bool BuildVtViewInfo_Storage(s_vtview_info& info, const s_vtview_param& param);

#endif
