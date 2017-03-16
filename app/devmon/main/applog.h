#ifndef __APPLOG_H__
#define __APPLOG_H__

#include "stdheader.h"
#include "stdmacro.h"

// Usage:
// log.set_logstate("devmon", false); -> devmon.txt, truncate mode
// log.set_logstate("devmon", true ); -> devmon_112324532.txt, truncate mode

struct s_app_log
{
    bool logstate;
    string logprefix;

    string logfile;
    fstream logstream;

public:
    void reset();
    void set_logstate(bool state);
    void set_logstate(const string& prefix, bool state, bool longname = true);

    s_app_log();
    ~s_app_log();

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

extern s_app_log app_log;
s_app_log* get_log_ptr();

void init_applog();
void update_logstate();

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
