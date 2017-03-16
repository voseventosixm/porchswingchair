#include "MiscUtil.h"
#include "StringUtil.h"
#include "HexFormatter.h"

#include "CoreData.h"
#include "CoreUtil.h"
#include "SmartUtil.h"

map<U8, string> sHiddenAttribute;
map<string, eIdentifyKey> sIdentifyKeyMap;

sAttribute::sAttribute()
{
    reset();
}

void sAttribute::reset()
{
    ID = 0;
    Value = Worst = Threshold = 0;
    LowRaw = HighRaw = 0;
    Name = "Reserved_Attribute";
    Note = "";
}

sIdentifySectorInfo::sIdentifySectorInfo()
{
    reset();
}

void sIdentifySectorInfo::reset()
{
    memset((void*) this, 0x00, sizeof(*this));
}

sIdentifyInfo::sIdentifyInfo()
{
    reset();
}

void sIdentifyInfo::reset()
{
    DriveName = "";
    DeviceModel = "";
    SerialNumber = "";
    FirmwareVersion = "";
    UserCapacity = 0;

    SectorInfo.reset();
}

string sIdentifyInfo::toString() const
{
    stringstream sstr;

    sstr << "Model: " << DeviceModel << endl;
    sstr << "Serial: " << SerialNumber << endl;
    sstr << "Capacity: " << UserCapacity << " GB" << endl;
    sstr << "Firmware: " << FirmwareVersion;

    return sstr.str();
}

string sIdentifyInfo::toFeatureString() const
{
    stringstream sstr;

    const sIdentifySectorInfo &p = SectorInfo;
    sstr << "SMART: "
         << (p.IsSMARTSupported ? "Supported" : "Unsupported") << " / "
         << (p.IsSMARTEnabled ? "Enabled" : "Disabled") << endl;

    sstr << "SECURITY: "
         << (p.IsSecuritySupported ? "Supported" : "Unsupported") << " / "
         << (p.IsUserPasswordPresent ? "Enabled" : "Disabled") << endl;

    sstr << "PACKET COMMAND: "
         << (p.IsPacketSupported ? "Supported" : "Unsupported") << " / "
         << (p.IsPacketEnabled ? "Enabled" : "Disabled") << endl;

    sstr << "REMOVABLE MEDIA: "
         << (p.IsRemovableSupported ? "Supported" : "Unsupported") << " / "
         << (p.IsRemovableEnabled ? "Enabled" : "Disabled") << endl;

    sstr << "POWER MANAGEMENT: "
         << (p.IsPowerSupported ? "Supported" : "Unsupported") << " / "
         << (p.IsPowerEnabled ? "Enabled" : "Disabled") << endl;

    return sstr.str();
}

sInquiryInfo::sInquiryInfo()
{
    reset();
}

void sInquiryInfo::reset()
{
    VendorName = "";
    ProductName = "";
    RevisionLevel = "";
    SerialNumber = "";
}

string sInquiryInfo::toString() const
{
    stringstream sstr;

    sstr << "Vendor: " << VendorName << endl;
    sstr << "Serial: " << SerialNumber << endl;
    sstr << "Product: " << ProductName << endl;
    sstr << "Revision: " << RevisionLevel;

    return sstr.str();
}

void sSmartInfo::reset()
{
    AttrMap.clear();
}

sDriveInfo::sDriveInfo()
{
    reset();
}

void sDriveInfo::reset()
{
    CaptureTime = 0;
    SmartInfo.reset();
    IdentifyInfo.reset();
    DriveErrorState = 0;
}

void sDriveInfo::setError(eDriveErrorState state) const
{
    DriveErrorState |= state;
}

sRawDriveInfo::sRawDriveInfo()
{
    reset();
}

void sRawDriveInfo::reset()
{
    DriveName = "";
    CaptureTime = 0;

    FILL_OBJECT(ValueSector, 0x00);
    FILL_OBJECT(ThresholdSector, 0x00);
    FILL_OBJECT(IdentifySector, 0x00);
}

sSlotLifeInfo::sSlotLifeInfo()
{
    reset();
}

void sSlotLifeInfo::reset()
{
    TeraByteWritten = 0;
    AverageLoad = 0;
    EstimateEOL = 0;
    RemainingLife = 0;
}

string sSlotLifeInfo::toString() const
{
    stringstream sstr;

    sstr << "Data Write: " << AverageLoad << " GB/hour" << endl;
    sstr << "Remaining Life: " << RemainingLife << " %" << endl;
    sstr << "Estimate Remaining TBW: " << TeraByteWritten << " TB" << endl;
    sstr << "Estimate Remaining Time: " << MiscUtil::ToTimeString(EstimateEOL, true) << endl;

    return sstr.str();
}

sSlotInfo::sSlotInfo()
{
    reset();
}

void sSlotInfo::reset()
{
    SlotTime = 0;
    Duration = 0;
    LBARead = 0;
    LBAWritten = 0;
    AveEraseCount = 0;
    MaxEraseCount = 0;
    SlotEraseCount = 0;
    RawCapacity = 0;
    Temperature = 0;
    LifeLeft = 0;
    SpareBlock = 0;
    ECCError = 0;
    CRCError = 0;
    ProgramFail = 0;
    EraseFail = 0;
    NandEndurance = 0;

    WriteAmp = 0;
    WearEfficiency = 0;

    ItemState = 0;

    SlotLifeInfo.reset();
}

void sSlotInfo::updateLifeInfo()
{
    sSlotLifeInfo& lifeInfo = SlotLifeInfo;

    U32 roundDuration = Duration;
    // U32 roundDuration = ((Duration + 86400 - 1) / 86400) * 86400;

    double nandEndurance = NandEndurance * LifeLeft / 100.0;

    // Writable capacity:
    double totalCapacityInGB = nandEndurance * RawCapacity;
    // Using WearEfficiency in calculating totalWritten
    // if ( (0 != MaxEraseCount) && (0 != LBAWritten))  totalCapacityInGB = (nandEndurance * LBAWritten) / (MaxEraseCount * 32.0);
    
    // Skip WearEfficiency in calculating totalWritten
    if ( (0 != SlotEraseCount) && (0 != LBAWritten)) totalCapacityInGB = (nandEndurance * LBAWritten) / (SlotEraseCount * 32.0);

    lifeInfo.TeraByteWritten = MiscUtil::RoundPrecision(totalCapacityInGB / 1024);

    lifeInfo.AverageLoad = (((double)LBAWritten * 112.5)) / (roundDuration); // AverageLoad per hour
    lifeInfo.AverageLoad = ((int) (lifeInfo.AverageLoad * 100)) / 100.0;

    // Calculate lifetime:
    U32 validLBAWritten = (0 == LBAWritten) ? 1 : LBAWritten;
    double durationInDay = (double) roundDuration / (3600 * 24);
    double writtenInGB = (double) validLBAWritten / 32;
    double writtenInDay = writtenInGB / durationInDay;
    lifeInfo.EstimateEOL = (int) (totalCapacityInGB /writtenInDay);

    // RemainingLife read from SMART
    lifeInfo.RemainingLife = LifeLeft;
}

