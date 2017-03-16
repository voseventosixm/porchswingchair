#ifndef __MQTTCALLBACK_AWS_H__
#define __MQTTCALLBACK_AWS_H__

#include "aws_iot_config.h"
#include "aws_iot_mqtt_client.h"

struct s_aws_mqtt_client
{
    AWS_IoT_Client client;
};

// AWS Port
bool aws_init_mqtt_client();
bool aws_publish_topics();
bool aws_subscribe_topics();
bool aws_process_messages();

typedef void (tAwsDisconnectCallback) (AWS_IoT_Client *clientptr, void *data);
extern tAwsDisconnectCallback aws_handle_disconnect;

typedef void (tAwsSubCallback) (AWS_IoT_Client *clientptr, char *topic, uint16_t len, IoT_Publish_Message_Params *params, void *data);
extern tAwsSubCallback aws_handle_general_message;
extern tAwsSubCallback aws_handle_get_vtview_data;
extern tAwsSubCallback aws_handle_get_connected_device;

typedef void (tAwsPubCallback) ();
extern tAwsPubCallback aws_handle_iam_connected_device;

#define AWS_DEF_VARS() \
    s_app_data* aptr = get_data_ptr(); \
    s_mqtt_task& task = get_task_ptr()->mqtt; \
    s_app_config* conf = get_config_ptr(); \
    s_cloud_config& cloud = conf->cloud; \
    s_device_config& device = conf->device; \
    s_identify_config& identify = conf->identify; \
    s_devmon_config& devmon = conf->devmon; \
    AWS_IoT_Client& client = ((s_aws_mqtt_client*) task.client)->client


#define AWS_GETPTR(cloud, offset) (cloud.string_buffer + cloud.offset)

#endif
