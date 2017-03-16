#ifndef __MQTTJSON_AWS_H__
#define __MQTTJSON_AWS_H__

#include "aws_iot_config.h"
#include "aws_iot_mqtt_client.h"

// general payload (build json only)
bool aws_build_json_vtview_data(char* jsonbuff, size_t jsonsize);
bool aws_build_json_device_identify(char* jsonbuff, size_t jsonsize);

// specific payload for each message
bool aws_build_shadow_vtview_data(char* jsonbuff, size_t jsonsize);

#endif