bool sSlotInfo::getData(eDataKey key, pair<U32, U32>& value) const
{
    switch(key)
    {
        #define MAP_ITEM(code, count, v0, w0, t0, v1, w1, t1, chart, name) case code: value.first = v0; value.second = w0; break;
        #include "DataKey.def"
        #undef MAP_ITEM
        default: return false;
    }

    return true;
}

bool sSlotInfo::getData(eDataKey key, U32 dataSetIndex, pair<U32, U32>& value) const
{
    if (dataSetIndex == 0)
    {
        switch(key)
        {
            #define MAP_ITEM(code, count, v0, w0, t0, v1, w1, t1, chart, name) case code: value.first = v0; value.second = w0; break;
            #include "DataKey.def"
            #undef MAP_ITEM
            default: return false;
        }
    }

    if (dataSetIndex == 1)
    {
        switch(key)
        {
            #define MAP_ITEM(code, count, v0, w0, t0, v1, w1, t1, chart, name) case code: value.first = v1; value.second = w1; break;
            #include "DataKey.def"
            #undef MAP_ITEM
            default: return false;
        }
    }

    return true;
}

sLifeInfo::sLifeInfo()
{
    reset();
}

void sLifeInfo::reset()
{
    SlotArray.clear();

    LastLifeInfo.reset();
}

void sLifeInfo::calcSlotInfo(int minIndex, int maxIndex, sSlotInfo &diffSlot) const
{
    #define SET_ITEM(diff, curr, field) diff.field  = curr.field
    #define ADD_ITEM(diff, curr, field) diff.field += curr.field
    #define SUB_ITEM(diff, curr, field) diff.field -= curr.field

    diffSlot.reset();

    if (minIndex == maxIndex)
    {
        diffSlot = SlotArray[minIndex];
    }
    else
    {
        // Accumulate all sSlotInfo in ranges [minIndex, maxIndex]

        ASSERT (minIndex < maxIndex);

        const sSlotInfo& maxSlot = SlotArray[maxIndex];

        SET_ITEM(diffSlot, maxSlot, SlotTime);
        SET_ITEM(diffSlot, maxSlot, LifeLeft);
        SET_ITEM(diffSlot, maxSlot, SpareBlock);
        SET_ITEM(diffSlot, maxSlot, RawCapacity);
        SET_ITEM(diffSlot, maxSlot, NandEndurance);
        SET_ITEM(diffSlot, maxSlot, AveEraseCount);
        SET_ITEM(diffSlot, maxSlot, MaxEraseCount);
        SET_ITEM(diffSlot, maxSlot, WearEfficiency);

        for (int i = minIndex; i <= maxIndex; i++)
        {
            const sSlotInfo& currSlot = SlotArray[i];

            // Accumulate these fields
            ADD_ITEM(diffSlot, currSlot, Duration);
            ADD_ITEM(diffSlot, currSlot, LBARead);
            ADD_ITEM(diffSlot, currSlot, LBAWritten);
            ADD_ITEM(diffSlot, currSlot, SlotEraseCount);
            ADD_ITEM(diffSlot, currSlot, CRCError);
            ADD_ITEM(diffSlot, currSlot, ECCError);
            ADD_ITEM(diffSlot, currSlot, ProgramFail);
            ADD_ITEM(diffSlot, currSlot, EraseFail);
        }

        diffSlot.WriteAmp = (U32) (((double)diffSlot.RawCapacity * 1024 * diffSlot.SlotEraseCount * 100) / (diffSlot.LBAWritten * 32));

        diffSlot.updateLifeInfo();
    }

    #undef SET_ITEM
    #undef ADD_ITEM
    #undef SUB_ITEM
}

void sLifeInfo::calcLifeInfo(int minIndex, int maxIndex, sSlotLifeInfo &lifeInfo) const
{
    sSlotInfo diffSlot;

    calcSlotInfo(minIndex, maxIndex, diffSlot);
    lifeInfo = diffSlot.SlotLifeInfo;
}

void sLifeInfo::getData(eDataKey key, vector<pair<double, double> > &valueArray) const
{
    valueArray.clear();

    int slotCount = SlotArray.size();
    for (int i = 0; i < slotCount; i++)
    {
        pair<U32, U32> unsignedData;
        pair<double, double> doubleData;
        const sSlotInfo& slot = SlotArray[i];

        if (true == slot.getData(key, unsignedData))
        {
            doubleData.first = (int) unsignedData.first;
            doubleData.second = (int) unsignedData.second;
        }

        valueArray.push_back(doubleData);
    }
}

void sLifeInfo::getData(eDataKey key, vector<vector<pair<double, double> > >&valueArray) const
{
    // for each dataset in key, create an data_array

    valueArray.clear();

    int dataSetCount = DataKeyUtil::GetDataSetCount(key);

    for (int dataSet = 0; dataSet < dataSetCount; dataSet++)
    {
        vector<pair<double, double> > dataArray;

        int slotCount = SlotArray.size();
        for (int i = 0; i < slotCount; i++)
        {
            pair<U32, U32> unsignedData;
            pair<double, double> doubleData;
            const sSlotInfo& slot = SlotArray[i];

            if (true == slot.getData(key, dataSet, unsignedData))
            {
                doubleData.first = (int) unsignedData.first;
                doubleData.second = (int) unsignedData.second;
            }

            dataArray.push_back(doubleData);
        }

        valueArray.push_back(dataArray);
    }
}

sDiffInfo::sDiffInfo()
{
    reset();
}

void sDiffInfo::reset()
{
    SlotArray.clear();
}

void sDiffInfo::getData(eDataKey key, vector<pair<double, double> > &valueArray) const
{
    valueArray.clear();

    int slotCount = SlotArray.size();
    for (int i = 0; i < slotCount; i++)
    {
        pair<U32, U32> unsignedData;
        pair<double, double> doubleData;
        const sSlotInfo& slot = SlotArray[i];

        if (true == slot.getData(key, unsignedData))
        {
            doubleData.first = (int) unsignedData.first;
            doubleData.second = (int) unsignedData.second;
        }

        valueArray.push_back(doubleData);
    }
}

void sDiffInfo::getData(eDataKey key, vector<vector<pair<double, double> > >&valueArray) const
{
    // for each dataset in key, create an data_array

    valueArray.clear();

    int dataSetCount = DataKeyUtil::GetDataSetCount(key);

    for (int dataSet = 0; dataSet < dataSetCount; dataSet++)
    {
        vector<pair<double, double> > dataArray;

        int slotCount = SlotArray.size();
        for (int i = 0; i < slotCount; i++)
        {
            pair<U32, U32> unsignedData;
            pair<double, double> doubleData;
            const sSlotInfo& slot = SlotArray[i];

            if (true == slot.getData(key, dataSet, unsignedData))
            {
                doubleData.first = (int) unsignedData.first;
                doubleData.second = (int) unsignedData.second;
            }

            dataArray.push_back(doubleData);
        }

        valueArray.push_back(dataArray);
    }
}

