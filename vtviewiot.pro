TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += DEVLOG UTILITY
# CONFIG += DEVMON AWSPORT UTILITY

LIBS += -pthread
QMAKE_CXXFLAGS += -std=c++0x -pthread

CONFIG(DEVMON)
{

INCLUDEPATH += app/devmon/main
INCLUDEPATH += app/devmon/mqtt
INCLUDEPATH += app/devmon/mqtt/aws
INCLUDEPATH += app/devmon/vtview
INCLUDEPATH += app/devmon/vtview/smart
INCLUDEPATH += app/devmon/vtview/temp

HEADERS += \
    app/devmon/main/appconfig.h \
    app/devmon/main/appdata.h \
    app/devmon/main/appmain.h \
    app/devmon/main/apptest.h \
    app/devmon/main/apputil.h \
    app/devmon/main/appresource.h \
    \
    app/devmon/mqtt/jsonkey.h \
    app/devmon/mqtt/mqttmsg.h \
    app/devmon/mqtt/mqttutil.h \
    app/devmon/mqtt/mqtthandler.h \
    app/devmon/mqtt/mqttcallback.h \
    app/devmon/mqtt/aws/mqttjson_aws.h \
    app/devmon/mqtt/aws/mqttcallback_aws.h \
    \
    app/devmon/vtview/geoip_interface.h \
    app/devmon/vtview/vtview_interface.h \
    app/devmon/vtview/temp_interface.h \
    app/devmon/vtview/smart_converter.h \
    app/devmon/vtview/smart_interface.h

SOURCES += \
    app/devmon/main/appconfig.cpp \
    app/devmon/main/appdata.cpp \
    app/devmon/main/appmain.cpp \
    app/devmon/main/apptest.cpp \
    app/devmon/main/apputil.cpp \
    app/devmon/main/appresource.cpp \
    \
    app/devmon/mqtt/mqttmsg.cpp \
    app/devmon/mqtt/mqttutil.cpp \
    app/devmon/mqtt/mqtthandler.cpp \
    app/devmon/mqtt/mqttcallback.cpp \
    app/devmon/mqtt/aws/mqttjson_aws.cpp \
    app/devmon/mqtt/aws/mqtthandler_aws.cpp \
    app/devmon/mqtt/aws/mqttcallback_aws.cpp \
    \
    app/devmon/vtview/geoip_interface.cpp \
    app/devmon/vtview/vtview_interface.cpp \
    app/devmon/vtview/temp_interface.cpp \
    app/devmon/vtview/smart_converter.cpp \
    app/devmon/vtview/smart_interface.cpp

OTHER_FILES += \
    app/devmon/makefile \
    app/devmon/makefile_arm \
    app/devmon/main/vt_usage.txt \
    app/devmon/main/configkey.def \
    app/devmon/mqtt/jsonkey.def \
    app/devmon/mqtt/mqttmap.def

OTHER_FILES += \
    config/devmon_config.json \
    config/cloud_config.json \
    config/device_config.json \
    config/device_identify.json
}

CONFIG(UTILITY)
{

INCLUDEPATH += utility/core
INCLUDEPATH += utility/geoip
INCLUDEPATH += utility/misc
INCLUDEPATH += utility/rapidjson
INCLUDEPATH += utility/smart

HEADERS += \
    utility/misc/fileutil.h \
    utility/misc/rjaccess.h \
    utility/misc/debuglog.h \
    utility/misc/errorstack.h \
    utility/misc/stdmacro.h \
    utility/misc/stdheader.h \
    utility/misc/sysheader.h \
    \
    utility/core/CommonStruct.h \
    utility/core/CoreData.h \
    utility/core/CoreMacro.h \
    utility/core/CoreHeader.h \
    utility/core/CoreUtil.h \
    utility/core/HexFormatter.h \
    utility/core/MiscUtil.h \
    utility/core/NVMEStruct.h \
    utility/core/SmartUtil.h \
    utility/core/StringUtil.h \
    \
    utility/smart/smart.h \
    utility/smart/smart_type.h \
    utility/smart/smart_attr.h \
    utility/smart/smart_common.h \
    \
    utility/geoip/geoip.h \
    utility/geoip/geoip_city.h \
    utility/geoip/geoip_config.h \
    utility/geoip/geoip_internal.h

SOURCES += \
    utility/misc/fileutil.cpp \
    utility/misc/rjaccess.cpp \
    utility/misc/debuglog.cpp \
    utility/misc/errorstack.cpp \
    \
    utility/core/CommonStruct.cpp \
    utility/core/CoreData.cpp \
    utility/core/CoreUtil.cpp \
    utility/core/HexFormatter.cpp \
    utility/core/MiscUtil.cpp \
    utility/core/SmartUtil.cpp \
    utility/core/StringUtil.cpp \
    \
    utility/smart/smart.c \
    utility/smart/smart_common.c \
    \
    utility/geoip/geoip.cpp \
    utility/geoip/geoip_city.cpp \
    utility/geoip/geoip_deprecated.cpp \
    utility/geoip/geoip_interface.cpp \
    utility/geoip/time_zone.cpp \
    utility/geoip/region_name.cpp

OTHER_FILES += \
    utility/misc/errorcode.def \
    utility/misc/vt_version.txt \
    utility/misc/vt_license.txt
}

CONFIG(AWSPORT)
{
INCLUDEPATH += utility/aws/include
INCLUDEPATH += utility/aws/platform/linux/common
INCLUDEPATH += utility/aws/platform/linux/mbedtls
INCLUDEPATH += utility/aws/external_libs/jsmn
INCLUDEPATH += utility/aws/external_libs/mbedTLS/include

HEADERS += \
    utility/aws/include/aws_iot_error.h \
    utility/aws/include/aws_iot_json_utils.h \
    utility/aws/include/aws_iot_log.h \
    utility/aws/include/aws_iot_mqtt_client.h \
    utility/aws/include/aws_iot_mqtt_client_common_internal.h \
    utility/aws/include/aws_iot_mqtt_client_interface.h \
    utility/aws/include/aws_iot_shadow_actions.h \
    utility/aws/include/aws_iot_shadow_interface.h \
    utility/aws/include/aws_iot_shadow_json.h \
    utility/aws/include/aws_iot_shadow_json_data.h \
    utility/aws/include/aws_iot_shadow_key.h \
    utility/aws/include/aws_iot_shadow_records.h \
    utility/aws/include/aws_iot_version.h \
    utility/aws/include/network_interface.h \
    utility/aws/include/threads_interface.h \
    utility/aws/include/timer_interface.h

SOURCES += \
    utility/aws/src/aws_iot_json_utils.c \
    utility/aws/src/aws_iot_mqtt_client.c \
    utility/aws/src/aws_iot_mqtt_client_common_internal.c \
    utility/aws/src/aws_iot_mqtt_client_connect.c \
    utility/aws/src/aws_iot_mqtt_client_publish.c \
    utility/aws/src/aws_iot_mqtt_client_subscribe.c \
    utility/aws/src/aws_iot_mqtt_client_unsubscribe.c \
    utility/aws/src/aws_iot_mqtt_client_yield.c \
    utility/aws/src/aws_iot_shadow.c \
    utility/aws/src/aws_iot_shadow_actions.c \
    utility/aws/src/aws_iot_shadow_json.c \
    utility/aws/src/aws_iot_shadow_records.c
}

