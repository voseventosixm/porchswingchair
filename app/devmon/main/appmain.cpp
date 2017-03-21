#include "debuglog.h"
#include "errorstack.h"

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
        init_debuglog("devmon");

        if (false == parse_params(argc, argv)) {
            set_error_code(eInvalidApplicationArgument); break; }

        if (false == init_resource()) {
            set_error_code(eCannotInitializeResource);  break; }

        // if (false == test_utility()) break;

        if (false == start_mqtt_handler()) {
            set_error_code(eCannotStartMqttHandler); break; }

        SHOWMSG("Joining threads");
        join_worker_thread();

        status = true;
    } while(0);

    DUMPSTACK();

    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}