void sDeviceInfo::getSlotData(eDataKey code, vector<pair<double, double> > &valueArray) const
{
    LifeInfo.getData(code, valueArray);
}

void sDeviceInfo::getDiffData(eDataKey code, vector<pair<double, double> > &valueArray) const
{
    DiffInfo.getData(code, valueArray);
}

void sDeviceInfo::getSlotData(eDataKey code, vector<vector<pair<double, double> > >&valueArray) const
{
    LifeInfo.getData(code, valueArray);
}

void sDeviceInfo::getDiffData(eDataKey code, vector<vector<pair<double, double> > >&valueArray) const
{
    DiffInfo.getData(code, valueArray);
}

void sDeviceData::reset()
{
    DeviceData.clear();

    SerialMap.clear();
    SerialIndex = 9999;
}

// -----------------------------------------------------------------
// Function for handling core data
// -----------------------------------------------------------------

// Load one drive info from history file
bool LoadDrive(const string& str, sDriveInfo& drive, bool autoCorrect)
{
    string line;
    stringstream sstr(str);

    const char sep = ';';

    bool status = false;

    drive.reset();
    sSmartInfo& smart = drive.SmartInfo;
    sIdentifyInfo& identify = drive.IdentifyInfo;

    do {
        // SerialNumber
        if (getline(sstr, line, sep).eof()) break;
        identify.SerialNumber = line;

        // CaptureTime
        if (getline(sstr, line, sep).eof()) break;
        drive.CaptureTime = atoi(line.c_str());

        // DriveName
        if (getline(sstr, line, sep).eof()) break;
        identify.DriveName = line;

        // DeviceModel
        if (getline(sstr, line, sep).eof()) break;
        identify.DeviceModel = line;

        // 2nd SerialNumber
        if (getline(sstr, line, sep).eof()) break;
        if(0 != identify.SerialNumber.compare(line)) break;

        // FirmwareVersion
        if (getline(sstr, line, sep).eof()) break;
        identify.FirmwareVersion = line;

        // UserCapacity
        if (getline(sstr, line, sep).eof()) break;
        identify.UserCapacity = atof(line.c_str());
        identify.UserCapacity = ((int) (identify.UserCapacity * 100)) / 100.0; // minor truncating

        // Array of attributes
        while(getline(sstr, line, sep))
        {
            sAttribute attr;

            attr.ID = atoi(line.c_str());

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.Name = line;

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.LowRaw = atoi(line.c_str());

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.Value = atoi(line.c_str());

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.Worst = atoi(line.c_str());

            if (getline(sstr, line, sep).eof()) break; // Skip scanning attributes
            attr.Threshold = atoi(line.c_str());

            // Change name if needed
            // string name, note;
            // if (true == CoreUtil::LookupAttributeText(attr.ID, name, note))
            // {
            //     attr.Name = name;
            //     attr.Note = note;
            // }

            // Validate ID after scanning all necessary attributes
            if (false == CoreUtil::ValidateAttributeID(attr.ID)) continue;

            // Correct value of some attributes (temperature, ..)
            SmartUtil::CorrectAttribute(attr);

            smart.AttrMap[attr.ID] = attr;
        }

        if(true == autoCorrect) SmartUtil::CorrectSmartInfo(NULL, NULL, smart);

        status = true;
    } while(0);

    if(true == status) AdaptDrive(drive);

    return status;
}

void AdaptDrive(sDriveInfo& info)
{
    // Change attribute names if needed
    tAttributeMap& smartMap = info.SmartInfo.AttrMap;

    string name, note;
    tAttributeMap::iterator iter;
    for (iter = smartMap.begin(); iter != smartMap.end(); iter++)
    {
        sAttribute& attr = iter->second;

        attr.Name = "Reserved_Attribute";
        if (true == CoreUtil::LookupAttributeText(attr.ID, name, note))
        {
            attr.Name = name;
            attr.Note = note;
        }
    }
}

// Add one drive into DeviceList
void AddDrive(sDeviceData& data, const sDriveInfo& inputInfo)
{
    sDriveInfo info = inputInfo;
    AdaptDrive(info);

    // Add drive into device
    string serialNum = info.IdentifyInfo.SerialNumber;
    map<string, sDeviceInfo>::iterator iter = data.DeviceData.find(serialNum);

    if (iter != data.DeviceData.end())
    {
        sDeviceInfo& device = data.DeviceData[serialNum];
        device.DriveSet.insert(info);
    }
    else
    {
        sDeviceInfo device;
        device.SerialNumber = serialNum;
        device.DriveSet.insert(info);

        data.DeviceData[serialNum] = device;
    }
}

// Refine and Add one drive into DeviceList
void RefineDrive(sDeviceData& data, const sDriveInfo& inputInfo)
{
    sDriveInfo info = inputInfo;
    AdaptDrive(info);

    string currSerialNumber = info.IdentifyInfo.SerialNumber;
    int currCapacity = (int) info.IdentifyInfo.UserCapacity;

    tSerialMap::iterator serialIter = data.SerialMap.find(currSerialNumber);
    if (serialIter == data.SerialMap.end())
    {
        // First time found this serial in input data
        tCapacityMap capMap;
        capMap[currCapacity] = currSerialNumber;

        data.SerialMap[currSerialNumber] = capMap;
    }
    else
    {
        tCapacityMap& capMap = serialIter->second;
        tCapacityMap::iterator capIter = capMap.find(currCapacity);
        if (capIter == capMap.end())
        {
            // Create new serial
            string newSerialNumber = currSerialNumber;
            stringstream sstr; sstr << setw(4) << setfill('0') << data.SerialIndex;
            string newTail = sstr.str();
            data.SerialIndex--;

            // Modify the serial number
            int currSerialLength = newSerialNumber.length();
            int tailLength = newTail.length();
            currSerialLength = currSerialLength - tailLength;
            for (int i = 0; i < tailLength; i++)
            {
                newSerialNumber[currSerialLength + i] = newTail[i];
            }

            // Add new pair to capMap
            capMap[currCapacity] = newSerialNumber;
            info.IdentifyInfo.SerialNumber = newSerialNumber;
        }
        else
        {
            // Use existing SerialNumber
            info.IdentifyInfo.SerialNumber = capIter->second;
        }
    }

    // Add drive into device
    string serialNumber = info.IdentifyInfo.SerialNumber;
    map<string, sDeviceInfo>::iterator iter = data.DeviceData.find(serialNumber);

    if (iter != data.DeviceData.end())
    {
        sDeviceInfo& device = data.DeviceData[serialNumber];
        device.DriveSet.insert(info);
    }
    else
    {
        sDeviceInfo device;
        device.SerialNumber = serialNumber;
        device.DriveSet.insert(info);

        data.DeviceData[serialNumber] = device;
    }
}

