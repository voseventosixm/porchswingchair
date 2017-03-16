#include "stdmacro.h"
#include "rjaccess.h"

#include "appdata.h"
#include "apputil.h"
#include "appconfig.h"

#define MAP_ITEM(name, keyval, defval) \
    const char* KEY_##name = keyval; \
    const char* DEF_##name = defval;
#include "configkey.def"
#undef MAP_ITEM

#define RJ_GET(src, keyword, keyvalue) \
    if (!rj_get(src, keyword, keyvalue)) \
        { LOGMSG("Cannot parse %s", keyword); break; }

// -----------------------------------------------------------------
// Generate config data
// -----------------------------------------------------------------

void generate_config(s_app_config& conf)
{
    generate_config(conf.cloud);
    generate_config(conf.device);
    generate_config(conf.identify);
    generate_config(conf.devmon);
}

void generate_config(s_cloud_config &config)
{
    config.cloud_name = DEF_ACTIVE_CLOUD;
    config.host_name = DEF_HOST_NAME;
    config.client_id = DEF_CLIENT_ID;
    config.thing_name = DEF_THING_NAME;
    config.cert_filename = DEF_CERT_FILE;
    config.rootca_filename = DEF_ROOTCA_FILE;
    config.privkey_filename = DEF_PRIVKEY_FILE;
    config.mqtt_port = atoi(DEF_MQTT_PORT);
}

void generate_config(s_device_config &config)
{
    config.device_name = DEF_DEVICE_NAME;
    config.group_name = DEF_GROUP_NAME;
    config.location = DEF_LOCATION;
    config.country = DEF_COUNTRY;
}

void generate_config(s_identify_config &config)
{
    config.capacity = atof(DEF_CAPACITY);;
    config.model_string = DEF_MODEL;
    config.serial_string = DEF_SERIAL;
    config.firmware_string = DEF_FIRMWARE;
}

void generate_config(s_devmon_config &config)
{
    config.debug_mode = false;

    config.shmem_name = DEF_SHMEM_NAME;
    config.shmem_version = DEF_SHMEM_VERSION;

    config.geoip_path = DEF_GEOIP_PATH;

    config.conf_cloud = DEF_CLOUD_CONFIG;
    config.conf_identify = DEF_IDENTIFY_CONFIG;
}

void generate_config(const string &filename, e_config_type conftype)
{
    #define MAP_ITEM(code, datatype) case code: { \
        datatype conf; generate_config (conf); write_string(filename, to_string(conf)); } break;

    switch(conftype)
    {
    MAP_ITEM(CONFTYPE_CLOUD, s_cloud_config);
    MAP_ITEM(CONFTYPE_DEVICE, s_device_config);
    MAP_ITEM(CONFTYPE_IDENTIFY, s_identify_config);
    MAP_ITEM(CONFTYPE_DEVMON, s_devmon_config);
    default: break;
    }

    #undef MAP_ITEM
}

// -----------------------------------------------------------------
// Parse config data
// -----------------------------------------------------------------

static bool parse_cloud_config();
static bool parse_device_config();
static bool parse_identify_config();
static bool parse_devmon_config();

bool parse_params(int argc, char **argv)
{
    s_app_data* adptr = get_data_ptr();

    if (argc < 2) { print_help(false); exit(1); }

    reset_config();

    string flag = argv[1];

    #define IFEQ(text, count, code) if ((0 == flag.compare(text)) && (count == argc)) opcode = code
    #define ELIF(text, count, code) else IFEQ(text, count, code)

    e_operation_code opcode = OP_INVALID;

    IFEQ( "-h", 2, OP_SHOW_HELP);
    ELIF( "-l", 2, OP_SHOW_LICENSE);
    ELIF( "-v", 2, OP_SHOW_VERSION);
    ELIF( "-f", 3, OP_MONITOR_DEVICE);
    ELIF("-ga", 3, OP_GENCONF_APP);
    ELIF("-gc", 3, OP_GENCONF_CLOUD);
    ELIF("-gi", 3, OP_GENCONF_IDENTIFY);
    ELIF("-gd", 3, OP_GENCONF_DEVICE);

    #undef IFEQ
    #undef ELIF

    adptr->opcode = opcode;

    #define MAP_ITEM(id, func, excode) case id: func; exit(excode); break

    switch(adptr->opcode)
    {
    MAP_ITEM(OP_INVALID,         print_help(false), 1);
    MAP_ITEM(OP_SHOW_HELP,       print_help( true), 0);
    MAP_ITEM(OP_SHOW_LICENSE,    print_license(), 0);
    MAP_ITEM(OP_SHOW_VERSION,    print_version(), 0);
    MAP_ITEM(OP_GENCONF_CLOUD,   generate_config(argv[2], CONFTYPE_CLOUD), 0);
    MAP_ITEM(OP_GENCONF_DEVICE,  generate_config(argv[2], CONFTYPE_DEVICE), 0);
    MAP_ITEM(OP_GENCONF_IDENTIFY,generate_config(argv[2], CONFTYPE_IDENTIFY), 0);
    MAP_ITEM(OP_GENCONF_APP,     generate_config(argv[2], CONFTYPE_DEVMON), 0);

    case OP_MONITOR_DEVICE: return parse_config(argv[2]); break;

    default: ASSERT(0); break;
    }

    #undef MAP_ITEM

    return true;
}

