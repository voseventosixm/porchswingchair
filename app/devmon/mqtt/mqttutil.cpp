#include "appdata.h"
#include "appmain.h"
#include "appconfig.h"
#include "appresource.h"

#include "mqttutil.h"

#define MAP_ITEM(name, keyval) \
    const char* JSON_##name = keyval;
#include "jsonkey.def"
#undef MAP_ITEM

bool is_aws_cloud()
{
    return 0 == get_config_ptr()->cloud.cloud_name.compare("aws");
}

bool is_azure_cloud()
{
    return 0 == get_config_ptr()->cloud.cloud_name.compare("azure");
}
