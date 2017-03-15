#ifndef COREDATA_H
#define COREDATA_H

#include "CoreMacro.h"
#include "CoreHeader.h"

#include "NVMEStruct.h"
#include "CoreUtil.h"

// -----------------------------------------------------------------
// Common enumeration
// -----------------------------------------------------------------
enum eNandType
{
    NAND_SLC,
    NAND_MLC,
};

// -----------------------------------------------------------------
// Static drive info
// -----------------------------------------------------------------
struct sAttribute
{
    U8  ID;
    U8  Value;
    U8  Worst;
    U8  Threshold;
    U32 LowRaw;
    U32 HighRaw;
    string Name;
    string Note;

    sAttribute();
    void reset();
};

typedef map<U8, sAttribute> tAttributeMap;
struct sSmartInfo
{
    tAttributeMap AttrMap; // ID <-> Attribute

    void reset();
};

struct sIdentifySectorInfo
{
    // SMART Feature
    bool IsSMARTSupported;          // W82:0
    bool IsSMARTEnabled;            // W85:0

    // SecureErase Feature
    bool IsSecuritySupported;       // W82:1
    bool IsUserPasswordPresent;     // W85:1
    bool IsDeviceLocked;            // W128:2
    bool IsDeviceFrozen;            // W128:3
    bool IsPasswordAttemptExceeded; // W128:4
    bool IsMasterPasswordMaximum;   // W128:8

    U16 SecureEraseTime;            // W89
    U16 EnhancedSecureEraseTime;    // W90

    // RemovableMedia
    bool IsRemovableSupported;      // W82:2
    bool IsRemovableEnabled;        // W85:2

    // PowerManagement
    bool IsPowerSupported;          // W82:3
    bool IsPowerEnabled;            // W85:3

    // PacketCommand
    bool IsPacketSupported;          // W82:4
    bool IsPacketEnabled;            // W85:4

    sIdentifySectorInfo();
    void reset();
};

struct sIdentifyInfo
{
    string DriveName;
    string DeviceModel;
    string SerialNumber;
    string FirmwareVersion;
    double UserCapacity;    // User capacity in GB ( not raw capacity )
    U32 LowCapacity;
    U32 HighCapacity;

    sIdentifySectorInfo SectorInfo;

    sIdentifyInfo();
    void reset();
    string toString() const;
    string toFeatureString() const;
};

struct sInquiryInfo
{
    string VendorName;
    string ProductName;
    string RevisionLevel;
    string SerialNumber;

    sInquiryInfo();
    void reset();
    string toString() const;
};

struct sDataRetentionInfo
{
    unsigned int RemainingDay_25C;
    unsigned int RemainingDay_40C;
    unsigned int RemainingDay_55C;
    unsigned int RemainingDay_70C;
    unsigned int RemainingDay_85C;
};

enum eDriveErrorState
{
    DRIVE_LACK_LBA_READ          = 1 << 0,
    DRIVE_LACK_LBA_WRITTEN       = 1 << 1,
    DRIVE_LACK_AVE_ERASE         = 1 << 2,
    DRIVE_LACK_MAX_ERASE         = 1 << 3,
    DRIVE_LACK_TEMPERATURE       = 1 << 4,
    DRIVE_INVALID_SLOT_TIME      = 1 << 5,
    DRIVE_INVALID_LBA_READ       = 1 << 6,
    DRIVE_INVALID_LBA_WRITTEN    = 1 << 7,
    DRIVE_INVALID_AVE_ERASE      = 1 << 8,
    DRIVE_INVALID_MAX_ERASE      = 1 << 9,
    DRIVE_ZERO_AVE_ERASE         = 1 << 10,

    DRIVE_INVALID_CRCERROR       = 1 << 11,
    DRIVE_INVALID_ECCERROR       = 1 << 12,
    DRIVE_INVALID_PROGRAMFAIL    = 1 << 13,
    DRIVE_INVALID_ERASEFAIL      = 1 << 14,
    DRIVE_INVALID_NANDENDURANCE  = 1 << 15,
};

struct sDriveInfo
{
    time_t CaptureTime;

    sSmartInfo SmartInfo;
    sIdentifyInfo IdentifyInfo;

    mutable int DriveErrorState;

    sDriveInfo();
    void reset();
    void setError(eDriveErrorState state) const;
};

struct sRawDriveInfo
{
    string DriveName;
    time_t CaptureTime;

    U8 IdentifySector[512];
    U8 ValueSector[512];
    U8 ThresholdSector[512];

    sRawDriveInfo();
    void reset();
};

