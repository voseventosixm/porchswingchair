#ifndef __STDMACRO_H__
#define __STDMACRO_H__

#define MAXSIZE 1024

#define INIT_BUFFER(name, value) \
    char name[MAXSIZE + 1]; \
    snprintf(name, MAXSIZE + 1, "%s", value.c_str())

#define DEFINE_BUFFER(name, size, capacity) \
    char name[capacity]; \
    size_t size = capacity

#define JSON_BUFFER(json) DEFINE_BUFFER(json##buff, json##size, MAXSIZE)

#define INIT_POINTER(name, base, offset) char* name = (char*) base + offset

#define ASSERT(e) if (!(e)) { printf("Assertion %s at line %d, file %s\n", #e, __LINE__, __FILE__); exit(1); }

#define VOID01(var1)                         (void(var1))
#define VOID02(var1, var2)                   VOID01(var1); VOID01(var2)
#define VOID03(var1, var2, var3)             VOID02(var1, var2); VOID01(var3)
#define VOID04(var1, var2, var3, var4)       VOID03(var1, var2, var3); VOID01(var4)
#define VOID05(var1, var2, var3, var4, var5) VOID04(var1, var2, var3, var4); VOID01(var5)

// RapidJson Reader
#define RJ_TESTKEY(obj, name) (obj.HasMember(name))
#define RJ_TESTBLN(obj, name) (RJ_TESTKEY(obj, name) && obj[name].IsBool())
#define RJ_TESTINT(obj, name) (RJ_TESTKEY(obj, name) && obj[name].IsInt())
#define RJ_TESTDBL(obj, name) (RJ_TESTKEY(obj, name) && obj[name].IsDouble())
#define RJ_TESTSTR(obj, name) (RJ_TESTKEY(obj, name) && obj[name].IsString())
#define RJ_GETBLN(obj, name, value) if (!RJ_TESTBLN(obj, name)) break; value = obj[name].GetBool()
#define RJ_GETINT(obj, name, value) if (!RJ_TESTINT(obj, name)) break; value = obj[name].GetInt()
#define RJ_GETDBL(obj, name, value) if (!RJ_TESTDBL(obj, name)) break; value = obj[name].GetDouble()
#define RJ_GETSTR(obj, name, value) if (!RJ_TESTSTR(obj, name)) break; value = obj[name].GetString()

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

#endif
