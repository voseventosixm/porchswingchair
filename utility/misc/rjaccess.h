#ifndef __RJACCESS_H__
#define __RJACCESS_H__

#include "stdmacro.h"
#include "stdheader.h"

#include "document.h"
#include "prettywriter.h"

using namespace rapidjson;

// RapidJson Reader
#define RJ_TESTKEY(obj, name) (obj.HasMember(name))
#define RJ_TESTBLN(obj, name) (RJ_TESTKEY(obj, name) && obj[name].IsBool())
#define RJ_TESTINT(obj, name) (RJ_TESTKEY(obj, name) && obj[name].IsInt())
#define RJ_TESTDBL(obj, name) (RJ_TESTKEY(obj, name) && obj[name].IsDouble())
#define RJ_TESTSTR(obj, name) (RJ_TESTKEY(obj, name) && obj[name].IsString())

#define RJ_GETBLN(obj, name, value) if (RJ_TESTBLN(obj, name)) value = obj[name].GetBool()
#define RJ_GETINT(obj, name, value) if (RJ_TESTINT(obj, name)) value = obj[name].GetInt()
#define RJ_GETDBL(obj, name, value) if (RJ_TESTDBL(obj, name)) value = obj[name].GetDouble()
#define RJ_GETSTR(obj, name, value) if (RJ_TESTSTR(obj, name)) value = obj[name].GetString()

// RapidJson Writer
#define RJ_TOSTRING() sb.GetString()
#define RJ_DECWRITER() StringBuffer sb; PrettyWriter<StringBuffer> writer(sb);
#define RJ_START() writer.StartObject()
#define RJ_STOP() writer.EndObject()
#define RJ_PUTKEY(key) writer.String(key)
#define RJ_PUTBLN(val) writer.Bool(val)
#define RJ_PUTDBL(val) writer.Double(val)
#define RJ_PUTINT(val) writer.Int(val)
#define RJ_PUTSTR(val) writer.String(val.c_str())

#define RJ_ADDBLN(key, val) RJ_PUTKEY(key); RJ_PUTBLN(val)
#define RJ_ADDINT(key, val) RJ_PUTKEY(key); RJ_PUTINT(val)
#define RJ_ADDDBL(key, val) RJ_PUTKEY(key); RJ_PUTDBL(val)
#define RJ_ADDSTR(key, val) RJ_PUTKEY(key); RJ_PUTSTR(val)

// RapidJson Access functions

#define MAP_ITEM(doctype, valtype) \
    bool rj_get(const doctype& obj, const char* name, valtype& value)

MAP_ITEM(Value   , bool  );
MAP_ITEM(Value   , int   );
MAP_ITEM(Value   , double);
MAP_ITEM(Value   , string);

MAP_ITEM(Document, bool  );
MAP_ITEM(Document, int   );
MAP_ITEM(Document, double);
MAP_ITEM(Document, string);

#undef MAP_ITEM

#endif
