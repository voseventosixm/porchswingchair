#include "MiscUtil.h"

int HeaderList::Size() const
{
    return Header.size();
}

int HeaderList::Length(unsigned int index) const
{
    return (index < Header.size()) ? Header[index].second : 0;
}

string HeaderList::Text(unsigned int index) const
{
    return (index < Header.size()) ? Header[index].first : "";
}

void HeaderList::AddHeader(const tHeaderItem &item)
{
    Header.push_back(item);
}

string HeaderList::ToString(const char *sep) const
{
    stringstream sstr;

    int size = Header.size();
    for(int i = 0; i < size; i++)
    {
        sstr
            << setfill(' ') << setw(Header[i].second)
            << Header[i].first
            << sep;
    }
    return sstr.str();
}

vector<string> HeaderList::ToStdStringList() const
{
    vector<string> lstr;

    int size = Header.size();
    for(int i = 0; i < size; i++) lstr.push_back(Header[i].first);

    return lstr;
}

vector<int> HeaderList::ToSizeList() const
{
    vector<int> lst;

    int size = Header.size();
    for(int i = 0; i < size; i++) lst.push_back( Length(i) );
    return lst;
}

string MiscUtil::ToHexString(int value)
{
    stringstream sstr;
    sstr << hex << uppercase << value;
    return sstr.str();
}

void MiscUtil::GetTimeInfo(int dayCount, int &year, int &month, int &day)
{
    year = dayCount / 365; dayCount %= 365;
    month = dayCount / 30; dayCount %= 30;
    day = dayCount;
}

string MiscUtil::ToTimeString(int dayCount, bool approx)
{
    stringstream sstr;
    int year, month, day;
    GetTimeInfo(dayCount, year, month, day);

    do
    {
        if(0 != year)
        {
            sstr << year << ((year > 1) ? " years" : " year");
        }

        if(0 != month)
        {
            if(year != 0) sstr << " ";
            sstr << month << ((month > 1) ? " months" : " month");
            if((year != 0) && approx) break;
        }

        if(0 != day)
        {
            if((month != 0) || (year != 0)) sstr << " ";
            sstr << day << ((day > 1) ? " days" : " day");
        }

        if((0 == year) && (0 == month) && (0 == day))
        {
            sstr << "less than 1 day";
        }
    } while(0);

    return sstr.str();
}

int MiscUtil::BitCount(unsigned int value)
{
    int index = 0;
    while(0 != value) { value >>= 1; index++; }
    return index;
}

int MiscUtil::RoundUp(double d)
{
    int res;

    if (d <= 1) return 1;

    int bitCount = BitCount((int) d);

    res = (1 << bitCount) >> 1;
    if (res < d) res <<= 1;

    return res;
}

double MiscUtil::RoundPrecision(double d)
{
    return (int) (d * 100) / 100.0;
}

string MiscUtil::FormatTime(const char* textFormat, const char* timeFormat, time_t sec)
{
    char timeBuff[80];
    int buffSize = sizeof(timeBuff) / sizeof(timeBuff[0]);
    strftime( timeBuff, buffSize, timeFormat, localtime(&sec));

    string timeString;
    FORMAT_STRING(timeString, textFormat, timeBuff);
    return timeString;
}

string MiscUtil::GetRandTempFile()
{
    U32 val0 = rand() % 65536;
    U32 val1 = rand() % 65536;
    U32 val2 = rand() % 65536;

    string name = FormatTime("DATA_%s", "%Y%m%d_%H%M%S", time(0));

    stringstream sstr;
    sstr << name << "_"
         << FRMT_U32(5, val0)
         << FRMT_U32(5, val1)
         << FRMT_U32(5, val2)
         << ".TMP";

    return sstr.str();
}

void MiscUtil::RequestAdminPermission()
{
	return;
}

void MiscUtil::WriteLog(const string &fileName, const string &log)
{
    ofstream fstr;
    fstr.open(fileName.c_str(), ios::out);

    fstr << log;
    fstr.close();
}

void MiscUtil::AppendLog(const string &fileName, const string &log)
{
    ofstream fstr;
    fstr.open(fileName.c_str(), ios::out | ios::app);

    fstr << log;
    fstr.close();
}

string MiscUtil::GetLicenseString()
{
    return "InvalidLicense";
}

string MiscUtil::GetAboutString(const string &appName, bool longFormat)
{
    const string sep = longFormat ? "" : "\n";

    stringstream sstr;

    sstr << appName;
    sstr << "\nVersion " << GetCurrentVersion();
    sstr << "\nCopyright (C) 2017 Virtium LLC";
    sstr << "\nAll rights reserved.\n";
    sstr << "\nFor support or question, contact " << sep << "software.support@virtium.com";

    return sstr.str();
}

string MiscUtil::GetCurrentVersion()
{
    return "1.0.2338";
}
