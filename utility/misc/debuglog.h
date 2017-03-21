#ifndef __DEBUGLOG_H__
#define __DEBUGLOG_H__

#include "stdheader.h"
#include "stdmacro.h"

struct s_debug_log
{
    bool logstate;
    string logprefix;

    string logfile;
    fstream logstream;

public:
    void reset();
    void set_logstate(bool state);
    void set_logstate(const string& prefix, bool state, bool longname = true);

    s_debug_log();
    ~s_debug_log();

public:
    void add(const string& log);
    void close();

private:
    void open_logstream();
    void close_logstream();

    string time_string() const;
    string time_prefix() const;

    bool test_logstream() const;
};

// ----------------------------------------------------
// Application level logger
// ----------------------------------------------------

s_debug_log* get_log_ptr();

void update_logstate(bool state);
void init_debuglog(const string& prefix);

// ----------------------------------------------------
// LOG macros
// ----------------------------------------------------

#define FORMAT_BUFFER(msgbuff, ...)\
        char msgbuff[MAXSIZE + 1], \
        char argbuff[MAXSIZE + 1]; \
        sprintf(argbuff, __VA_ARGS__); \
        sprintf(msgbuff, "[%s(%s:%04d)] %s", __FUNCTION__, __FILE__, __LINE__, argbuff); \
    } while(0)

#define LOGMSG(...) do { \
        FORMAT_BUFFER(msgbuff, __VA_ARGS__); \
        get_log_ptr()->add(string(msgbuff)); } while(0)

#define LOGSTR(frmt, valstr) LOGMSG(frmt, valstr.c_str())
#define LOGMSGIF(cond, ...) if (cond) LOGMSG(__VA_ARGS__)
#define LOGSTRIF(cond, ...) if (cond) LOGSTR(__VA_ARGS__)

#define SHOWMSG(...) do { \
        FORMAT_BUFFER(msgbuff, __VA_ARGS__); \
        cout << msgbuff << endl; } while(0)

#define SHOWSTR(frmt, valstr) SHOWMSG(frmt, valstr.c_str())
#define SHOWMSGIF(cond, ...) if (cond) SHOWMSG(__VA_ARGS__)
#define SHOWSTRIF(cond, ...) if (cond) SHOWSTR(__VA_ARGS__)

#endif
