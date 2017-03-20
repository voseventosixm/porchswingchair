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

extern s_debug_log debug_log;
s_debug_log* get_log_ptr();

void update_logstate(bool state);
void init_debuglog(const string& prefix);

// LOG macros

#define LOGMSG(...) do { \
        char content[MAXSIZE + 1], buffer[MAXSIZE + 1]; \
        sprintf(buffer, __VA_ARGS__); \
        sprintf(content, "[%s:%04d] %s", __FUNCTION__, __LINE__, buffer); \
        get_log_ptr()->add(string(content)); \
    } while(0)

#define LOGSTR(frmt, valstr) LOGMSG(frmt, valstr.c_str())
#define LOGMSGIF(cond, ...) if (cond) LOGMSG(__VA_ARGS__)
#define LOGSTRIF(cond, ...) if (cond) LOGSTR(__VA_ARGS__)

#endif
