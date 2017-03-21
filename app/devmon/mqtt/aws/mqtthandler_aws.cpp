#include "stdmacro.h"
#include "debuglog.h"

#include "apputil.h"
#include "appdata.h"
#include "appresource.h"

#include "mqttmsg.h"
#include "mqttutil.h"
#include "mqttcallback.h"
#include "mqttcallback_aws.h"
#include "mqttjson_aws.h"

#include "aws_iot_shadow_interface.h"
#include "aws_iot_mqtt_client_interface.h"

static bool mqtt_init_client();
static bool mqtt_connect_client();

static bool mqtt_init_shadow();
static bool mqtt_connect_shadow();

static bool aws_subscribe_topic(e_mqttmsg_id id, tAwsSubCallback callback);

bool aws_init_mqtt_client()
{
    int msize = sizeof (s_aws_mqtt_client);

    s_aws_mqtt_client* mptr = (s_aws_mqtt_client*) malloc (msize);
    memset(mptr, 0x00, msize);

    s_mqtt_task& task = get_task_ptr()->mqtt;
    task.client = mptr;

    bool status = false;
    do {
        if (false == mqtt_init_client())
        { set_error_code(eCannotInitClientObject); break; }

        if (false == mqtt_connect_client())
        { set_error_code(eCannotConnectClientObject); break; }

        if (false == mqtt_init_shadow())
        { set_error_code(eCannotInitThingShadow); break; }

        if (false == mqtt_connect_shadow())
        { set_error_code(eCannotConnectThingShadow); break; }

        status = true;
    } while(0);

    set_error_if(!status, eCannotInitMqttClient);

    return status;
}

bool aws_publish_topics()
{
    bool status = false;

    do {
        aws_handle_iam_connected_device();

        status = true;
    } while(0);

    set_error_if(!status, eCannotPublishMqttTopics);

    return status;
}

bool aws_subscribe_topics()
{
    bool status = false;

    do {
        if (false == aws_subscribe_topic(SUB_GET_CONNECTED_DEVICE, aws_handle_get_connected_device))
        { set_error_code(eCannotSubGetConnectedDevice); break; }

        if (false == aws_subscribe_topic(SUB_GET_VTVIEW_DATA, aws_handle_get_vtview_data))
        { set_error_code(eCannotSubGetVtviewData); break; }

        status = true;
    } while(0);

    set_error_if(!status, eCannotSubscribeMqttTopics);

    return status;
}

bool aws_process_messages()
{
    AWS_DEF_VARS();

    while(!task.request_stop)
    {
        SHOWMSG("calling mqtt_yield");

        IoT_Error_t rc = aws_iot_mqtt_yield(&client, 100);

        if(NETWORK_ATTEMPTING_RECONNECT == rc)
        {
            continue;
        }

        sleep(1);
    }
}

// ------------------------------------------------------------------------
// static utility functions
// ------------------------------------------------------------------------

static bool mqtt_init_client()
{
    AWS_DEF_VARS();

    IoT_Client_Init_Params param = iotClientInitParamsDefault;

    param.enableAutoReconnect = false;
    param.pHostURL = AWS_GETPTR(cloud, host_offset);
    param.port = cloud.mqtt_port;
    param.pRootCALocation = AWS_GETPTR(cloud, rootca_offset);
    param.pDeviceCertLocation = AWS_GETPTR(cloud, cert_offset);
    param.pDevicePrivateKeyLocation = AWS_GETPTR(cloud, privkey_offset);
    param.mqttCommandTimeout_ms = 20000;
    param.tlsHandshakeTimeout_ms = 5000;
    param.isSSLHostnameVerify = true;
    param.disconnectHandler = aws_handle_disconnect;
    param.disconnectHandlerData = NULL;

    if(SUCCESS != aws_iot_mqtt_init(&client, &param)) return false;

    return true;
}

static bool mqtt_connect_client()
{
    AWS_DEF_VARS();

    IoT_Client_Connect_Params param = iotClientConnectParamsDefault;

    param.isCleanSession = true;
    param.isWillMsgPresent = false;
    param.MQTTVersion = MQTT_3_1_1;
    param.keepAliveIntervalInSec = 10;
    param.pClientID = AWS_GETPTR(cloud, client_offset);
    param.clientIDLen = (uint16_t) strlen(param.pClientID);

    if(SUCCESS != aws_iot_mqtt_connect(&client, &param)) return false;
    if(SUCCESS != aws_iot_mqtt_autoreconnect_set_status(&client, true)) return false;

    return true;
}

static bool mqtt_init_shadow()
{
    AWS_DEF_VARS();

    ShadowInitParameters_t param = ShadowInitParametersDefault;

    param.port = cloud.mqtt_port;
    param.disconnectHandler = NULL;
    param.enableAutoReconnect = false;
    param.pHost = AWS_GETPTR(cloud, host_offset);
    param.pRootCA = AWS_GETPTR(cloud, rootca_offset);
    param.pClientCRT = AWS_GETPTR(cloud, cert_offset);
    param.pClientKey = AWS_GETPTR(cloud, privkey_offset);

    if(SUCCESS != aws_iot_shadow_init(&client, &param)) return false;

    return true;
}

static bool mqtt_connect_shadow()
{
    AWS_DEF_VARS();

    ShadowConnectParameters_t param = ShadowConnectParametersDefault;
    param.pMyThingName = AWS_GETPTR(cloud, thing_offset);
    param.pMqttClientId = AWS_GETPTR(cloud, client_offset);
    param.mqttClientIdLen = (uint16_t) strlen(param.pMqttClientId);

    if(SUCCESS != aws_iot_shadow_connect(&client, &param)) return false;

    return true;
}

static bool aws_subscribe_topic(e_mqttmsg_id id, tAwsSubCallback callback)
{
    AWS_DEF_VARS();

    IoT_Error_t rc = FAILURE;

    const char* topic = get_message_buffer(id);
    unsigned short topic_len = strlen(topic);
    if (0 == topic_len) return false;

    rc = aws_iot_mqtt_subscribe (&client, topic, topic_len, QOS0, callback, NULL);

    return rc == SUCCESS;
}
