#ifndef SMART_CONVERTER_H
#define SMART_CONVERTER_H

#include "smart.h"
#include "smart_type.h"
#include "smart_common.h"

#include "vtview_interface.h"

// Declaration of structures in core smart code

struct sSlotInfo;
struct sDriveInfo;
struct sDeviceInfo;
struct sDeviceData;
struct sIdentifyInfo;

// Load vtview info from shmem

bool LoadHistory(sDeviceData& data, const s_vtview_param& param);
bool BuildDriveInfo(sDriveInfo& info, const s_vtview_param& param, const cmn_smart_device& device);
bool AddSmartLog(sDriveInfo& drive, const s_vtview_param& param, const cmn_smart_entry& entry, unsigned int currtime);
bool BuildIdentifyInfo(sIdentifyInfo& identify, const s_vtview_param& param);

// Build vtview info

void BuildVtViewInfo(s_vtview_info& info, const sSlotInfo& slot);
bool BuildVtViewInfo(s_vtview_info& info, const sDeviceInfo& data);
bool BuildVtViewInfo(s_vtview_info& info, const sDeviceData& data);
bool BuildVtViewInfo(vector<s_vtview_info>& info, const sDeviceData& data);

#endif
