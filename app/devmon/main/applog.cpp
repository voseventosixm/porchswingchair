
#include "stdmacro.h"
#include "stdheader.h"
#include "apputil.h"

#include "applog.h"

s_app_log app_log;

s_app_log* get_log_ptr()
{
    return &app_log;
}

void init_applog()
{
    get_log_ptr()->set_logstate("devmon", true, false);
}

void update_logstate()
{
    get_log_ptr()->set_logstate(get_config_ptr()->devmon.debug_mode);
}

// -----------------------------------------------------
// implement application logger
// -----------------------------------------------------

void s_app_log::open_logstream()
{
    logstream.open(logfile.c_str(), ios::out | ios::trunc);
}

void s_app_log::close_logstream()
{
    if (true == logstream.is_open())
    {
        logstream.flush(); logstream.close();
    }
}

void s_app_log::set_logstate(bool state)
{
    set_logstate(logprefix, state);
}

void s_app_log::set_logstate(const string &prefix, bool state, bool longname)
{
    close_logstream();

    logstate = state;
    logprefix = prefix;

    if (true == logstate)
    {
        stringstream sstr;
        sstr << logprefix;
        if (longname) sstr << "_" << time_string();
        sstr << ".wl";

        logfile = sstr.str();

        open_logstream();
    }
}

bool s_app_log::test_logstream() const
{
    return logstate && logstream.is_open();
}

string s_app_log::time_string() const
{
    time_t rawtime; time(&rawtime);
    tm* timeinfo = localtime(&rawtime);

    char buffer [80];
    strftime(buffer, 80, "%Y%m%d_%H%M%S", timeinfo);

    return string(buffer);
}

string s_app_log::time_prefix() const
{
    return "[" + time_string() + "] ";
}

void s_app_log::reset()
{
    logfile = "";

    logstate = true;
    logprefix = "applog";

    close_logstream();
}

s_app_log::s_app_log()
{
    reset();
}

s_app_log::~s_app_log()
{
    close();
}

void s_app_log::close()
{
    close_logstream();
}

void s_app_log::add(const string &log)
{
    string prefix = time_prefix();

    if (!test_logstream())
    {
        cout << prefix << log << endl;
    }
    else
    {
        logstream << time_prefix() << log << endl;
        logstream.flush();
    }
}
