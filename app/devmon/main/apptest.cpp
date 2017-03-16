
#include "apptest.h"
#include "appdata.h"
#include "apputil.h"
#include "appconfig.h"

bool test_utility()
{
    do_test_geoip();

    return false;
}

// -----------------------------------------------------
// Test utilities
// -----------------------------------------------------

#include "geoip_interface.h"

bool do_test_geoip()
{
    s_geoip_info info;

    // const char* ipaddr = "115.78.160.208";
    // bool status = get_geoip_info(ipaddr, info);

    bool status = get_geoip_info(info);

    if (true == status)
    {
        stringstream sstr;
        // sstr << "IP Address: " << ipaddr << endl;
        sstr << "City: " << info.city << endl;
        sstr << "Country: " << info.country << endl;
        sstr << "Ladtitude: " << info.ladtitude << endl;
        sstr << "Longtitude: " << info.longtitude << endl;

        cout << sstr.str() << endl;
    }

    return status;
}
