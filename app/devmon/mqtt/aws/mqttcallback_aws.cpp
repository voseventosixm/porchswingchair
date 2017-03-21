#include "stdmacro.h"
#include "debuglog.h"

#include "appdata.h"
#include "appresource.h"

#include "mqttmsg.h"
#include "mqttjson_aws.h"
#include "mqttcallback.h"
#include "mqttcallback_aws.h"

#include "aws_iot_shadow_interface.h"
#include "aws_iot_mqtt_client_interface.h"

// ------------------------------------------------------------------------
// misc callbacks
// ------------------------------------------------------------------------

void aws_handle_disconnect(AWS_IoT_Client *clientptr, void *data)
{
    SHOWMSG("Client disconnected");

    IoT_Error_t rc = FAILURE;

    if(NULL == clientptr) return;

    if(true == aws_iot_is_autoreconnect_enabled(clientptr))
    {
        SHOWMSG("Auto Reconnect is enabled, Reconnecting attempt will start now");
    }
    else
    {
        SHOWMSG("Auto Reconnect not enabled. Starting manual reconnect...");

        rc = aws_iot_mqtt_attempt_reconnect(clientptr);
        if(NETWORK_RECONNECTED == rc) SHOWMSG("Manual Reconnect Successful");
        else                          SHOWMSG("Manual Reconnect Failed - %d", rc);
    }
}

// ------------------------------------------------------------------------
// handle publish topics
// ------------------------------------------------------------------------

void aws_handle_iam_connected_device()
{
    AWS_DEF_VARS();

    JSON_BUFFER(json);

    SHOWMSG("Build json content: iam_connected_device");
    if (false == aws_build_json_device_identify(jsonbuff, jsonsize)) return;

    IoT_Publish_Message_Params param;
    param.qos = QOS0;
    param.isRetained = 0;
    param.payload = (void *) jsonbuff;
    param.payloadLen = strlen(jsonbuff);

    IoT_Error_t rc;

    const char* topic = get_message_buffer(PUB_IAM_CONNECTED_DEVICE);
    unsigned short topic_len = strlen(topic);
    if (0 == topic_len) return;

    rc = aws_iot_mqtt_publish(&client, topic, topic_len, &param);
    if (rc == MQTT_REQUEST_TIMEOUT_ERROR)
    {
        SHOWMSG("Publish ACK not received on topic %s", topic);
    }

    SHOWMSG("Publish topic: %s", topic);
}

// ------------------------------------------------------------------------
// handle subscribe topics
// ------------------------------------------------------------------------

void aws_handle_general_message(
    AWS_IoT_Client *clientptr,
    char *topic, uint16_t len,
    IoT_Publish_Message_Params *params,
    void *data)
{
    string msgname = string(topic, len);

    SHOWSTR("Dispatch message: %s", msgname);

    e_mqttmsg_id msgid = get_message_id(msgname);

    #define MAP_ITEM(code, func) case code: func(clientptr, topic, len, params, data); break

    switch(msgid)
    {
        MAP_ITEM(SUB_GET_VTVIEW_DATA, aws_handle_get_vtview_data);
        MAP_ITEM(SUB_GET_CONNECTED_DEVICE, aws_handle_get_connected_device);

        default:
            SHOWMSG("Received unhandled message: %s", topic);
            break;
    }
}

void aws_handle_get_vtview_data(
    AWS_IoT_Client *clientptr,
    char *topic, uint16_t len,
    IoT_Publish_Message_Params *params,
    void *data)
{
    SHOWMSG("Receive message: %s", topic);

    AWS_DEF_VARS();

    JSON_BUFFER(json);
    if (false == aws_build_shadow_vtview_data(jsonbuff, jsonsize)) return;

    aws_iot_shadow_update(&client, cloud.thing_name.c_str(), jsonbuff, NULL, NULL, 4, true);
}

void aws_handle_get_connected_device(
    AWS_IoT_Client *clientptr,
    char *topic, uint16_t len,
    IoT_Publish_Message_Params *params,
    void *data)
{
    SHOWSTR("Receive message: %s", string(topic, len));

    aws_handle_iam_connected_device();
}
