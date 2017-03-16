
#include "applog.h"
#include "appdata.h"
#include "apputil.h"
#include "apptest.h"
#include "appconfig.h"
#include "mqtthandler.h"

#include "appmain.h"

int main(int argc, char** argv)
{
    bool status = false;

    do {
        init_applog();

        if (false == parse_params(argc, argv)) break;

        if (false == init_resource()) break;

        // if (false == test_utility()) break;

        if (false == start_mqtt_handler()) break;

        status = true;
    } while(0);

    LOGMSG("Joining threads");

    join_worker_thread();

    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}
