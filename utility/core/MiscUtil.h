#ifndef MISCUTIL_H
#define MISCUTIL_H

#include "CoreMacro.h"
#include "CoreHeader.h"

typedef pair<string, int> tHeaderItem;

class HeaderList
{
public:
    int Size() const;
    int Length(unsigned int index) const;
    string Text(unsigned int index) const;
    void AddHeader(const tHeaderItem& item);

public:
    vector<int> ToSizeList() const;
    vector<string> ToStdStringList() const;
    string ToString(const char* sep) const;

public:
    vector<tHeaderItem> Header;
};

#define MakeHeaderList(name, cont) HeaderList cont; do { \
    U32 size = sizeof (name) / sizeof (name[0]); \
    for (U32 i = 0; i < size; i++) { \
        tHeaderItem item = name[i]; \
        if (0 == item.second) item.second = item.first.length(); \
        cont.AddHeader(item); \
    }\
} while(0)

class MiscUtil
{
public:
    static int RoundUp(double d);
    static int BitCount(unsigned int value);

    static double RoundPrecision(double d);
    
    static string ToHexString(int value);

    static string ToTimeString(int dayCount, bool approx = false);

    static void GetTimeInfo(int dayCount, int& year, int& month, int& day);

    static string FormatTime(const char* textFormat, const char* timeFormat, time_t sec);

    static string GetRandTempFile();

    static void RequestAdminPermission();

    static string GetLicenseString();
    static string GetCurrentVersion();
    static string GetAboutString(const string& appName, bool longFormat = false);

    static void WriteLog(const string& fileName, const string& log);
    static void AppendLog(const string& fileName, const string& log);
};

#endif // MISCUTIL_H