bool parse_config(const string &filename)
{
    s_app_data* dptr = get_data_ptr();

    if (NULL == dptr) return false;

    dptr->conf_app = filename;

    bool status = false;

    do {
        if (false == parse_devmon_config()) break;

        if (false == parse_cloud_config()) break;
        if (false == parse_device_config()) break;
        if (false == parse_identify_config()) break;

        status = true;
    } while(0);

    if (true == status) update_logstate();

    return status;
}

bool parse_devmon_config()
{
    s_app_data* dptr = get_data_ptr();

    bool status = false;
    do {
        Document doc;

        ifstream istr; istr.open(dptr->conf_app.c_str(), ios::binary);
        if (false == istr.is_open())
            { LOGSTR("Cannot open devmon config file: %s", dptr->conf_app); break; }

        stringstream sstr; sstr << istr.rdbuf();
        if (doc.Parse(sstr.str().c_str()).HasParseError() || !doc.IsObject())
            { LOGMSG("Cannot parse json document:\n%s", doc.GetString()); break; }

        s_devmon_config& conf = dptr->conf.devmon;

        RJ_GET(doc, KEY_SHMEM_NAME, conf.shmem_name);
        RJ_GET(doc, KEY_SHMEM_VERSION, conf.shmem_version);
        RJ_GET(doc, KEY_DEBUG_MODE, conf.debug_mode);
        RJ_GET(doc, KEY_GEOIP_PATH, conf.geoip_path);
        RJ_GET(doc, KEY_CLOUD_CONFIG, conf.conf_cloud);
        RJ_GET(doc, KEY_DEVICE_CONFIG, conf.conf_device);
        RJ_GET(doc, KEY_IDENTIFY_CONFIG, conf.conf_identify);

        dptr->conf_cloud = conf.conf_cloud;
        dptr->conf_device = conf.conf_device;
        dptr->conf_identify = conf.conf_identify;

        status = true;
    } while(0);

    if (true == status)
    {
        s_devmon_config& conf = dptr->conf.devmon;

        char* cptr = conf.string_buffer;
        size_t cpos = 0;
        size_t maxpos = sizeof(conf.string_buffer) / sizeof(conf.string_buffer[0]);

        #define MAP_ITEM(fieldname, fieldpos) do { \
            ASSERT(cpos < maxpos); \
            size_t fieldlen = fieldname.length(); \
            fieldpos = cpos; \
            memcpy(cptr, fieldname.c_str(), fieldlen); cptr += fieldlen; \
            *cptr = 0; cptr += 1; \
            cpos += fieldlen + 1; \
        } while(0)

        MAP_ITEM(conf.geoip_path, conf.geoip_offset);

        #undef MAP_ITEM
    }

    LOGMSGIF(status, "Application config:\n %s", to_string(dptr->conf.devmon).c_str());

    LOGMSGIF(!status, "Cannot parse devmon config");

    return status;
}