// Update LifeInfo of single device
void UpdateLifeInfo(sDeviceInfo& devInfo)
{
    sLifeInfo& lifeInfo = devInfo.LifeInfo;
    tDriveSet& driveSet = devInfo.DriveSet;

    lifeInfo.reset();

    sSlotInfo prevSmart, currSmart, lastSmart, diffSlot;
    U32 defaultNandEndurance = CoreUtil::LookupNandEndurance(devInfo.SerialNumber);

    tDriveSet::iterator iter;
    for (iter = driveSet.begin(); iter != driveSet.end(); iter++)
    {
        const sDriveInfo& driveInfo = *iter;
        const sSmartInfo& smartInfo = driveInfo.SmartInfo;

        currSmart.reset();

        // Read attribute from currSmart;

        // Mandatory attributes
        #define READ_MANDATORY_ATTRIBUTE(attrID, attrValue, errCode) if (false == ReadAttribute(smartInfo, attrID, attrValue)) { driveInfo.setError(errCode); continue; }
        READ_MANDATORY_ATTRIBUTE(ATTR_LBA_READ, currSmart.LBARead, DRIVE_LACK_LBA_READ);
        READ_MANDATORY_ATTRIBUTE(ATTR_LBA_WRITTEN, currSmart.LBAWritten, DRIVE_LACK_LBA_WRITTEN);
        READ_MANDATORY_ATTRIBUTE(ATTR_AVE_ERASE, currSmart.AveEraseCount, DRIVE_LACK_AVE_ERASE);
        READ_MANDATORY_ATTRIBUTE(ATTR_MAX_ERASE, currSmart.MaxEraseCount, DRIVE_LACK_MAX_ERASE);
        READ_MANDATORY_ATTRIBUTE(ATTR_TEMPERATURE, currSmart.Temperature, DRIVE_LACK_TEMPERATURE);
        #undef READ_MANDATORY_ATTRIBUTE

        currSmart.SlotTime = driveInfo.CaptureTime;

        // double userCapInGB = (driveInfo.IdentifyInfo.UserCapacity * 93.13) / 100.0; // (convert from 1000 to 1024 unit)
        double userCapInGB = driveInfo.IdentifyInfo.UserCapacity;                     // Dont need to convert to 1024 unit
        currSmart.RawCapacity = MiscUtil::RoundUp(userCapInGB);                       // Rounding to next Power_Of_2

        // Verify attributes
        #define VERIFY_MANDATORY_ATTRIBUTE(errCond, errCode) if (errCond) { driveInfo.setError(errCode); continue; }
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.SlotTime >= currSmart.SlotTime, DRIVE_INVALID_SLOT_TIME);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.LBARead > currSmart.LBARead, DRIVE_INVALID_LBA_READ);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.LBAWritten > currSmart.LBAWritten, DRIVE_INVALID_LBA_WRITTEN);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.AveEraseCount > currSmart.AveEraseCount, DRIVE_INVALID_AVE_ERASE);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.MaxEraseCount >  currSmart.MaxEraseCount, DRIVE_INVALID_MAX_ERASE);
        #undef VERIFY_MANDATORY_ATTRIBUTE

        currSmart.ItemState |= MANDATORY_ITEM;

        // Optional attributes
        #define READ_OPTIONAL_ATTRIBUTE(attrID, attrValue, optionCode) if (true == ReadAttribute(smartInfo, attrID, attrValue)) { currSmart.ItemState |= optionCode; }
        READ_OPTIONAL_ATTRIBUTE(ATTR_LIFE_LEFT,      currSmart.LifeLeft     , OPTIONAL_ITEM_LIFELEFT      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_SPARE_BLOCK,    currSmart.SpareBlock   , OPTIONAL_ITEM_SPAREBLOCK    );
        READ_OPTIONAL_ATTRIBUTE(ATTR_ECC_ERROR,      currSmart.ECCError     , OPTIONAL_ITEM_ECCERROR      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_CRC_ERROR,      currSmart.CRCError     , OPTIONAL_ITEM_CRCERROR      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_PROGRAM_FAIL,   currSmart.ProgramFail  , OPTIONAL_ITEM_PROGRAMFAIL   );
        READ_OPTIONAL_ATTRIBUTE(ATTR_ERASE_FAIL,     currSmart.EraseFail    , OPTIONAL_ITEM_ERASEFAIL     );
        READ_OPTIONAL_ATTRIBUTE(ATTR_NAND_ENDURANCE, currSmart.NandEndurance, OPTIONAL_ITEM_NANDENDURANCE );
        #undef READ_OPTIONAL_ATTRIBUTE

        int optionState = currSmart.ItemState & prevSmart.ItemState;

        #define VERIFY_OPTIONAL_ATTRIBUTE(errCond, optionCode, errCode) if ((optionState & optionCode) && (errCond)) { driveInfo.setError(errCode); continue; }
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.NandEndurance != currSmart.NandEndurance, OPTIONAL_ITEM_NANDENDURANCE, DRIVE_INVALID_NANDENDURANCE);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.CRCError > currSmart.CRCError, OPTIONAL_ITEM_CRCERROR, DRIVE_INVALID_CRCERROR);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.ECCError > currSmart.ECCError, OPTIONAL_ITEM_ECCERROR, DRIVE_INVALID_ECCERROR);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.ProgramFail > currSmart.ProgramFail, OPTIONAL_ITEM_PROGRAMFAIL, DRIVE_INVALID_PROGRAMFAIL);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.EraseFail > currSmart.EraseFail, OPTIONAL_ITEM_ERASEFAIL, DRIVE_INVALID_ERASEFAIL);
        #undef VERIFY_OPTIONAL_ATTRIBUTE

        // Calculate different between currSmart and prevSmart
        do {
            if (0 == (prevSmart.ItemState & MANDATORY_ITEM)) break; // Skip the first record
            if(currSmart.ItemState != prevSmart.ItemState) break;   // Different in optional fields

            diffSlot = currSmart;
            diffSlot.Duration = currSmart.SlotTime - prevSmart.SlotTime;
            diffSlot.LBARead -= prevSmart.LBARead;
            diffSlot.LBAWritten -= prevSmart.LBAWritten;
            diffSlot.SlotEraseCount = currSmart.AveEraseCount - prevSmart.AveEraseCount;

            if( optionState & OPTIONAL_ITEM_CRCERROR ) diffSlot.CRCError -= prevSmart.CRCError;
            if( optionState & OPTIONAL_ITEM_ECCERROR ) diffSlot.ECCError -= prevSmart.ECCError;
            if( optionState & OPTIONAL_ITEM_PROGRAMFAIL) diffSlot.ProgramFail -= prevSmart.ProgramFail;
            if( optionState & OPTIONAL_ITEM_ERASEFAIL) diffSlot.EraseFail -= prevSmart.EraseFail;

            // Set default nandEndurance
            if( !(optionState & OPTIONAL_ITEM_NANDENDURANCE)) diffSlot.NandEndurance = defaultNandEndurance;

            diffSlot.WearEfficiency = 0;
            if (0 != currSmart.MaxEraseCount)
            {
                diffSlot.WearEfficiency = (double) currSmart.AveEraseCount / (double) currSmart.MaxEraseCount;
            }

            diffSlot.WriteAmp = 0;
            if (0 != diffSlot.LBAWritten)
            {
                diffSlot.WriteAmp = (U32) (((double)diffSlot.RawCapacity * 1024 * diffSlot.SlotEraseCount * 100) / (diffSlot.LBAWritten * 32));
            }

            diffSlot.updateLifeInfo();

            lifeInfo.SlotArray.push_back(diffSlot);
        } while(0);

        prevSmart = currSmart;
    }

    int slotCount = lifeInfo.SlotArray.size();
    if (0 != slotCount)
    {
        sSlotInfo& lastSlot = lifeInfo.SlotArray[slotCount - 1];        // Last slot
        lifeInfo.LastLifeInfo = lastSlot.SlotLifeInfo;
    }
}
// Update DiffInfo of single device
void UpdateDiffInfo(sDeviceInfo& devInfo)
{
    sDiffInfo& diffInfo = devInfo.DiffInfo;
    tDriveSet& driveSet = devInfo.DriveSet;

    diffInfo.reset();

    sSlotInfo prevSmart, currSmart, lastSmart, diffSlot;
    U32 defaultNandEndurance = CoreUtil::LookupNandEndurance(devInfo.SerialNumber);

    tDriveSet::iterator iter;
    for (iter = driveSet.begin(); iter != driveSet.end(); iter++)
    {
        const sDriveInfo& driveInfo = *iter;
        const sSmartInfo& smartInfo = driveInfo.SmartInfo;

        currSmart.reset();

        // Read attribute from currSmart;

        // Mandatory attributes
        #define READ_MANDATORY_ATTRIBUTE(attrID, attrValue, errCode) if (false == ReadAttribute(smartInfo, attrID, attrValue)) { driveInfo.setError(errCode); continue; }
        READ_MANDATORY_ATTRIBUTE(ATTR_LBA_READ, currSmart.LBARead, DRIVE_LACK_LBA_READ);
        READ_MANDATORY_ATTRIBUTE(ATTR_LBA_WRITTEN, currSmart.LBAWritten, DRIVE_LACK_LBA_WRITTEN);
        READ_MANDATORY_ATTRIBUTE(ATTR_AVE_ERASE, currSmart.AveEraseCount, DRIVE_LACK_AVE_ERASE);
        READ_MANDATORY_ATTRIBUTE(ATTR_MAX_ERASE, currSmart.MaxEraseCount, DRIVE_LACK_MAX_ERASE);
        READ_MANDATORY_ATTRIBUTE(ATTR_TEMPERATURE, currSmart.Temperature, DRIVE_LACK_TEMPERATURE);
        #undef READ_MANDATORY_ATTRIBUTE

        currSmart.SlotTime = driveInfo.CaptureTime;

        // double userCapInGB = (driveInfo.IdentifyInfo.UserCapacity * 93.13) / 100.0; // (convert from 1000 to 1024 unit)
        double userCapInGB = driveInfo.IdentifyInfo.UserCapacity;                     // Dont need to convert to 1024 unit
        currSmart.RawCapacity = MiscUtil::RoundUp(userCapInGB);                       // Rounding to next Power_Of_2

        // Verify attributes
        #define VERIFY_MANDATORY_ATTRIBUTE(errCond, errCode) if (errCond) { driveInfo.setError(errCode); continue; }
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.SlotTime >= currSmart.SlotTime, DRIVE_INVALID_SLOT_TIME);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.LBARead > currSmart.LBARead, DRIVE_INVALID_LBA_READ);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.LBAWritten > currSmart.LBAWritten, DRIVE_INVALID_LBA_WRITTEN);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.AveEraseCount > currSmart.AveEraseCount, DRIVE_INVALID_AVE_ERASE);
        VERIFY_MANDATORY_ATTRIBUTE(prevSmart.MaxEraseCount >  currSmart.MaxEraseCount, DRIVE_INVALID_MAX_ERASE);
        #undef VERIFY_MANDATORY_ATTRIBUTE

        currSmart.ItemState |= MANDATORY_ITEM;

        // Optional attributes
        #define READ_OPTIONAL_ATTRIBUTE(attrID, attrValue, optionCode) if (true == ReadAttribute(smartInfo, attrID, attrValue)) { currSmart.ItemState |= optionCode; }
        READ_OPTIONAL_ATTRIBUTE(ATTR_LIFE_LEFT,      currSmart.LifeLeft     , OPTIONAL_ITEM_LIFELEFT      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_SPARE_BLOCK,    currSmart.SpareBlock   , OPTIONAL_ITEM_SPAREBLOCK    );
        READ_OPTIONAL_ATTRIBUTE(ATTR_ECC_ERROR,      currSmart.ECCError     , OPTIONAL_ITEM_ECCERROR      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_CRC_ERROR,      currSmart.CRCError     , OPTIONAL_ITEM_CRCERROR      );
        READ_OPTIONAL_ATTRIBUTE(ATTR_PROGRAM_FAIL,   currSmart.ProgramFail  , OPTIONAL_ITEM_PROGRAMFAIL   );
        READ_OPTIONAL_ATTRIBUTE(ATTR_ERASE_FAIL,     currSmart.EraseFail    , OPTIONAL_ITEM_ERASEFAIL     );
        #undef READ_OPTIONAL_ATTRIBUTE

        int optionState = currSmart.ItemState & prevSmart.ItemState;

        #define VERIFY_OPTIONAL_ATTRIBUTE(errCond, optionCode, errCode) if ((optionState & optionCode) && (errCond)) { driveInfo.setError(errCode); continue; }
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.CRCError > currSmart.CRCError, OPTIONAL_ITEM_CRCERROR, DRIVE_INVALID_CRCERROR);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.ECCError > currSmart.ECCError, OPTIONAL_ITEM_ECCERROR, DRIVE_INVALID_ECCERROR);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.ProgramFail > currSmart.ProgramFail, OPTIONAL_ITEM_PROGRAMFAIL, DRIVE_INVALID_PROGRAMFAIL);
        VERIFY_OPTIONAL_ATTRIBUTE(prevSmart.EraseFail > currSmart.EraseFail, OPTIONAL_ITEM_ERASEFAIL, DRIVE_INVALID_ERASEFAIL);
        #undef VERIFY_OPTIONAL_ATTRIBUTE

        // Calculate different between currSmart and prevSmart
        do {
            if (0 == (prevSmart.ItemState & MANDATORY_ITEM)) break; // Skip the first record
            if(currSmart.ItemState != prevSmart.ItemState) break;   // Different in optional fields

            diffSlot = currSmart;
            diffSlot.Duration = currSmart.SlotTime - prevSmart.SlotTime;
            diffSlot.LBARead -= prevSmart.LBARead;
            diffSlot.LBAWritten -= prevSmart.LBAWritten;
            diffSlot.SlotEraseCount = currSmart.AveEraseCount - prevSmart.AveEraseCount;

            if( optionState & OPTIONAL_ITEM_CRCERROR ) diffSlot.CRCError -= prevSmart.CRCError;
            if( optionState & OPTIONAL_ITEM_ECCERROR ) diffSlot.ECCError -= prevSmart.ECCError;
            if( optionState & OPTIONAL_ITEM_PROGRAMFAIL) diffSlot.ProgramFail -= prevSmart.ProgramFail;
            if( optionState & OPTIONAL_ITEM_ERASEFAIL) diffSlot.EraseFail -= prevSmart.EraseFail;

            diffInfo.SlotArray.push_back(diffSlot);
        } while(0);

        prevSmart = currSmart;
    }
}

