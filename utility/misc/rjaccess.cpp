#include "rjaccess.h"

#define MAP_ITEM(doctype, valtype, testfunc, getfunc) \
    bool rj_get(const doctype& obj, const char* name, valtype& value) { \
        bool status = RJ_TESTKEY(obj, name) && obj[name].testfunc(); \
        if(status) obj[name].getfunc(); \
        return status; \
    }

MAP_ITEM(Value   , bool  , IsBool  , GetBool  )
MAP_ITEM(Value   , int   , IsInt   , GetInt   )
MAP_ITEM(Value   , double, IsDouble, GetDouble)
MAP_ITEM(Value   , string, IsString, GetString)

MAP_ITEM(Document, bool  , IsBool  , GetBool  )
MAP_ITEM(Document, int   , IsInt   , GetInt   )
MAP_ITEM(Document, double, IsDouble, GetDouble)
MAP_ITEM(Document, string, IsString, GetString)

#undef MAP_ITEM
