#ifndef __APPUTIL_H__
#define __APPUTIL_H__

#include "appdata.h"

// ------------------------------------------------------
// common utility
// ------------------------------------------------------

void set_error_code(e_error_code code);
void set_error_if(bool cond, e_error_code code);

void stop_worker_thread();

void join_worker_thread();

// ------------------------------------------------------
// common utility
// ------------------------------------------------------
void write_string(const string& filename, const string& content);

// ------------------------------------------------------
// text utility
// ------------------------------------------------------

void print_license();
void print_version();
void print_help(bool state);

#endif
