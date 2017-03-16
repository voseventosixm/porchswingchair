#include "StringUtil.h"

StringUtil::sUtilData StringUtil::s_Data;

StringUtil::sUtilData::sUtilData()
{
    m_PathDelimiter = string("/\\");
    m_Initialized = true;
}

void SystemInit_StringUtil()
{
    StringUtil::SystemInit();
}

void StringUtil::SystemInit()
{
    s_Data.m_PathDelimiter = string("/\\");

    s_Data.m_Initialized = true;
}

string& StringUtil::LeftTrim(string& s)
{
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));

    return s;
}

string& StringUtil::RightTrim(string& s)
{
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());

    return s;
}

string& StringUtil::Trim(string& s)
{
    return LeftTrim(RightTrim(s));
}

void StringUtil::Trim(vector<string>& strList)
{
    for (int i = 0, strCount = strList.size(); i < strCount; i++) Trim(strList[i]);
}

void StringUtil::Split(const string& inputString, char delim, vector<string>& itemArray)
{
    itemArray.clear();

    string line;
    stringstream sstr(inputString);
    while(getline(sstr, line, delim))
    {
        itemArray.push_back(line);
    }
}

void StringUtil::HardSplit(const string& inputString, char delim, vector<string>& itemArray)
{
    itemArray.clear();

    string line;
    stringstream sstr(inputString);
    while(getline(sstr, line, delim))
    {
        if(0 == line.length()) continue;
        itemArray.push_back(Trim(line));
    }
}

void StringUtil::HardSplit(const string& inputString, const string& delim, vector<string>& itemArray)
{
    itemArray.clear();

    size_t next = 0, last = 0;
    size_t len = delim.length();

    while ((next = inputString.find(delim, last)) != string::npos)
    {
        itemArray.push_back(inputString.substr(last, next - last));
        last = next + len;
    }

    itemArray.push_back(inputString.substr(last));
}

string StringUtil::Concat(const vector<string> &inputList, const string &sep)
{
    return Concat(inputList, sep, 0, inputList.size());
}

string StringUtil::Concat(const vector<string> &inputList, const string &sep, U32 startIndex, U32 count)
{
    string res;

    U32 itemcount = inputList.size();
    U32 endidx = (0 == count) ? itemcount : MIN2(itemcount, startIndex + count);

    for (U32 i = startIndex; i < endidx; i++) res += inputList[i] + sep;

    U32 reslen = res.length();
    if (0 != reslen) res = res.substr(0, reslen - sep.length());

    return res;
}

