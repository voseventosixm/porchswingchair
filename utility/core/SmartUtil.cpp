#include "MiscUtil.h"
#include "StringUtil.h"

#include "SmartUtil.h"

void SmartUtil::CorrectAttribute(sAttribute& attr)
{
    // Correct values of some attribute:
    switch(attr.ID)
    {
        case 0xC2: {
            // Temperature. Extend the sign-bit if needed

            U8 currTemp = attr.LowRaw & 0xFF;
            attr.LowRaw = (currTemp >= 128) ? (0xFFFFFF00 | currTemp) : currTemp;
        } break;

        default: break;
    }
}

bool SmartUtil::ParseSmartAttribute(const unsigned char* valSector, const unsigned char* thrSector, sAttribute& attr)
{
    unsigned char attrID = valSector[0];
    if((0 == attrID) || (attrID != thrSector[0])) return false;

    unsigned char attrThr = thrSector[1];
    unsigned char attrVal = valSector[3];
    unsigned char attrWor = valSector[4];
    unsigned long attrRawHigh = (valSector[10] << 8) | (valSector[9]);
    unsigned long attrRawLow  = (valSector[8] << 24) | (valSector[7] << 16) | (valSector[6] << 8) | (valSector[5]);

    if (false == CoreUtil::ValidateAttributeID(attrID)) return false;

    attr.ID = attrID;

    // Fix name of attributes

    string name;
    if(true == CoreUtil::LookupAttributeName(attr.ID, name))
    {
        attr.Name = name;
    }

    attr.Value = attrVal;
    attr.Worst = attrWor;
    attr.Threshold = attrThr;

    attr.LowRaw = attrRawLow;
    attr.HighRaw = attrRawHigh;

    return true;
}

void SmartUtil::ParseSmartData(const unsigned char* valSector, const unsigned char* thrSector, sSmartInfo& smartInfo, bool autoCorrect)
{
    const U8* valPtr = (U8*) valSector;
    const U8* thrPtr = (U8*) thrSector;

    valPtr += 2;
    thrPtr += 2;

    // Standard attributes
    for (int i = 0; i < 30; i++)
    {
        sAttribute attr;
        if (true == ParseSmartAttribute(valPtr, thrPtr, attr))
        {
            CorrectAttribute(attr);
            smartInfo.AttrMap[attr.ID] = attr;
        }

        valPtr += 12;
        thrPtr += 12;
    }

    if (true == autoCorrect) CorrectSmartInfo((U8*)valSector, (U8*)thrSector, smartInfo);
}

void SmartUtil::BuildNvmeSmartInfo(const U8 *logSector, nvme_smart_log &nvmeInfo)
{
    memcpy((void*) &nvmeInfo, logSector, sizeof(nvmeInfo));

    // Update endians ...
}

void SmartUtil::BuildNvmeIdentifyInfo(const U8 *logSector, nvme_id_ctrl &nvmeInfo)
{
    memcpy((void*) &nvmeInfo, logSector, sizeof(nvmeInfo));

    // Update endians ...
}

