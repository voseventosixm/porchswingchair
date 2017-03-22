#include "fileutil.h"

bool read_file(const string &filename, string &json)
{
    return false;
}

void write_string(const string &filename, const string &content)
{
    ofstream fstr;
    fstr.open(filename.c_str(), ios::out);

    fstr << content;

    fstr.close();
}

bool read_text(const string &name, string &text)
{
    ifstream fstr;
    fstr.open(name.c_str(), ios::in);

    if (false == fstr.is_open()) return false;

    text = string((istreambuf_iterator<char>(fstr)), istreambuf_iterator<char>());

    return true;
}

bool write_text(const string &name, const string &text)
{
    ofstream fstr;
    fstr.open(name.c_str(), ios::out);

    if (false == fstr.is_open()) return false;

    fstr << text;

    return true;
}

bool read_data(const string &name, U32 offset, U8* buffer, U32 size)
{
    ifstream fstr;
    fstr.open(name.c_str(), ios::in | ios::binary);

    if (false == fstr.is_open()) return false;

    fstr.seekg(offset);

    fstr.read((char*) buffer, size);

    fstr.close();

    return true;
}

bool write_data(const string &name, const U8* buffer, U32 size)
{
    ofstream fstr;
    fstr.open(name.c_str(), ios::out | ios::binary);

    if (false == fstr.is_open()) return false;

    fstr.write((char*) buffer, size);

    fstr.close();

    return true;
}

bool append_data(const string &name, const U8* buffer, U32 size)
{
    ofstream fstr;
    fstr.open(name.c_str(), ios::out | ios::binary | ios::app);

    if (false == fstr.is_open()) return false;

    fstr.write((const char*) buffer, size);

    fstr.close();

    return true;
}
