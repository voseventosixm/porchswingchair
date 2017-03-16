#ifndef __MQTTMSG_H__
#define __MQTTMSG_H__

#include "stdmacro.h"
#include "stdheader.h"
#include "sysheader.h"

enum e_mqttmsg_id
{
    #define MAP_ITEM(code, name) code,
    #include "mqttmap.def"
    #undef MAP_ITEM
};

string get_message_name(e_mqttmsg_id code);
e_mqttmsg_id get_message_id(const string& name);

const char* get_message_buffer(e_mqttmsg_id code);

#endif
