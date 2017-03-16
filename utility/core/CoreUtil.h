#ifndef COREUTIL_H
#define COREUTIL_H

#include "CoreMacro.h"
#include "CoreHeader.h"

enum eAttributeID
{
    ATTR_MAX_ERASE = 0xA5,
    ATTR_AVE_ERASE = 0xA7,
    ATTR_TEMPERATURE = 0xC2,
    ATTR_LBA_WRITTEN = 0xF1,
    ATTR_LBA_READ = 0xF2,
    ATTR_LIFE_LEFT = 0xF8,
    ATTR_SPARE_BLOCK = 0xF9,
    ATTR_ECC_ERROR = 0xC3,
    ATTR_CRC_ERROR = 0xC7,
    ATTR_PROGRAM_FAIL = 0xB5,
    ATTR_ERASE_FAIL = 0xB6,
    ATTR_NAND_ENDURANCE = 0xA8,
};

enum eIdentifyKey
{
    #define MAP_ITEM(code, name) code,
    #include "IdentifyKey.def"
    #undef MAP_ITEM
};

class CoreUtil
{
public:    
    struct sAttributeParam
    {
        bool Show;
        string Name;
        string Note;
        sAttributeParam() {}
        sAttributeParam(const string& name, const string& note, bool show) {Name = name; Note = note; Show = show; }
    };

    typedef map<U8, sAttributeParam> tAttributeNameMap;
    typedef map<string, eIdentifyKey> tIdentifyKeyMap;

    struct sUtilData
    {
        bool m_ShowAttributeName;
        tAttributeNameMap m_AttrNameMap;
        tIdentifyKeyMap m_IdentifyKeyMap;
        
        sUtilData();
        void InitAttrNameMap();
        void InitIdentifyKeyMap();
    };
    
public:
    static bool ValidateAttributeID(U8 id);
    static bool LookupAttributeName(U8 id, string& name);
    static bool LookupAttributeNote(U8 id, string& note);
    static bool LookupAttributeText(U8 id, string&, string&);   // name:note
    static void LookupAttributeList(vector<string>& attrList);            // name
    static void LookupAttributeList(vector<pair<U8, string> >& attrList); // id:name
    static double LookupNandEndurance(const string& serialNum);
    static bool LookupIdentifyKey(const string& str, eIdentifyKey& key);

    static void CopyWord(unsigned char* buff, int offset, unsigned short val);
    static void CopyWordArray(unsigned char* buff, int offset, const unsigned char* val, int len);

    static void CopyWord(unsigned char* pdst0, const unsigned char* src, int wordcount, bool byteswap = false);
    static void SwapWord(unsigned char* buffer, int itemCount);  // itemCount: number of Word
    static void SwapDWord(unsigned char* buffer, int itemCount); // itemCount: number of DWord
    static void SwapCopy_0123_2301(unsigned char* dst, const unsigned char* src, int itemCount);
    static void CopySwapWord(unsigned char* dst, const unsigned char* src, int itemCount);  // itemCount: number of Word
    static void CopySwapDWord(unsigned char* dst, const unsigned char* src, int itemCount);  // itemCount: number of DWord

private:
    static sUtilData s_Data;
};

enum eDataKey
{
    #define MAP_ITEM(code, count, v0, w0, t0, v1, w1, t1, chart, name) code,
    #include "DataKey.def"
    #undef MAP_ITEM
};

typedef map<eDataKey, string> tSlotDataNameMap;

class DataKeyUtil
{
public:
    struct sUtilData
    {
        tSlotDataNameMap m_NameMap;

        sUtilData();
        void InitNameMap();
    };

public:
    static int GetDataSetCount(eDataKey code);
    static bool LookupDataName(eDataKey code, string& name);
    static bool LookupDataName(eDataKey code, vector<string>& nameList);

public:

    static sUtilData s_Data;
};

#endif // COREUTIL_H