bool parse_cloud_config()
{
    s_app_data* dptr = get_data_ptr();

    bool status = false;
    do {
        Document doc;

        ifstream istr;
        istr.open(dptr->conf_cloud.c_str(), ios::binary);
        if (false == istr.is_open())
            { LOGSTR("Cannot open cloud config file: %s", dptr->conf_cloud); break; }

        stringstream sstr;
        sstr << istr.rdbuf();
        if (doc.Parse(sstr.str().c_str()).HasParseError() || !doc.IsObject())
            { LOGMSG("Cannot parse json document:\n%s", doc.GetString()); break; }

        s_cloud_config& conf = dptr->conf.cloud;

        RJ_GET(doc, KEY_ACTIVE_CLOUD, conf.cloud_name);

        const Value& obj = doc[conf.cloud_name.c_str()];
        if (false == obj.IsObject()) break;

        RJ_GET(obj, KEY_HOST_NAME, conf.host_name);
        RJ_GET(obj, KEY_CLIENT_ID, conf.client_id);
        RJ_GET(obj, KEY_THING_NAME, conf.thing_name);
        RJ_GET(obj, KEY_CERT_FILE, conf.cert_filename);
        RJ_GET(obj, KEY_ROOTCA_FILE, conf.rootca_filename);
        RJ_GET(obj, KEY_PRIVKEY_FILE, conf.privkey_filename);
        RJ_GET(obj, KEY_MQTT_PORT, conf.mqtt_port);

        status = true;
    } while(0);

    if (true == status)
    {
        s_cloud_config& conf = dptr->conf.cloud;

        char* cptr = conf.string_buffer;
        size_t cpos = 0;
        size_t maxpos = sizeof(conf.string_buffer) / sizeof(conf.string_buffer[0]);

        #define MAP_ITEM(fieldname, fieldpos) do { \
            ASSERT(cpos < maxpos); \
            size_t fieldlen = fieldname.length(); \
            fieldpos = cpos; \
            memcpy(cptr, fieldname.c_str(), fieldlen); cptr += fieldlen; \
            *cptr = 0; cptr += 1; \
            cpos += fieldlen + 1; \
        } while(0)

        MAP_ITEM(conf.host_name, conf.host_offset);
        MAP_ITEM(conf.client_id, conf.client_offset);
        MAP_ITEM(conf.thing_name, conf.thing_offset);
        MAP_ITEM(conf.rootca_filename, conf.rootca_offset);
        MAP_ITEM(conf.cert_filename, conf.cert_offset);
        MAP_ITEM(conf.privkey_filename, conf.privkey_offset);

        #undef MAP_ITEM
    }

    LOGMSGIF(!status, "Cannot parse cloud config");

    return status;
}

bool parse_device_config()
{
    s_app_data* dptr = get_data_ptr();

    bool status = false;
    do {
        Document doc;

        ifstream istr; istr.open(dptr->conf_device.c_str(), ios::binary);
        if (false == istr.is_open())
            { LOGSTR("Cannot open device config file: %s", dptr->conf_device); break; }

        stringstream sstr; sstr << istr.rdbuf();
        if (doc.Parse(sstr.str().c_str()).HasParseError() || !doc.IsObject())
            { LOGMSG("Cannot parse json document:\n%s", doc.GetString()); break; }

        s_device_config& conf = dptr->conf.device;

        RJ_GET(doc, KEY_DEVICE_NAME, conf.device_name);
        RJ_GET(doc, KEY_GROUP_NAME, conf.group_name);
        RJ_GET(doc, KEY_LOCATION, conf.location);
        RJ_GET(doc, KEY_COUNTRY, conf.country);

        status = true;
    } while(0);

    LOGMSGIF(!status, "Cannot parse device config");

    return status;
}

bool parse_identify_config()
{
    s_app_data* dptr = get_data_ptr();

    bool status = false;
    do {
        Document doc;

        ifstream istr; istr.open(dptr->conf_identify.c_str(), ios::binary);
        if (false == istr.is_open())
            { LOGSTR("Cannot open device identify file: %s", dptr->conf_identify); break; }

        stringstream sstr; sstr << istr.rdbuf();
        if (doc.Parse(sstr.str().c_str()).HasParseError() || !doc.IsObject())
            { LOGMSG("Cannot parse json document:\n%s", doc.GetString()); break; }

        s_identify_config& conf = dptr->conf.identify;

        RJ_GET(doc, KEY_MODEL, conf.model_string);
        RJ_GET(doc, KEY_SERIAL, conf.serial_string);
        RJ_GET(doc, KEY_FIRMWARE, conf.firmware_string);
        RJ_GET(doc, KEY_CAPACITY, conf.capacity);

        status = true;
    } while(0);

    LOGMSGIF(!status, "Cannot parse identify config");

    return status;
}

// -----------------------------------------------------------------
// Utilities on config data
// -----------------------------------------------------------------

void reset_config()
{
    s_app_data* dptr = get_data_ptr();

    dptr->conf_app = "";
    dptr->conf_cloud = "";
    dptr->conf_device = "";
    dptr->conf_identify = "";

    reset_config(dptr->conf.cloud);
    reset_config(dptr->conf.device);
    reset_config(dptr->conf.identify);
    reset_config(dptr->conf.devmon);
}

