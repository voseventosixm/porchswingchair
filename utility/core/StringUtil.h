#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include "CoreMacro.h"
#include "CoreHeader.h"

class StringUtil
{
public:
    struct sUtilData
    {
        bool m_Initialized;

        string m_PathDelimiter;

        sUtilData();
    };

public:
    static void SystemInit();

public:
    // String processing
    static string& LeftTrim(string& inputString);
    static string& RightTrim(string& inputString);
    static string& Trim(string& inputString);
    static void Trim(vector<string>& inputList);

    static void RemoveChars(string &str, char* charsToRemove);
    
    static void Split(const string& inputString, char delim, vector<string>& itemArray);
    static void HardSplit(const string& inputString, char delim, vector<string>& itemArray);
    static void HardSplit(const string& inputString, const string& delim, vector<string>& itemArray);

    static string Concat(const vector<string>& inputList, const string& sep);
    static string Concat(const vector<string>& inputList, const string& sep, U32 startIndex, U32 count = 0);

public:
    static bool IsDecimal(const string& strNum);

    static string ToUppercase(const string& str);
    static string ToLowercase(const string& str);

    static bool IsHexNumber(const string& str);
    static bool IsBinNumber(const string& str);

    // Not implemented yet
    // static bool IsDecNumber(const string& str);
    // static bool IsRealNumber(const string& str);
    // static bool IsNominalString(const string& str);
    // static bool IsLiteralString(const string& str);

    // Test with objects (string / char)
    static bool StartWith(const string& str, const string& pattern, bool skipSpace);
    static bool EndWith(const string& str, const string& pattern, bool skipSpace);

    // Test with charset
    static bool StartWithCharset(const string& str, const string& pattern, bool skipSpace);
    static bool EndWithCharset(const string& str, const string& pattern, bool skipSpace);

    static size_t GetCharCount(const string& str);
    static size_t GetDigitCount(const string& str);

    static string ToCapitalString(const string& str);
    static string NormalizePuncs(const string& str);

    static string ToDecString(U32 w, U32    value);
    static string ToDecString(U32 w, double value);

    static string ToHexString(U32 w, U32    value);

    static string ToSimpleForm00(const string& str);

private:
    static sUtilData s_Data;
};

#endif //STRINGUTIL_H
