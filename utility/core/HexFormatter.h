#ifndef HEXFORMATTER_H
#define HEXFORMATTER_H

#include "CoreMacro.h"
#include "CoreHeader.h"

struct HexFormatter
{
public:
    enum eValueMode
    {
        MODE_BIN,
        MODE_HEX,
    };

public:
    U32 ColSize;
    U32 ColCount;
    U32 Offset;

    bool ShowHex;
    bool ShowText;
    bool ShowOffset;

    eValueMode Mode;

    HexFormatter();

    void setMode(eValueMode mode);
    void setSize(U32 colSize = 8, U32 colCount = 4, U32 offset = 0);
    void setState(bool showHex = true, bool showText = true, bool showOffset = true);

    string toString(const U8* bufPtr, U32 bufSize) const;
    void saveString(const string& fileName, const U8* bufPtr, U32 bufSize) const;

public:
    // static function, using default formatter
    static string ToHexString(const U8* bufPtr, U32 bufSize);
    static void SaveHexString(const string& fileName, const U8* bufPtr, U32 bufSize);
};

#endif //HEXFORMATTER_H