void reset_config(s_cloud_config &conf)
{
    conf.cloud_name = "";
    conf.host_name = "";
    conf.client_id = "";
    conf.thing_name = "";
    conf.cert_filename = "";
    conf.rootca_filename = "";
    conf.privkey_filename = "";

    conf.mqtt_port = 0;

    conf.host_offset = 0;
    conf.client_offset = 0;
    conf.thing_offset = 0;
    conf.cert_offset = 0;
    conf.rootca_offset = 0;
    conf.privkey_offset = 0;

    memset(conf.string_buffer, 0x00, sizeof(conf.string_buffer) / sizeof(conf.string_buffer[0]));
}

void reset_config(s_device_config &conf)
{
    conf.device_name = "";
    conf.group_name = "";
    conf.location = "";
    conf.country = "";
}

void reset_config(s_identify_config &conf)
{
    conf.model_string = "";
    conf.serial_string = "";
    conf.firmware_string = "";

    conf.capacity = 0;
}

void reset_config(s_devmon_config &conf)
{
    conf.debug_mode = false;

    conf.shmem_name = "";
    conf.shmem_version = "";

    conf.geoip_path = "";

    conf.conf_cloud = "";
    conf.conf_identify = "";
    conf.conf_device = "";
}

// -----------------------------------------------------------------
// Viewing config data
// -----------------------------------------------------------------

string to_string(const s_cloud_config& conf)
{
    RJ_DECWRITER(); RJ_START();

    RJ_ADDSTR(KEY_ACTIVE_CLOUD ,conf.cloud_name);
    RJ_ADDSTR(KEY_HOST_NAME    ,conf.host_name);
    RJ_ADDSTR(KEY_CLIENT_ID    ,conf.client_id);
    RJ_ADDSTR(KEY_THING_NAME   ,conf.thing_name);
    RJ_ADDSTR(KEY_CERT_FILE    ,conf.cert_filename);
    RJ_ADDSTR(KEY_ROOTCA_FILE  ,conf.rootca_filename);
    RJ_ADDSTR(KEY_PRIVKEY_FILE ,conf.privkey_filename);
    RJ_ADDINT(KEY_MQTT_PORT    ,conf.mqtt_port);
    RJ_STOP();

    return RJ_TOSTRING();
}

string to_string(const s_device_config& conf)
{
    RJ_DECWRITER(); RJ_START();

    RJ_ADDSTR(KEY_DEVICE_NAME  ,conf.device_name);
    RJ_ADDSTR(KEY_GROUP_NAME   ,conf.group_name);
    RJ_ADDSTR(KEY_LOCATION     ,conf.location);
    RJ_ADDSTR(KEY_COUNTRY      ,conf.country);
    RJ_STOP();

    return RJ_TOSTRING();
}

string to_string(const s_identify_config& conf)
{
    RJ_DECWRITER(); RJ_START();

    RJ_ADDSTR(KEY_MODEL        ,conf.model_string);
    RJ_ADDSTR(KEY_SERIAL       ,conf.serial_string);
    RJ_ADDSTR(KEY_FIRMWARE     ,conf.firmware_string);
    RJ_ADDDBL(KEY_CAPACITY     ,conf.capacity);
    RJ_STOP();

    return RJ_TOSTRING();
}

string to_string(const s_devmon_config& conf)
{
    RJ_DECWRITER(); RJ_START();

    RJ_ADDSTR(KEY_SHMEM_NAME     ,conf.shmem_name);
    RJ_ADDSTR(KEY_SHMEM_VERSION  ,conf.shmem_version);
    RJ_ADDBLN(KEY_DEBUG_MODE     ,conf.debug_mode);
    RJ_ADDSTR(KEY_CLOUD_CONFIG   ,conf.conf_cloud);
    RJ_ADDSTR(KEY_DEVICE_CONFIG  ,conf.conf_device);
    RJ_ADDSTR(KEY_IDENTIFY_CONFIG,conf.conf_identify);
    RJ_ADDSTR(KEY_GEOIP_PATH     ,conf.geoip_path);
    RJ_STOP();

    return RJ_TOSTRING();
}

string to_string(const s_app_config& param)
{
    stringstream sstr;

    sstr << to_string(param.cloud) << endl;
    sstr << to_string(param.device) << endl;
    sstr << to_string(param.identify) << endl;
    sstr << to_string(param.devmon) << endl;

    return sstr.str();
}
