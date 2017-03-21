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
        e_error_code code = logstack[i];

        sstr << "[" << FRMT_U32(2, i) << "] "
             << "# " << FRMT_STR(36, to_enum_string(code)) << " "
             << "# " << FRMT_STR(36, to_text_string(code)) << " "
             << endl;
    }

    return sstr.str();
}

string s_error_stack::to_text_string(e_error_code code) const
{
    stringstream sstr;

    switch(code)
    {
    #define MAP_ITEM(code, text) case code: sstr << text; break;
    #include "errorcode.def"
    #undef MAP_ITEM

    default: ASSERT(0); break;
    }

    return sstr.str();
}

string s_error_stack::to_enum_string(e_error_code code) const
{
    stringstream sstr;

    switch(code)
    {
    #define MAP_ITEM(code, text) case code: sstr << #code; break;
    #include "errorcode.def"
    #undef MAP_ITEM

    default: ASSERT(0); break;
    }

    return sstr.str();
}