struct DriveInfoComparator {
    bool operator()(const sDriveInfo& lhs, const sDriveInfo& rhs) {
        return lhs.CaptureTime < rhs.CaptureTime;
    }
};

struct sVirtiumSmartLogEntryUnmapped
{
    U32 Index;
    U16 TimeStamp;
    struct
    {
        U8 SmartId;
        U8 CurrentValue;
        U32 PrimaryRawData;
    } SmartData[ 30 ];
};

struct sVirtiumSmartLog
{
    static const U16 NUMBER_OF_ENTRY = 365;

    sVirtiumSmartLogEntryUnmapped Entries[NUMBER_OF_ENTRY];
};

// -----------------------------------------------------------------
// Dynamic drive info
// -----------------------------------------------------------------
enum eSlotItemState
{
    MANDATORY_ITEM = 1 << 0,
    OPTIONAL_ITEM_LIFELEFT = 1 << 1,
    OPTIONAL_ITEM_SPAREBLOCK = 1 << 2,
    OPTIONAL_ITEM_ECCERROR = 1 << 3,
    OPTIONAL_ITEM_CRCERROR = 1 << 4,
    OPTIONAL_ITEM_PROGRAMFAIL = 1 << 5,
    OPTIONAL_ITEM_ERASEFAIL = 1 << 6,
    OPTIONAL_ITEM_NANDENDURANCE = 1 << 7,
};

struct sSlotLifeInfo
{
    double TeraByteWritten; // Number of written bytes in SSD's lifetime
    double AverageLoad;     // GB written per day
    double EstimateEOL;
    double RemainingLife;   // Percentage read from SMART data

    sSlotLifeInfo();
    void reset();
    string toString() const;
};

struct sSlotInfo
{
    // Mandatory items
    time_t SlotTime;
    U32 Duration;      // Slot duration in miliseconds (CurrValue - PrevValue)
    U32 LBARead;       // CurrValue or (CurrValue - PrevValue)
    U32 LBAWritten;    // CurrValue or (CurrValue - PrevValue)
    U32 AveEraseCount; // CurrValue
    U32 MaxEraseCount; // CurrValue
    U32 SlotEraseCount;// CurrValue or (CurrValue - PrevValue)
    U32 RawCapacity;   // CurrValue (rounded to nearest power_of_2)
    U32 Temperature;   // CurrValue
    U32 WriteAmp;      // WriteAmp * 100
    double WearEfficiency;

    // Optional items
    U32 LifeLeft;      // CurrValue (percentage, <= 100)
    U32 SpareBlock;    // CurrValue (percentage, <= 100)
    U32 ECCError;      // Hardware_ECC_Recovered
    U32 CRCError;      // UDMA_CRC_Error_Count
    U32 ProgramFail;   // ID 181
    U32 EraseFail;     // ID 182
    U32 NandEndurance; // CurrValue

    U32 ItemState;     // Status of optional items

    sSlotLifeInfo SlotLifeInfo;

    sSlotInfo();
    void reset();
    void updateLifeInfo();
    bool getData(eDataKey code, pair<U32, U32>& value) const;
    bool getData(eDataKey code, U32 dataSetIndex, pair<U32, U32>& value) const;
};

struct sLifeInfo
{
    // Life info
    sSlotLifeInfo LastLifeInfo;

    // History info
    vector<sSlotInfo> SlotArray;


    sLifeInfo();
    void reset();
    void calcSlotInfo(int minIndex, int maxIndex, sSlotInfo& slotInfo) const;
    void calcLifeInfo(int minIndex, int maxIndex, sSlotLifeInfo& lifeInfo) const;
    void getData(eDataKey code, vector<pair<double, double> >& valueArray) const;
    void getData(eDataKey code, vector<vector<pair<double, double> > >& valueArray) const;
};

struct sDiffInfo
{
    vector<sSlotInfo> SlotArray;

    sDiffInfo();
    void reset();
    void getData(eDataKey code, vector<pair<double, double> >& valueArray) const;
    void getData(eDataKey code, vector<vector<pair<double, double> > >& valueArray) const;
};

// -----------------------------------------------------------------
// DeviceList
// -----------------------------------------------------------------

typedef set<sDriveInfo, DriveInfoComparator> tDriveSet;

struct sDeviceInfo
{
    string SerialNumber;
    sLifeInfo LifeInfo;
    sDiffInfo DiffInfo;

    tDriveSet DriveSet; // Set of DriveInfo, sorted by CaptureTime

public:
    void getSlotData(eDataKey code, vector<pair<double, double> >& valueArray) const;
    void getSlotData(eDataKey code, vector<vector<pair<double, double> > >& valueArray) const;

