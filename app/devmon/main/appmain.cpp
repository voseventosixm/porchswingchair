
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

        LOGMSG("Init resources");
        if (false == init_resource()) break;

        // if (false == test_utility()) break;

        LOGMSG("Start worker threads");
        if (false == start_mqtt_handler()) break;

        LOGMSG("Joining threads");
        join_worker_thread();

        status = true;
    } while(0);

    s_app_error* estack = get_error_stack();
    LOGSTRIF(estack->is_error(), "ErrorStack: \n%s", estack->to_string());

    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}