void SmartUtil::ParseNvmeSmartInfo(const U8 *logSector, sSmartInfo &smartInfo)
{

    // Available attributes from nvme specs:
    // Temperature (Kelvin) (1:2)
    // RemainingSpare(%)    (3)
    //
    // DataUnitRead(sector * 1000) (32:47)
    // DataUnitWritten(sector * 1000) (48:63)
    // HostReadCommand       (64:79)
    // HostWriteCommand      (80:95)
    // PowerCycleCount       (112:127)
    // PowerOnHours          (128:143)
    // IntegrityError (ECC, CRC) (160:175)

    nvme_smart_log nvmeInfo;
    BuildNvmeSmartInfo(logSector, nvmeInfo);

    tAttributeMap& attrMap = smartInfo.AttrMap;
    tAttributeMap::iterator iter;

    smartInfo.reset();

    #define READ_U16(value, ptr) value = ((ptr)[1] << 8) | ((ptr)[0] << 0)
    #define READ_U32(value, ptr) value = ((ptr)[3] << 24) | ((ptr)[2] << 16) | ((ptr)[1] << 8) | ((ptr)[0] << 0)

    #define ADD_ATTR(id,low,high,val,worst,threshold) do { \
            sAttribute attr; \
            CoreUtil::LookupAttributeName(id, attr.Name); \
            attr.ID = id; attr.LowRaw = low; attr.HighRaw = high; \
            attr.Value = val; attr.Worst = worst; attr.Threshold = threshold; \
            attrMap[attr.ID] = attr; \
        } while(0);

    if(true) { // Temperature:
        U32 val0; READ_U16(val0, nvmeInfo.temperature + 0);
        U32 tmpC = val0 - 273;
        ADD_ATTR(ATTR_TEMPERATURE, tmpC, 0, 100, 100, 100);
    }

    if (true) { // RemainingSpare
        U32 value = nvmeInfo.avail_spare;
        ADD_ATTR(ATTR_SPARE_BLOCK, value, 0, 100, 100, 100);
    }

    if (true) { // TotalDataRead
        U32 val0; READ_U32(val0, nvmeInfo.data_units_read + 0);
        U32 val1; READ_U32(val1, nvmeInfo.data_units_read + 4);
        ADD_ATTR(ATTR_LBA_READ, val0, val1, 100, 100, 100);
    }

    if (true) { // TotalDataWrite - 16 bytes
        U32 val0; READ_U32(val0, nvmeInfo.data_units_written + 0);
        U32 val1; READ_U32(val1, nvmeInfo.data_units_written + 4);
        ADD_ATTR(ATTR_LBA_WRITTEN, val0, val1, 100, 100, 100);
    }

    if (true) { // ECC Error - 16 bytes
        U32 val0; READ_U32(val0, nvmeInfo.media_errors + 0);
        U32 val1; READ_U32(val1, nvmeInfo.media_errors + 4);
        ADD_ATTR(ATTR_ECC_ERROR, val0, val1, 100, 100, 100);
        ADD_ATTR(ATTR_CRC_ERROR, val0, val1, 100, 100, 100);
    }

    if (true) { // PowerOnHours - 16 bytes
        U32 val0; READ_U32(val0, nvmeInfo.power_on_hours + 0);
        U32 val1; READ_U32(val1, nvmeInfo.power_on_hours + 4);
        ADD_ATTR( 9, val0, val1, 100, 100, 100);
    }

    if (true) { // PowerCycleCount - 16 bytes
        U32 val0; READ_U32(val0, nvmeInfo.power_cycles + 0);
        U32 val1; READ_U32(val1, nvmeInfo.power_cycles + 4);
        ADD_ATTR(12, val0, val1, 100, 100, 100);
    }
}

void SmartUtil::ParseNvmeIdentifyInfo(const U8 *logSector, sIdentifyInfo &driveInfo)
{
    nvme_id_ctrl nvmeInfo;
    BuildNvmeIdentifyInfo(logSector, nvmeInfo);

    char buf[256];

    if(1) {
        memset(buf, 0x00, sizeof(buf));
        CoreUtil::CopySwapWord((U8*) buf, (U8*) nvmeInfo.mn, 20);
        driveInfo.DeviceModel = string(buf);
    }

    if(1) {
        memset(buf, 0x00, sizeof(buf));
        CoreUtil::CopySwapWord((U8*) buf, (U8*) nvmeInfo.sn, 10);
        driveInfo.SerialNumber = string(buf);
    }

    if(1) {
        memset(buf, 0x00, sizeof(buf));
        CoreUtil::CopySwapWord((U8*) buf, (U8*) nvmeInfo.fr, 4);
        driveInfo.FirmwareVersion = string(buf);
    }
}

void SmartUtil::CorrectSmartInfo(const U8* valSector, const U8* thrSector, sSmartInfo &info)
{
    // USB devices don't support standard SMART attributes / values
    // --> Need to reorganize Smart info

    if (true == CorrectSmartInfo_HyperstoneUSB(valSector, thrSector, info)) return;
}

