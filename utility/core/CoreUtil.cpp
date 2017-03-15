#include "CoreUtil.h"
#include "CoreData.h"

CoreUtil::sUtilData CoreUtil::s_Data;
DataKeyUtil::sUtilData DataKeyUtil::s_Data;

void CoreUtil::sUtilData::InitAttrNameMap()
{
    // Init AttributeName map
    typedef pair<U8, sAttributeParam> tItemInfo;
    tItemInfo itemArray[] = {
        #define MAP_ITEM(code, show, name, note) tItemInfo(code, sAttributeParam(name, note, show)),
        #include "AttrName.def"
        #undef MAP_ITEM
    };
    
    for (U32 i = 0, itemCount = sizeof (itemArray) / sizeof (itemArray[0]); i < itemCount; i++)
    {
        tItemInfo& info = itemArray[i];
        sAttributeParam& param = info.second;

        if ( m_ShowAttributeName || param.Show )
        {
            m_AttrNameMap[info.first] = info.second;
        }
    }
}

void CoreUtil::sUtilData::InitIdentifyKeyMap()
{
    typedef pair<eIdentifyKey, string> tItemInfo;
    tItemInfo itemArray[] = {
        #define MAP_ITEM(code, name) tItemInfo(code, name),
        #include "IdentifyKey.def"
        #undef MAP_ITEM
    };
    
    for (U32 i = 0, itemCount = sizeof (itemArray) / sizeof (itemArray[0]); i < itemCount; i++)
    {
        tItemInfo& info = itemArray[i];
        m_IdentifyKeyMap[info.second] = info.first;
    }
}

CoreUtil::sUtilData::sUtilData()
{
    m_ShowAttributeName = false;

    InitAttrNameMap();
    InitIdentifyKeyMap();
}

bool CoreUtil::ValidateAttributeID(U8 id)
{
	return true;
}

bool CoreUtil::LookupAttributeName(U8 id, string& name)
{   
    bool status = false;

    tAttributeNameMap::iterator iter = s_Data.m_AttrNameMap.find(id);
    if (iter != s_Data.m_AttrNameMap.end())
    {
        const sAttributeParam& param = iter->second;
        name = param.Name;
        status = true;
    }
    
    return status;
}

void CoreUtil::LookupAttributeList(vector<string>& attrList)
{
    attrList.clear();

    tAttributeNameMap::iterator iter;
    for(iter = s_Data.m_AttrNameMap.begin(); iter != s_Data.m_AttrNameMap.end(); iter++)
    {
        const sAttributeParam& param = iter->second;
        attrList.push_back(param.Name);
    }
}

void CoreUtil::LookupAttributeList(vector<pair<U8, string> >& attrList)
{
    attrList.clear();

    tAttributeNameMap::iterator iter;
    for(iter = s_Data.m_AttrNameMap.begin(); iter != s_Data.m_AttrNameMap.end(); iter++)
    {
        U8 attrID = iter->first;
        const sAttributeParam& param = iter->second;
        attrList.push_back(pair<U8, string> (attrID, param.Name));
    }
}

bool CoreUtil::LookupAttributeNote(U8 id, string& note)
{
    bool status = false;

    tAttributeNameMap::iterator iter = s_Data.m_AttrNameMap.find(id);
    if (iter != s_Data.m_AttrNameMap.end())
    {
        const sAttributeParam& param = iter->second;
        note = param.Note;
        status = true;
    }

    return status;
}

bool CoreUtil::LookupAttributeText(U8 id, string& name, string& note)
{
    bool status = false;

    tAttributeNameMap::iterator iter = s_Data.m_AttrNameMap.find(id);
    if (iter != s_Data.m_AttrNameMap.end())
    {
        const sAttributeParam& param = iter->second;
        name = param.Name;
        note = param.Note;
        status = true;
    }

    return status;
}

bool CoreUtil::LookupIdentifyKey(const string& text, eIdentifyKey& key)
{
    bool status = false;

    tIdentifyKeyMap::iterator iter = s_Data.m_IdentifyKeyMap.find(text);
    if (iter != s_Data.m_IdentifyKeyMap.end())
    {
        key = iter->second;
        status = true;
    }

    return status;
}

double CoreUtil::LookupNandEndurance(const string& serialNum)
{
    // StorFly VSFB25PC064G-100
    // StorFly - VSFB25PC064G-100
    // P: 100000; // C: 3000

    double nandEndurance = 3000;

    size_t dashpos = serialNum.rfind("-");
    bool invalidSerial = (dashpos == string::npos) || (dashpos < 6);

    if(true == invalidSerial) return nandEndurance;

    char enduranceChar = serialNum[dashpos - 6];

    switch (enduranceChar)
    {
        case 'C': nandEndurance = 3000; break;
        case 'P': nandEndurance = 100000; break;
        default: break;
    }

    return nandEndurance;
}

DataKeyUtil::sUtilData::sUtilData()
{
    InitNameMap();
}

void DataKeyUtil::sUtilData::InitNameMap()
{
    typedef pair<eDataKey, string> tItemInfo;
    tItemInfo itemArray[] = {
        #define MAP_ITEM(key, count, v0, w0, t0, v1, w1, t1, chart, name) tItemInfo(key, name),
        #include "DataKey.def"
        #undef MAP_ITEM
    };

    for (U32 i = 0, itemCount = sizeof (itemArray) / sizeof (itemArray[0]); i < itemCount; i++)
    {
        tItemInfo& info = itemArray[i];
        m_NameMap[info.first] = info.second;
    }
}

