#include "debuglog.h"
#include "errorstack.h"

#include "appdata.h"
#include "apputil.h"
#include "apptest.h"
#include "appconfig.h"
#include "loghandler.h"
#include "loadhandler.h"

#include "appmain.h"

int main(int argc, char** argv)
{
    bool status = false;

    do {
        init_debuglog("devlog");

        if (false == parse_params(argc, argv)) { set_error_code(eInvalidApplicationArgument); break; }

        if (false == init_resource()) { set_error_code(eCannotInitializeResource);  break; }

        if (false == start_load_handler()) { set_error_code(eCannotStartLoadHandler); break; }

        if (false == start_log_handler()) { set_error_code(eCannotStartLogHandler); break; }

        SHOWMSG("Joining threads");
        join_worker_thread();

        status = true;
    } while(0);

    DUMPSTACK();

    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}