// Update Slot info on sDeviceData
void UpdateSlotInfo(sDeviceData& data)
{
    tDeviceMap::iterator iter;
    for (iter = data.DeviceData.begin(); iter != data.DeviceData.end(); iter++)
    {
        UpdateLifeInfo(iter->second);
        UpdateDiffInfo(iter->second);
    }
}

void UpdateDrive(sDeviceData &deviceData, const sDriveInfo& drive)
{
    AddDrive(deviceData, drive);
    UpdateSlotInfo(deviceData);
}

void UpdateDrive(sDeviceData &deviceData, const list<sDriveInfo> &driveList)
{
    list<sDriveInfo>::const_iterator iter;
    for (iter = driveList.begin(); iter != driveList.end(); iter++)
    {
        AddDrive(deviceData, *iter);
    }

    UpdateSlotInfo(deviceData);
}

// Load all drives from history file
bool LoadHistory(sDeviceData& data, const string& filename)
{
    string line;
    fstream fstr(filename.c_str());

    data.DeviceData.clear();

    while(getline(fstr, line, '\n'))
    {
        sDriveInfo info;
        if(true == LoadDrive(line, info)) AddDrive(data, info);
    }

    // Update LifeInfo of devices
    UpdateSlotInfo(data);

    return true;
}

// Refine drives from history file. Change SerialNum if needed
bool RefineHistory(sDeviceData& data, const string& filename)
{
    string line;
    ifstream fstr(filename.c_str());

    data.DeviceData.clear();

    while(getline(fstr, line, '\n'))
    {
        sDriveInfo info;
        if(true == LoadDrive(line, info)) RefineDrive(data, info);
    }

    // Update LifeInfo of devices
    UpdateSlotInfo(data);

    return true;
}