bool SmartUtil::CorrectSmartInfo_HyperstoneUSB(const U8* valSector, const U8* thrSector, sSmartInfo &info)
{
    bool validValue;

    validValue =
            (NULL != valSector) &&
            (NULL != thrSector);

    if (false == validValue) return false;

    // Testing for HyperstoneUSBDevice
    tAttributeMap& attrMap = info.AttrMap;
    tAttributeMap::iterator iter;

    // Testing other data in valSector/thrSector
    U16 w0 = GET_WORD(valSector, 0  ); // Struct Version
    U16 w193 = GET_WORD(valSector, 193); // SMART Hyperstone structure version

    validValue =
            (0x10 == w0) &&
            (0x04 == w193);

    if (false == validValue) return false;

    // Checking for some mandatory attributes

    #define TEST_ID(n) bool is##n = (attrMap.end() != (iter = attrMap.find(n))); sAttribute a##n; if (true == is##n) a##n = iter->second
    #define READ_ID(n) TEST_ID(n); if (false == is##n) return false

    // Mandatory attributes
    READ_ID(242); // Total LBA Read
    READ_ID(241); // Total LBA Written

    // Optional attributes
    TEST_ID(229); // Erase Count. Actuallly it is Remaining Life Left
    TEST_ID(196); // Spare Block Count
    TEST_ID(199); // UDMA CRC Error
    TEST_ID(204); // Correctable ECC Error

    // Unused attributes
    TEST_ID(12);  // Power On Count
    TEST_ID(203); // Total ECC Error
    TEST_ID(213); // Spare Block Count Worst Chip
    TEST_ID(214); // Anchor Block Status
    TEST_ID(215); // Trim Status
    TEST_ID(232); // Total Number of Read command

    #undef READ_ID
    #undef TEST_ID

    validValue =
            (100 == a241.Value) && (100 == a241.Worst) &&
            (100 == a242.Value) && (100 == a242.Worst);

    if (false == validValue) return false;

    // It is Hyperstone USB here.

    info.reset();

    #define ADD_ATTR(id,low,high,val,worst,threshold) do { \
            sAttribute attr; \
            CoreUtil::LookupAttributeName(id, attr.Name); \
            attr.ID = id; attr.LowRaw = low; attr.HighRaw = high; \
            attr.Value = val; attr.Worst = worst; attr.Threshold = threshold; \
            attrMap[attr.ID] = attr; \
        } while(0);

    if(true == is242) {
        // Total LBAs Read Attribute a242
        // Value  : Attribute value. This value is fixed at 100.
        // LowRaw : Total number of LBAs read from the disk, divided by 65536
        // HighRaw:

        sAttribute& org = a242;
        ADD_ATTR(ATTR_LBA_READ, org.LowRaw, org.HighRaw, org.Value, org.Worst, org.Threshold);
    }

    if(true == is241) {
        // Total LBAs Written Attribute a241
        // Value  : Attribute value. This value is fixed at 100.
        // LowRaw : Total number of LBAs written to the disk, divided by 65536
        // HighRaw:

        sAttribute& org = a241;
        ADD_ATTR(ATTR_LBA_WRITTEN, org.LowRaw, org.HighRaw, org.Value, org.Worst, org.Threshold);
    }

    if(true) {
        // Erase Count Attribute a229
        // Value  : Attribute value. The value returned here is an estimation of the
        //          remaining card life, in percent, based on the number of flash block
        //          erases compared to the target number of erase cycles per block.
        // LowRaw : Estimated total number of block erases
        // HighRaw: Estimated total number of block erases

        U16 w199 = GET_WORD(valSector, 199);
        U16 w200 = GET_WORD(valSector, 200);
        U32 aveEraseCount = (w200 << 16) | w199;
        ADD_ATTR(ATTR_AVE_ERASE, aveEraseCount, 0, 100, 100, 100); // Average Erase Count
    }

    if(true) {
        // Dummy attribute
        ADD_ATTR(ATTR_MAX_ERASE, 0, 0, 100, 100, 100); // Max_Erase_Count
        ADD_ATTR(ATTR_TEMPERATURE, 0, 0, 100, 100, 100); // Temperature
    }

    // Optional attributes ---------------------------------------------------

    if(true == is229) {
        // Erase Count Attribute a229
        // Value  : Attribute value. The value returned here is an estimation of the
        //          remaining card life, in percent, based on the number of flash block
        //          erases compared to the target number of erase cycles per block.
        // LowRaw : Estimated total number of block erases
        // HighRaw: Estimated total number of block erases

        sAttribute& org = a229;
        ADD_ATTR(ATTR_LIFE_LEFT, org.Value, 0, 100, org.Worst, 100); // Remaining Life Left
    }

    if(true == is196) {
        // Read SpareBlockCount a196
        // Value  : Percentage of remaining spare block over all flash chips
        //          (100 * current_spare_block / initial_spare_block)
        // LowRaw : initial_spare_block
        // HighRaw: current_spare_block

        sAttribute& org = a196;
        ADD_ATTR(ATTR_SPARE_BLOCK, org.Value, 0, org.Value, org.Worst, 100);
    }

    if(true == is199) {
        // UDMA CRC Errors Attribute a199
        // Value  : Attribute value. This value is fixed at 100
        // LowRaw : Total number of host IF CRC errors. This value is fixed at 0
        // HighRaw:

        sAttribute& org = a199;
        ADD_ATTR(ATTR_CRC_ERROR, org.LowRaw, org.HighRaw, org.Value, org.Worst, 100);
    } while(0);

    if(true == is204) {
        // Correctable ECC Errors Attribute a204
        // Value  : Attribute value. This value is fixed at 100
        // LowRaw : Total number of correctable ECC errors
        // HighRaw:

        sAttribute& org = a204;
        ADD_ATTR(ATTR_ECC_ERROR, org.LowRaw, org.HighRaw, org.Value, org.Worst, 100);
    } while(0);

    // Unused attribute -------------------------------------------------------
    if(true == is12) {
        // Power On Count Attribute a12
        // Value  : Attribute value. This value is fixed at 100
        // LowRaw : Number of Power On cycles
        // HighRaw:

        sAttribute& org = a12;
        ADD_ATTR(12 , org.LowRaw, org.HighRaw, org.Value, org.Worst, 100);
    }

    if(true == is203) {
        // Total ECC Errors Attribute a203
        // Value  : Attribute value. This value is fixed at 100
        // LowRaw : Total number of ECC errors (correctable and uncorrectable)
        // HighRaw:

        sAttribute& org = a203;
        ADD_ATTR(196, org.LowRaw, org.HighRaw, org.Value, org.Worst, 100);
    }

    if(true == is213) {
        // Spare Block Count Worst Chip Attribute a213
        // Value  : Attribute value. This value is fixed at 100
        // LowRaw : Initial number of spare blocks of the flash chip with the lowest current number of spare blocks
        // HighRaw: Current number of spare blocks of the flash chip with the lowest current number of spare blocks

        // sAttribute& org = a213;
        // ADD_ATTR(195, org.LowRaw, org.HighRaw, org.Value, org.Worst, 100);
    }

    if(true == is214) {
        //  Anchor Block Status Attribute a214
        // Value  : Attribute value. This value is fixed at 100
        // LowRaw : Anchor Block Write Count
        // HighRaw:

        // sAttribute& org = a214;
        // ADD_ATTR(195, org.LowRaw, org.HighRaw, org.Value, org.Worst, 100);
    }

    if(true == is215) {
        // Trim Status Attribute a215
        // Value  : Attribute value
        // LowRaw : Total number of correctable ECC errors
        // HighRaw:

        // sAttribute& org = a215;
        // ADD_ATTR(195, org.LowRaw, org.HighRaw, org.Value, org.Worst, 100);
    }

    if(true == is232) {
        // Total Number of Reads Attribute a232
        // Value  : Attribute value. This value is fixed at 100
        // LowRaw : Total number of flash read commands
        // HighRaw: Total number of flash read commands

        // sAttribute& org = a232;
        // ADD_ATTR(XXX, org.LowRaw, org.HighRaw, 100, 100, 100);
    }

    #undef ADD_ATTR

    return true;
}

