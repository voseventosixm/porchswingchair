#include "appdata.h"
#include "appresource.h"

#include "mqttmsg.h"
#include "mqttutil.h"
#include "mqttjson_aws.h"
#include "mqttcallback.h"
#include "mqttcallback_aws.h"

#include "aws_iot_shadow_interface.h"
#include "aws_iot_mqtt_client_interface.h"

#include "geoip_interface.h"
#include "vtview_interface.h"

#include "document.h"
#include "prettywriter.h"

using namespace rapidjson;

// --------------------------------------------------------------------------
// build payload json data
// --------------------------------------------------------------------------

bool aws_build_json_device_identify(char *jsonbuff, size_t jsonsize)
{
    if (NULL == jsonbuff) return false;

    // Prepare internal information
    s_geoip_info geo;
    if (false == get_geoip_info(geo)) return false;

    AWS_DEF_VARS();

    RJ_DECWRITER(); RJ_START();
    RJ_ADDSTR(JSON_THING_NAME, cloud.thing_name);
    RJ_ADDSTR(JSON_CLIENT_ID,  cloud.client_id);

    RJ_ADDSTR(JSON_MODEL,      identify.model_string);
    RJ_ADDDBL(JSON_CAPACITY,   identify.capacity);

    RJ_ADDSTR(JSON_LOCATION,   device.location);
    RJ_ADDSTR(JSON_GROUP_NAME, device.group_name);

    RJ_ADDSTR(JSON_CITY,       geo.city);
    RJ_ADDSTR(JSON_COUNTRY,    geo.country);
    RJ_ADDDBL(JSON_LONGTITUDE, geo.longtitude);
    RJ_ADDDBL(JSON_LADTITUDE,  geo.ladtitude);
    RJ_STOP();

    int buffsize = snprintf(jsonbuff, jsonsize, "%s", RJ_TOSTRING());
    if ((buffsize < 0) || (buffsize >= jsonsize)) return false;

    return true;
}

bool aws_build_json_vtview_data(char *jsonbuff, size_t jsonsize)
{
    if (NULL == jsonbuff) return false;

    // Prepare internal information
    s_vtview_info view;
    if (false == get_vtview_info(view)) return false;

    AWS_DEF_VARS();

    RJ_DECWRITER(); RJ_START();

    // Identify information
    RJ_ADDSTR(JSON_MODEL,        identify.model_string);
    RJ_ADDSTR(JSON_SERIAL,       identify.serial_string);
    RJ_ADDSTR(JSON_FIRMWARE,     identify.firmware_string);
    RJ_ADDDBL(JSON_CAPACITY,     identify.capacity);

    // SMART attributes
    RJ_ADDDBL(JSON_WRITE_AMP,    view.write_amp);
    RJ_ADDDBL(JSON_EST_REMTIME,  view.remaining_time);
    RJ_ADDDBL(JSON_REM_TBW,      view.remaining_tbw);
    RJ_ADDDBL(JSON_DATA_WRITTEN, view.data_written);
    RJ_ADDDBL(JSON_DATA_READ,    view.data_read);
    RJ_ADDDBL(JSON_REM_LIFE,     view.remaining_life);
    RJ_ADDDBL(JSON_REM_SPARE,    view.remaining_spare);
    RJ_ADDDBL(JSON_TEMPERATURE,  view.temperature);
    RJ_STOP();

    int buffsize = snprintf(jsonbuff, jsonsize, "%s", RJ_TOSTRING());
    if ((buffsize < 0) || (buffsize >= jsonsize)) return false;

    return true;
}

// --------------------------------------------------------------------------
// build shadow update json data
// --------------------------------------------------------------------------

bool aws_build_shadow_vtview_data(char* jsonbuff, size_t jsonsize)
{
    s_vtview_info view;
    if (false == get_vtview_info(view)) return false;

    #define MAP_ITEM(name, callback, data, key, dtype) \
        jsonStruct_t name; if (1) { name.cb = callback; name.pData = (void*) data;  name.pKey = key; name.type = dtype; }

    MAP_ITEM(jsWriteAmp,    NULL, &view.write_amp,       JSON_WRITE_AMP,    SHADOW_JSON_DOUBLE);
    MAP_ITEM(jsRemTime,     NULL, &view.remaining_time,  JSON_EST_REMTIME,  SHADOW_JSON_DOUBLE);
    MAP_ITEM(jsRemTBW,      NULL, &view.remaining_tbw,   JSON_REM_TBW,      SHADOW_JSON_DOUBLE);
    MAP_ITEM(jsDataWritten, NULL, &view.data_written,    JSON_DATA_WRITTEN, SHADOW_JSON_UINT32);
    MAP_ITEM(jsDataRead,    NULL, &view.data_read,       JSON_DATA_READ,    SHADOW_JSON_UINT32);
    MAP_ITEM(jsRemLife,     NULL, &view.remaining_life,  JSON_REM_LIFE,     SHADOW_JSON_DOUBLE);
    MAP_ITEM(jsRemSpare,    NULL, &view.remaining_spare, JSON_REM_SPARE,    SHADOW_JSON_DOUBLE);
    MAP_ITEM(jsTemperature, NULL, &view.temperature,     JSON_TEMPERATURE,  SHADOW_JSON_DOUBLE);

    #undef MAP_ITEM

    IoT_Error_t rc;

    rc = aws_iot_shadow_init_json_document(jsonbuff, jsonsize);
    if (SUCCESS != rc) return false;

    rc = aws_iot_shadow_add_reported(jsonbuff, jsonsize, 7,
                                     &jsWriteAmp,
                                     &jsRemTime,
                                     &jsRemTBW,
                                     &jsDataWritten,
                                     &jsDataRead,
                                     &jsRemLife,
                                     &jsRemSpare );
    if (SUCCESS != rc) return false;

    rc = aws_iot_finalize_json_document(jsonbuff, jsonsize);
    if (SUCCESS != rc) return false;

    return true;
}
