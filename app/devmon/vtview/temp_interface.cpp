#include "temp_interface.h"

// ---------------------------------------------------------------
// Build vtview info from temperature
// ---------------------------------------------------------------

bool BuildVtViewInfo_Temperature(s_vtview_info& info, s_vtview_param& param)
{
	// Read temperature from low level layers
	info.temperature = 22;
	
	return true;
}