    void getDiffData(eDataKey code, vector<pair<double, double> >& valueArray) const;
    void getDiffData(eDataKey code, vector<vector<pair<double, double> > >& valueArray) const;
};

typedef map<string, sDeviceInfo> tDeviceMap;

typedef map<int, string> tCapacityMap;
typedef map<string, tCapacityMap> tSerialMap;

// List of Devices in running time
struct sDeviceData
{
    tDeviceMap DeviceData; // SerialNum : Device

    int SerialIndex;
    tSerialMap SerialMap;

    void reset();
};

typedef set<string> tSerialSet;

// -----------------------------------------------------------------
// Function for handling core data
// -----------------------------------------------------------------

// Load drive info from datastring
bool LoadDrive(const string& line, sDriveInfo& info, bool autoCorrect = true);

// Add DriveInfo into DeviceInfo. Also update LifeInfo
void AdaptDrive(sDriveInfo& drive); // Adapt drive's info
void AddDrive(sDeviceData& data, const sDriveInfo& drive); // Set of devices

// Update LifeInfo in device
void UpdateDiffInfo(sDeviceInfo& info); // Single device
void UpdateLifeInfo(sDeviceInfo& info); // Single device
void UpdateSlotInfo(sDeviceData& data); // Set of devices


// Add DriveInfo into DeviceInfo and update LifeInfo
void UpdateDrive(sDeviceData &deviceData, const sDriveInfo& drive);
void UpdateDrive(sDeviceData &deviceData, const list<sDriveInfo> &driveList);

// Load history information from filename
bool LoadHistory(sDeviceData& data, const string& filename);
bool SaveHistory(const sDeviceData& data, const string& filename);
bool SaveLifeInfo(const sDeviceInfo& info, const string& filename);
bool SaveDeviceInfo(const sDeviceInfo& info, const string& filename);
bool SaveDriveInfo(const sDriveInfo& info, const string& filename);

bool AppendHistory(const list<sDriveInfo>& data, const string& filename);
bool AppendHistory(const list<sRawDriveInfo>& data, const string& filename);

// Refine history
bool RefineHistory(sDeviceData& data, const string& filename);
void RefineDrive(sDeviceData& data, const sDriveInfo& drive); // Set of devices

// Utility for manipulating Smart
U8 GetSubKey(U8 key);
bool ReadAttribute(const sSmartInfo& info, U8 id, U32& value);           // Read raw value (4 bytes)
bool ReadAttribute(const sSmartInfo& info, U8 id, double& value);        // Read raw value
bool ReadAttribute(const sSmartInfo& info, U8 id, U32& low, U32& high);  // Read raw value (6 bytes)
eNandType GetNandType(const sSmartInfo& info, const string& serialNo = string());

// Get pointer to DeviceInfo
sDeviceInfo* GetDevicePtr(sDeviceData& data, unsigned int offset);
sDeviceInfo* GetDevicePtr(sDeviceData& data, const string& serial);

// Get pointer to IdentifyInfo
const sDriveInfo* GetDrivePtr(const sDeviceInfo& device, unsigned int offset);       // Offset from the begining of DriveSet
const sSmartInfo* GetSmartPtr(const sDeviceInfo& device, unsigned int offset);       // Offset from the begining of DriveSet
const sIdentifyInfo* GetIdentifyPtr(const sDeviceInfo& device, unsigned int offset); // Offset from the begining of DriveSet

const sDriveInfo* GetLastDrivePtr(const sDeviceInfo& device);
const sSmartInfo* GetLastSmartPtr(const sDeviceInfo& device);
const sIdentifyInfo* GetLastIdentifyPtr(const sDeviceInfo& device);

// Utility function
string ToString(const sSlotInfo& info, bool showHeader = false);
string ToString(const sLifeInfo& info);
string ToString(const sSmartInfo& info);
string ToString(const sIdentifyInfo& info);
string ToString(const sDriveInfo& info);
string ToString(const sRawDriveInfo& info);
string ToString(const sDeviceInfo& info);
string ToString(const sDeviceData& data);
string ToString(const list<sDriveInfo>& infoList);
string ToString(const list<sRawDriveInfo>& infoList);

string ToVerboseString(const sSmartInfo& info);
string ToVerboseString(const sIdentifyInfo& info);
string ToVerboseString(const sDriveInfo& info);
string ToVerboseString(const sDeviceInfo& info);
string ToVerboseString(const sDataRetentionInfo& info);

string ToShortString(const sDeviceInfo& info);
string ToShortString(const tSerialMap& serialMap);

void GetPowerOffDataRetentionEstimate (eNandType nandType, U32 eraseCount, sDataRetentionInfo& info);

#endif
