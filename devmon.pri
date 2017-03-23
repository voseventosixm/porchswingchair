
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
