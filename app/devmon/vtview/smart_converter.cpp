#include "CoreData.h"
#include "CoreUtil.h"
#include "SmartUtil.h"

#include "smart.h"
#include "smart_type.h"
#include "smart_common.h"

#include "smart_converter.h"

// ---------------------------------------------------------------
// Build vtview info utilities
// ---------------------------------------------------------------

void BuildVtViewInfo(s_vtview_info &info, const sSlotInfo &slot)
{
    info.data_read = slot.LBARead;
    info.data_written = slot.LBAWritten;
    info.remaining_life = slot.LifeLeft;
    info.remaining_spare = slot.SpareBlock;

    info.write_amp = slot.WriteAmp;
}

bool BuildVtViewInfo(s_vtview_info &info, const sDeviceInfo &data)
{
    const sLifeInfo& life = data.LifeInfo;
    const vector<sSlotInfo>& slotarr = life.SlotArray;

    // Extract information from last slot
    U32 count = slotarr.size();
    if (0 == count) return false;

    U32 index = count - 1;
    const sSlotInfo& slot = slotarr[index];

    BuildVtViewInfo(info, slot);

    return true;
}

bool BuildVtViewInfo(s_vtview_info &info, const sDeviceData &data)
{
    // get the first device in data only

    const tDeviceMap& devmap = data.DeviceData;

    if (0 == devmap.size()) return false;

    tDeviceMap::const_iterator iter = devmap.begin();
    return BuildVtViewInfo(info, iter->second);
}

bool BuildVtViewInfo(vector<s_vtview_info> &info, const sDeviceData &data)
{
    info.clear();

    const tDeviceMap& devmap = data.DeviceData;

    for (tDeviceMap::const_iterator iter = devmap.begin(); iter != devmap.end(); iter++)
    {
        s_vtview_info item;
        if (true == BuildVtViewInfo(item, iter->second))
        {
            info.push_back(item);
        }
    }

    return 0 != info.size();
}

// ---------------------------------------------------------------
// build history utilities
// ---------------------------------------------------------------

bool BuildIdentifyInfo(sIdentifyInfo &identify, const s_vtview_param& param)
{
    identify.SerialNumber    = "FakeSerial";
    identify.DriveName       = "FakeDrive";
    identify.DeviceModel     = "FakeModel";
    identify.FirmwareVersion = "FakeVersion";

    identify.UserCapacity    = param.capacity;

    return true;
}

bool AddSmartLog(sDriveInfo& drive, const s_vtview_param& param, const cmn_smart_entry& entry, unsigned int currtime)
{
    drive.reset();

    BuildIdentifyInfo(drive.IdentifyInfo, param);

    sSmartInfo& smart = drive.SmartInfo;

    drive.CaptureTime = currtime;

    for (U32 i = 0; i < MAX_ATTR_COUNT; i++)
    {
        sAttribute attr;
        const cmn_smart_attr& item = entry.attr_list[i];

        if (0 == item.attr_id) continue;

        unsigned int id = (unsigned int) (item.attr_id);
        unsigned int value = (unsigned int) (item.raw_value);

        // Fix Spare Value
        if (((249 == id) || (161 == id)) && (0 == value)) value = 100;

        attr.ID = id;
        attr.LowRaw = value;
        attr.Name = "UnknownAttribute";
        attr.Value = 100;
        attr.Worst = 100;
        attr.Threshold = 100;

        // Validate ID after scanning all necessary attributes
        if (false == CoreUtil::ValidateAttributeID(attr.ID)) continue;

        // Correct value of some attributes (temperature, ..)
        SmartUtil::CorrectAttribute(attr);

        smart.AttrMap[attr.ID] = attr;
    }

    SmartUtil::CorrectSmartInfo(NULL, NULL, smart);

    AdaptDrive(drive);

    return true;
}

bool BuildDriveInfo(sDriveInfo& info, const s_vtview_param& param, const cmn_smart_device& device)
{
    cmn_smart_data* dataptr = get_smart_data(device.smart_pool_idx);
    cmn_smart_fulllog* fulllog = get_device_fulllog(dataptr, NULL);

    unsigned int currtime = time(0);
    int offset = fulllog->current_entry;
    for (int i = 0; i < MAX_LOG_COUNT; i++)
    {
        int itemindex = (offset + MAX_LOG_COUNT - i) % MAX_LOG_COUNT;
        const cmn_smart_entry& entry = fulllog->entry_list[itemindex];
        if (0 == entry.index) continue;

        // entry_time_stamp is in minute units
        currtime = currtime - entry.time_stamp * 60;

        // New entry at low-index
        AddSmartLog(info, param, entry, currtime);
    }
}

bool LoadHistory(sDeviceData &data, const s_vtview_param& param)
{
    init_smart_buffer(param.memptr);
    const cmn_smart_buffer* smartptr = get_smart_buffer();

    for (U32 i = 0; i < smartptr->device_count; i++)
    {
        sDriveInfo info;
        const cmn_smart_device& device = smartptr->device_list[i];

        if (true == BuildDriveInfo(info, param, device)) AddDrive(data, info);
    }

    UpdateSlotInfo(data);

    return true;
}