void SmartUtil::BuildIdentifySector(unsigned char* buff, const struct sIdentifyInfo* id)
{
    memset((void*) buff, 0x00, 512);

    CoreUtil::CopyWordArray(buff, 27, (unsigned char*)id->DeviceModel.c_str(), 20);  // buffer, word-offset, data, word-count
    CoreUtil::CopyWordArray(buff, 10, (unsigned char*)id->SerialNumber.c_str(), 10);
    CoreUtil::CopyWordArray(buff, 23, (unsigned char*)id->FirmwareVersion.c_str(), 4);

    CoreUtil::CopyWord(buff, 60, (id->LowCapacity      ) & 0xFFFF);
    CoreUtil::CopyWord(buff, 61, (id->LowCapacity >> 16) & 0xFFFF);

    // For 48b supported
    // copy_word(buff, 100, id->low_cap & 0xFFFF);
    // copy_word(buff, 101, (id->low_cap >> 16) & 0xFFFF);
    // copy_word(buff, 102, id->high_cap & 0xFFFF);
    // copy_word(buff, 103, (id->high_cap >> 16) & 0xFFFF);
}

void SmartUtil::BuildSmartSector(unsigned char* sector, const struct sSmartInfo* smartInfo)
{
    memset((void*) sector, 0x00, 512);

    unsigned char* ptr = &sector[2];

    tAttributeMap::const_iterator iter;
    for (iter = smartInfo->AttrMap.begin(); iter != smartInfo->AttrMap.end(); iter++)
    {
        const sAttribute& attr = iter->second;

        ptr[0] = attr.ID;
        ptr[3] = attr.Value;
        ptr[4] = 100;
        ptr[5] = attr.LowRaw;
        ptr[6] = (attr.LowRaw >> 8);
        ptr[7] = (attr.LowRaw >> 16);
        ptr[8] = (attr.LowRaw >> 24);
        ptr[9] = attr.HighRaw;
        ptr[10] = (attr.HighRaw >> 8);

        ptr += 12;// next buffer for next attribute
    }
}

void SmartUtil::BuildThresholdSector(unsigned char* sector, const struct sSmartInfo* smartInfo)
{
    memset((void*) sector, 0x00, 512);

    unsigned char* ptr = &sector[2];

    tAttributeMap::const_iterator iter;
    for (iter = smartInfo->AttrMap.begin(); iter != smartInfo->AttrMap.end(); iter++)
    {
        const sAttribute& attr = iter->second;

        ptr[0] = attr.ID;
        ptr[1] = attr.Threshold;

        ptr += 12;// next buffer for next attribute
    }
}
