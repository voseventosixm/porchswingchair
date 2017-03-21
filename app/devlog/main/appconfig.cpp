#include "stdmacro.h"
#include "rjaccess.h"
#include "debuglog.h"

#include "appdata.h"
#include "apputil.h"
#include "appconfig.h"

#define MAP_ITEM(name, keyval, defval) \
    const char* KEY_##name = keyval; \
    const char* DEF_##name = defval;
#include "configkey.def"
#undef MAP_ITEM

#define RJ_GET(src, keyword, keyvalue, errorcode) \
    if (!rj_get(src, keyword, keyvalue)) \
        { set_error_code(errorcode); break; }

// -----------------------------------------------------------------
// Generate config data
// -----------------------------------------------------------------

void generate_config(s_app_config& conf)
{
    generate_config(conf.program);
}

void generate_config(s_program_config &config)
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
    MAP_ITEM(CONFTYPE_PROGRAM, s_program_config);
    default: break;
    }

    #undef MAP_ITEM
}

// -----------------------------------------------------------------
// Parse config data
// -----------------------------------------------------------------

static bool parse_program_config();

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
    MAP_ITEM(OP_GENCONF_APP,     generate_config(argv[2], CONFTYPE_PROGRAM), 0);

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

    dptr->conf_program = filename;

    bool status = false;

    do {
        if (false == parse_program_config()) break;

        status = true;
    } while(0);

    s_app_config* cptr = get_config_ptr();
    if (true == status) update_logstate(cptr->program.debug_mode);

    return status;
}

bool parse_program_config()
{
    s_app_data* dptr = get_data_ptr();

    bool status = false;
    do {
        Document doc;

        ifstream istr; istr.open(dptr->conf_program.c_str(), ios::binary);
        if (false == istr.is_open())
        { set_error_code(eCannotOpenApplicationConfig); break; }

        stringstream sstr; sstr << istr.rdbuf();
        if (doc.Parse(sstr.str().c_str()).HasParseError() || !doc.IsObject())
        { set_error_code(eCannotParseApplicationConfigJson); break; }

        s_program_config& conf = dptr->conf.program;

        RJ_GET(doc, KEY_SHMEM_NAME, conf.shmem_name, eShmemNameJsonNotFound);
        RJ_GET(doc, KEY_SHMEM_VERSION, conf.shmem_version, eShmemVersionJsonNotFound);
        RJ_GET(doc, KEY_DEBUG_MODE, conf.debug_mode, eDebugModeJsonNotFound);
        RJ_GET(doc, KEY_GEOIP_PATH, conf.geoip_path, eGeoipPathJsonNotFound);
        RJ_GET(doc, KEY_CLOUD_CONFIG, conf.conf_cloud, eCloudConfigJsonNotFound);
        RJ_GET(doc, KEY_DEVICE_CONFIG, conf.conf_device, eDeviceConfigJsonNotFound);
        RJ_GET(doc, KEY_IDENTIFY_CONFIG, conf.conf_identify, eDeviceIdentifyJsonNotFound);

        status = true;
    } while(0);

    if (true == status)
    {
        s_program_config& conf = dptr->conf.program;

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

    LOGMSGIF(status, "Application config:\n %s", to_string(dptr->conf.program).c_str());

    set_error_if(!status, eInvalidApplicationConfig);

    return status;
}

// -----------------------------------------------------------------
// Utilities on config data
// -----------------------------------------------------------------

void reset_config()
{
    s_app_data* dptr = get_data_ptr();

    dptr->conf_program = "";

    reset_config(dptr->conf.program);
}

void reset_config(s_program_config &conf)
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

string to_string(const s_program_config& conf)
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

    sstr << to_string(param.program) << endl;

    return sstr.str();
}