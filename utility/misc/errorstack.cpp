#include "errorstack.h"

s_error_stack::s_error_stack()
{
    reset();
}

void s_error_stack::reset()
{
    logstack.clear();
}

void s_error_stack::clear()
{
    logstack.clear();
}

bool s_error_stack::is_error() const
{
    return 0 != logstack.size();
}

void s_error_stack::push_error(e_error_code state)
{
    logstack.push_back(state);
}

string s_error_stack::to_string() const
{
    stringstream sstr;

    for (int i = 0; i < logstack.size(); i++)
    {
        sstr << to_string(logstack[i]) << endl;
    }

    return sstr.str();
}

string s_error_stack::to_string(e_error_code code) const
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