bool SaveHistory(const sDeviceData& data, const string& filename)
{
    ofstream fstr(filename.c_str());
    fstr << ToString(data);
    fstr.close();

    return true;
}

bool AppendHistory(const list<sDriveInfo>& data, const string& filename)
{
    ofstream fstr(filename.c_str(), ios_base::out | ios_base::app);

    if (fstr.fail()) return false;

    fstr << ToString(data);
    fstr.close();

    return true;
}

bool AppendHistory(const list<sRawDriveInfo>& data, const string& filename)
{
    ofstream fstr(filename.c_str(), ios_base::out | ios_base::app);

    if (fstr.fail()) return false;

    fstr << ToString(data);
    fstr.close();

    return true;
}

bool SaveLifeInfo(const sDeviceInfo& info, const string& filename)
{
    ofstream fstr(filename.c_str());

    // Common information:
    fstr << "Device: " << info.SerialNumber << endl;
    fstr << ToString(info.LifeInfo);
    fstr.close();

    return true;
}

bool SaveDeviceInfo(const sDeviceInfo& info, const string& filename)
{
    ofstream fstr(filename.c_str());

    fstr << ToVerboseString(info);
    fstr.close();

    return true;
}

bool SaveDriveInfo(const sDriveInfo& info, const string& filename)
{
    ofstream fstr(filename.c_str());

    fstr << ToVerboseString(info);
    fstr.close();

    return true;
}

U8 GetSubKey(U8 mainKey)
{
    U8 subKey = 0;

    switch (mainKey)
    {
    case 169: subKey = 248; break; // Remaining-Life-Left
    case 248: subKey = 169; break;
    default: break;
    }

    return subKey;
}

bool ReadAttribute(const sSmartInfo& info, U8 key, double& value)
{
    U32 lowRaw = 0, highRaw = 0;
    if (false == ReadAttribute(info, key, lowRaw, highRaw)) return false;

    value = (double) lowRaw;

    return true;
}

bool ReadAttribute(const sSmartInfo& info, U8 key, U32& value)
{
    U32 lowRaw = 0, highRaw = 0;
    if (false == ReadAttribute(info, key, lowRaw, highRaw)) return false;

    value = lowRaw;

    return true;
}

bool ReadAttribute(const sSmartInfo& info, U8 key, U32& low, U32& high)
{
    map<U8, sAttribute>::const_iterator iter = info.AttrMap.find(key);
    if(info.AttrMap.end() == iter)
    {
        U8 subkey = GetSubKey(key);
        if(0 != subkey) iter = info.AttrMap.find(subkey); // second chance on this key:

        if(info.AttrMap.end() == iter) return false;
    }

    const sAttribute& attr = iter->second;

    low = attr.LowRaw;
    high = attr.HighRaw;

    return true;
}

eNandType GetNandType(const sSmartInfo &info, const string &serialNo)
{
    U32 nandEndurance = 0;

    if (false == ReadAttribute(info, ATTR_NAND_ENDURANCE, nandEndurance))
    {
        // Using default value from device model:
        nandEndurance = CoreUtil::LookupNandEndurance(serialNo);
    }

    eNandType nandType = NAND_SLC;

    if (nandEndurance <= 10000) nandType = NAND_MLC;

    return nandType;
}

sDeviceInfo* GetDevicePtr(sDeviceData &data, unsigned int offset)
{
    unsigned int index = 0;
    tDeviceMap::iterator iter;
    for (iter = data.DeviceData.begin(); iter != data.DeviceData.end(); iter++, index++)
    {
        if(index == offset)
        {
            return &iter->second;
        }
    }

    return NULL;
}

sDeviceInfo* GetDevicePtr(sDeviceData &data, const string& serialNum)
{
    tDeviceMap::iterator iter = data.DeviceData.find(serialNum);
    return (iter != data.DeviceData.end()) ? &iter->second : NULL;
}

const sDriveInfo* GetDrivePtr(const sDeviceInfo& device, unsigned int offset)
{
    // Offset from the begining of DriveSet
    const sDriveInfo* info = NULL;

    unsigned int driveCount = device.DriveSet.size();
    if (driveCount != 0)
    {
        tDriveSet::iterator iter;
        if(offset < driveCount)
        {
            iter = device.DriveSet.begin();
            for (unsigned int i = 0; i < offset; i++, iter++);
        }
        else
        {
            iter = device.DriveSet.end();
            iter--;
        }

        info = &(*iter);
    }

    return info;
}

const sSmartInfo* GetSmartPtr(const sDeviceInfo& device, unsigned int offset)
{
    // Offset from the begining of DriveSet
    const sSmartInfo* info = NULL;

    const sDriveInfo* drivePtr = GetDrivePtr(device, offset);
    if (NULL != drivePtr) info = &drivePtr->SmartInfo;

    return info;
}

const sIdentifyInfo* GetIdentifyPtr(const sDeviceInfo& device, unsigned int offset)
{
    // Offset from the begining of DriveSet
    const sIdentifyInfo* info = NULL;

    const sDriveInfo* drivePtr = GetDrivePtr(device, offset);
    if (NULL != drivePtr) info = &drivePtr->IdentifyInfo;

    return info;
}

