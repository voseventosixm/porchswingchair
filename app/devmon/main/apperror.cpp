#include "apperror.h"

s_app_error::s_app_error()
{
    reset();
}

void s_app_error::reset()
{
    logstack.clear();
}

void s_app_error::clear()
{
    logstack.clear();
}

bool s_app_error::is_error() const
{
    return 0 != logstack.size();
}

void s_app_error::push_error(e_error_code state)
{
    logstack.push_back(state);
}

string s_app_error::to_string() const
{
    stringstream sstr;

    for (int i = 0; i < logstack.size(); i++)
    {
        sstr << to_string(logstack[i]) << endl;
    }

    return sstr.str();
}

string s_app_error::to_string(e_error_code code) const
{
    stringstream sstr;

    switch(code)
    {
    #define MAP_ITEM(code, text) case code: sstr << text; break;
    #include "errorcode.def"
    #undef MAP_ITEM

    default: sstr << "Unknown error code"; break;
    }

    return sstr.str();
}
