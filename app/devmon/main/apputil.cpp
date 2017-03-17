
#include "apputil.h"
#include "appdata.h"
#include "appconfig.h"
#include "mqtthandler.h"

// ------------------------------------------------------
// common utilities
// ------------------------------------------------------

void print_help(bool state)
{
    const string usage =
        #include "vt_usage.txt"
    ;

    cout << (state ? "" : "Invalid arguments!") << endl
         << usage << endl;
}

void print_license()
{
    const string license =
        #include "vt_license.txt"
    ;

    cout << license << endl;
}

void print_version()
{
    const string version =
        #include "vt_version.txt"
    ;

    cout << version << endl;
}

// ------------------------------------------------------
// common utilities
// ------------------------------------------------------

void write_string(const string &filename, const string &content)
{
    ofstream fstr;
    fstr.open(filename.c_str(), ios::out);

    fstr << content;

    fstr.close();
}

// ------------------------------------------------------
// application utilities
// ------------------------------------------------------

void set_error_code(e_error_code code)
{
    get_error_stack()->push_error(code);
}

void set_error_if(bool cond, e_error_code code)
{
    if (cond) set_error_code(code);
}

void stop_worker_thread()
{
    stop_mqtt_handler();
}

void join_worker_thread()
{
    join_mqtt_handler();
}