const sSmartInfo* GetLastSmartPtr(const sDeviceInfo& device)
{
    return GetSmartPtr(device, device.DriveSet.size());
}

const sIdentifyInfo* GetLastIdentifyPtr(const sDeviceInfo& device)
{
    return GetIdentifyPtr(device, device.DriveSet.size());
}

const sDriveInfo* GetLastDrivePtr(const sDeviceInfo& device)
{
    return GetDrivePtr(device, device.DriveSet.size());
}

string ToString(const sSlotInfo& slot, bool showHeader)
{
    stringstream sstr;

    const char* sep = ";";

    tHeaderItem itemList[] = {
        tHeaderItem("Time", 0),
        tHeaderItem("Duration", 0),
        tHeaderItem("RawCap(G)", 0),
        tHeaderItem("LBA(32M)", 0),
        tHeaderItem("AveEr", 0),
        tHeaderItem("MaxEr", 0),
        tHeaderItem("Temp(C)", 0),
        tHeaderItem("Life(%)", 0),
        tHeaderItem("Spare(%)", 0),
        tHeaderItem("WriteAmp", 0),
    };

    MakeHeaderList(itemList, header);
    if (true == showHeader) sstr << header.ToString(";") << endl;

    // Dump contents
    if (slot.ItemState & MANDATORY_ITEM)
    {
        sstr << FRMT_U32(header.Length(0), slot.SlotTime) << sep;
        sstr << FRMT_U32(header.Length(1), slot.Duration) << sep;
        sstr << FRMT_U32(header.Length(2), slot.RawCapacity) << sep;
        sstr << FRMT_U32(header.Length(3), slot.LBAWritten) << sep;
        sstr << FRMT_U32(header.Length(4), slot.AveEraseCount) << sep;
        sstr << FRMT_U32(header.Length(5), slot.MaxEraseCount) << sep;
        sstr << FRMT_U32(header.Length(6), slot.Temperature) << sep;

        if (slot.ItemState & OPTIONAL_ITEM_LIFELEFT) sstr << FRMT_U32(header.Length(7), slot.LifeLeft);
        sstr << sep;

        if (slot.ItemState & OPTIONAL_ITEM_SPAREBLOCK) sstr << FRMT_U32(header.Length(8), slot.SpareBlock);
        sstr << sep;

        sstr << FRMT_DBL(header.Length(9), slot.WriteAmp / 100.0) << sep;
    }

    return sstr.str();
}

string ToString(const sLifeInfo& info)
{
    stringstream sstr;

    sSlotLifeInfo life = info.LastLifeInfo;

    sstr << "TBW: " << life.TeraByteWritten << endl;
    sstr << "EstimateEOL: " << life.EstimateEOL << endl;

    // Dump contents
    U32 slotCount = info.SlotArray.size();
    for (U32 i = 0; i < slotCount; i++)
    {
        const sSlotInfo& slot = info.SlotArray[i];
        sstr << ToString(slot, (i == 0)) << endl;
    }

    return sstr.str();
}

string ToString(const sSmartInfo& info)
{
    stringstream sstr;
    const char* sep = ";";

    tAttributeMap::const_iterator iter;
    for (iter = info.AttrMap.begin(); iter != info.AttrMap.end(); ++iter)
    {
        const sAttribute& attr = iter->second;

        ASSERT (iter->first == attr.ID);

        sstr
            << (U32) attr.ID  << sep
            << attr.Name      << sep
            << (U32) attr.LowRaw    << sep
            << (U32) attr.Value     << sep
            << (U32) attr.Worst     << sep
            << (U32) attr.Threshold << sep;
    }

    return sstr.str();
}

string ToString(const sIdentifyInfo& info)
{
    stringstream sstr;
    const char* sep = ";";

    sstr
        << info.DriveName       << sep
        << info.DeviceModel     << sep
        << info.SerialNumber    << sep
        << info.FirmwareVersion << sep
        << info.UserCapacity    << sep;

    return sstr.str();
}

string ToString(const sDriveInfo& info)
{
    stringstream sstr;
    const char* sep = ";";

    sstr
        << info.IdentifyInfo.SerialNumber << sep
        << info.CaptureTime << sep;

    sstr << ToString(info.IdentifyInfo);
    sstr << ToString(info.SmartInfo);

    return sstr.str();
}

string ToString(const sRawDriveInfo& info)
{
    stringstream sstr;
    const char* sep = ";";

    sstr
        << info.CaptureTime << sep
        << info.DriveName << sep;

    sstr << endl;

    // Identify sector
    // HexFormatter frmt;
    // frmt.setSize(512, 1);
    // frmt.setState(true, false, false);
    // sstr << frmt.toString(info.IdentifySector, 512);
    // sstr << frmt.toString(info.ValueSector, 512);
    // sstr << frmt.toString(info.ThresholdSector, 512);

    sstr << HexFormatter::ToHexString(info.IdentifySector, 512) << endl;
    sstr << HexFormatter::ToHexString(info.ValueSector, 512) << endl;
    sstr << HexFormatter::ToHexString(info.ThresholdSector, 512) << endl;

    return sstr.str();
}

string ToString(const list<sDriveInfo>& info)
{
    stringstream sstr;

    list<sDriveInfo>::const_iterator iter;
    for (iter = info.begin(); iter != info.end(); iter++)
    {
        sstr << ToString(*iter) << endl;
    }

    return sstr.str();
}

string ToString(const list<sRawDriveInfo>& info)
{
    stringstream sstr;

    list<sRawDriveInfo>::const_iterator iter;
    for (iter = info.begin(); iter != info.end(); iter++)
    {
        sstr << ToString(*iter) << endl;
    }

    return sstr.str();
}

string ToString(const sDeviceInfo& info)
{
    stringstream sstr;

    tDriveSet::const_iterator iter;
    for (iter = info.DriveSet.begin(); iter != info.DriveSet.end(); iter++)
    {
        sstr << ToString(*iter) << endl;
    }

    return sstr.str();
}

string ToString(const sDeviceData& data)
{
    stringstream sstr;

    tDeviceMap::const_iterator iter;
    for (iter = data.DeviceData.begin(); iter != data.DeviceData.end(); iter++)
    {
        sstr << ToString(iter->second);
    }

    return sstr.str();
}

string ToVerboseString(const sIdentifyInfo& info)
{
    stringstream sstr;

    sstr << "Name: " << info.DriveName << endl;
    sstr << "Model: " << info.DeviceModel << endl;
    sstr << "Serial: " << info.SerialNumber << endl;
    sstr << "Version: " << info.FirmwareVersion << endl;
    sstr << "UserCap: " << info.UserCapacity << endl;

    return sstr.str();
}

