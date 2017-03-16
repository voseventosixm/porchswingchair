#ifndef __MQTTUTIL_H__
#define __MQTTUTIL_H__

#define MAP_ITEM(name, keyval) \
    extern const char* JSON_##name;
#include "jsonkey.def"
#undef MAP_ITEM

bool is_aws_cloud();
bool is_azure_cloud();

#define SC_INIT() unsigned int stepcode = 0
#define SC_NEXT() stepcode++
#define SC_GET() stepcode

#endif
