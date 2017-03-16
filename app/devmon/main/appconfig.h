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

struct s_cloud_config
{
    string cloud_name;
    string host_name;
    string client_id;
    string thing_name;
    string cert_filename;
    string rootca_filename;
    string privkey_filename;

    int mqtt_port;

    size_t host_offset;
    size_t client_offset;
    size_t thing_offset;
    size_t cert_offset;
    size_t rootca_offset;
    size_t privkey_offset;

    char string_buffer[MAXSIZE];
};

struct s_identify_config
{
    string model_string;
    string serial_string;
    string firmware_string;

    double capacity;
};

struct s_device_config
{
    string device_name;
    string group_name;
    string location;
    string country;
};

struct s_devmon_config
{
    string shmem_name;
    string shmem_version;

    bool   debug_mode;

    string geoip_path;

    string conf_cloud;
    string conf_device;
    string conf_identify;

    size_t geoip_offset;
    char string_buffer[MAXSIZE];
};

struct s_app_config
{
    s_cloud_config cloud;
    s_device_config device;
    s_identify_config identify;
    s_devmon_config devmon;
};

enum e_config_type
{
    CONFTYPE_INVALID,
    CONFTYPE_CLOUD,
    CONFTYPE_DEVICE,
    CONFTYPE_IDENTIFY,
    CONFTYPE_DEVMON,
};

// Parsing config data
bool parse_params(int argc, char** argv);
bool parse_config(const string& filename);

// Generating config data
void generate_config(s_app_config& conf);
void generate_config(s_cloud_config& conf);
void generate_config(s_device_config& conf);
void generate_config(s_identify_config& conf);
void generate_config(s_devmon_config& conf);
void generate_config(const string& filename, e_config_type conftype);

// Viewing config data
string to_string(const s_app_config& param);
string to_string(const s_cloud_config& param);
string to_string(const s_device_config& param);
string to_string(const s_identify_config& param);
string to_string(const s_devmon_config& param);

// Utilities on configuration data
void reset_config();
void reset_config(s_cloud_config& conf);
void reset_config(s_device_config& conf);
void reset_config(s_identify_config& conf);
void reset_config(s_devmon_config& conf);

#endif