bool DataKeyUtil::LookupDataName(eDataKey id, string& name)
{
    bool status = false;

    tSlotDataNameMap::iterator iter = s_Data.m_NameMap.find(id);
    if (iter != s_Data.m_NameMap.end())
    {
        name = iter->second;
        status = true;
    }

    return status;
}

bool DataKeyUtil::LookupDataName(eDataKey id, vector<string>& nameList)
{
    nameList.clear();

    switch(id)
    {
        #define MAP_ITEM(key, count, v0, w0, t0, v1, w1, t1, chart, name) \
            case key: if(count <= 1) break; \
            nameList.push_back(t0); nameList.push_back(t1); break;
        #include "DataKey.def"
        #undef MAP_ITEM
    }

    return true;
}

int DataKeyUtil::GetDataSetCount(eDataKey code)
{
    int dataSetCount = 0;

    switch(code)
    {
        #define MAP_ITEM(key, count, v0, w0, t0, v1, w1, t1, chart, name) case key: dataSetCount = count; break;
        #include "DataKey.def"
        #undef MAP_ITEM
    }

    return dataSetCount;
}

void CoreUtil::CopyWord(unsigned char* buff, int offset, unsigned short val)
{
    unsigned char* ptr = buff + offset * 2; // word offset

    ptr[0] = val & 0xFF;
    ptr[1] = (val >> 8) & 0xFF;
}

void CoreUtil::CopyWordArray(unsigned char* buff, int offset, const unsigned char* val, int len)
{
    int i;
    int blen = len * 2;
    unsigned char* ptr = buff + offset * 2; // word offset

    for (i = 0; i < blen; i+=2)
    {
        ptr[i] = val[i + 1];
        ptr[i + 1] = val[i];
    }
}

void CoreUtil::CopyWord(unsigned char* pdst0, const unsigned char* src, int wordcount, bool byteswap)
{
    unsigned char* pdst1 = pdst0 + 1;
    const unsigned char* psrc0 = src + (byteswap ? 1 : 0);
    const unsigned char* psrc1 = src + (byteswap ? 0 : 1);
    for (int i = 0; i < wordcount; *pdst0 = *psrc0, *pdst1 = *psrc1, pdst0+=2, psrc0+=2, pdst1+=2, psrc1+=2, ++i);
}

void CoreUtil::SwapCopy_0123_2301(unsigned char *dst, const unsigned char *src, int itemCount)
{
    U8* d0 = dst + 0;
    U8* d1 = dst + 1;
    U8* d2 = dst + 2;
    U8* d3 = dst + 3;
    const U8* s = src;
    for(int i = 0; i < itemCount; *d2 = *s++, *d3 = *s++, *d0 = *s++, *d1 = *s++, d0+=4, d1+=4, d2+=4, d3+=4, ++i);
}

void CoreUtil::CopySwapWord(unsigned char *dst, const unsigned char* src, int itemCount)
{
    // This function copy and swap bytes in WORD. itemCount is number of WORDs in buffer
    // d1 = s2; d2 = s1;

    U8* d1 = dst + 0;
    U8* d2 = dst + 1;
    const U8* s = src;
    for(int i = 0;
        i < itemCount;
        *d1 = *s++, *d2 = *s++, /* Copy & Swap Byte 1+2 */
        d1+=2, d2+=2, ++i);
}

void CoreUtil::SwapDWord(unsigned char *buffer, int itemCount)
{
    // This function swap words in DWORD. itemCount is number of DWORDs in buffer
    // 1234 -> 3412

    U8 value;
    U8* d0 = buffer + 0;
    U8* d1 = buffer + 1;
    U8* d2 = buffer + 2;
    U8* d3 = buffer + 3;
    for(int i = 0;
        i < itemCount;
        value = *d0, *d0 = *d2, *d2 = value, /* Swap Byte 1+3 */
        value = *d1, *d1 = *d3, *d3 = value, /* Swap Byte 2+4 */
        d0+=4, d1+=4, d2+=4, d3+=4, ++i);
}

void CoreUtil::SwapWord(unsigned char *buffer, int itemCount)
{
    // This function swap bytes in WORD. itemCount is number of WORDs in buffer
    // 12 -> 21

    U8 value;
    U8* d0 = buffer + 0;
    U8* d1 = buffer + 1;
    for(int i = 0;
        i < itemCount;
        value = *d0, *d0 = *d1, *d1 = value, /* Swap Byte 1+2 */
        d0+=2, d1+=2, ++i);
}

void CoreUtil::CopySwapDWord(unsigned char *dst, const unsigned char* src, int itemCount)
{
    // This function swap words in DWORD. itemCount is number of DWORDs in buffer
    // d1 = s3; d2 = s4; d3 = s1; d4 = s2;

    U8* d1 = dst + 0;
    U8* d2 = dst + 1;
    U8* d3 = dst + 2;
    U8* d4 = dst + 3;
    const U8* s = src;
    for(int i = 0;
        i < itemCount;
        *d3 = *s++, *d4 = *s++, *d1 = *s++, *d2 = *s++, /* Copy & Swap Word 12+34 */
        d1+=4, d2+=4, d3+=4, d4+=4, ++i);
}
