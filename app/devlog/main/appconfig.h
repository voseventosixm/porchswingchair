#ifndef __APPCONFIG_H__
#define __APPCONFIG_H__

#include  "sysheader.h"
#include  "stdheader.h"
#include  "stdmacro.h"

#define MAP_ITEM(name, keyval, defval) \
    extern const char* KEY_##name; \
    extern const char* DEF_##name;
#include "configkey.def"
#undef MAP_ITEM

struct s_program_config
{
    bool   debug_mode;

    string shmem_name;
    string shmem_version;

    string config_path;
    string backup_path;
    string binary_path;
    string device_path;

    unsigned int freq_fulllog; // logging time in seconds
    unsigned int freq_currlog; // logging time in seconds
};

struct s_app_config
{
    s_program_config program;
};

enum e_config_type
{
    CONFTYPE_INVALID,
    CONFTYPE_PROGRAM,
};

// Parsing config data
bool parse_params(int argc, char** argv);
bool parse_config(const string& filename);

// Generating config data
void generate_config(s_app_config& conf);
void generate_config(s_program_config& conf);
void generate_config(const string& filename, e_config_type conftype);

// Viewing config data
string to_string(const s_app_config& param);
string to_string(const s_program_config& param);

// Utilities on configuration data
void reset_config();
void reset_config(s_program_config& conf);

#endif
