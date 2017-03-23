#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include "stdmacro.h"
#include "stdheader.h"

bool read_file(const string& filename, string& json);
void write_string(const string& filename, const string& content);

bool read_text(const string& name, string& text);
bool write_text(const string& name, const string& text);

bool write_data(const string &name, const U8* buffer, U32 size);
bool append_data(const string &name, const U8* buffer, U32 size);
bool read_data(const string &name, U32 offset, U8* buffer, U32 size);

bool is_directory(const string& path);
bool is_regular_file(const string& path);
bool read_filelist(const string& root, vector<string>& namelist);

#endif
