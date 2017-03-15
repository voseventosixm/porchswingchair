#ifndef GEOIP_INTERFACE_H
#define GEOIP_INTERFACE_H

#include "stdheader.h"

struct s_geoip_info
{
    string city;
    string country;
    double ladtitude;
    double longtitude;
};

bool get_geoip_info(s_geoip_info& info);
bool get_geoip_info(const char* ipaddr, s_geoip_info& info);

bool lookup_ip_address(char* ipaddr, size_t ipsize);

#endif
