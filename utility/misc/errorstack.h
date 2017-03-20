#ifndef __ERRORSTACK_H__
#define __ERRORSTACK_H__

#include "stdmacro.h"
#include "stdheader.h"

enum e_error_code
{
    #define MAP_ITEM(code, text) code,
    #include "errorcode.def"
    #undef MAP_ITEM
};

struct s_error_stack
{
    vector<e_error_code> logstack;

public:
    void reset();
    s_error_stack();

public:
    void clear();
    bool is_error() const;
    void push_error(e_error_code state);

    string to_string() const;
    string to_string(e_error_code code) const;
};

#endif
