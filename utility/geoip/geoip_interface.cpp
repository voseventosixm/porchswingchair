#include "stdmacro.h"
#include "appdata.h"

#include "geoip.h"
#include "geoip_city.h"
#include "geoip_internal.h"
#include "geoip_interface.h"


static bool load_geoip_database();
static GeoIP* load_geoip_type(int type);

// ----------------------------------------------------------
// interface functions
// ----------------------------------------------------------

bool get_geoip_info(s_geoip_info &info)
{
    char ipaddr[MAXSIZE];
    size_t ipsize = sizeof(ipaddr) / sizeof(ipaddr[0]);

    if (false == lookup_ip_address(ipaddr, ipsize)) return false;

    return get_geoip_info(ipaddr, info);
}

bool get_geoip_info(const char* ipaddr, s_geoip_info& info)
{
    if (NULL == ipaddr) return false;

    if (false == load_geoip_database()) return false;

    uint32_t ipnum = _GeoIP_lookupaddress(ipaddr);
    if (0 == ipnum) return false;

    // Read country information
    do {
        GeoIP* gi = load_geoip_type(GEOIP_COUNTRY_EDITION);
        if (NULL == gi) return false;

        int item_max = (int)GeoIP_num_countries();
        int item_id = GeoIP_id_by_ipnum(gi, ipnum);

        if (item_id < 0 || item_id >= item_max) return false;

        const char* item_name = GeoIP_country_name[item_id];

        info.country = string(item_name);

        GeoIP_delete(gi);
    } while(0);

    // Read city information
    do {
        GeoIP* gi = load_geoip_type(GEOIP_CITY_EDITION_REV1);
        if (NULL == gi) return false;

        GeoIPRecord * gir = GeoIP_record_by_ipnum(gi, ipnum);
        if (NULL == gir) return false;

        info.city = string(gir->city);
        info.ladtitude = gir->latitude;
        info.longtitude = gir->longitude;

        GeoIPRecord_delete(gir);
        GeoIP_delete(gi);
    } while(0);

    return true;
}

// ----------------------------------------------------------
// Get IP address
// ----------------------------------------------------------

static bool lookup_ip_address(const char* server, char* buffer, size_t length);

bool lookup_ip_address(char* ipbuffer, size_t iplength)
{
    if ((NULL == ipbuffer) || (1 >= iplength)) return false;

    #define MAP_ITEM(state, server) \
        if (state && (true == lookup_ip_address(server, ipbuffer, iplength))) return true;
    #include "iplookup.def"
    #undef MAP_ITEM

    return false;
}

bool lookup_ip_address(const char* server, char* buffer, size_t length)
{
    const char* wgetcmd = "wget -qO-";

    char cmdline[1024];
    snprintf(cmdline, 1024, "%s %s", wgetcmd, server);

    FILE* fp = popen(cmdline, "r");
    if (fp == NULL) return false;

    memset(buffer, 0x00, length);
    fgets(buffer, length - 1, fp);

    return 0 != strlen(buffer);
}

// ----------------------------------------------------------
// Load GeoIP database
// ----------------------------------------------------------

bool load_geoip_database()
{
    s_app_data* dptr = get_data_ptr();
    s_devmon_config& conf = dptr->conf.devmon;

    GeoIP_setup_custom_directory(conf.string_buffer + conf.geoip_offset);
    _GeoIP_setup_dbfilename();

    return true;
}

GeoIP* load_geoip_type(int type)
{
    GeoIP* gi = NULL;

    if (GeoIP_db_avail(type)) {
        gi = GeoIP_open_type(type, GEOIP_STANDARD | GEOIP_SILENCE);
        if (NULL != gi)
        {
            gi->charset = GEOIP_CHARSET_UTF8;
        }
    }

    return gi;
}

// ----------------------------------------------------------
// End
// ----------------------------------------------------------