string ToVerboseString(const sSmartInfo& info)
{
    #define APP_FRMT_U32(w, value) setw(w) << setfill(' ') << (U32) value

    stringstream sstr;
    const char* sep = " ";

    tHeaderItem itemList[] = {
        tHeaderItem("ID", 3),
        tHeaderItem("Attribute", 28),
        tHeaderItem("High Raw", 10),
        tHeaderItem("Low Raw", 10),
        tHeaderItem("Value", 5),
        tHeaderItem("Worst", 5),
        tHeaderItem("Threshold", 9),
    };

    MakeHeaderList(itemList, header);
    sstr << header.ToString(" ") << endl;

    tAttributeMap::const_iterator iter;
    for (iter = info.AttrMap.begin(); iter != info.AttrMap.end(); ++iter)
    {
        const sAttribute& attr = iter->second;

        ASSERT (iter->first == attr.ID);

        sstr
            << APP_FRMT_U32(header.Length(0), attr.ID)        << sep
            << FRMT_STR(header.Length(1), attr.Name)          << sep
            << APP_FRMT_U32(header.Length(2), attr.HighRaw)    << sep
            << APP_FRMT_U32(header.Length(3), attr.LowRaw)    << sep
            << APP_FRMT_U32(header.Length(4), attr.Value)     << sep
            << APP_FRMT_U32(header.Length(5), attr.Worst)     << sep
            << APP_FRMT_U32(header.Length(6), attr.Threshold) << sep
            << endl;
    }

    return sstr.str();

    #undef APP_FRMT_U32
}

string ToVerboseString(const sDriveInfo& info)
{
    stringstream sstr;

    string sep(55, '-');

    char timeSlot[80];
    strftime(
        timeSlot, sizeof(timeSlot) / sizeof(timeSlot[0]),
        "%Y-%m-%d %H:%M:%S", localtime(&info.CaptureTime));

    string errorString = (0 == info.DriveErrorState) ? "This is a valid record" : "This is an invalid record";

    sstr << "Capture Time: " << timeSlot << endl;
    sstr << "Status: " << errorString << endl << sep << endl;
    sstr << ToVerboseString(info.IdentifyInfo) << sep << endl;
    sstr << ToVerboseString(info.SmartInfo) << sep << endl;

    return sstr.str();
}

string ToVerboseString(const sDeviceInfo& info)
{
    stringstream sstr;

    tDriveSet::const_iterator iter;
    for (iter = info.DriveSet.begin(); iter != info.DriveSet.end(); iter++)
    {
        sstr << ToVerboseString(*iter) << endl;
    }

    return sstr.str();
}

string ToVerboseString(const sDataRetentionInfo& info)
{
    stringstream sstr;

    sstr << "Remaining days at 25 celcius " << info.RemainingDay_25C << endl;
    sstr << "Remaining days at 40 celcius " << info.RemainingDay_40C << endl;
    sstr << "Remaining days at 55 celcius " << info.RemainingDay_55C << endl;
    sstr << "Remaining days at 70 celcius " << info.RemainingDay_70C << endl;
    sstr << "Remaining days at 85 celcius " << info.RemainingDay_85C << endl;

    return sstr.str();
}

string ToShortString(const tSerialMap& serialMap)
{
    stringstream sstr;

    tSerialMap::const_iterator serialIter;
    for (serialIter = serialMap.begin(); serialIter != serialMap.end(); serialIter++)
    {
        string oldSerial = serialIter->first;

        const tCapacityMap& capMap = serialIter->second;
        tCapacityMap::const_iterator capIter;

        int mapCount = capMap.size();
        sstr << "OldSerial: " << left << setw(21) << oldSerial << "MapCount: " << mapCount << endl;
        for (capIter = capMap.begin(); capIter != capMap.end(); capIter++)
        {
            int capacity = capIter->first;
            string newSerial = capIter->second;

            sstr << "NewSerial: " << left << setw(21) << newSerial << "Capacity: " << capacity << endl;
        }

        sstr << endl;
    }

    return sstr.str();
}

string ToShortString(const sDeviceInfo& info)
{
    stringstream sstr;

    string serialNumber;
    string deviceModel;
    string updateTime;

    int driveCount = info.DriveSet.size();

    if (0 != driveCount)
    {
        const sDriveInfo& drive = *(info.DriveSet.rbegin());

        serialNumber = drive.IdentifyInfo.SerialNumber;
        deviceModel = drive.IdentifyInfo.DeviceModel;

        char timeBuff[80]; int buffSize = sizeof(timeBuff) / sizeof(timeBuff[0]);
        strftime( timeBuff, buffSize, "%Y-%m-%d %H:%M:%S", localtime(&drive.CaptureTime));
        updateTime = timeBuff;
    }

    sstr << "Last Update: " << updateTime << endl;
    sstr << "Device Model: " << deviceModel << endl;
    sstr << "Serial Number: " << serialNumber << endl;
    sstr << "Number of Records: " << driveCount << endl;
    sstr << endl;

    sstr << "No CaptureTime         RecordStatus\n";
    sstr << "-- ------------------- --------------------------------\n";

    int recordIndex = 0;
    tDriveSet::const_iterator iter;
    for (iter = info.DriveSet.begin(); iter != info.DriveSet.end(); iter++, recordIndex++)
    {
        const sDriveInfo& drive = *iter;

        char timeBuff[80]; int buffSize = sizeof(timeBuff) / sizeof(timeBuff[0]);
        strftime( timeBuff, buffSize, "%Y-%m-%d %H:%M:%S", localtime(&drive.CaptureTime));

        sstr << setw(2)  << left << recordIndex << " ";
        sstr << setw(19) << left << timeBuff << " ";

        if(0 != drive.DriveErrorState) sstr << left << "Invalid record" << " ";
        sstr << endl;
    }

    return sstr.str();
}

void GetPowerOffDataRetentionEstimate (eNandType nandType, U32 eraseCount, sDataRetentionInfo &info)
{
    double MLCFactor[] = {
        18, -1,            // Deg85, A, B
        77.944, -0.994346, // Deg70, A, B
        450, -1,           // Deg55, A, B
        3000, -1,          // Deg40, A, B
        24000, -1,         // Deg25, A, B
    };

    double SLCFactor[] = {
        600, -1,           // Deg85, A, B
        2113.69, -0.957078,// Deg70, A, B
        12287.3, -0.975084,// Deg55, A, B
        100000, -1,        // Deg40, A, B
        800000, -1,        // Deg25, A, B
    };

    double* ptr = SLCFactor;

    switch (nandType)
    {
    case NAND_MLC: ptr = MLCFactor; break;
    case NAND_SLC: ptr = SLCFactor; break;
    default: break;
    }

    // Set threshold to 10 years
    #define MAP_ITEM(name, index) do { \
        double remainingYear = ptr[index * 2 + 0] * pow(eraseCount, ptr[index * 2 + 1]); \
        if (remainingYear > 10) remainingYear = 10; \
        info.name = (int)(remainingYear * 365); \
    } while(0)

    MAP_ITEM(RemainingDay_85C, 0);
    MAP_ITEM(RemainingDay_70C, 1);
    MAP_ITEM(RemainingDay_55C, 2);
    MAP_ITEM(RemainingDay_40C, 3);
    MAP_ITEM(RemainingDay_25C, 4);

    #undef MAP_ITEM
}
