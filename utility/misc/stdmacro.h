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

// Format utilities
#define BOOL_STR(val) (val ? "True" : "False")
#define FRMT_DEC(w) setw(w) << setfill('0') << dec
#define FRMT_HEX(w) setw(w) << setfill('0') << hex
#define FRMT_DBL(w, val) right << setw(w) << val                                        /* Format Double */
#define FRMT_U32(w, val) right << setw(w) << setfill('0') << dec << (unsigned int) val  /* Format U32 */
#define FRMT_STR(w, str)  left << setw(w) << setfill(' ') << str                        /* Format String */
#define FRMT_BLN(w, val)  left << setw(w) << setfill(' ') << BOOL_STR(val)              /* Format Boolean */

#endif
