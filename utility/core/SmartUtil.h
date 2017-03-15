#ifndef SMARTUTIL_H
#define SMARTUTIL_H

#include "CoreData.h"
#include "CoreUtil.h"

class SmartUtil
{
public: // Handle SMART data
    static void CorrectAttribute(sAttribute& attr);
    static bool ParseSmartAttribute(const unsigned char* valSector, const unsigned char* thrSector, sAttribute& attr);
    static void ParseSmartData(const unsigned char* valSector, const unsigned char* thrSector, sSmartInfo& smartInfo, bool autoCorrect = true);

public:
    static void CorrectSmartInfo(const U8* valSector, const U8* thrSector, sSmartInfo& info);
    static bool CorrectSmartInfo_HyperstoneUSB(const U8* valSector, const U8* thrSector, sSmartInfo& info);

public:
	static void ParseNvmeSmartInfo(const U8* logSector, sSmartInfo& info);
	static void ParseNvmeIdentifyInfo(const U8* logSector, sIdentifyInfo& info);
	
	static void BuildNvmeSmartInfo(const U8* logSector, nvme_smart_log& nvmeInfo);
	static void BuildNvmeIdentifyInfo(const U8* logSector, nvme_id_ctrl& nvmeInfo);

public:
    static void BuildIdentifySector(unsigned char* buff, const struct sIdentifyInfo* id);
    static void BuildSmartSector(unsigned char* sector, const struct sSmartInfo* smartInfo);
    static void BuildThresholdSector(unsigned char* sector, const struct sSmartInfo* id);
};

#endif // SMARTUTIL_H