void StringUtil::RemoveChars(string &str, char *charsToRemove)
{
    int charCount = strlen(charsToRemove);
    
    for (int i = 0; i < charCount; i++)
    {
        str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
}

string StringUtil::ToUppercase(const string &str)
{
    string outstr = str;
    for (U32 i = 0, len = str.length(); i < len; i++) outstr[i] = toupper(str[i]);

    return outstr;
}

string StringUtil::NormalizePuncs(const string &str)
{
    const string orgitem = "[{}]_:";
    const string mapitem = "(())--";

    string newstr = str;

    for (U32 i = 0, sc = str.length(); i < sc; i++)
    {
        size_t cpos = orgitem.find(str[i]);
        if (string::npos != cpos) newstr[i] = mapitem[cpos];
    }

    return newstr;
}

string StringUtil::ToCapitalString(const string &str)
{
    string normstr;

    // Lowercase all chars
    normstr = StringUtil::ToLowercase(str);

    // Remove punctuations
    normstr = StringUtil::NormalizePuncs(normstr);

    vector<string> parts;
    StringUtil::HardSplit(normstr, ' ', parts);

    // Capitalize first char
    for (U32 i = 0, pc = parts.size(); i < pc; i++) { string& p = parts[i]; p[0] = toupper(p[0]); }

    return StringUtil::Concat(parts, " ");
}

string StringUtil::ToSimpleForm00(const string &str)
{
    // Lowercase
    string str0 = StringUtil::ToLowercase(str);

    if (0 == str0.length()) return str;

    // Remove spaces
    string str1;
    for (U32 i = 0, len = str0.length(); i < len; i++) {
        char c = str0[i];
        if (isalpha(c)) str1 += c;
    }

    if (0 == str1.length()) return str0;

    return str1;
}

string StringUtil::ToLowercase(const string &str)
{
    string outstr = str;
    for (U32 i = 0, len = str.length(); i < len; i++) outstr[i] = tolower(str[i]);

    return outstr;
}

bool StringUtil::IsDecimal(const string& strNum)
{
    size_t strLen = strNum.length();

    if(0 == strLen) return false;

    for(size_t nPos = 0; nPos < strLen; ++ nPos )
    {
        if(0 == isdigit(strNum[nPos])) return false;
    }

    return true;
}

bool StringUtil::IsBinNumber(const string &str)
{
    const char* p = str.c_str();

    while(*p && ((*p >= '0') && (*p <= '1'))) p++;

    return *p == 0; // point to last position
}

bool StringUtil::IsHexNumber(const string &str)
{
    const char* p = str.c_str();

    if (2 >= str.length()) return false;

    if ((p[0] != '0') || (
                (p[1] != 'x') &&
                (p[1] != 'X'))) return false;

    p += 2;
    while(*p && (
              ((*p >= '0') && (*p <= '9')) ||
              ((*p >= 'A') && (*p <= 'F')) ||
              ((*p >= 'a') && (*p <= 'f')))) p++;

    return *p == 0; // point to last position
}

bool StringUtil::StartWith(const string &str, const string &pattern, bool skipSpace)
{
    size_t strSize = str.length();
    size_t patSize = pattern.length();
    size_t startPat = str.find(pattern);
    size_t endPat = startPat + patSize;
    size_t startTest = 0, endTest = startPat;

    if (startPat == string::npos) return false;
    if (!skipSpace && (startTest != endTest)) return false;

    size_t i;
    for (i = startTest; (i < endTest) && skipSpace && isspace(str[i]); i++);

    return i == endTest;
}

bool StringUtil::EndWith(const string &str, const string &pattern, bool skipSpace)
{
    size_t strSize = str.length();
    size_t patSize = pattern.length();
    size_t startPat = str.rfind(pattern);
    size_t endPat = startPat + patSize;
    size_t startTest = endPat, endTest = strSize;

    if (startPat == string::npos) return false;
    if (!skipSpace && (startTest != endTest)) return false;

    size_t i;
    for (i = startTest; (i < endTest) && skipSpace && isspace(str[i]); i++);

    return i == endTest;
}

bool StringUtil::StartWithCharset(const string &str, const string& pattern, bool skipSpace)
{
    const char* p = str.c_str();
    if (skipSpace) while(*p && isspace(*p)) p++;

    return *p && (string::npos != pattern.find(*p));
}

bool StringUtil::EndWithCharset(const string &str, const string& pattern, bool skipSpace)
{
    size_t len = str.length();
    if (0 == len) return false;

    const char* p0 = str.c_str();
    const char* p1 = p0 + len - 1;

    if (skipSpace) while((p1 != p0) && isspace(*p1)) p1--;

    return (string::npos != pattern.find(*p1));
}

size_t StringUtil::GetDigitCount(const string &str)
{
    size_t sum = 0;
    for (size_t i = 0, len = str.length(); i < len; i++)
    {
        char c = str[i];

        if (('0' <= c) && (c <= '9')) sum++;
    }

    return sum;
}

size_t StringUtil::GetCharCount(const string &str)
{
    size_t sum = 0;
    for (size_t i = 0, len = str.length(); i < len; i++)
    {
        char c = str[i];

        if ((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'))) sum++;
    }

    return sum;
}

string StringUtil::ToDecString(U32 w, U32 value)
{
    stringstream sstr;
    sstr << FRMT_U32(w, value);

    return sstr.str();
}

string StringUtil::ToHexString(U32 w, U32 value)
{
    stringstream sstr;
    sstr << FRMT_HEX(w) << value;

    return sstr.str();
}

string StringUtil::ToDecString(U32 w, double value)
{
    stringstream sstr;
    sstr << FRMT_DBL(w, value);

    return sstr.str();
}
