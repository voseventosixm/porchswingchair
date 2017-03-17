#ifndef __APPERROR_H__
#define __APPERROR_H__

#include "stdmacro.h"
#include "stdheader.h"

enum e_error_code
{
    #define MAP_ITEM(code, text) code,
    #include "errorcode.def"
    #undef MAP_ITEM
};

struct s_app_error
{
    vector<e_error_code> logstack;

public:
    void reset();
    s_app_error();

public:
    void clear();
    bool is_error() const;
    void push_error(e_error_code state);

    string to_string() const;
    string to_string(e_error_code code) const;
};

#endif
